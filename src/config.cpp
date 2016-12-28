#include <ArduinoJson.h>
#include <FS.h>
#include "config.h"

struct Configuration config;

#define CONFIG_BUF_SZ 1024

bool save_config(void)
{
    bool ret = false;

    StaticJsonBuffer<200> json_buf;
    JsonObject &json = json_buf.createObject();
    json["wifi_ssid"] = config.wifi_ssid;
    json["wifi_pass"] = config.wifi_pass;
    json["banner"] = config.banner;
    json["ts_api_key"] = config.ts_api_key;
    json["sleep_time"] = config.sleep_time;

    File file = SPIFFS.open("/config.json", "w");
    if (!file) {
        goto out;
    }

    json.printTo(file);

out:
    return ret;
}

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

        tmp = json["ts_api_key"];
        if (tmp) {
            config.ts_api_key = strdup(tmp);
        } else {
            config.banner = strdup("");
        }

        tmp = json["sleep_time"];
        if (tmp) {
            config.sleep_time = atoi(tmp);
        }
        if (config.sleep_time < 20) {
            config.sleep_time = 20;

        }
    }

    ret = true;

out:
    file.close();

    return ret;
}

