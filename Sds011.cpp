#include "Sds011.h"

Sds011::Sds011(Stream& out) : _out(out)
{
}

bool Sds011::device_info(String& firmware_version, String& device_id)
{
    _send_cmd(CMD_FIRMWARE, NULL, 0);
    delay(200);

    bool ok = _read_response(CMD_FIRMWARE);
    if (!(ok && crc_ok())) {
        return false;
    }

    firmware_version = String(_buf[3]) + "-" + String(_buf[4]) + "-" + String(_buf[5]);
    device_id = (_buf[6] < 0x10) ? String("0") + String(_buf[6], HEX) : String(_buf[6], HEX);
    device_id += (_buf[7] < 0x10) ? String("0") + String(_buf[7], HEX) : String(_buf[7], HEX);
    return true;
}

bool Sds011::set_mode(Report_mode mode)
{
    uint8_t data[] = { 0x1, mode };
    _send_cmd(CMD_MODE, data, 2);
    delay(200);
    return _read_response(CMD_MODE) && crc_ok() && _buf[3] == 0x1 && _buf[4] == mode;
}

bool Sds011::set_sleep(bool sleep)
{
    uint8_t data[] = { 0x1, !sleep };
    _send_cmd(CMD_SLEEP, data, 2);
    delay(200);
    return _read_response(CMD_SLEEP) && crc_ok() && !_buf[4] == sleep;
}

bool Sds011::query_data(int& pm25, int& pm10)
{
    _send_cmd(CMD_QUERY_DATA, NULL, 0);
    delay(200);
    bool ok = _read_response(CMD_QUERY_DATA);
    if (!ok || !crc_ok()) {
        return false;
    }

    pm25 = _buf[2] | (static_cast<unsigned int>(_buf[3]) << 8);
    pm10 = _buf[4] | (static_cast<unsigned int>(_buf[5]) << 8);
    return true;
}

bool Sds011::query_data(int& pm25, int& pm10, int n)
{
    int pm25_table[n];
    int pm10_table[n];

    for (int i = 0; n > 0 && i < n; ++i) {
        bool ok = query_data(pm25_table[i], pm10_table[i]);
        if (!ok) {
            --n;
            --i;
            continue;
        }
        delay(1000);
    }

    _filter_data(n, pm25_table, pm10_table, pm25, pm10);
    return n > 0;
}

bool Sds011::query_data_auto(int& pm25, int& pm10)
{
    bool ok = _read_response(CMD_QUERY_DATA);
    if (!ok || !crc_ok()) {
        return false;
    }

    pm25 = _buf[2] | (static_cast<unsigned int>(_buf[3]) << 8);
    pm10 = _buf[4] | (static_cast<unsigned int>(_buf[5]) << 8);
    return true;
}

bool Sds011::query_data_auto(int& pm25, int& pm10, int n)
{
    int pm25_table[n];
    int pm10_table[n];

    for (int i = 0; n > 0 && i < n; ++i) {
        bool ok = query_data_auto(pm25_table[i], pm10_table[i]);
        if (!ok) {
            --n;
            --i;
            continue;
        }
        delay(1000);
    }

    _filter_data(n, pm25_table, pm10_table, pm25, pm10);
    return n > 0;
}

bool Sds011::crc_ok()
{
    uint8_t crc = 0;
    for (int i = 2; i < 8; ++i) {
        crc += _buf[i];
    }
    return crc == _buf[8];
}

bool Sds011::timeout()
{
    return _timeout;
}

void Sds011::_send_cmd(enum Command cmd, const uint8_t* data, uint8_t len)
{
    uint8_t i;
    uint8_t crc;

    _buf[0] = 0xAA;
    _buf[1] = 0xB4;
    _buf[2] = cmd;
    _buf[15] = 0xff;
    _buf[16] = 0xff;
    _buf[18] = 0xAB;

    crc = cmd + _buf[15] + _buf[16];

    for (i = 0; i < 12; ++i) {
        if (i < len) {
            _buf[3 + i] = data[i];
        }
        else {
            _buf[3 + i] = 0;
        }
        crc += _buf[3 + i];
    }

    _buf[17] = crc;

    _out.write(_buf, sizeof(_buf));
}

uint8_t Sds011::_read_byte(long unsigned deadline)
{
    while (!_out.available()) {
        if (deadline > 0 && millis() > deadline) {
            _timeout = true;
            return 0;
        }
        delay(1);
    }

    _timeout = false;
    return _out.read();
}

void Sds011::_ignore_response()
{
    delay(200);
    while (_out.available())
        _out.read();
}

bool Sds011::_read_response(enum Command cmd)
{
    uint8_t i = 0;
    long unsigned deadline = millis() + 1000;
    while (i < 3) {
        _buf[i] = _read_byte(deadline);
        if (timeout()) return false;
        switch (i++) {
        case 0: if (_buf[0] != 0xAA) i = 0; break;
        case 1: if (_buf[1] != ((cmd == CMD_QUERY_DATA) ? 0xC0 : 0xC5)) i = 0; break;
        case 2: if (cmd != CMD_QUERY_DATA && _buf[2] != cmd) i = 0; break;
        }
    }
    for (i = 3; i < 10; i++) {
        _buf[i] = _read_byte(deadline);
    }

    return !timeout() && _buf[9] == 0xAB;
}

String Sds011::_buf_to_string(void)
{
    String ret = "";
    uint8_t i = 0;

    for (i = 0; i < 19; i++) {
        char c = (_buf[i] >> 4) + '0';
        if (c > '9') {
            c += 'A' - '9' - 1;
        }
        ret += c;

        c = (_buf[i] & 0xf) + '0';
        if (c > '9') {
            c += 'A' - '9' - 1;
        }
        ret += c;
    }

    return ret;
}

void Sds011::_filter_data(int n, const int* pm25_table, const int* pm10_table, int& pm25, int& pm10)
{
    int pm25_min, pm25_max, pm10_min, pm10_max, pm25_sum, pm10_sum;

    pm10_sum = pm10_min = pm10_max = pm10_table[0];
    pm25_sum = pm25_min = pm25_max = pm25_table[0];

    for (int i = 1; i < n; ++i) {
        if (pm10_table[i] < pm10_min) {
            pm10_min = pm10_table[i];
        }
        if (pm10_table[i] > pm10_max) {
            pm10_max = pm10_table[i];
        }
        if (pm25_table[i] < pm25_min) {
            pm25_min = pm25_table[i];
        }
        if (pm25_table[i] > pm25_max) {
            pm25_max = pm25_table[i];
        }
        pm10_sum += pm10_table[i];
        pm25_sum += pm25_table[i];
    }

    if (n > 2) {
        pm10 = (pm10_sum - pm10_max - pm10_min) / (n - 2);
        pm25 = (pm25_sum - pm25_max - pm25_min) / (n - 2);
    }
    else if (n > 1) {
        pm10 = (pm10_sum - pm10_min) / (n - 1);
        pm25 = (pm25_sum - pm25_min) / (n - 1);
    }
    else {
        pm10 = pm10_sum / n;
        pm25 = pm25_sum / n;
    }
}
