/*
Weight scale by Konstantinos Anastasakis https://kostislab.blogspot.gr/

 Setup your scale and start WITHOUT a weight on the scale
 Once readings are displayed place a known weight on the scale
 Αdjust the calibration_factor with the potentiometer in inverse proportion of the measurements until the output readings match the known weight

HX711 conection
 *The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
 Arduino pin D6 -> HX711 CLK
 Arduino pin D5 -> HX711 DOUT
 Arduino pin 5V -> HX711 VCC
 Arduino pin GND -> HX711 GND

Sensor conection
 Excitation+ (E+) or VCC is red
 Excitation- (E-) or ground is black.
 Output+ (O+), Signal+ (S+)+ or Amplifier+ (A+) is white
 O-, S-, or A- is green or blue
 more info hire: https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide

 LCD display conection
  I2C Address
  -----( Declare Constants )-----
  -----( Declare objects )-----
  https://arduino-info.wikispaces.com/LCD-Blue-I2C
  set the LCD address to 0x27 for a 16 chars 2 line display. A FEW use address 0x3F.
  ​Some of I2C LCD interfaces have pins (or solder pads) that can be changed to change the address.
  They are usually labelled as A0-A1-A2 . Here's how the address change from a default 0x27 or 0x3F,
  if you connect the address pads together. (1 = Not Connected. 0 = Connected):
  A0	A1	A2	HEX Address
  1	1	1	0x27
  0	1	1	0x26
  1	0	1	0x25
  0	0	1	0x24
  1	1	0	0x23
  0	1	0	0x22
  1	0	0	0x21
  0	0	0	0x20

 If no i2c adapter the connections will be:
  * LCD RS pin to digital pin 12
  * LCD Enable pin to digital pin 11
  * LCD D4 pin to digital pin 5
  * LCD D5 pin to digital pin 4
  * LCD D6 pin to digital pin 3
  * LCD D7 pin to digital pin 2
  * LCD R/W pin to ground
  * LCD VSS pin to ground
  * LCD VCC pin to 5V
  * 10K resistor:
  * ends to +5V and ground
  * wiper to LCD VO pin (pin 3)

Timer with DS3231 module
 Future plans is to put a timer with a beeper, so I don't burn the food...
 maybe a bluetooth and esp also... :)

Potentiometer conection
GND, A2, VCC

Calibration, Zero and Memory Button conection
D10,  D11,  D12 *All to GND, it will be LOW when pressed.

Soft power with ..
Not completed yet needs more work...
power output pin D? *keeps the board alive
read power state pin D?
*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  //https://bitbucket.org/fmalpartida/new-liquidcrystal/src
#include "HX711.h"  //https://github.com/bogde/HX711
#include "LowPower.h"
// #include <Sodaq_DS3231.h>  //RTC Library https://github.com/SodaqMoja/Sodaq_DS3231

// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // For i2c adapter v1
// LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // For i2c adapter v2

#define DOUT 7
#define CLK  6
HX711 scale(DOUT, CLK);

int potPin = 2;    // select the input pin for the potentiometer

float calibration_factor = 0;
float units;
float ounces;

int new_weight = 0; //
int old_weight = 0; //
int delta;  //
int maxnum = 10;     //
int count = 0;       //

const int calibButton = 10;
const int zeroButton = 11;
const int memoryButton = 12;

unsigned long time; // For the millis

// Functions
void calibration_code() {
  lcd.setCursor(0, 0);
  lcd.print("Turn the potenti");
  lcd.print(" ");
  lcd.print(" ");
  delay(1000);

  while(digitalRead(calibButton) == LOW) { //while loop "do something" if the button is presed and stop if the button is realeased
    lcd.setCursor(0, 0);
    lcd.print("Grams: ");
    lcd.print(scale.get_units(5), 1);
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("CalFactor:");
    lcd.print(calibration_factor);
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
  }

  lcd.setCursor(0, 1);
  lcd.print(" "); // just to clear the screen, any better ideas??
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
}

void zero_code() {
    lcd.setCursor(0, 1);
    lcd.print("Zeroing");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    scale.tare();
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print(" "); // just to clear the screen, any better ideas??
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
}

void memory_code() {
  delay(100);
  int memoryvalue = scale.read_average();
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("A=");
  lcd.print(memoryvalue);
  lcd.print(" ");
  lcd.print("Sum=");
  lcd.print(memoryvalue);
  lcd.print(" ");
  lcd.print(" ");
}
// End function

void setup() {
  // Serial.begin(9600);

  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  // pinMode(6, OUTPUT);
  // pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  // pinMode(10, INPUT_PULLUP);
  // pinMode(11, INPUT_PULLUP);
  // pinMode(12, INPUT_PULLUP);
  // pinMode(13, INPUT_PULLUP); //this powers the led that is connected on pin13
  pinMode(14, INPUT_PULLUP); //A0 *Analog pins
  pinMode(15, INPUT_PULLUP); //A1
  // pinMode(16, INPUT_PULLUP); //A2
  pinMode(17, INPUT_PULLUP); //A3
  pinMode(18, INPUT_PULLUP); //A4
  pinMode(19, INPUT_PULLUP); //A5
  pinMode(20, INPUT_PULLUP); //A6
  pinMode(21, INPUT_PULLUP); //A7

  pinMode(calibButton, INPUT_PULLUP);
  pinMode(zeroButton, INPUT_PULLUP);
  pinMode(memoryButton, INPUT_PULLUP);
  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed.

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  delay(100);
  scale.set_scale();
  delay(100);
  scale.tare();  //Reset the scale to 0
  lcd.clear();  //Clear the lcd

// // Serial print for debaging
//   Serial.println("HX711 calibration sketch");
//   Serial.println("Remove all weight from scale");
//   Serial.println("After readings begin, place known weight on scale");
//   Serial.println("Turn the potentiometer in inverse proportion of the measurements");
}

void loop() {
  new_weight = scale.read_average();
  delta = new_weight - old_weight;

  time = millis(); //Returns the number of milliseconds since the Arduino board began running the current program.
    // This number will overflow (go back to zero), after approximately 50 days.
  calibration_factor = analogRead(potPin); // read the value from the potentiometer
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  lcd.setCursor(0, 0);
  lcd.print("Grams: ");
  lcd.print(scale.get_units(5), 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");

  if (digitalRead(calibButton) == LOW) {calibration_code();}
  if (digitalRead(zeroButton) == LOW) {zero_code();}
  if (digitalRead(memoryButton) == LOW) {memory_code();}

  if (delta < 1000) {
    count++;
  } else {
    count = 0;
  }

  if (count > maxnum) {
    scale.power_down(); // put the ADC in sleep mode
    lcd.noDisplay();
    lcd.noBacklight(); // turn off backlight
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // put the board to sleep
    // scale.power_up();
  }

// // Serial print for debaging
//   Serial.print("calibration_factor: ");
//   Serial.println(calibration_factor);
//   Serial.print("Reading: ");
//   delay(100);
//   Serial.print(scale.get_units(5), 1);
//   Serial.println(" grams");
//   long zero_factor = scale.read_average(); //Get a baseline reading
//   Serial.print("Zero factor: "); //This can be used to remove the need to
//    //tare the scale. Useful in permanent scale projects.
//   Serial.println(zero_factor);
//   Serial.print("delta");
//   Serial.println(delta);
//   Serial.print("new_weight");
//   Serial.println(new_weight);
//   Serial.print("old_weight");
//   Serial.println(old_weight);
//   Serial.print("count");
//   Serial.println(count);
// //

  old_weight = new_weight; // Update the old readings
}
