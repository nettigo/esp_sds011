#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "Pcd8544.h"
#include "wifi_sta_pass.h"

#define WIFI_AP_SSID "ESPdust"
#define WIFI_AP_PASS "dustdust"

extern pcd8544::Pcd8544 display;

ESP8266WebServer server(80);

void setup_setup(void)
{
    display.clear();
    display.setCursor(0, 0);
    display.println("WIFI");

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
    display.println(WiFi.softAPIP().toString().c_str());

    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        display.println("Wifi Connection Failed!");
    }

    display.println(WiFi.localIP().toString().c_str());

    server.on("/", [](void) {
        server.send(200, "text/plain", "hello from esp8266!");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        display.setCursor(0, 5);
        display.println(String(progress / (total / 100)).c_str());
    });
    ArduinoOTA.begin();

    server.begin();
}

void setup_loop(void)
{
    ArduinoOTA.handle();
    server.handleClient();
    delay(1000);
}
