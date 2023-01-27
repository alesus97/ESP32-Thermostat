#include "setup.h"
#include "printToDisplay.h"



// -------------------------- ISR ------------------------
void IRAM_ATTR onTimer() {
  
  expiredTimer = true;

  if (thermostat.target_temp > dht.readTemperature()) {

    if (digitalRead(RELAY_PIN) != 1) {
      digitalWrite(RELAY_PIN, HIGH);
      onActivationThermostat = true;
    }

  } else if (digitalRead(RELAY_PIN) != 0) {
  
      digitalWrite(RELAY_PIN, LOW);
      onDeactivationThermostat = true;
  }
}

void IRAM_ATTR increaseISR() {
  button_time = millis();

  if (button_time - last_button_time > debounceDelay) {
    if (thermostat.target_temp < 30) {
      thermostat.target_temp++;
      increaseButton.pressed = true;
      last_button_time = button_time;

    } else {
      increaseButton.pressed = true;
      last_button_time = button_time;
      Serial.println("Temperatura massima target raggiunta");
    }
  }
}


void IRAM_ATTR decreaseISR() {
  button_time = millis();
  if (button_time - last_button_time > debounceDelay) {
    if (thermostat.target_temp > 10) {
      decreaseButton.pressed = true;
      last_button_time = button_time;
      thermostat.target_temp--;

    } else {
      decreaseButton.pressed = true;
      last_button_time = button_time;
      Serial.println("Temperatura minima target raggiunta");
    }
  }
}

// -------------------------- SETUP ------------------------

void setup() {
  Serial.begin(115200);

  //Abilitazione bottoni in input
  pinMode(increaseButton.PIN, INPUT_PULLUP);
  pinMode(decreaseButton.PIN, INPUT_PULLUP);

  //Collegamento isr al bottone
  attachInterrupt(increaseButton.PIN, increaseISR, RISING);
  attachInterrupt(decreaseButton.PIN, decreaseISR, RISING);

  //Abilitazione Relay in output
  pinMode(RELAY_PIN, OUTPUT);

  dht.begin();

  // Configurazione Timer 0 dell'ESP32 per generare un'interruzione ogni 2 minuti

  // Leggi qua per info sul clock e sul prescaler https://circuitdigest.com/microcontroller-projects/esp32-timers-and-timer-interrupts
  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, /*120000000*/ 3000000, true);  //2 minuti
  timerAlarmEnable(My_timer);                  //Just Enable

  //Configurazione del display
  lcd.init();
  lcd.backlight();

  //Messaggio di avvio
  lcd.setCursor(1, 0);
  lcd.print("Progetto CSD v1");
  lcd.setCursor(0, 1);
  lcd.print("Alessio Carusio");
  delay(3000);

  //Mostro la temperatura
  LCDdisplayTemp();
}



// Insieme di funzioni per stampare sul display

void LCDdisplayTemp() {
  lcd.clear();
  lcd.home();
  lcd.print("Temp: ");
  lcd.print(dht.readTemperature());  //Forse qua ci va il valore di temp letto nell'interrupt e non una nuova lettura
  lcd.print(" \337C");
  lcd.setCursor(0, 1);
  lcd.print("Target: ");
  lcd.print(thermostat.target_temp);
  lcd.print(" \337C");
}

void LCDdisplayTargetTemp(int targetTemp) {
  lcd.clear();
  lcd.home();
  lcd.print("  Target temp");
  lcd.setCursor(0, 1);
  lcd.print("attuale: ");
  lcd.print(targetTemp);
  lcd.print(" \337C");
}

void LCDdisplayOnRiscaldamentoActivation() {
  lcd.clear();
  lcd.home();
  lcd.print(" Riscaldamento ");
  lcd.setCursor(0, 1);
  lcd.print("     acceso     ");
}

void LCDdisplayOnRiscaldamentoDeactivation() {
  lcd.clear();
  lcd.home();
  lcd.print(" Riscaldamento ");
  lcd.setCursor(0, 1);
  lcd.print("     spento     ");
}



// -------------------------- LOOP ------------------------
void loop() {

  /*
  
  Nota: L'aggiornamento del display non può avvenire nelle ISR dei bottoni o nella timer-interrupt, perchè... 
  ...The problem is almost certainly that the LCD library uses interrupts and they are disabled when in an ISR. Have the ISR set a flag variable and do the LCD output in loop()
  approfondimento a: https://forum.arduino.cc/t/lcd-display-inside-a-timerinterrupt/962576/3
  
  Una soluzione può essere quella di still use the timer to make the measurements at specific time intervals then display the results in loop() when a new measurement has been made
  You will, of course, be writing the loop() function such that it does not block the execution of the program which would prevent the timely display of the values

 */

  //Se si verifica una delle due interruzioni dei bottoni, allora devo aggiornare lo schermo mostrando il nuovo valore di temperatura target
  if (increaseButton.pressed || decreaseButton.pressed) {
    LCDdisplayTargetTemp(thermostat.target_temp);
    increaseButton.pressed = false;
    decreaseButton.pressed = false;
    delay(2000);
    LCDdisplayTemp();
  }


  //Quando scade la timer interrupt, aggiorno il display mostrando i nuovi valori di temperatura.
  if (expiredTimer) {
    LCDdisplayTemp();
    expiredTimer = false;
  }

  if (onActivationThermostat) {
    LCDdisplayOnRiscaldamentoActivation();
    onActivationThermostat = false;

    delay(8000);
    LCDdisplayTemp();

  } else if (onDeactivationThermostat) {
    LCDdisplayOnRiscaldamentoDeactivation();
    onDeactivationThermostat = false;

    delay(8000);
    LCDdisplayTemp();
  }
}