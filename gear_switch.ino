/*
  Chris' Landrover gear control system
*/

// Pins
const int up_button_pin = 2;     // the number of the pushbutton pin
const int down_button_pin = 3;     // the number of the pushbutton pin

const int turbo_state_pin = 4;
const int esc_state_pin = 5;

const int gear_control_1 =  7;      //
const int gear_control_2 =  8;      //
const int turbo_control =  6;      //
const int ecu_control = 9;
const int clutch_control = 10;      //

const int display_pin_8 = 12; //g
const int display_pin_7 = A0; //f
const int display_pin_2 = A5; //a
const int display_pin_3 = A4; //b
const int display_pin_1 = A3; //dp
const int display_pin_4 = A2; //c
const int display_pin_5 = A1; //d
const int display_pin_6 = 13; //e

const int segment_display[10][8] = {    
  { 1, 1, 1, 1, 1, 1, 0, 1 }, // 0
  { 0, 1, 1, 0, 0, 0, 0, 1 }, // 1
  { 1, 1, 0, 1, 1, 0, 1, 1 }, // 2
  { 1, 1, 1, 1, 0, 0, 1, 1 }, // 3
  { 0, 1, 1, 0, 0, 1, 1, 1 }, // 4
  { 1, 0, 1, 1, 0, 1, 1, 1 }, // 5
  { 1, 0, 1, 1, 1, 1, 1, 1 }, // 6
  { 1, 1, 1, 0, 0, 0, 0, 1 }, // 7
  { 1, 1, 1, 1, 1, 1, 1, 1 }, // 8
  { 1, 1, 1, 0, 0, 1, 1, 1 }  // 9
};   

//const int segment_pin_var[8] = {display_pin_1, display_pin_2, display_pin_3, display_pin_4, display_pin_5, display_pin_6, display_pin_7, display_pin_8};
//const int segment_pin_var[8] = {A7, A6, A5, A4, A3, A2, A1, 13};

// variables
int buttonState = 0;         // variable for reading the pushbutton status
unsigned int last_interupt;

const int gear_trigger_delay = 1000;

int set_gear = 1;
int current_gear = 1;
bool allow_update = true;

void setup() {

  Serial.begin(9600);
  // initialize the pins as an output:
  pinMode(gear_control_1, OUTPUT);
  pinMode(gear_control_2, OUTPUT);
  pinMode(turbo_control,  OUTPUT);
  pinMode(clutch_control, OUTPUT);
  pinMode(ecu_control, OUTPUT);

  pinMode(display_pin_1, OUTPUT);
  pinMode(display_pin_2, OUTPUT);
  pinMode(display_pin_3, OUTPUT);
  pinMode(display_pin_4, OUTPUT);
  pinMode(display_pin_5, OUTPUT);
  pinMode(display_pin_6, OUTPUT);
  pinMode(display_pin_7, OUTPUT);
  pinMode(display_pin_8, OUTPUT);

  // initialize the pushbutton pin as an input:
  pinMode(up_button_pin, INPUT);
  pinMode(down_button_pin, INPUT);
  pinMode(turbo_state_pin, INPUT);
  pinMode(esc_state_pin, INPUT);

  attachInterrupt(digitalPinToInterrupt(up_button_pin), gear_up, FALLING);
  attachInterrupt(digitalPinToInterrupt(down_button_pin), gear_down, FALLING);

  analogWrite(clutch_control, 255); //enguage the clutch

  digitalWrite(gear_control_1, HIGH); //set to 1st gear
  digitalWrite(gear_control_2, LOW);
  write_num(1);

  digitalWrite(ecu_control, LOW);

  Serial.println(set_gear);
}

void loop() {

  if ( digitalRead(esc_state_pin) == HIGH ) { 

    digitalWrite(ecu_control, LOW);
    

    //check if the turbo is turned on
    if (digitalRead(turbo_state_pin) == HIGH) {
      digitalWrite(turbo_control, HIGH);
    } else {
      digitalWrite(turbo_control, LOW);
    }

    

    if (millis()-last_interupt > gear_trigger_delay){
      allow_update = true;
    }

    // if the user presses a change gear button
    if (set_gear != current_gear) { //as opposed to doing a update system, check whether the gears are at the correct settings

      allow_update = false;

      if (abs(set_gear - current_gear) > 1) {
        Serial.println("skipping gear");
        if (set_gear > current_gear) {
          Serial.println("skipping gear upwards");
          set_gear = current_gear +1;
        } else {
          Serial.println("skipping gear downwards");
          set_gear = current_gear - 1;
        }
      }

      //gear control settings
      //go to 80% for 100% volts
      clutch_disengage(); //disenguage the clutch

      if (set_gear == 1 ) { //1st gear
        digitalWrite(gear_control_1, HIGH);
        digitalWrite(gear_control_2, LOW);
        write_num(1);
        current_gear = 1;
      }
      else if (set_gear == 2) { //2nd gear
        digitalWrite(gear_control_1, LOW);
        digitalWrite(gear_control_2, LOW);
        write_num(2);
        current_gear = 2;
      }
      else if (set_gear == 3) { //3rd gear
        digitalWrite(gear_control_1, LOW);
        digitalWrite(gear_control_2, HIGH);
        write_num(3);
        current_gear = 3;
      }
      else if (set_gear == 4) { //4th gear
        digitalWrite(gear_control_1, HIGH);
        digitalWrite(gear_control_2, HIGH);
        write_num(4);
        current_gear = 4;
      }
      
      //reenguage the clutch
      clutch_engage();
      allow_update = true;
      Serial.print("Current gear is ");
      Serial.println(current_gear);
    }
  } else {
    digitalWrite(ecu_control, HIGH);
    allow_update = false;
  }
  delay(100); //how often the loop runs
}


//--------Functions ----------
//----------------------------

void gear_up() //function to detect gear change up
{
  Serial.print("allow_update = ");
  Serial.println(allow_update);  
  if (((millis() - last_interupt) > gear_trigger_delay ) && allow_update==true) {
    //Update count
    //detachInterrupt(digitalPinToInterrupt(up_button_pin));
    if (set_gear < 4) {
      set_gear++;
    }
    
    last_interupt = millis();
    //attachInterrupt(digitalPinToInterrupt(up_button_pin), gear_up, FALLING);
    Serial.println("gear set to ");
    Serial.println(set_gear);
    allow_update = false;
  }
}

void gear_down() //function to detect gear change down
{
  Serial.print("allow_update = ");
  Serial.println(allow_update);
  if (((millis() - last_interupt) > gear_trigger_delay) && allow_update==true) {
    //detachInterrupt(digitalPinToInterrupt(down_button_pin));
    if (set_gear > 1) {
      set_gear = set_gear - 1;
    }
    
    last_interupt = millis();
    allow_update = false;
    //attachInterrupt(digitalPinToInterrupt(down_button_pin), gear_down, FALLING);
    Serial.println("Set to gear ");
    Serial.println(set_gear);
  }
}

void clutch_disengage() //function to detect gear change down
{
  analogWrite(clutch_control, 125); //255 = 12V, 0 = 0V .... 191 approx = 9V
  delay(500); //delay to allow solonoid to open
}

void clutch_engage() //function to detect gear change down
{
  analogWrite(clutch_control, 255); //255 = 12V, 0 = 0V .... 191 approx = 9V
  delay(500); //delay to allow solonoid to close
}

// writes values to the sev seg pins
void write_num(int number)
{
  //for (int j=0; j < 7; j  ) {
  //digitalWrite(segment_pin_var[j], segment_display[number][j]);
  //}
  digitalWrite(display_pin_1, segment_display[number][7] ); //pin 1 is the dot 
  digitalWrite(display_pin_2, segment_display[number][0] );
  digitalWrite(display_pin_3, segment_display[number][1] );
  digitalWrite(display_pin_4, segment_display[number][2] );
  digitalWrite(display_pin_5, segment_display[number][3] );
  digitalWrite(display_pin_6, segment_display[number][4] );
  digitalWrite(display_pin_7, segment_display[number][5] );
  digitalWrite(display_pin_8, segment_display[number][6] );
}
