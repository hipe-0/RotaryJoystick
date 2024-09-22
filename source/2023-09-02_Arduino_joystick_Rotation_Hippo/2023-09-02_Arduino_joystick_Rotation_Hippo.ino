// Simple gamepad example that demonstraits how to read five Arduino
// digital pins and map them to the Arduino Joystick library.
//
// The digital pins 2 - 6 are grounded when they are pressed.
// Pin 2 = UP
// Pin 3 = RIGHT
// Pin 4 = DOWN
// Pin 5 = LEFT
// Pin 6 = FIRE

// pin A5 = analog input

//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//

//--------------------------------------------------------------------

#include <Joystick.h>

const int ROTATION_PIN = 5;
const int ROTATION_BUTTON_LEFT=6;
const int ROTATION_BUTTON_RIGHT=7;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  8, 0,                  // Button Count, Hat Switch Count
  true, true, true,      // X, Y and Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering


// Last state of the buttons
int lastdirectionState[4] = {0,0,0,0};
int lastButtonState[6] = {0,0,0,0,0,0};
int lastRotationState;

const int TIMER_FOR_ROT_BUTTON_RESET = 10;
const int ANALOG_ROTATION_THRESHOLD = 1;
int resetCounter;

void setup() {

  // Initialize Button Pins
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  // Initialize Directional Pins
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);

  // Initialize Joystick Library
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
  Joystick.setZAxisRange(0, 11);

  lastRotationState = analogRead(ROTATION_PIN) * 12 / 1024;
  resetCounter = 0;
}


// Constant that maps the phyical pin to the joystick button.
const int pinToButtonMap = 7;
const int pinToDirectionMap = 3;

void loop() {
  buttonCommands();
  directionalCommands();
  rotationalCommands();
  delay(10);
}

void buttonCommands()
{
  // Read pin values for buttons
  for (int index = 0; index < 6; index++)
  {
    int currentButtonState = !digitalRead(index + pinToButtonMap);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }
}

void directionalCommands()
{
  // Read pin values for direction
  for (int index = 0; index < 4; index++)
  {
    int currentButtonState = !digitalRead(index + pinToDirectionMap);
    if (currentButtonState != lastdirectionState[index])
    {
      switch (index) {
        case 0: // UP
          if (currentButtonState == 1) {
            Joystick.setYAxis(-1);
          } else {
            Joystick.setYAxis(0);
          }
          break;
        case 1: // RIGHT
          if (currentButtonState == 1) {
            Joystick.setXAxis(1);
          } else {
            Joystick.setXAxis(0);
          }
          break;
        case 2: // DOWN
          if (currentButtonState == 1) {
            Joystick.setYAxis(1);
          } else {
            Joystick.setYAxis(0);
          }
          break;
        case 3: // LEFT
          if (currentButtonState == 1) {
            Joystick.setXAxis(-1);
          } else {
            Joystick.setXAxis(0);
          }
          break;
      }
      lastdirectionState[index] = currentButtonState;
    }
  }
}



void rotationalCommands()
{
  int rotationValue, analogValue;
  analogValue = analogRead(ROTATION_PIN);
  rotationValue = analogValue * 12 / 1024;  // read the input pin
  int differenceWithPrevious;
  differenceWithPrevious = rotationValue - lastRotationState;
  if (differenceWithPrevious >= 11) differenceWithPrevious-= 12;
  if (differenceWithPrevious <= -11) differenceWithPrevious+= 12;
  
  lastRotationState = rotationValue;

  Joystick.setZAxis(rotationValue);
  
  Serial.print(" input=");
  Serial.println(analogValue);
  

  if ( differenceWithPrevious <= -ANALOG_ROTATION_THRESHOLD) 
  {
     Joystick.setButton(ROTATION_BUTTON_LEFT, true);
     resetCounter = TIMER_FOR_ROT_BUTTON_RESET;
     Serial.println(" <LEFT ROT> ");
  Serial.print("                    difference");
  Serial.println(differenceWithPrevious);

  }

  if ( differenceWithPrevious >= ANALOG_ROTATION_THRESHOLD) 
  {
     Joystick.setButton(ROTATION_BUTTON_RIGHT, true);
     resetCounter = TIMER_FOR_ROT_BUTTON_RESET;
     Serial.println(" <RIGHT ROT> ");
  Serial.print("                    difference");
  Serial.println(differenceWithPrevious);

  }
  

  if(resetCounter > 0) {
    //Serial.println(" reset=");
    //Serial.println(resetCounter);

    resetCounter--;
    if(resetCounter == 0) {
      Serial.println(" reset ");

      Joystick.setButton(ROTATION_BUTTON_RIGHT, false);
      Joystick.setButton(ROTATION_BUTTON_LEFT, false);
    }
  }

}
