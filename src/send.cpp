#include <ESP8266HTTPClient.h>
#include "send.h"
#include "config.h"

extern struct Configuration config;

int send_ts(uint16_t pm25, uint16_t pm10, int16_t t, uint16_t h)
{
    HTTPClient http;
    char buf[1024];

    if (strlen(config.ts_api_key)==0) {
        return -1;
    }

    snprintf(buf, sizeof(buf), "http://api.thingspeak.com/update?api_key=%s&field1=%d&field2=%d&field3=%d&field4=%d",
            config.ts_api_key, (int)pm25, (int)pm10, (int)t, (int)h);

    http.begin(buf);

    return http.GET();
}
