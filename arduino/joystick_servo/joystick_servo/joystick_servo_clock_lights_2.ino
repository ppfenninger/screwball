/******************************************************************************
Screwball Arduino-Joystick-Servo Control 

Using Qwiic Joystiic Example 1 - Basic Readings
Nick Poole @ SparkX
Feb 6, 2018
https://github.com/sparkfunX/Qwiic_Joystiic

This sketch communicates with the Qwiic Joystiic breakout using I2C and 
reports the horizontal, vertical and button values at a rate of 1Hz using
the serial port.

If a connection is succesfully made to the Joystiic on startup, the device
will also return its currently configured I2C Slave Address. For new units, 
this address will be 0x20, any other value on a new unit may be symptomatic 
of communication error.

Connect SCL to A5, SDA to A4

******************************************************************************/

#include <Wire.h>
#include <Servo.h> 
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment(); 

#include <Adafruit_NeoPixel.h>
#define PIN      4
#define N_LEDS 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
 
Servo horizontalservo_j1; 
Servo verticalservo_j1; 
Servo horizontalservo_j2;  
Servo verticalservo_j2;  

byte JOY1_ADDR = 0x02; //joystick #2
byte JOY2_ADDR = 0x20; //joystick #3

int reading = 0;
int horizontalval_j2 = 0;
int verticalval_j2 = 0;
boolean buttonval_j2 = 0;

int horizontalval_j1 = 0;
int verticalval_j1 = 0;
boolean buttonval_j1 = 0;

int zero_horizontal_j1;
int zero_vertical_j1;
int prev_horizontal_j1 = 0;
int prev_vertical_j1 = 0;
int horizontal_write_j1 = 0;
int vertical_write_j1 = 0;

int zero_horizontal_j2;
int zero_vertical_j2;
int prev_horizontal_j2 = 0;
int prev_vertical_j2 = 0;
int horizontal_write_j2 = 0;
int vertical_write_j2 = 0;

unsigned long currentTime = millis();
unsigned long previousTimeJoystick = 0;    
unsigned long previousTimeClock = millis();    
unsigned long previousTimeLights = millis(); 
const long joystick_interval = 40; //DO NOT REDUCE THIS!!!!!!
const int lights_interval = 25;

int16_t counter, minute, counterMax;

bool j1_turn = 0;

const int leftPin = 4;
const int rightPin = 5;
const int outPin = 7;

int left_status = 0;
int right_status = 0;

int game_status = 0;
int game_on = 0;
int left_on = 0;
int right_on = 0;
int game_over = 0;
int game_won = 0;
int game_lost = 0;
int blink_on = 0;

void setup() {

  strip.begin();
  Wire.begin();        // join i2c bus
  Serial.begin(9600);  // start serial for output

  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);

  pinMode(outPin, OUTPUT);

  // Print the address currently in EEPROM on the Joystiic
  Serial.print("Slave Address in EEPROM: 0x");
  Serial.println(getAddr(JOY2_ADDR), HEX);

  Serial.print("Slave Address in EEPROM: 0x");
  Serial.println(getAddr(JOY1_ADDR), HEX);
  
  horizontalservo_j1.attach(10);  // attaches the servo on pin 9 to the servo object 
  verticalservo_j1.attach(11);  // attaches the servo on pin 9 to the servo object 

  horizontalservo_j2.attach(9);  // attaches the servo on pin 9 to the servo object 
  verticalservo_j2.attach(6);  // attaches the servo on pin 9 to the servo object 

  horizontalservo_j1.write(100);
  verticalservo_j1.write(84);

  horizontalservo_j2.write(100);
  verticalservo_j2.write(84);

  delay(40);
  zero_horizontal_j1 = getHorizontal(JOY1_ADDR);
  delay(40);
  zero_vertical_j1 = getVertical(JOY1_ADDR);
  delay(40);
  zero_horizontal_j2 = getHorizontal(JOY2_ADDR);
  delay(40);
  zero_vertical_j2 = getVertical(JOY2_ADDR);

  horizontal_write_j1 = zero_horizontal_j1;
  vertical_write_j1 = zero_vertical_j1;
  horizontal_write_j2 = zero_horizontal_j2;
  vertical_write_j2 = zero_vertical_j2;

  matrix.begin(0x70);

  matrix.setBrightness(8);

  counterMax = 30; //time in seconds
  counter = counterMax; 

  minute = counter/60;
  matrix.writeDigitNum(0, minute/10 % 10);
  matrix.writeDigitNum(1, minute % 10);
  matrix.drawColon(true);
  matrix.writeDigitNum(3, (counter  - minute*60)/10 % 10);
  matrix.writeDigitNum(4, (counter  - minute*60) % 10); 
  matrix.writeDisplay(); 

  for(uint16_t i=0; i<(strip.numPixels()); i++) {
      strip.setPixelColor(i  , strip.Color(0, 0, 0)); // Draw new pixel
  }
  strip.show();
  previousTimeLights = millis();
  

}

void loop() {
  currentTime = millis();

  left_status = digitalRead(leftPin);
  right_status = digitalRead(rightPin);

  if (game_on == 0){
    if ((left_status == HIGH) && (right_status == HIGH)){
      game_on = 1;
      left_on = 1;
      right_on = 1;
    }
    if ((currentTime - previousTimeLights) > 300){
      pregame_lights();
    }
  }

  if (game_on){
      //joystick 1
    if (currentTime - previousTimeClock >= joystick_interval){
      if (currentTime - previousTimeJoystick >= joystick_interval) {
        // save the last time we read from joysticks
        previousTimeJoystick = currentTime;

        if (j1_turn){
          if (left_on){
            runJ1();
          }
          j1_turn = 0;
        }
        else{
          if (right_on){
            runJ2();
          }
          j1_turn = 1;
        }
      }
      else{
        horizontalservo_j1.write(horizontal_write_j1);
        verticalservo_j1.write(vertical_write_j1);
        horizontalservo_j2.write(horizontal_write_j2);
        verticalservo_j2.write(vertical_write_j2);
      }
    }
  

  currentTime = millis();

  if (currentTime - previousTimeClock >= 1000){
    if (game_over == 0){
      if ((left_status == LOW) && (right_status == LOW)){
        game_over = 1;
        game_won = 1;
        victory_lights();
        game_on = 0;
      }
      else{
        if ((left_status == LOW)){
           left_on = 0;
             if (currentTime - previousTimeLights > 100){
                side_won_lights(0);
             }
        }
        else{
           if (currentTime - previousTimeLights > 100){
            side_not_won_lights(0);
           }
        }
         if ((right_status == LOW)){
           right_on = 0;
           if (currentTime - previousTimeLights > 100){
            side_won_lights(1);
           }
        }
        else{
           if (currentTime - previousTimeLights > 100){
            side_not_won_lights(1);
           }
        }
      }
      
      if (game_over == 0){
         counter --;
      }
    
      if (counter <= 0){
        game_over = 1; 
        digitalWrite(outPin, HIGH);
      }
    }

    if ((game_over == 1) && (left_status == LOW)){
        left_on = 0;
    }
    if ((game_over == 1) && (right_status == LOW)){
        right_on = 0;
    }
    if ((game_over == 1) && (left_status == LOW) && (right_status == LOW)){
        game_on = 0;
    }

    if (game_on == 0){
      if ((left_status == LOW) && (right_status == HIGH)){
        game_won = 1;
        game_lost = 0;
        victory_lights();
      }
      if ((left_status == HIGH) && (right_status == LOW)){
        game_lost = 1;
        game_won = 0;
      }
    }

   

    if (game_lost){
      if (currentTime - previousTimeLights > 500){
        if (blink_on){
          lost_lights_off();
          blink_on = 0;
        }
        else{
          lost_lights_on();
          blink_on = 1;
        }
      }
    }

    if (counter >= 600){
      minute = counter/60;
      matrix.writeDigitNum(0, minute/10 % 10);
      matrix.writeDigitNum(1, minute % 10);
      matrix.drawColon(true);
      matrix.writeDigitNum(3, (counter  - minute*60)/10 % 10);
      matrix.writeDigitNum(4, (counter  - minute*60) % 10); 
      matrix.writeDisplay();  
    }

    else if (counter >= 60){
      matrix.writeDigitRaw(0, 0); 
      minute = counter/60;
      matrix.writeDigitNum(1, minute % 10);
      matrix.drawColon(true);
      matrix.writeDigitNum(3, (counter  - minute*60)/10 % 10);
      matrix.writeDigitNum(4, (counter  - minute*60) % 10);
      matrix.writeDigitRaw(0, 0); 
      matrix.writeDisplay();
    }

    
    else if (counter >=0){
      matrix.writeDigitRaw(0, 0);
      matrix.writeDigitRaw(1, 0);
      matrix.drawColon(true); 
      matrix.writeDigitNum(3, (counter/10) % 10);
      matrix.writeDigitNum(4, (counter % 10));
      matrix.writeDisplay(); 
   }  
    
   previousTimeClock = currentTime; 
   if (game_over){
     digitalWrite(outPin,HIGH);
   }
   else{
     digitalWrite(outPin,LOW);
   }
  }
}
}


// Run the joystick reading and servo writing for joystick 1
void runJ1() {
  horizontalval_j1 = getHorizontal(JOY1_ADDR);
  verticalval_j1 = getVertical(JOY1_ADDR);
  buttonval_j1 = getButton(JOY1_ADDR);

   if (abs(verticalval_j1 - prev_vertical_j1) < 2){
    verticalval_j1 = prev_vertical_j1;
  }

  if (abs(horizontalval_j1 - prev_horizontal_j1) < 2){
    horizontalval_j1 = prev_horizontal_j1;
  }


  if ((horizontalval_j1 < 1024) & (horizontalval_j1 > -1)){
    horizontal_write_j1 = int(map(horizontalval_j1, -1, 1024, 70, 105));
    horizontalservo_j1.write(horizontal_write_j1);
  }
  else{
    Serial.println("HORIZONTAL OUT OF RANGE");
  }

  if ((verticalval_j1 < 1024) & (verticalval_j1 > -1)){
    vertical_write_j1 = int(map(verticalval_j1, -1, 1024, 85, 125));
    verticalservo_j1.write(vertical_write_j1);
  }
  else{
    Serial.println("VERTICAL OUT OF RANGE");
  }

  if ((verticalval_j1 != prev_vertical_j1) | (prev_horizontal_j1 != horizontalval_j1)){
    Serial.print("H1: ");
    Serial.println(horizontalval_j1);
    Serial.print("V1: ");
    Serial.println(verticalval_j1);
    Serial.print("B1: ");
    Serial.println(buttonval_j1);
    Serial.println();
  }

  prev_horizontal_j1 = horizontalval_j1;
  prev_vertical_j1 = verticalval_j1;
}


// Runs the joystick reading and servo writing for joystick 2
void runJ2() {
  horizontalval_j2 = getHorizontal(JOY2_ADDR);
  verticalval_j2 = getVertical(JOY2_ADDR);
  buttonval_j2 = getButton(JOY2_ADDR);

  if (abs(verticalval_j2 - prev_vertical_j2) < 2){
    verticalval_j2 = prev_vertical_j2;
  }

  if (abs(horizontalval_j2 - prev_horizontal_j2) < 2){
    horizontalval_j2 = prev_horizontal_j2;
  }

    
  if ((horizontalval_j2 < 1024) & (horizontalval_j2 > -1)){
    horizontal_write_j2 = int(map(horizontalval_j2, -1, 1024, 70, 105));
    horizontalservo_j2.write(horizontal_write_j2);
  }
  else{
    Serial.println("HORIZONTAL OUT OF RANGEL");
  }

  if ((verticalval_j2 < 1024) & (verticalval_j2 > -1)){
    vertical_write_j2 = int(map(verticalval_j2, -1, 1024, 85, 125));
    verticalservo_j2.write(vertical_write_j2);
  }
  else{
    Serial.println("VERTICAL OUT OF RANGE");
  } 

  if ((verticalval_j2 != prev_vertical_j2) | (prev_horizontal_j2 != horizontalval_j2)){
  Serial.print("H2: ");
  Serial.println(horizontalval_j2);
  Serial.print("V2: ");
  Serial.println(verticalval_j2);
  Serial.print("B2: ");
  Serial.println(buttonval_j2);
  Serial.println();
  }
    prev_vertical_j2 = verticalval_j2;
    prev_horizontal_j2 = horizontalval_j2;
}


// getHorizontal() returns an int between 0 and 1023 
// representing the Joystiic's horizontal position
// (axis indicated by silkscreen on the board)
// centered roughly on 512

int getHorizontal(byte ADDR) {

  Wire.beginTransmission(ADDR);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(ADDR, 2);

  while (Wire.available()) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    reading = (uint16_t)msb << 8 | lsb;
  }

  Wire.endTransmission();
  return reading;

}

// getVertical() returns an int between 0 and 1023 
// representing the Joystiic's vertical position
// (axis indicated by silkscreen on the board)
// centered roughly on 512

int getVertical(byte ADDR) {

  Wire.beginTransmission(ADDR);
  Wire.write(0x02);
  Wire.endTransmission(false);
  Wire.requestFrom(ADDR, 2);

  while (Wire.available()) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    reading = (uint16_t)msb << 8 | lsb;
  }

  Wire.endTransmission();
  return reading;

}

// getButton() returns a byte indicating the 
// position of the button where a 0b1 is not
// pressed and 0b0 is pressed

byte getButton(byte ADDR) {

  byte btnbyte = 0b00000000;

  Wire.beginTransmission(ADDR);
  Wire.write(0x04);
  Wire.endTransmission(false);
  Wire.requestFrom(ADDR, 1);    // request 1 byte

  while (Wire.available()) {
    btnbyte = Wire.read();
  }

  Wire.endTransmission();
  return btnbyte;

}

// getAddr() returns a byte indicating the 
// 7-bit Slave Address that the Joystiic 
// currently has stored in EEPROM. Because
// address changes do not take effect until
// power reset, the value returned from 
// getAddr() may not match the address that 
// the device is currently using. As a 
// result, this call can be used to confirm
// that a setAddr() call was successful.

byte getAddr(byte ADDR) {

  byte addr = 0x00;

  Wire.beginTransmission(ADDR);
  Wire.write(0x06);
  Wire.endTransmission(false);
  Wire.requestFrom(ADDR, 1);    // request 1 byte

  while (Wire.available()) {
    addr = Wire.read();
  }

  Wire.endTransmission();
  return addr;

}

// setAddr() takes a 7-bit I2C Slave Address 
// and passes it to the Joystiic. The Joystiic
// will store this address in EEPROM and use
// it on power reset. If a reserved address is
// passed to this function, it will send it to
// the Joystiic, but the Joystiic will not set
// store it in EEPROM. 
// getAddr() can be used to confirm
// that a setAddr() call was successful.

void setAddr(byte addr) {

  Wire.beginTransmission(0x02);
  Wire.write(0x05);
  Wire.write(addr);
  Wire.endTransmission();

}

static void victory_lights() {
  for(uint16_t i=0; i<5; i++){
  for(uint16_t i=0; i<strip.numPixels()+54; i++) {
      strip.setPixelColor(i  , strip.Color(255, 255, 255)); // Draw new pixel
      strip.setPixelColor(i-2  , strip.Color(255, 0, 0));
      strip.setPixelColor(i-4, strip.Color(0, 0, 255));

      strip.setPixelColor(i-10, strip.Color(255, 255, 255)); // Draw new pixel
      strip.setPixelColor(i-12  , strip.Color(255, 0, 0));
      strip.setPixelColor(i-14, strip.Color(0, 0, 255));

      strip.setPixelColor(i-20 , strip.Color(255, 255, 255)); // Draw new pixel
      strip.setPixelColor(i-22  , strip.Color(255, 0, 0));
      strip.setPixelColor(i-24, strip.Color(0, 0, 255));

      strip.setPixelColor(i-30 , strip.Color(255, 255, 255)); // Draw new pixel
      strip.setPixelColor(i-32  , strip.Color(255, 0, 0));
      strip.setPixelColor(i-34, strip.Color(0, 0, 255));

      strip.setPixelColor(i-40 , strip.Color(255, 255, 255)); // Draw new pixel
      strip.setPixelColor(i-42  , strip.Color(255, 0, 0));
      strip.setPixelColor(i-44, strip.Color(0, 0, 255));

      strip.setPixelColor(i-50 , strip.Color(255, 255, 255)); // Draw new pixel
      strip.setPixelColor(i-52  , strip.Color(255, 0, 0));
      strip.setPixelColor(i-54, strip.Color(0, 0, 255));
      
      strip.show();
      delay(25);
  }
}
for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , strip.Color(0, 0, 0)); // Draw new pixel
  }
  strip.show();
}

static void lost_lights_on() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , strip.Color(255, 0, 0)); // Draw new pixel
  }
  strip.show();
  previousTimeLights = millis();
}

static void lost_lights_off() {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , strip.Color(0, 0, 0)); // Draw new pixel
  }
    strip.show();
    previousTimeLights = millis();
}

static void side_won_lights(int side){
  if (side == 0){
    for(uint16_t i=0; i<(strip.numPixels()/2); i++) {
      strip.setPixelColor(i  , strip.Color(0, 255, 0)); // Draw new pixel
  }
  }
  else{
    for(uint16_t i=(strip.numPixels()/2); i<(strip.numPixels()); i++) {
      strip.setPixelColor(i  , strip.Color(0, 255, 0)); // Draw new pixel
  }
  }
  strip.show();
  previousTimeLights = millis();
}


static void side_not_won_lights(int side){
  if (side == 0){
    for(uint16_t i=0; i<(strip.numPixels()/2); i++) {
      strip.setPixelColor(i  , strip.Color(255, 0, 0)); // Draw new pixel
  }
  }
  else{
    for(uint16_t i=(strip.numPixels()/2); i<(strip.numPixels()); i++) {
      strip.setPixelColor(i  , strip.Color(255, 0, 0)); // Draw new pixel
  }
  }
  strip.show();
  previousTimeLights = millis();
}

static void pregame_lights(){
  if (blink_on){
    for(uint16_t i=0; i<(strip.numPixels()); i++) {
      strip.setPixelColor(i  , strip.Color(0, 0, 0)); // Draw new pixel
    }
    blink_on = 0;
  }
  else{
    for(uint16_t i=0; i<(strip.numPixels()); i++) {
     strip.setPixelColor(i  , strip.Color(0, 0, 255));
    }
    blink_on = 1;
  }
  strip.show();
  previousTimeLights = millis();
}




/*
 *   red = strip.Color(255, 0, 0);
  green = strip.Color(0, 255, 0);
  blue = strip.Color(0, 0, 255);
 */
