#ifndef _SDS011_H
#define _SDS011_H

#if ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif

#include <Stream.h>

class Sds011
{
public:
    enum Command {
        CMD_MODE = 2,
        CMD_QUERY_DATA = 4,
        CMD_DEVICE_ID,
        CMD_SLEEP = 6,
        CMD_FIRMWARE = 7,
        CMD_WORKING_PERIOD
    };

    enum Report_mode {
        ACTIVE = 0,
        QUERY = 1
    };

    Sds011(Stream& out);
    bool device_info(String& firmware_version, String& device_id);
    bool set_mode(Report_mode mode);
    bool set_sleep(bool sleep);
    bool query_data(int& pm25, int& pm10);
    bool query_data(int& pm25, int& pm10, int n);
    bool query_data_auto(int& pm25, int& pm10);
    bool query_data_auto(int& pm25, int& pm10, int n);
    bool timeout();
    bool crc_ok();

private:
    void _send_cmd(enum Command cmd, const uint8_t* buf, uint8_t len);
    uint8_t _read_byte(long unsigned deadline = 0);
    String _buf_to_string();
    void _ignore_response();
    bool _read_response(enum Command cmd);
    void _filter_data(int n, const int* pm25_table, const int* pm10_table, int& pm25, int& pm10);

    Stream& _out;
    uint8_t _buf[19];
    bool _timeout = false;
};

#endif
