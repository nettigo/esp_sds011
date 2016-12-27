#pragma once

struct Configuration {
    char *wifi_ssid;
    char *wifi_pass;
    char *banner;
};

bool load_config(void);
bool save_config(void);
