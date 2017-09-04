/*
Weight scale by Konstantinos Anastasakis //https://github.com/kon-anast/kan-weight_scale
My blog https://kostislab.blogspot.gr/

Read the README file

*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  //https://bitbucket.org/fmalpartida/new-liquidcrystal/src
#include "HX711.h"  //https://github.com/bogde/HX711
#include "LowPower.h" //http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
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

const int calibButton = 10; // add const if this should never change
const int zeroButton = 11;
int zeroState = 0;
const int memoryButton = 12;

// Voltage Reference pin
const int voltRefPin = A0;
float voltMult = 2.35; //multiplier
// Voltage divider resistors
int resistor1 = 10000; // R1 resistor connected to GND and AO
int resistor2 = 7500; // R2 connected to Vcc and A0

// Sleep
  int count = 0;
  // const int interval = 32000; // Interval is how long we wait until it goes to sleep
  // unsigned long previousMillis=0; // Tracks the time since last event fired
  // unsigned long currentMillis;

//----Functions----//

void sleep_code() {
  // Allow wake up pin to trigger interrupt on low.
  // attachInterrupt(0, sleep, LOW); //https://www.allaboutcircuits.com/technical-articles/using-interrupts-on-arduino/
  scale.power_down(); // put the ADC in sleep mode
  lcd.noDisplay();
  lcd.noBacklight(); // turn off backlight

    // for(int i=0;i<2;i++)
  LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);  // put the board to sleep

  if (digitalRead(calibButton) == HIGH && digitalRead(zeroButton) == HIGH &&
   digitalRead(memoryButton) == HIGH) {
     sleep_code();
   }
   else {
    setup();
   }
}

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
  int memoryvalue = scale.read_average();
  delay(10);
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

void vref_code() {

  //Variables for voltage divider
  float denominator;
	//Convert resistor values to division value
  //  voltage divider equation: R2 / (R1 + R2)
  denominator = (float)resistor2 / (resistor1 + resistor2);

  float voltage;
  //Obtain RAW voltage data
  voltage = analogRead(voltRefPin);
  //Convert to actual voltage (..* 0 - 5 Vdc)
  voltage = (voltage / 1024) * voltMult;
  //Convert to voltage before divider
  //  Divide by divider = multiply
  //  Divide by 1/5 = multiply by 5
  voltage = voltage / denominator;

  //Output to serial & LCD
  //  Serial.print("Volts: ");
  //  Serial.println(voltage);
  //  delay(500);  //Delay to make serial out readable

	lcd.setCursor(0, 1);
  lcd.print(voltage);
	lcd.print("v ");

} // void vref_code close

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

  // set up for batt voltage measurement
  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
  // ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  // delay(50);  // Let mux settle a little to get a more stable A/D conversion

	// vRef.begin();	// Start Voltage Reference

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.display();
  lcd.backlight();
  // lcd.clear();  //Clear the lcd

  count = 0;  //count the loops

  scale.power_up();
  delay(10);
  scale.set_scale();
  delay(10);
  scale.tare();  //Reset the scale to 0

// Serial print for debaging
//   Serial.println("HX711 calibration sketch");
//   Serial.println("Remove all weight from scale");
//   Serial.println("After readings begin, place known weight on scale");
//   Serial.println("Turn the potentiometer in inverse proportion of the measurements");
}

void loop() {
  new_weight = scale.read_average();
  delta = new_weight - old_weight;
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
  // lcd.setCursor(0, 1);

	vref_code();

  if (digitalRead(calibButton) == LOW) {calibration_code();}
  if (digitalRead(zeroButton) == LOW) {zero_code();}
  if (digitalRead(memoryButton) == LOW) {memory_code();}

// Send the scale to sleep based on how much time has passed
  // Get snapshot of time
  // unsigned long currentMillis = millis();
  // if ((unsigned long)(currentMillis - previousMillis) >= interval) {
  //    sleep_code(); // Go for sleep if it is more then interval, will be chaged if use Soft power system
  //    // Use the snapshot to set track time until next event
  //    previousMillis = currentMillis;
  // }
// Send the scale to sleep be counting the loops,
  // if (delta < 10000) {count++;} else {count = 0;}
  // if (count > maxnum) {sleep_code();}

// Serial print for debaging
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

  old_weight = new_weight; // Update the old readings
}
