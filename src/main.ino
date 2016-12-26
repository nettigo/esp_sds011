#include <SoftwareSerial.h>
#include "Pcd8544.h"
#include "Expander.h"
#include "config.h"

#include <Wire.h>

extern struct Configuration config;

pcd8544::Pcd8544 display(13, 12, 14);
expander::Expander expand(0x38);

static bool set_press;

void normal_loop(void);
void normal_setup(void);
void setup_loop(void);
void setup_setup(void);

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
