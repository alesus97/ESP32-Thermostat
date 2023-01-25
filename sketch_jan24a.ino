#include "DHT.h"


#define INCREASE_BUTTON_PIN 26
#define DECREASE_BUTTON_PIN 25
#define RELAY_PIN 32
#define LED_PIN 14
#define DHT_PIN 33


const int target_temp = 20;


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


//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;
unsigned long last_button_time = 0;

bool expiredTimer = false;


// -------------------------- ISR ------------------------
void IRAM_ATTR onTimer() {
  Serial.println(dht.readTemperature());

  if (thermostat.target_temp > dht.readTemperature()) {

    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);

  } else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
  expiredTimer = true;
}


void IRAM_ATTR increaseISR() {
  button_time = millis();
  if (button_time - last_button_time > 250) {
    if (thermostat.target_temp < 30) {
      thermostat.target_temp++;
      increaseButton.pressed = true;
      last_button_time = button_time;
      Serial.print("Target Temperature: ");
      Serial.println(thermostat.target_temp);
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
  My_timer = timerBegin(0, 9600, true);  //2 minuti
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 1000000, true);
  timerAlarmEnable(My_timer);  //Just Enable
}



// -------------------------- LOOP ------------------------

void loop() {}