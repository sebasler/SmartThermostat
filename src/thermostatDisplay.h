#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_MCP9808.h>
#include "definitions.h"


// All the Icons! I'm great at photoshop(apparently)
static const unsigned char PROGMEM fan_0[] =
{
  0x01, 0xdc, 0x00, 0x02, 0x02, 0x00, 0x06, 0x01, 0x00, 0x04, 0x01, 0x00, 0x04, 0x01, 0x00, 0x04, 
  0x06, 0x00, 0x02, 0x1c, 0x00, 0x01, 0x20, 0x00, 0x60, 0xf3, 0xc0, 0x90, 0x9e, 0x60, 0x88, 0x90, 
  0x20, 0x86, 0xf0, 0x10, 0x81, 0xf8, 0x10, 0x80, 0x48, 0x10, 0x80, 0x48, 0x10, 0x80, 0xd8, 0x10, 
  0x80, 0x98, 0x30, 0xc1, 0x98, 0x20, 0x73, 0x08, 0xe0, 0x1e, 0x0f, 0xc0
};
static const unsigned char PROGMEM fan_1[] =
{
  0x00, 0x1f, 0x80, 0x00, 0x60, 0xe0, 0x00, 0xc0, 0x30, 0x00, 0x80, 0x30, 0x38, 0x80, 0x10, 0x44, 
  0xc0, 0x10, 0x86, 0x40, 0x10, 0x82, 0x4f, 0xf0, 0x82, 0xf9, 0xc0, 0x01, 0x98, 0x00, 0x80, 0x9f, 
  0x00, 0x80, 0xf9, 0xc0, 0x81, 0x08, 0x60, 0x62, 0x10, 0x30, 0x3c, 0x10, 0x10, 0x00, 0x20, 0x10, 
  0x00, 0x40, 0x30, 0x00, 0x80, 0x20, 0x00, 0x80, 0x60, 0x00, 0x7f, 0xc0
};
static const unsigned char PROGMEM fan_2[] =
{
  0x3f, 0x07, 0x80, 0x71, 0x0c, 0xe0, 0x41, 0x98, 0x30, 0xc1, 0x90, 0x10, 0x81, 0xb0, 0x10, 0x81, 
  0x20, 0x10, 0x81, 0x20, 0x10, 0x81, 0xf8, 0x10, 0x80, 0xf6, 0x10, 0x40, 0x91, 0x10, 0x67, 0x90, 
  0x90, 0x3c, 0xf0, 0x60, 0x00, 0x48, 0x00, 0x03, 0x84, 0x00, 0x06, 0x02, 0x00, 0x08, 0x02, 0x00, 
  0x08, 0x02, 0x00, 0x08, 0x06, 0x00, 0x04, 0x04, 0x00, 0x03, 0xb8, 0x00
};
static const unsigned char PROGMEM fan_3[] =
{
  0x3f, 0xe0, 0x00, 0x60, 0x10, 0x00, 0x40, 0x10, 0x00, 0xc0, 0x20, 0x00, 0x80, 0x40, 0x00, 0x80, 
  0x83, 0xc0, 0xc0, 0x84, 0x60, 0x61, 0x08, 0x10, 0x39, 0xf0, 0x10, 0x0f, 0x90, 0x10, 0x01, 0x98, 
  0x00, 0x39, 0xf4, 0x10, 0xff, 0x24, 0x10, 0x80, 0x26, 0x10, 0x80, 0x32, 0x20, 0x80, 0x11, 0xc0, 
  0xc0, 0x10, 0x00, 0xc0, 0x30, 0x00, 0x70, 0x60, 0x00, 0x1f, 0x80, 0x00
};
static const unsigned char PROGMEM cool_2[] =
{
  0x0c, 0x63, 0x00, 0x06, 0x66, 0x00, 0x03, 0x6c, 0x00, 0x01, 0xf8, 0x00, 0x60, 0xf0, 0x60, 0x38, 
  0x61, 0xc0, 0x0c, 0x63, 0x00, 0xc6, 0x66, 0x30, 0x67, 0xfe, 0x60, 0x3f, 0xff, 0xc0, 0x01, 0xf8, 
  0x00, 0x3f, 0xff, 0xc0, 0x66, 0x66, 0x60, 0xcc, 0x63, 0x30, 0x18, 0x61, 0x90, 0x30, 0xf0, 0xc0, 
  0x61, 0xf8, 0x60, 0x03, 0x6c, 0x00, 0x06, 0x66, 0x00, 0x00, 0x63, 0x00
};
static const unsigned char PROGMEM cool_1[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x06, 0x56, 0x00, 0x03, 
  0xdc, 0x00, 0x01, 0xd8, 0x00, 0x10, 0xf0, 0x80, 0x18, 0xf1, 0x80, 0x3f, 0xff, 0xc0, 0x3f, 0xff, 
  0xc0, 0x18, 0xf1, 0x80, 0x30, 0xf0, 0xc0, 0x01, 0xd8, 0x00, 0x03, 0xdc, 0x00, 0x06, 0x56, 0x00, 
  0x04, 0x52, 0x00, 0x00, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char PROGMEM cool_0[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xd0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0xf0, 0x00, 0x03, 0xfc, 0x00, 0x03, 0xfc, 
  0x00, 0x00, 0xf0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char PROGMEM heat_0[] =
{
  0x00, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x71, 0xc0, 0x00, 0xc3, 0x00, 0x01, 0x86, 0x00, 0x1b, 
  0x06, 0x00, 0x3a, 0x06, 0x00, 0x7e, 0x06, 0x60, 0x4c, 0x02, 0xe0, 0xcc, 0x03, 0xe0, 0x82, 0x60, 
  0x60, 0x80, 0x60, 0x60, 0x80, 0x70, 0x20, 0x80, 0x58, 0x30, 0xc1, 0x48, 0x30, 0xc3, 0xcc, 0x30, 
  0x62, 0x0c, 0x60, 0x32, 0x0c, 0xc0, 0x1f, 0x0f, 0x80, 0x03, 0x3e, 0x00
};
static const unsigned char PROGMEM heat_1[] =
{
  0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x38, 0xe0, 0x00, 0x0c, 0x30, 0x00, 0x06, 0x18, 0x00, 0x06, 
  0x0d, 0x80, 0x06, 0x05, 0xc0, 0x66, 0x07, 0xe0, 0x74, 0x03, 0x20, 0x7c, 0x03, 0x30, 0x60, 0x64, 
  0x10, 0x60, 0x60, 0x10, 0x40, 0xe0, 0x10, 0xc1, 0xa0, 0x10, 0xc1, 0x28, 0x30, 0xc3, 0x3c, 0x30, 
  0x63, 0x04, 0x60, 0x33, 0x04, 0xc0, 0x1f, 0x07, 0x80, 0x07, 0xcc, 0x00
};

static const unsigned char PROGMEM noInternet[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x70, 
  0x00, 0x1f, 0xe0, 0xe0, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0x80, 0x0f, 0xe0, 0x1f, 0xc0, 
  0x1f, 0x00, 0x0f, 0xe0, 0x3c, 0x03, 0x9c, 0xf0, 0x78, 0x3f, 0xf8, 0x78, 0x60, 0xff, 0xfc, 0x18, 
  0x03, 0xf9, 0xff, 0x00, 0x07, 0xc1, 0xcf, 0x80, 0x07, 0x03, 0x83, 0x80, 0x06, 0x0f, 0xc1, 0x80, 
  0x00, 0x3f, 0xf0, 0x00, 0x00, 0xff, 0xf8, 0x00, 0x00, 0xf8, 0x3c, 0x00, 0x00, 0xf0, 0x18, 0x00, 
  0x00, 0xe7, 0x80, 0x00, 0x01, 0xcf, 0xc0, 0x00, 0x03, 0x8f, 0xc0, 0x00, 0x07, 0x0f, 0xc0, 0x00, 
  0x0e, 0x07, 0x80, 0x00, 0x1c, 0x07, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


extern volatile int count;
extern int targetTemp;
extern bool pendingTempUpdate;
extern bool heatON;
extern bool coolON;
extern bool fanON;
extern bool connected;
extern int state;
extern float temp;
extern volatile unsigned long lastInput;

extern Adafruit_SSD1306 display;
extern Menu mainMenu;
extern Menu offsetMenu;
extern Menu toleranceMenu;
extern Menu modeMenu;
extern Menu currentMenu;

void showGUI();
void displayCool(bool nextFrame);
void displayHeat(bool nextFrame);
void displayFan(bool nextFrame);
void displayFanMinutes();
void displayIcons();
void displayIcons();
void displayHome();

// Control the display & menu
void showGUI()
{
  if(state != 0 && millis()-lastInput > 10000)                  //If in a menu and no input has been detected for 10s go back to home screen
  {
    state = 0;
    mainMenu.reset();
    offsetMenu.reset();
    toleranceMenu.reset();
    modeMenu.reset();
  }
  if (!state)                                                   //Check if on the home screen (state = 0)
  {
    if(count > 0)
    {
      if(targetTemp < MAX_TEMP)
      {
        targetTemp++;
        pendingTempUpdate = true;
      }
      count--;
    }
    else if(count < 0)
    {
      if(targetTemp > MIN_TEMP)
      {
        targetTemp--;
        pendingTempUpdate = true;
      }
        count++;
      }
    displayHome();
  }
  else                                                        //If not update menu items accordingly
  {
    if(count > 0)
    {
      currentMenu.moveDown();
      count--;
    }
    else if(count < 0)
    {
      currentMenu.moveUp();
      count++;
    }
    currentMenu.displayMenu();
  }
  display.display();
}


// Displays cool icon. If nextFrame==true then advances one frame
void displayCool(bool nextFrame)
{
  //Displays cool icon. If animate==true then advance to next frame
  static int frame = 0;

  //if in heat mode do not show snowflake icon
  if(mode == 1)
  {
    return;
  }
  
  if(nextFrame)
  {
    frame=(frame+1)%3;
    return;
  }

  if(coolON)
  {
    switch(frame)
    {
      case 0:
        display.drawBitmap(90+displayShiftX, 22, cool_0, ICON_WIDTH, ICON_HEIGHT, 1);
        break;
      case 1:
        display.drawBitmap(90+displayShiftX, 22, cool_1, ICON_WIDTH, ICON_HEIGHT, 1);
        break;
      case 2:
        display.drawBitmap(90+displayShiftX, 22, cool_2, ICON_WIDTH, ICON_HEIGHT, 1);
        break; 
    } 
  }
  else
  {
    display.drawBitmap(90+displayShiftX, 22, cool_2, ICON_WIDTH, ICON_HEIGHT, 1);
  }
}


// Displays heat icon. If nextFrame==true then advances one frame
void displayHeat(bool nextFrame)
{
  //Displays heat icon. If animate==true then advance to next frame
  static int frame = 0;

  //if in cool mode do not show fire icon
  if(mode == 2)
  {
    return;
  }

  //Only increase frame if animate is true 
  if(nextFrame)
  {
    frame=(frame+1)%3;
    return;
  }

  if(heatON)
  {
    if(frame)
    {
      display.drawBitmap(90+displayShiftX, 0, heat_1, ICON_WIDTH, ICON_HEIGHT, 1);
    }
    else
    {
      display.drawBitmap(90+displayShiftX, 0, heat_0, ICON_WIDTH, ICON_HEIGHT, 1);
    }
  }
  else
  {
    display.drawBitmap(90+displayShiftX, 0, heat_1, ICON_WIDTH, ICON_HEIGHT, 1);
  }
}


// Displays fan icon. If nextFrame==true then advances one frame
void displayFan(bool nextFrame)
{
  static int frame = 0;
  
  if(nextFrame)             //Only increase frame if nextFrame is true 
  {
    frame=(frame+1)%3;
    return;
  }
  if(fanON)
  {
    switch(frame)
    {
      case 0:
        display.drawBitmap(90+displayShiftX, 44, fan_0, ICON_WIDTH, ICON_HEIGHT, 1);
        break;
      case 1:
        display.drawBitmap(90+displayShiftX, 44, fan_1, ICON_WIDTH, ICON_HEIGHT, 1);
        break;
      case 2:
        display.drawBitmap(90+displayShiftX, 44, fan_2, ICON_WIDTH, ICON_HEIGHT, 1);
        break;
      case 3:
        display.drawBitmap(90+displayShiftX, 44, fan_3, ICON_WIDTH, ICON_HEIGHT, 1);
        break;
    }
  }
  else
  {
    display.drawBitmap(90+displayShiftX, 44, fan_0, ICON_WIDTH, ICON_HEIGHT, 1);
  }
   
}


// Shows number of minutes that fan will be on. Automatically formats the number of minutes into hours if large enough
void displayFanMinutes()
{
  display.setCursor(65+displayShiftX, 50);
  display.setTextSize(1);
  
  if(!fanMinutes)
  {
    return;
  }
  if(fanMinutes <= 60)                                //If fanMinutes < 60 display fanMinutes in minutes
  {
    if(fanMinutes <= 9)
    {
      display.print(' ');

    }
    display.print(fanMinutes);
    display.print('M');

  }
  else                                                // If fanMinutes > 60 display fanMinutes in hours
  {
    display.setCursor(65+displayShiftX, 50);
    display.setTextSize(1);

    int h = round((float)fanMinutes/60);
    if(h <= 9)
    {
      display.print(' ');

    }
    display.print(h);
    display.print('H');

  } 
}


// Advances all the Icon frames. Generally called every 250ms
void advanceIconFrame()
{
  displayCool(true);
  displayHeat(true);
  displayFan(true);
}

// Displays all the icons
void displayIcons()
{
  displayCool(false);
  displayHeat(false);
  displayFan(false);
}


// Display Home Screen elements: Current Temp & Set Temp
void displayHome()
{
  display.clearDisplay();
  display.setCursor(displayShiftX,0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print(F("Set "));
  display.print(targetTemp);
  display.println(unit);
  display.setTextSize(1);
  display.println();
  display.setCursor(displayShiftX, display.getCursorY());
  display.setTextSize(5);
  display.print(temp,0);
  display.setTextSize(2);
  display.println(unit);
  if(connected)                   // If connected display the Icons & fanMinutes
  {
    displayIcons();
    displayFanMinutes();
  }
  else                            // If not connected show noInternet symbol
  {
    display.drawBitmap(80+displayShiftX, 35, noInternet, 30, 30, 1);
  }
  
}
