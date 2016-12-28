#pragma once

struct Configuration {
    char *wifi_ssid;
    char *wifi_pass;
    char *banner;
    char *ts_api_key;
    int sleep_time;
};

bool load_config(void);
bool save_config(void);
