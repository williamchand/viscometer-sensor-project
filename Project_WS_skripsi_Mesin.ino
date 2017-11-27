#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_Buttons.h>
#include <AccelStepper.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>
//HX711
#define CLK 7
#define DOUT 8
// Data wire is plugged into port 9 on the Arduino
#define ONE_WIRE_BUS 9
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
int x, y;
//calibrator
float calibration_factor = -96650;
//==== Define variable
int encoder_pin = 18; // pulse output from the module
volatile byte pulses = 0; // number of pulses
unsigned long timeold = 0;
// number of pulses per revolution
// based on your encoder disc
unsigned int pulsesperturn = 100;
float temperature = 0.0;
float velocity = 0.0;
float velocityactual = 0.0;
float force = 0.0;
float viscosity = 0.0;
int condition = 0;
int propeller = 1;
int motorspeed = 0;
//bufferprintLCD
char bufferS70[32] = "";
char value[10] = "";
// Setup oneWire Dallas Temperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer, outsideThermometer;
HX711 scale(DOUT, CLK);
//==== Creating Objects of the LCD touch
UTFT myGLCD(SSD1289, 38, 39, 40, 41); //Parameters should be adjusted to your Display/Shield model
URTouch myTouch( 6, 5, 4, 3, 2);
UTFT_Buttons  myButtons(&myGLCD, &myTouch);
// Define a stepper motor 1 for arduino
// direction Digital 10 (CW), pulses Digital 11 (CLK)
AccelStepper myStepper(1, 10, 11);
void setup() {
  // Initial setup
  Serial.begin(9600);
  LCDSetup();
  senseSetup();
}
// ====== Main setup ======
//setup our LCD
void LCDSetup() {
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  drawHomeScreen();  // Draws the Home Screen
  drawtime();
}
void senseSetup() {
  pinMode(encoder_pin, INPUT);
  //Interrupt 5 is digital pin 18
  //Triggers on Falling Edge (change from HIGH to LOW)
  attachInterrupt(5, counter, FALLING);
  sensors.begin();
  scale.set_scale(calibration_factor);
  scale.tare();
  // Change these to suit your stepper if you want
  myStepper.setMaxSpeed(1000);//1100
}
void counter()
{
  //Update count
  pulses++;
}
void loop() {
  if (condition == 1) {
    speedsense();
    tempload();
    motorservice();
    viscosityeq();
  }
  clicktft();
  drawtime();  // Draws the TIME
}
// ====== Custom Funtions ======

void speedsense() {
  if (millis() - timeold >= 1000) {
    //Don't process interrupts during calculations
    detachInterrupt(5);
    velocityactual = (60 * 1000 / pulsesperturn ) / (millis() - timeold) * pulses;
    timeold = millis();
    pulses = 0;
    //Restart the interrupt processing
    attachInterrupt(5, counter, FALLING);
  }
}

void viscosityeq() {
  switch (propeller) {
    case 1:

      break;
    case 2:

      break;
    case 3:

      break;
    case 4:

      break;
    default:
      break;
  }
}
void motorservice() {
  if (velocity > 0) {
    if ((velocityactual > velocity) && (motorspeed > velocity - 30)) {
      motorspeed--;
    } else if ((velocityactual < velocity) && (motorspeed < velocity + 30)) {
      motorspeed++;
    }
    myStepper.setSpeed(motorspeed);
    // step 1/100 of a revolution:
    myStepper.runSpeed();
  }
}
// get temperature information
void tempload() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  temperature = sensors.getTempCByIndex(0);
  force = 9.78 * scale.get_units();
}
// drawing our home screen
void drawHomeScreen() {
  myGLCD.fillScr(VGA_WHITE);
  myGLCD.setColor(VGA_YELLOW); // Sets color to green
  myGLCD.drawLine(0, 160, 319, 160); // Draws the green line
  myGLCD.drawLine(0, 195, 319, 195); // Draws the green line
  myGLCD.drawLine(107, 160, 107, 239);
  myGLCD.drawLine(213, 160, 213, 239);
  myGLCD.fillRoundRect (4, 200, 103, 235);
  myGLCD.fillRoundRect (112, 200, 158, 235);
  myGLCD.fillRoundRect (162, 200, 208, 235);
  myGLCD.fillRoundRect (218, 200, 264, 235);
  myGLCD.fillRoundRect (268, 200, 314, 235);
  myGLCD.setColor(VGA_RED);
  myGLCD.drawRoundRect (4, 200, 103, 235);
  myGLCD.drawRoundRect (112, 200, 158, 235);
  myGLCD.drawRoundRect (162, 200, 208, 235);
  myGLCD.drawRoundRect (218, 200, 264, 235);
  myGLCD.drawRoundRect (268, 200, 314, 235);
  myGLCD.setBackColor(VGA_WHITE);
  myGLCD.setColor(VGA_BLUE); // Sets color to redorange
  myGLCD.setFont(SmallFont); // Sets font to small
  myGLCD.print("Set Condition", 3, 170); // Prints the string on the screen
  myGLCD.print("Set Speed", 125, 170); // Prints the string on the screen
  dtostrf(velocity, 8, 2, value);
  sprintf(bufferS70, "Velocity = %s rpm", value);
  myGLCD.print(bufferS70, 4, 125 ); // Prints the string on the screen
  myGLCD.print("Set Propeller", 215, 170); // Prints the string on the screen
  sprintf(bufferS70, "Propeller %d", propeller);
  myGLCD.print(bufferS70, RIGHT, 125); // Prints the string on the screen
  myGLCD.setBackColor(VGA_YELLOW);
  myGLCD.setFont(BigFont);
  myGLCD.print("OFF", 30, 210);
  myGLCD.print("<", 128, 210);
  myGLCD.print(">", 178, 210);
  myGLCD.print("<", 234, 210);
  myGLCD.print(">", 284, 210);
}
//draw time
void drawtime() {
  // Title
  myGLCD.setBackColor(VGA_WHITE);
  myGLCD.setColor(VGA_BLUE); // Sets color to redorange
  myGLCD.setFont(SmallFont); // Sets font to small
  dtostrf(temperature, 8, 2, value);
  sprintf(bufferS70, "Temperature = %s C", value);
  myGLCD.print(bufferS70, 4, 5); // Prints the string on the screen
  dtostrf(force, 8, 2, value);
  sprintf(bufferS70, "Force = %s N", value);
  myGLCD.print(bufferS70, 4, 35); // Prints the string on the screen
  dtostrf(viscosity, 8, 2, value);
  sprintf(bufferS70, "Viscosity = %s Pa.s", value);
  myGLCD.print(bufferS70, 4, 65); // Prints the string on the screen
  dtostrf(velocityactual, 8, 2, value);
  sprintf(bufferS70, "Velocity Actual = %s rpm", value);
  myGLCD.print(bufferS70, 4, 95); // Prints the string on the screen
}

// function algorithm check
void clicktft() {
  if (myTouch.dataAvailable()) {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY();
    // If we press the UpSpeed  Button
    if ((x >= 162) && (x <= 208) && (y >= 200) && (y <= 235)) {
      if (velocity <= 580) {
        velocity += 20;
        myGLCD.setBackColor(VGA_WHITE);
        myGLCD.setColor(VGA_BLUE); // Sets color to redorange
        myGLCD.setFont(SmallFont); // Sets font to small
        dtostrf(velocity, 8, 2, value);
        sprintf(bufferS70, "Velocity = %s rpm", value);
        myGLCD.print(bufferS70, 4, 125 ); // Prints the string on the screen
      }
    }
    // If we press the DownSpeed Button
    if ((x >= 112) && (x <= 158) && (y >= 200) && (y <= 235)) {
      if (velocity >= 20) {
        velocity -= 20;
        myGLCD.setBackColor(VGA_WHITE);
        myGLCD.setColor(VGA_BLUE); // Sets color to redorange
        myGLCD.setFont(SmallFont); // Sets font to small
        dtostrf(velocity, 8, 2, value);
        sprintf(bufferS70, "Velocity = %s rpm", value);
        myGLCD.print(bufferS70, 4, 125 ); // Prints the string on the screen
      }
    }
    // If we press the UpProppeler Button
    if ((x >= 268) && (x <= 314) && (y >= 200) && (y <= 235)) {
      if (propeller < 4) {
        propeller++;
        myGLCD.setBackColor(VGA_WHITE);
        myGLCD.setColor(VGA_BLUE); // Sets color to redorange
        myGLCD.setFont(SmallFont); // Sets font to small
        sprintf(bufferS70, "Propeller %d", propeller);
        myGLCD.print(bufferS70, RIGHT, 125); // Prints the string on the screen
      }
    }
    // If we press the DownProppeler Button
    if ((x >= 218) && (x <= 264) && (y >= 200) && (y <= 235)) {
      if (propeller > 1) {
        propeller--;
        myGLCD.setBackColor(VGA_WHITE);
        myGLCD.setColor(VGA_BLUE); // Sets color to redorange
        myGLCD.setFont(SmallFont); // Sets font to small
        sprintf(bufferS70, "Propeller %d", propeller);
        myGLCD.print(bufferS70, RIGHT, 125); // Prints the string on the screen
      }
    }
    // Condition
    if ((x >= 4) && (x <= 103) && (y >= 200) && (y <= 235)) {
      myGLCD.setColor(VGA_BLUE); // Sets color to redorange
      myGLCD.setBackColor(VGA_YELLOW);
      myGLCD.setFont(BigFont);
      if (condition == 0) {
        condition = 1;
        motorspeed = (int) velocity;
        myGLCD.print(" ON ", 20, 210);
      } else {
        motorspeed = velocity;
        velocityactual = 0.0;
        temperature = 0.0;
        force = 0.0;
        viscosity = 0.0;
        condition = 0;
        myGLCD.print("OFF", 30, 210);
      }
    }
  }
}
