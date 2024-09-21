/* Rotary encoder game paddle
by Hippolyte Mounier
*/
#include <ezButton.h>
#include "Mouse.h"

#define CLK_PIN 3
#define CLK_PIN2 2
#define DT_PIN 4
#define SW_PIN 7


#define SPINNER_DEBOUNCE_TIME 1

volatile int           counter = 0;
volatile unsigned long last_time;  // for debouncing
volatile bool          lastWasFall;
int                    prev_counter;

ezButton ezbutton(SW_PIN);  // create ezButton object that attach to pin 4

void setup() {
  Serial.begin(9600);
  Mouse.begin();

  // configure encoder pins as inputs
  pinMode(CLK_PIN, INPUT);
  pinMode(CLK_PIN2, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(A0, INPUT);

  ezbutton.setDebounceTime(50);  // set debounce time to 50 milliseconds

  // use interrupt for CLK pin is enough
  // call ISR_encoderChange() when CLK pin changes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_encoderRise, RISING);
  attachInterrupt(digitalPinToInterrupt(CLK_PIN2), ISR_encoderFall, FALLING);
}

bool lastState;

void loop() 
{
  int diff;
  int sampledCounter;
  sampledCounter = counter;

  ezbutton.loop();  // MUST call the loop() function first

  if (prev_counter != sampledCounter) 
  {
    //DEBUGVAL("sampledCounter: ", sampledCounter)

    diff = (sampledCounter - prev_counter)*20;
    if(diff > 127) diff = 127;
    if(diff < -127) diff = -127;

    Mouse.move(diff, 0, 0);
    Serial.print("mouse move: ") ; Serial.println(diff);
  }

  bool b = !ezbutton.getState();

  if (b != lastState)
  {
    if (b)
    {
      Mouse.press(MOUSE_LEFT);
      Serial.println("button pressed");
    }
    else
    {
      Mouse.release(MOUSE_LEFT);
      Serial.println("button released");
    }
  }

  lastState = b;
  prev_counter = sampledCounter;

  delay(20);
}

void ISR_encoderRise() 
{
  if (!lastWasFall) return;

  int delta = millis() - last_time;
  if (delta < SPINNER_DEBOUNCE_TIME)
    return;
  last_time = millis();

  bool b = digitalRead(DT_PIN) == HIGH;
  if (b)     counter--;
  else       counter++;

  //DEBUGVAL2("rise dt", digitalRead(DT_PIN), "delta", delta)

  lastWasFall = false;
}


void ISR_encoderFall() 
{
  if (lastWasFall) return;

  int delta = millis() - last_time;
  if (delta < SPINNER_DEBOUNCE_TIME)
    return;
  last_time = millis();


  bool b = digitalRead(DT_PIN) == HIGH;
  if (b)     counter++;
  else       counter--;


  //DEBUGVAL2("    fall dt", digitalRead(DT_PIN), "delta", delta)

  lastWasFall = true;
}
