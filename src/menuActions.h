#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include "menu.h"


extern Adafruit_SSD1306 display;
extern int state;
extern float temp;
extern int displayShiftX;
extern float offset;
extern float tolerance;
extern int mode;
extern unsigned int fanMinutes;

extern Menu currentMenu;
extern Menu mainMenu;
extern Menu fanMenu;
extern Menu offsetMenu;
extern Menu toleranceMenu;
extern Menu modeMenu;


void updateCurrentMenu()
{
  int curIndex = currentMenu.selectionIndex();
  switch(curIndex)
  {
    case 0:
      currentMenu = fanMenu;
      break;
    case 1:
      currentMenu = offsetMenu;
      break;
    case 2:
      currentMenu = toleranceMenu;
      break;
    case 3:
      currentMenu = modeMenu;
      break;
    case 4:
      state = 0;
      break;
  }
  Serial.println("Switched Menu");

}

void menuBack()
{
  currentMenu = mainMenu;
}


void setOffset()
{
  offset = strtof(currentMenu.selection().c_str(), NULL);
  Serial.println(offset);
  state = 0;
}


void setTolerance()
{
  tolerance = strtof(currentMenu.selection().c_str(), NULL);
  Serial.println(tolerance);
  state = 0;
}


void setMode()
{
  mode = currentMenu.selectionIndex();
  state = 0;
}


void setFan()
{
  float sel = strtof(currentMenu.selection().c_str(), NULL);
  if((int)sel%5 != 0)
  {
    fanMinutes = sel*60;
  }
  else
  {
    fanMinutes = sel;
  }
  Serial.println(fanMinutes);
  state = 0;
  Serial.println((int)(sel));
}