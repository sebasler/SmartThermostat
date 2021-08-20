/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME "USERNAME"
#define IO_KEY "KEY"

/******************************* WIFI **************************************/

#define WIFI_SSID "IOT"
#define WIFI_PASS "password"

#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
