#include <SoftwareSerial.h>
#include "Pcd8544.h"
#include "Expander.h"
#include "ArduinoJson.h"
#include "FS.h"

#include <Wire.h>

static struct configuration {
    char *wifi_ssid;
    char *wifi_pass;
    char *banner;
} config;

pcd8544::Pcd8544 display(13, 12, 14);
expander::Expander expand(0x38);

StaticJsonBuffer<200> jsonBuffer;

static bool set_press;

void normal_loop(void);
void normal_setup(void);
void setup_loop(void);
void setup_setup(void);

bool load_config(void)
{
    char buf[1024];
    const char *tmp;
    StaticJsonBuffer<200> json_buf;
    bool ret = false;
    size_t size;

    memset(&config, 0, sizeof(config));

    SPIFFS.begin();

    File file = SPIFFS.open("/config.json", "r");
    if (!file) {
        goto out2;
    }

    size = file.size();
    if (size > sizeof(buf)) {
        goto out;
    }

    file.readBytes(buf, size);

    {
        JsonObject &json = json_buf.parseObject(buf);

        if (!json.success()) {
            goto out;
        }

        tmp = json["wifi_ssid"];
        if (tmp) {
            config.wifi_ssid = strdup(tmp);
        }

        tmp = json["wifi_pass"];
        if (tmp) {
            config.wifi_pass = strdup(tmp);
        }

        tmp = json["banner"];
        if (tmp) {
            config.banner = strdup(tmp);
        } else {
            config.banner = strdup("");
        }
    }

    ret = true;

out:
    file.close();
out2:
    SPIFFS.end();

    return ret;
}

void setup()
{
    bool clear = true;
    char *banner;

    expand.begin();

    String r = ESP.getResetReason();

    if (r == "Deep-Sleep Wake") {
        clear = false;
    }

    Serial.begin(9600);

    display.begin();

    if (!load_config()) {
        banner = strdup("NO CONFIG");
    } else {
        banner = config.banner;
    }

    if (clear) {
        display.clear();
        display.setCursor(0,0);
        display.println("Hello");
        display.println(banner);
    }

    delay(1000);
    set_press = (expand.readByte() & 0b10 ) != 0b10;

    if (set_press) {
        setup_setup();
    } else {
        normal_setup();
    }
}

void loop()
{
    if (set_press) {
        setup_loop();
    } else {
        normal_loop();
    }
}
