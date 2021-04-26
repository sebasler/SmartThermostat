/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME "smue2001"
#define IO_KEY "d9dfb2d6a1124315aec787c4ea8d31e0"

/******************************* WIFI **************************************/

#define WIFI_SSID "IOT"
#define WIFI_PASS "password"

#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
