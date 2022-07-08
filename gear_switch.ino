/*
  Chris' Landrover gear control system
*/

// Pins
const int up_button_pin = 2;     // the number of the pushbutton pin
const int down_button_pin = 3;     // the number of the pushbutton pin

const int turbo_state_pin = 4;
const int esc_state_pin = 5;

const int gear_control_1 =  6;      //
const int gear_control_2 =  7;      //
const int turbo_control =  8;      //
const int clutch_control =  10;      //

const int display_pin_1 = A7;
const int display_pin_2 = A6;
const int display_pin_3 = A5;
const int display_pin_4 = A4;
const int display_pin_5 = A3;
const int display_pin_6 = A2;
const int display_pin_7 = A1;
const int display_pin_8 = 13;

const int segment_display[10][7] = {    { 1, 1, 1, 1, 1, 1, 0 }, // 0
  { 0, 1, 1, 0, 0, 0, 0 }, // 1
  { 1, 1, 0, 1, 1, 0, 1 }, // 2
  { 1, 1, 1, 1, 0, 0, 1 }, // 3
  { 0, 1, 1, 0, 0, 1, 1 }, // 4
  { 1, 0, 1, 1, 0, 1, 1 }, // 5
  { 1, 0, 1, 1, 1, 1, 1 }, // 6
  { 1, 1, 1, 0, 0, 0, 0 }, // 7
  { 1, 1, 1, 1, 1, 1, 1 }, // 8
  { 1, 1, 1, 0, 0, 1, 1 }
};   // 9

//const int segment_pin_var[8] = {display_pin_1, display_pin_2, display_pin_3, display_pin_4, display_pin_5, display_pin_6, display_pin_7, display_pin_8};
const int segment_pin_var[8] = {A7, A6, A5, A4, A3, A2, A1, 13};

// variables
int buttonState = 0;         // variable for reading the pushbutton status
unsigned int last_interupt;

int set_gear = 1;
int current_gear = 1;

void setup() {

  Serial.begin(9600);
  // initialize the pins as an output:
  pinMode(gear_control_1, OUTPUT);
  pinMode(gear_control_2, OUTPUT);
  pinMode(turbo_control, OUTPUT);
  pinMode(clutch_control, OUTPUT);

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

  write_num(1);

  Serial.println(set_gear);
}

void loop() {

  if ( esc_state_pin ) { 

    //check if the turbo is turned on
    if (digitalRead(turbo_state_pin) == HIGH) {
      digitalWrite(turbo_control, HIGH);
    } else {
      digitalWrite(turbo_control, LOW);
    }

    // if the user presses a change gear button

    if (set_gear != current_gear) { //as opposed to doing a update system, check whether the gears are at the correct settings

      if (abs(set_gear - current_gear) > 1) {
        Serial.println("skipping gear");
        if (set_gear > current_gear) {
          Serial.println("skipping gear upwards");
        } else {
          Serial.println("skipping gear downwards");
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
      else if (set_gear == 4) {
        digitalWrite(gear_control_1, HIGH);
        digitalWrite(gear_control_2, HIGH);
        write_num(4);
        current_gear = 4;
      }
      
      //reenguage the clutch
      clutch_engage();
      Serial.print("Current gear is ");
      Serial.println(current_gear);
    }
  }
  delay(100); //how often the loop runs
}


//--------Functions ----------
//----------------------------

void gear_up() //function to detect gear change up
{
  if ((millis() - last_interupt) > 100) {
    //Update count
    detachInterrupt(digitalPinToInterrupt(up_button_pin));
    if (set_gear < 4) {
      set_gear++;
    }
    attachInterrupt(digitalPinToInterrupt(up_button_pin), gear_up, FALLING);
    last_interupt = millis();
    Serial.println("gear set to ");
    Serial.println(set_gear);
  }
}

void gear_down() //function to detect gear change down
{
  if ((millis() - last_interupt) > 100) {
    detachInterrupt(digitalPinToInterrupt(down_button_pin));
    if (set_gear > 1) {
      set_gear = set_gear - 1;
    }
    attachInterrupt(digitalPinToInterrupt(down_button_pin), gear_down, FALLING);
    last_interupt = millis();
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
  analogWrite(clutch_control, 255); //255 = 12V, 0 = 0V ... 191 approx = 9V
  delay(500); //delay to allow solonoid to close
}

// writes values to the sev seg pins
void write_num(int number)
{
  //for (int j=0; j < 7; j  ) {
  //digitalWrite(segment_pin_var[j], segment_display[number][j]);
  digitalWrite(display_pin_1, segment_display[number][0] );
  digitalWrite(display_pin_2, segment_display[number][1] );
  digitalWrite(display_pin_3, segment_display[number][2] );
  digitalWrite(display_pin_4, segment_display[number][3] );
  digitalWrite(display_pin_5, segment_display[number][4] );
  digitalWrite(display_pin_6, segment_display[number][5] );
  digitalWrite(display_pin_7, segment_display[number][6] );
  digitalWrite(display_pin_8, segment_display[number][7] );
  //}
}
