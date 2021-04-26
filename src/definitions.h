#define sendDelay 5

#define SW D5 //Button Press
#define DT D6 //Rotary Encoder
#define CLK D7 //Rotary Encoder

#define HEAT_PIN D4
#define COOL_PIN D0
#define FAN_PIN D3

#define unit 'F'
#define MAX_TEMP 99
#define MIN_TEMP 40
#define circBuffSize 15

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define ICON_HEIGHT 20
#define ICON_WIDTH 20