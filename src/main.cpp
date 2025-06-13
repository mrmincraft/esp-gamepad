#include <Arduino.h>
#include <BleGamepad.h>

//Bluetooth declaration
BleGamepad bleGamepad("ESP Controller", "ElectroPoint4u", 100);

//joystick pins
uint8_t jx = 39;
uint8_t jy = 36;
//butons pins
uint8_t BT0 = 23;
uint8_t BT1 = 22;
uint8_t BT2 = 21;

#define numOfButtons 3
uint8_t previousButtonStates[numOfButtons];
uint8_t currentButtonStates[numOfButtons];
uint8_t buttonPins[numOfButtons] = {BT0, BT1, BT2};
uint8_t physicalButtons[numOfButtons] = {1, 2, 3};

uint8_t prosesValues(int value, bool reverse)
{
  if (value >= 2200)
  {
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1800)
  {
    value = map(value, 1800, 0, 127, 0);  
  }
  else
  {
    value = 127;
  }

  if (reverse)
  {
    value = 254 - value;
  }
  return value;
}

void setup() {
  Serial.begin(115200);
  pinMode(jx,INPUT);
  pinMode(jx,INPUT);
  for (uint8_t currentPinIndex = 0; currentPinIndex < numOfButtons; currentPinIndex++)
  {
    pinMode(buttonPins[currentPinIndex], INPUT_PULLUP);
    previousButtonStates[currentPinIndex] = HIGH;
    currentButtonStates[currentPinIndex] = HIGH;
  }
  
  BleGamepadConfiguration bleGamepadConfig;
  bleGamepadConfig.setAutoReport(false); // to disable auto reporting, and then use bleGamepad.sendReport(); as needed
  bleGamepadConfig.setButtonCount(numOfButtons);
  bleGamepad.begin(&bleGamepadConfig);
  // changing bleGamepadConfig after the begin function has no effect, unless you call the begin function again
}



void loop() {
  uint8_t y = 0 ;
  uint8_t x = 0 ;
  static uint8_t old_x = 0;
  static uint8_t old_y = 0;
  //making sure that the device is connected
  if (bleGamepad.isConnected()){
    for (uint8_t currentIndex = 0; currentIndex < numOfButtons; currentIndex++)
    {
      currentButtonStates[currentIndex] = digitalRead(buttonPins[currentIndex]);
      if (currentButtonStates[currentIndex] != previousButtonStates[currentIndex])
      {
        if (currentButtonStates[currentIndex] == LOW) 
        {
          bleGamepad.press(physicalButtons[currentIndex]);
        }else
        {
          bleGamepad.release(physicalButtons[currentIndex]);

        }
      }
    }
      if (memcmp((const void *)currentButtonStates, (const void *)previousButtonStates, sizeof(currentButtonStates)) != 0)
      {
        for (uint8_t currentIndex = 0; currentIndex < numOfButtons; currentIndex++)
        {
        previousButtonStates[currentIndex] = currentButtonStates[currentIndex];
        }
      }
    x = prosesValues(analogRead(jx),true);
    y = prosesValues(analogRead(jy),true);
    if (x != old_x || y != old_y)
    {
        old_x = x ;
        old_y = y ;
        Serial.printf("X: %d Y: %d \n",x,y);
        bleGamepad.setLeftThumb(x, y);
    }
    bleGamepad.sendReport();
    delay(10);
  }
}