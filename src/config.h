#pragma once
#include <Arduino.h>

// Inputs and Relay Settings
const int up_button_pin = GPIO_NUM_39;     // the number of the pushbutton pin
const int down_button_pin = GPIO_NUM_34;     // the number of the pushbutton pin
const int turbo_state_pin = GPIO_NUM_35;
const int ecu_state_pin = GPIO_NUM_32;

const int gear_control_1 =  GPIO_NUM_17;      // working
const int gear_control_2 =  GPIO_NUM_16;      // working
const int turbo_control =  GPIO_NUM_18;      //working
const int ecu_control = GPIO_NUM_19;        //working 

const int clutch_control = GPIO_NUM_26;      //working

// Screen Configurations

const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 32; // OLED display height, in pixels
const int OLED_RESET = 4; // Reset pin # (or -1 if sharing Arduino reset pin)