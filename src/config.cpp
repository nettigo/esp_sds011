#include <ArduinoJson.h>
#include <FS.h>
#include "config.h"

struct Configuration config;

#define CONFIG_BUF_SZ 1024

bool load_config(void)
{
    char buf[CONFIG_BUF_SZ];
    const char *tmp;
    StaticJsonBuffer<200> json_buf;
    bool ret = false;
    size_t size;

    memset(&config, 0, sizeof(config));

    File file = SPIFFS.open("/config.json", "r");
    if (!file) {
        return ret;
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

    return ret;
}

