#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#define INCREASE_BUTTON_PIN 26
#define DECREASE_BUTTON_PIN 25
#define RELAY_PIN 32
#define LED_PIN 14
#define DHT_PIN 33


const int target_temp = 20;
int lcdColumns = 16;
int lcdRows = 2;


struct Button {
  const uint8_t PIN;
  bool pressed;
};

struct Thermostat {
  int target_temp;
};

hw_timer_t *My_timer = NULL;

DHT dht(DHT_PIN, DHT22);
Button increaseButton = { INCREASE_BUTTON_PIN, false };
Button decreaseButton = { DECREASE_BUTTON_PIN, false };
Thermostat thermostat = { target_temp };

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
/* LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);   */

LiquidCrystal_I2C lcd(0x27, 16, 2);



//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;
unsigned long last_button_time = 0;

bool expiredTimer = false;


// -------------------------- ISR ------------------------
void IRAM_ATTR onTimer() {
  //Aggiornamento display
  expiredTimer = true;

  if (thermostat.target_temp > dht.readTemperature()) {



    /*lcd.clear();
  lcd.home(); */
    //lcd.print(" Riscaldamento ");

    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);

    //  LCDdisplayRiscaldamentoAccesoMsg();

  } else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
}

bool pressed;
void IRAM_ATTR increaseISR() {
  button_time = millis();

  if (button_time - last_button_time > 250) {
    if (thermostat.target_temp < 30) {
      pressed = true;
      thermostat.target_temp++;
      increaseButton.pressed = true;
      last_button_time = button_time;

      Serial.print("Target Temperature: ");
      Serial.println(thermostat.target_temp);
      //  LCDdisplayTargetTemp(thermostat.target_temp);
    } else {
      increaseButton.pressed = true;
      last_button_time = button_time;
      Serial.println("Temperatura massima target raggiunta");
    }
  }
}


void IRAM_ATTR decreaseISR() {
  button_time = millis();
  if (button_time - last_button_time > 250) {
    if (thermostat.target_temp > 10) {
      decreaseButton.pressed = true;
      last_button_time = button_time;
      thermostat.target_temp--;
      Serial.print("Target Temperature: ");
      Serial.println(thermostat.target_temp);

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
  pinMode(increaseButton.PIN, INPUT_PULLUP);
  pinMode(decreaseButton.PIN, INPUT_PULLUP);
  attachInterrupt(increaseButton.PIN, increaseISR, RISING);
  attachInterrupt(decreaseButton.PIN, decreaseISR, RISING);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  dht.begin();

  // Leggi qua per info sul clock e sul prescaler https://circuitdigest.com/microcontroller-projects/esp32-timers-and-timer-interrupts
  My_timer = timerBegin(0, 9600 /*80*/, true);  //2 minuti
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 1000000, true);
  timerAlarmEnable(My_timer);  //Just Enable


  lcd.init();
  lcd.backlight();

  lcd.setCursor(1, 0);
  lcd.print("Progetto CSD v1");
  lcd.setCursor(0, 1);
  lcd.print("Alessio Carusio");

  delay(3000);

  LCDdisplayTemp();
}



// -------------------------- LOOP ------------------------

void LCDdisplayTemp() {
  lcd.clear();
  lcd.home();
  lcd.print("Temp: ");
  lcd.print(dht.readTemperature());  // display the temperature
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

void LCDdisplayRiscaldamentoAccesoMsg() {
  lcd.clear();
  lcd.home();
  lcd.print(" Riscaldamento ");
  lcd.print("  Target temp");
  lcd.setCursor(0, 1);
  lcd.print("   acceso      ");
}


void loop() {

  //The problem is almost certainly that the LCD library uses interrupts and they are disabled when in an ISRHave the ISR set a flag variable and do the LCD output in loop()
  // https://forum.arduino.cc/t/lcd-display-inside-a-timerinterrupt/962576/3

 
  if (increaseButton.pressed || decreaseButton.pressed) {
    LCDdisplayTargetTemp(thermostat.target_temp);
    increaseButton.pressed = false;
    decreaseButton.pressed = false;

    delay(3000);
    LCDdisplayTemp();
  }

  //You can still use the timer to make the measurements at specific time intervals then display the results in loop() when a new measurement has been made
  //You will, of course, be writing the loop() function such that it does not block the execution of the program which would prevent the timely display of the values
  if(expiredTimer){
    LCDdisplayTemp();
    expiredTimer = false;
  }
}