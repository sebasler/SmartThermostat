#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include "menu.h"


extern int state;
extern float temp;
extern int displayShiftX;
extern float offset;
extern float tolerance;
extern int mode;
extern unsigned int fanMinutes;

extern Adafruit_SSD1306 display;
extern Menu currentMenu;
extern Menu mainMenu;
extern Menu fanMenu;
extern Menu offsetMenu;
extern Menu toleranceMenu;
extern Menu modeMenu;

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Menu Functions to be run on menu item click
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


// Change currentMenu based on the current selection
void updateCurrentMenu()
{
  switch(currentMenu.selectionIndex())
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


// Go back a menu. Sets currentMenu to mainMenu
void menuBack()
{
  currentMenu = mainMenu;
}

// Sets offset based on current selection
void setOffset()
{
  offset = strtof(currentMenu.selection().c_str(), NULL);
  Serial.println(offset);
  state = 0;
}


// Set tolerance based on current selection
void setTolerance()
{
  tolerance = strtof(currentMenu.selection().c_str(), NULL);
  Serial.println(tolerance);
  state = 0;
}


// Set mode based on current selection
void setMode()
{
  mode = currentMenu.selectionIndex();
  state = 0;
}


// Set fan to run for a certain amount of time based on selection
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