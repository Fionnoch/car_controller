#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <config.h>

//#include <SPI.h>
//#include <SoftwareSerial.h>
//#include <Canbus.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// esp32 generic lib
// CAN_device_t CAN_cfg;               // CAN Config
// unsigned long previousMillis = 0;   // will store last time a CAN Message was send
// const int interval = 1000;          // interval at which send CAN Messages (milliseconds)
// const int rx_queue_size = 10;       // Receive Queue size

// sparkfun Declare CAN variables for communication
char *EngineRPM;
char buffer[64]; // Data will be temporarily stored to this buffer before being written to the file

// variables
int buttonState = 0; // variable for reading the pushbutton status
unsigned int last_interupt;
const int gear_trigger_debounce = 1000;
int set_gear = 1;
int current_gear = 1;
bool allow_gear_change = true;

// #####################################
// put function declarations here:
// #############################

void gear_up() // function to detect gear change up
{
  Serial.println("Gearing up");
  if (((millis() - last_interupt) > gear_trigger_debounce) && allow_gear_change == true)
  {
    if (set_gear < 4)
    { // prevents gear from going above 4
      set_gear++;
    }
    // Prevent the car skipping gears
    last_interupt = millis();  // prevents gears from being triggered based on time
    allow_gear_change = false; // prevents gears from being triggered until gear change has physically happened
    Serial.println("Desired Gear set to ");
    Serial.println(set_gear);
  }
}

void gear_down() // function to detect gear change down
{
  Serial.println("Gearing Down");
  if (((millis() - last_interupt) > gear_trigger_debounce) && allow_gear_change == true)
  {
    if (set_gear > 1)
    {
      set_gear = set_gear - 1;
    }
    // Prevent the car skipping gears
    last_interupt = millis();  // prevents gears from being triggered based on time
    allow_gear_change = false; // prevents gears from being triggered until gear change has physically happened
    Serial.println("Desired Gear set to ");
    Serial.println(set_gear);
  }
}

void clutch_disengage() // function to detect gear change down
{
  analogWrite(clutch_control, 125); // 255 = 12V, 0 = 0V .... 191 approx = 9V
  delay(500);                       // delay to allow solonoid to open
}

void clutch_engage() // function to detect gear change down
{
  analogWrite(clutch_control, 255); // 255 = 12V, 0 = 0V .... 191 approx = 9V
  delay(500);                       // delay to allow solonoid to close
}

void update_display(int number) // function to update the number on the display
{

  Serial.println("Updating display");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print("Gear ");
  display.print(number);
  if (digitalRead(ecu_state_pin))
  {
    display.print(" E");
  }
  if (digitalRead(turbo_state_pin))
  {
    display.print(" T");
  }
  //display.println("");

display.display(); 
}


void setup()
{

  Serial.begin(115200);
  Wire.begin(i2c_sda, i2c_scl);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32 display
    Serial.println(F("SSD1306 display allocation failed"));
    for(;;);
  }
  delay(2000);

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Starting");
  display.display(); 
  delay(2000);

  // generic esp32 lib
  // CAN_cfg.speed = CAN_SPEED_125KBPS;
  // CAN_cfg.tx_pin_id = GPIO_NUM_5;
  // CAN_cfg.rx_pin_id = GPIO_NUM_4;
  // CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
  //   // Init CAN Module
  //   ESP32Can.CANInit();

  // sparkfun canbus library
  //Canbus.init(CANSPEED_500); //CANSPEED_125 CANSPEED_250 CANSPEED_500 all in kbps

  // initialize the pins as an output:
  pinMode(gear_control_1, OUTPUT);
  pinMode(gear_control_2, OUTPUT);
  pinMode(turbo_control, OUTPUT);
  pinMode(clutch_control, OUTPUT);
  pinMode(ecu_control, OUTPUT);

  // initialize the pushbutton pin as an input:
  pinMode(up_button_pin, INPUT); // pinMode(up_button_pin, INPUT_PULLUP); //  pinMode(up_button_pin, INPUT);
  pinMode(down_button_pin, INPUT);
  pinMode(turbo_state_pin, INPUT);
  pinMode(ecu_state_pin, INPUT);

  attachInterrupt(digitalPinToInterrupt(up_button_pin), gear_up, FALLING);
  attachInterrupt(digitalPinToInterrupt(down_button_pin), gear_down, FALLING);

  analogWrite(clutch_control, 255); // enguage the clutch

  digitalWrite(gear_control_1, HIGH); // set to 1st gear
  digitalWrite(gear_control_2, LOW);
  update_display(1);

  digitalWrite(ecu_control, LOW);

  Serial.print("Current Gear: ");
  Serial.println(set_gear);
}

void loop()
{

  // sparkfun:
  // Canbus.ecu_req(ENGINE_RPM,buffer); //sparkfun Request engine RPM
  // ENGINE_RPM, ENGINE_COOLANT_TEMP, VEHICLE_SPEED, MAF_SENSOR, O2_VOLTAGE, THROTTLE derrived from https://en.wikipedia.org/wiki/OBD-II_PIDs
  // EngineRPM = buffer;
  // Serial.print("Engine RPM: "); //Uncomment for Serial debugging
  // Serial.println(buffer);
  // delay(100);

  if (digitalRead(ecu_state_pin) == HIGH)
  {                                 // if automatic gear controller switch is turned on
    digitalWrite(ecu_control, LOW); // turn on the controller via the relay

    // check if the turbo is turned on
    if (digitalRead(turbo_state_pin) == HIGH)
    {
      digitalWrite(turbo_control, HIGH);
    }
    else
    {
      digitalWrite(turbo_control, LOW);
    }

    // check whether the current gear is the same as the set gear. Gear settings controls are handles in an interrupt function
    if (set_gear != current_gear)
    {

      allow_gear_change = false; // prevents gears from being changed while a gear shift is happening. This is already called in the intterupt function but this is a backup
      // debug scenario to check if gear skipping detected
      if (abs(set_gear - current_gear) > 1)
      {
        Serial.println("skip gear scenario detected");
        if (set_gear > current_gear)
        {
          Serial.println("skipping gear upwards");
          set_gear = current_gear + 1;
        }
        else
        {
          Serial.println("skipping gear downwards");
          set_gear = current_gear - 1;
        }
      }

      // gear control settings
      // go to 80% for 100% volts
      clutch_disengage(); // disenguage the clutch

      if (set_gear == 1)
      { // 1st gear
        digitalWrite(gear_control_1, HIGH);
        digitalWrite(gear_control_2, LOW);
        update_display(1);
        current_gear = 1;
      }
      else if (set_gear == 2)
      { // 2nd gear
        digitalWrite(gear_control_1, LOW);
        digitalWrite(gear_control_2, LOW);
        update_display(2);
        current_gear = 2;
      }
      else if (set_gear == 3)
      { // 3rd gear
        digitalWrite(gear_control_1, LOW);
        digitalWrite(gear_control_2, HIGH);
        update_display(3);
        current_gear = 3;
      }
      else if (set_gear == 4)
      { // 4th gear
        digitalWrite(gear_control_1, HIGH);
        digitalWrite(gear_control_2, HIGH);
        update_display(4);
        current_gear = 4;
      }

      // reenguage the clutch
      clutch_engage();
      allow_gear_change = true;
      Serial.print("Current gear is ");
      Serial.println(current_gear);
    }
  }
  else
  { // ECU not enabled
    digitalWrite(ecu_control, HIGH);
    allow_gear_change = false;
  }
  delay(100); // how often the loop runs
}
