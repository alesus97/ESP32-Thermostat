#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#ifndef SETUP_H
#define SETUP_H

#define INCREASE_BUTTON_PIN 26
#define DECREASE_BUTTON_PIN 25
#define RELAY_PIN 32
#define LED_PIN 14
#define DHT_PIN 33


#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define LCD_I2C_ADDRESS 0x27

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
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);


//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;
unsigned long last_button_time = 0;


// LCD change message variables
bool expiredTimer = false;
bool onActivationThermostat = false;
bool onDeactivationThermostat = false;

#endif