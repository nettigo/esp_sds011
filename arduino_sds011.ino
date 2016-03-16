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

#ifdef ESP8266
Pcd8544 display(13, 12, 14);
#else
Pcd8544 display(A3, A2, A1, A0, 13);
#endif

String val_to_str(uint16_t v)
{
    String r;

    if (v > 999) {
        r = String(v/10);
    } else {
        r = String(v/10);
        if (v%10) {
            r += String(".") + String(v%10);
        }
    }

    for (int i = 4 - r.length(); i > 0; i--) {
        r = String(" ") + r;
    }

    return r;
}

void display_data(uint16_t pm25, uint16_t pm10)
{
    display.clear();
    display.setCursor(0, 0);

    display.print("    2.5   10");
    display.setCursor(0, 1);

    display.print("ug ");
    display.print(val_to_str(pm25).c_str());

    display.setCursor(8*7, 1);
    display.print(val_to_str(pm10).c_str());

    display.setCursor(0, 2);
    display.print("%  ");
    display.print(val_to_str((10*pm25/PM25_NORM)*10).c_str());
    display.setCursor(8*7, 2);
    display.print(val_to_str((10*pm10/PM10_NORM)*10).c_str());
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
