#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include "definitions.h"

extern Adafruit_SSD1306 display;
extern int state;
extern float temp;
extern int displayShiftX;
extern float offset;
extern float tolerance;
extern int mode;
extern unsigned int fanMinutes;


class Menu;

extern Menu mainMenu;
extern Menu fanMenu;
extern Menu offsetMenu;
extern Menu toleranceMenu;
extern Menu modeMenu;
extern Menu currentMenu;

class Menu
{
  public:
    int index;
    int start;
    int length;
    Menu(String x[], void (*functions[10])(int), int l);
    Menu();
    void displayMenu();
    void moveDown();
    void moveUp();
    void clickSelection();
    void reset()
    {
      index = 0;
      start = 0;
    }
    String selection()
    {
      return values[index+start];
    }
    int selectionIndex()
    {
      return index+start;
    }

  private:
    String values[10];
    void (*onClickFunctions[10])(int);
};

Menu::Menu()
{

}

Menu::Menu(String x[], void (*functions[])(int), int l)
{
  index = 0;
  start = 0;
  length = l;
  for(int i = 0; i < l; i++)
  {
    values[i] = x[i];
    onClickFunctions[i] = functions[i];
  }

}

void Menu::moveDown()
{
  if(index < 3)
  {
    index++;
  }
  else if (start+4 < length)
  {
    start++;
  }
}

void Menu::moveUp()
{
  if (index > 0)
  {
    index--;
  }
  else if (start > 0)
  {
    start--;
  }
}

void Menu::displayMenu()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  for(int i = 0; i < 4; i++)
  {
    if(i == index)
    {
      display.fillCircle(display.getCursorX()+5,display.getCursorY()+6, 5, SSD1306_WHITE);
    }
    display.setCursor(display.getCursorX()+15,display.getCursorY());
    display.println(values[i+start]);
    
  }  
}

void Menu::clickSelection()
{
  (*onClickFunctions[index+start])(index+start);
}


void setMenu(int i)
{
  switch(i)
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


void menuBack(int i)
{
  currentMenu = mainMenu;
}


void setOffset(int i)
{
  offset = strtof(currentMenu.selection().c_str(), NULL);
  Serial.println(offset);
  state = 0;
}


void setTolerance(int i)
{
  tolerance = strtof(currentMenu.selection().c_str(), NULL);
  Serial.println(tolerance);
  state = 0;
}


void setMode(int i)
{
  mode = currentMenu.selectionIndex();
  state = 0;
}


void setFan(int i)
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

