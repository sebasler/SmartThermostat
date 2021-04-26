#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include "definitions.h"


extern Adafruit_SSD1306 display;


// Menu class
class Menu
{
  public:
    int index;
    int start;
    int length;
    Menu(String x[], void (*functions[10])(), int l);
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
    void (*onClickFunctions[10])();
};


// Default constructor
Menu::Menu()
{

}


// Constructor for Menu with data. 
Menu::Menu(String x[], void (*functions[])(), int l)
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


// Move down one selection in menu
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


// Move up one selection in menu
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


// Displays the current menu items and selection
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


// Executes the function pointer associated with the current selection
void Menu::clickSelection()
{
  (*onClickFunctions[index+start])();
}






