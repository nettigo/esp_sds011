#include <SoftwareSerial.h>
#include "Sds011.h"
#include "Pcd8544.h"
#include "Expander.h"
#include "Dht.h"
#include "DHT.h"

#ifdef ESP8266
#include <Wire.h>
#include <Ticker.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "wifi_sta_pass.h"
#endif

static const int PM25_NORM=25;
static const int PM10_NORM=40;
static const int SAMPLES=10;

#ifdef ESP8266
sds011::Sds011 sensor(Serial);
pcd8544::Pcd8544 display(13, 12, 14);
expander::Expander expand(0x38);
Ticker timer1;
Ticker timer2;
#else
// RX, TX
SoftwareSerial mySerial(8,9);
sds011::Sds011 sensor(mySerial);
pcd8544::Pcd8544 display(A3, A2, A1, A0, 13);
#endif

dht::Dht dht22(14);

static bool set_press;

String val_to_str(uint16_t v)
{
    String r;

    r = String(v/10);
    if (v < 1000 && v%10) {
        r += String(".") + String(v%10);
    }

    for (int i = 4 - r.length(); i > 0; i--) {
        r = String(" ") + r;
    }

    return r;
}

void display_data(uint16_t pm25, uint16_t pm10, uint16_t t, uint16_t h)
{
    display.clear();
    display.setCursor(0, 0);

    display.println("    2.5   10");

    display.print("ug ");
    display.print(val_to_str(pm25).c_str());

    display.setCursor(8*7, 1);
    display.println(val_to_str(pm10).c_str());

    display.print("%  ");
    display.print(val_to_str((10*pm25/PM25_NORM)*10).c_str());
    display.setCursor(8*7, 2);
    display.print(val_to_str((10*pm10/PM10_NORM)*10).c_str());

    display.setCursor(0, 4);
    display.print("t:  ");
    display.print(val_to_str(t).c_str());
    display.print("C");
    display.setCursor(0, 5);
    display.print("h:  ");
    display.print(val_to_str(h).c_str());
    display.print("%");
}

void setup()
{
    bool clear = true;

#ifdef ESP8266
    expand.begin();
#else
    mySerial.begin(9600);
#endif
    Serial.begin(9600);

    display.begin();
    dht22.begin();

#ifdef ESP8266

    set_press = (expand.readByte() & 0b10 ) != 0b10;
    if (set_press) {
        display.clear();
        display.setCursor(0, 0);
        display.println("WIFI");
        WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            display.println("Connection Failed! Rebooting...");
            delay(5000);
            ESP.restart();
        }
        display.println(WiFi.localIP().toString().c_str());

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            display.setCursor(0, 5);
            display.println(String(progress / (total / 100)).c_str());
          });
        ArduinoOTA.begin();
        return;
    }

    expand.attachInterrupt(iter);
#endif


#ifdef ESP8266
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.forceSleepBegin(); // Use WiFi.forceSleepWake() to enable wifi

    String r = ESP.getResetReason();

    if (r == "Deep-Sleep Wake") {
        clear = false;
    }
#endif

    if (clear) {
        display.clear();
        display.setCursor(0,0);
        display.println("Hello");
    }

    sensor.set_sleep(false);
    sensor.set_mode(sds011::QUERY);

}

#ifdef ESP8266
void turnOff(void)
{
    expand.digitalWrite(0, HIGH);
}

void turnOn(void)
{
    uint8_t b = expand.readByte();

    if ((b & 0b10) != 0b10) {
        timer2.once_ms(5000, turnOff);
        expand.digitalWrite(0, LOW);
    }
}

void iter()
{
    timer1.once_ms(30, turnOn);
}
#endif

void loop()
{
    int pm25, pm10;
    bool ok;

    if (set_press) {
        ArduinoOTA.handle();
        delay(1000);
        return;
    }

    sensor.set_sleep(false);
    delay(1000);
    ok = sensor.query_data_auto(&pm25, &pm10, SAMPLES);
    sensor.set_sleep(true);

    uint16_t t = dht22.get_temperature();
    uint16_t h = dht22.get_humidity();

    if (ok) {
        display_data(pm25, pm10, t, h);
    } else {
        display.clear();
        display.setCursor(0, 0);
        display.println("NO SENSOR!");
    }

#ifdef ESP8266
    ESP.deepSleep(1000*1000*10, WAKE_RF_DEFAULT);
#else
    delay(10000);
#endif
}
