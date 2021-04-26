#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_MCP9808.h>
#include <MD_REncoder.h>
#include "config.h"
#include "menuActions.h"
#include "thermostatDisplay.h"
#include "definitions.h"



//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// START OF VARIABLES
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int state = 0;
float temp = 0;
int displayShiftX = 0;
float offset = 0;
float tolerance = 1;
int mode = 0;

float tempBuffer[circBuffSize];
int writeIndex = 0;

unsigned int fanMinutes = 0;
unsigned int seconds = 0;
unsigned int minutes = 0;
unsigned long last = millis();

int targetTemp = 76;
bool heatON = false;
bool coolON = false;
bool fanON = false;
bool pendingTempUpdate = false;
bool connected = false;

volatile bool buttonPress = false;
volatile unsigned long lastInput = 0;
volatile int count = 0;

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// END OF VARIABLES
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-






//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// START OF OBJECTS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);                     // Create SSD1306 Display Object

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();                                             // Create the MCP9808 temperature sensor object

MD_REncoder rotary = MD_REncoder(DT, CLK);                                                    // Create rotary encoder object

AdafruitIO_Feed *thermostatSetTemp = io.feed("thermostatSetTemp");                            // IO feeds for MQTT
AdafruitIO_Feed *thermostatTemp = io.feed("thermostatTemp");


// Menu Items
void (*mainMenuSelections[5])() = {updateCurrentMenu, updateCurrentMenu, updateCurrentMenu, updateCurrentMenu, updateCurrentMenu};
Menu mainMenu =      Menu(new String[5]{"Set Fan", "Offset", "Tolerance", "Mode", "EXIT"}, mainMenuSelections, 5);

void (*offsetMenuSelections[10])() = {setOffset, setOffset, setOffset, setOffset, setOffset, setOffset, setOffset, setOffset, setOffset, menuBack};
Menu offsetMenu =    Menu(new String[10]{"-2.0", "-1.5", "-1.0", "-0.5", " 0", " 0.5", " 1.0", " 1.5", " 2.0", " BACK"}, offsetMenuSelections, 10);

void (*toleranceMenuSelections[7])() = {setTolerance, setTolerance, setTolerance, setTolerance, setTolerance, setTolerance, menuBack};
Menu toleranceMenu = Menu(new String[7]{"0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "BACK"}, toleranceMenuSelections, 7);

void (*modeMenuSelections[4])() = {setMode, setMode, setMode, menuBack};
Menu modeMenu =      Menu(new String[4]{"Both", "Heat", "Cool", "BACK"}, modeMenuSelections, 4);

void (*fanMenuSelections[10])() = {setFan, setFan, setFan, setFan, setFan, setFan, setFan, setFan, setFan, menuBack};
Menu fanMenu  =      Menu(new String[10]{"OFF", "15 min ", "30 min", " 1 hour", " 2 hour", " 4 hour", " 8 hour", "12 hour", "24 hour", "BACK"}, fanMenuSelections, 10);

Menu currentMenu = mainMenu;

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// END OF OBJECTS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-





//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// START OF FUNCTIONS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void setupTempsensor();
void setupOLED();
void setupAdafruitIO();
void updateTemp();
void taskScheduler();
void updateTime();
void onButtonPress();
void thermostatFunctions();
void handleThermostatSetTempMessage(AdafruitIO_Data *data);


// Initialize connection to temperature sensor and set up circular buffer
void setupTempsensor()
{
  if (!tempsensor.begin(0x18))                            // Check if MCP9808 connected properly
  {
    Serial.println("Unable to find MCP9808!");
    while (1);
  }  
  Serial.println("Found MCP9808!");

  tempsensor.setResolution(3);                            // Set resolution to 0.0625°C and turn on tempsensor
  tempsensor.wake();

  
  float t;
  if(unit == 'F')                                         // Make initial reading
  {
    t = tempsensor.readTempF();
  }
  else
  {
    t = tempsensor.readTempC();
  }

  for(int i = 0; i < circBuffSize; i++)                   // Setup circular buffer with initial reading
  {
    tempBuffer[i] = t;
  }
}

// Setup function for OLED screen
void setupOLED()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);                                                // Don't proceed, loop forever
  }
  display.clearDisplay();
}


// Connects to adafruit IO service
void setupAdafruitIO()
{
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  thermostatSetTemp->onMessage(handleThermostatSetTempMessage);

  // ENSURE THAT YOU HAVE CALLED setupOLED() at this point 
  // Displays initial connecting message
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println(F("Connecting"));
  display.println("");
  display.setTextSize(1);
  while(io.status() < AIO_CONNECTED) 
  {
    static unsigned long start = millis();
    if(millis()-start>20000)
    {
      Serial.println("Unable to establish connection!");
      connected = false;
      return;
    }
    display.print('.');
    display.display();
    Serial.print(".");
    delay(500);
  }
  connected = true;
  Serial.println();
  Serial.println(io.statusText());
  thermostatSetTemp->get();
}

// Read current temperature and add to circular buffer. Calculate average temperature
void updateTemp()
{
  float t;
  if (unit == 'F')
  {
    t = tempsensor.readTempF();

  }
  else
  {
    t = tempsensor.readTempC();
  }
  tempBuffer[writeIndex] = t;
  writeIndex = (writeIndex+1)%circBuffSize;
  float total = 0;
  for(int i = 0; i < circBuffSize; i ++)
  {
    total = total + tempBuffer[i];
  }
  temp = total/circBuffSize;
}


// Tasks that need to occur with precise timings should be put here
void taskScheduler()
{
  static unsigned int lastSecond = 0;
  static unsigned int lastMinute = 0;
  static unsigned long lastFrameUpdate = millis();
  static unsigned int lastDataUpload = 0;
  static unsigned int lastConnectionCheck = 0;
  
  // Update Icon Frame every 250ms
  if (millis()-lastFrameUpdate > 250)
  {
    lastFrameUpdate = millis();
    advanceIconFrame();
  }


  // Functions that need to run every second
  if (seconds - lastSecond > 0)
  {
    updateTemp();
    lastSecond = seconds;
  }
  

  // io.run() is PAINFULLY slow to run. In order to maintain decent menu responsiveness it is only called every 2 seconds
  // In preliminary testing this didn't cause any issues with receiving updates or sending values but not entirely sure.
  if(seconds-lastConnectionCheck >2)
  {
    if(io.run(0, true) == 1)                              // If connection not established set connection to false. Try to reestablish the connection
    {
      connected = false;
      io.run();
    }
    else
    {
      connected = true;
    }
    lastConnectionCheck = seconds;
  }


  // If it has been more than 10 seconds since the lastDataUpload, upload new data
  if (seconds-lastDataUpload > 10)
  {
    Serial.println("Updating Temperature");
    thermostatTemp->save(temp);
    if(pendingTempUpdate)
    {
      thermostatSetTemp->save(targetTemp);
      pendingTempUpdate = false;
    }
    lastDataUpload = seconds;
  }


  // Functions that need to run every minute
  if (minutes - lastMinute > 0)
  {
    displayShiftX = (displayShiftX+1)%15;       // Burn in Prevention. Shift home screen over by one pixel every minute
    
    if (fanMinutes > 0)                         // Update Fan Minutes
    {
      fanMinutes--;
    }

    lastMinute = minutes;
  }
  
}


// Update the fake timer
void updateTime()
{
  if(millis()-last > 1000)
  {
    last = millis();
    seconds++;
  }
  if(seconds > 59)
  {
    seconds = 0;
    minutes++;
  }
  if(minutes > 59)
  {
    minutes = 0;
  }
}


// Functions to run on a button press
void onButtonPress()
{
  if (state != 0)                       // If in a menu select current item
  {
    currentMenu.clickSelection();
  }
  else                                  // Go into menu selection and display mainMenu
  {
    state++;
    currentMenu = mainMenu;
  }
  
}


// Functions to control the thermostat
void thermostatFunctions()
{
  // CONTROLS RELAYS TO SWITCH FURNACE
  if (fanMinutes > 0 && !fanON)
  {
    fanON = true;
    digitalWrite(FAN_PIN, LOW);
  }

  if (!fanMinutes && fanON && !coolON && !heatON)
  {
    // turn off fan if fanminutes = 0, fan is on, and both cool and heat are off
    fanON = false;
    digitalWrite(FAN_PIN, HIGH);
  }

  if (targetTemp > temp+tolerance && !heatON)
  {
    heatON = true;
    fanON = true;
    digitalWrite(HEAT_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
  }
  if (targetTemp < temp && heatON)
  {
    heatON = false;
    digitalWrite(HEAT_PIN, HIGH);
    // Once heat has turned off continue to run fan for 5 min for extra efficiency;
    if(fanMinutes < 5)
    {
      fanMinutes = 5;
    }
    
  }
  if (targetTemp < temp-tolerance && !coolON)
  {
    coolON = true;
    fanON = true;
    digitalWrite(COOL_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
  }
  if (targetTemp> temp && coolON)
  {
    coolON = false;
    digitalWrite(COOL_PIN, HIGH);
    // Once cool has turned off continue to run fan for 5 min for extra efficiency
    if(fanMinutes < 5)
    {
      fanMinutes = 5;
    }
    
  }

}


//Handler for one IOT feed
void handleThermostatSetTempMessage(AdafruitIO_Data *data) 
{
  //Message handler for Set Thermostat Temp
  
  if(pendingTempUpdate)                   //Don't update temp from internet if pending temperature update from rotary encoder
  {
    return;
  }
  Serial.print("received <- ");
  String value(data->value());
  Serial.println(value);
  int t = roundf(strtof(value.c_str(), NULL));
  
  if (t >= MIN_TEMP && t <= MAX_TEMP)     // If received value is within valid temperature range update target temperature
  {
    targetTemp = t;
    Serial.print("Setting thermostat to: ");
    Serial.print(t);
    Serial.println(unit);
  }
  else 
  {
    Serial.println("Invalid Temperature!");
  }

}


// Interrupt for button press with debounce
void ICACHE_RAM_ATTR buttonInterrupt()
{
  static unsigned long lastButton = 0;
  unsigned long now = millis();
  if(now-lastButton > 300)                  // Check for debounce
  {
    lastButton = now;
    Serial.println("Button Press Detected");
    buttonPress = true;
    onButtonPress();
  }
  lastInput = now;
  
}


// Interrupt for rotary encoder
void ICACHE_RAM_ATTR rotaryInterrupt()
{
  unsigned int result = rotary.read();
  if (result == DIR_CW)
  {
    count++;
    Serial.println(count);
  } else if (result == DIR_CCW)
  {
    count--;
    Serial.println(count);
  }
  lastInput = millis();
}


// Setup function to initialize pins & setup peripheral devices
void setup() 
{
  Serial.begin(9600);                      // Initiate serial connection

  if(unit != 'F' && unit != 'C')           // Verify that the unit is correct
  {
    Serial.println("INVALID UNIT");
    for(;;);
  }

  //Initiate temp sensor, OLED, rotary encoder & connect to Adafruit IO
  setupTempsensor();
  setupOLED();
  rotary.begin();
  setupAdafruitIO();
  

  //Set up inputs for rotary encoder and outputs for thermostat
  pinMode(SW, INPUT);
  pinMode(DT, INPUT);
  pinMode(CLK, INPUT);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(COOL_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  //Initial values for heat, cool, and fan. OFF
  digitalWrite(HEAT_PIN, HIGH);
  digitalWrite(COOL_PIN, HIGH);
  digitalWrite(FAN_PIN, HIGH);

  //Attatch interrupts for rotary encoder
  attachInterrupt(digitalPinToInterrupt(SW), buttonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(CLK), rotaryInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT), rotaryInterrupt, CHANGE);
}


// Main Loop
void loop() 
{
  updateTime();                                             //Update the fake timer

  taskScheduler();                                          //Run Scheduled tasks

  thermostatFunctions();                                    //Control HVAC system
  
  showGUI();                                                //Shows GUI & updates based on user or cloud inputs

}

