#include <SoftwareSerial.h>
#include "Sds011.h"
#include "Pcd8544.h"

static const int PM25_NORM=25;
static const int PM10_NORM=40;
static const int SAMPLES=10;

#ifdef ESP8266
Sds011 sensor(Serial);
#else
// RX, TX
SoftwareSerial mySerial(8,9);
Sds011 sensor(mySerial);
#endif

Pcd8544 display;

void display_data(int pm25, int pm10)
{
    display.clear();
    display.setCursor(0, 0);

    display.print("    2.5   10");
    display.setCursor(0, 1);

    display.print("ug ");
    display.print(String(pm25/10).c_str());
    display.print(".");
    display.print(String(pm25%10).c_str());

    display.setCursor(8*7, 1);
    display.print(String(pm10/10).c_str());
    display.print(".");
    display.print(String(pm10%10).c_str());

    display.setCursor(0, 2);
    display.print("%  ");
    display.print(String(10*pm25/PM25_NORM).c_str());
    display.setCursor(8*7, 2);
    display.print(String(10*pm10/PM10_NORM).c_str());
}

void setup()
{
#ifndef ESP8266
    mySerial.begin(9600);
#endif
    Serial.begin(9600);

#ifdef ESP8266
    String r=ESP.getResetReason();

    if (r=="Deep-Sleep Wake") {
    } else {
        display.clear();
    }
    display.setCursor(0,0);
    display.print(r.c_str());
#else
    display.clear();
#endif

    sensor.set_sleep(false);
    sensor.set_mode(QUERY);
}

void loop()
{
    int pm25, pm10;

    sensor.set_sleep(false);
    delay(1000);
    sensor.query_data_auto(&pm25, &pm10, SAMPLES);
    sensor.set_sleep(true);

    display_data(pm25, pm10);

#ifdef ESP8266
    ESP.deepSleep(1000*1000*10, WAKE_RF_DEFAULT);
#else
    delay(60000);
#endif
}
