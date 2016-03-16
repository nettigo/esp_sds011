#include "Sds011.h"

Sds011::Sds011(Stream &out) : _out(out)
{
}

String Sds011::firmware_version(void)
{
    _send_cmd(CMD_FIRMWARE, NULL, 0);
    _read_response();

    return String(_buf[3])+"_"+String(_buf[4])+"_"+String(_buf[5]);
}

void Sds011::set_mode(Report_mode mode)
{
    uint8_t data[] = {0x1, mode};
    _send_cmd(CMD_MODE, data, 2);
    _ignore_response();
}

void Sds011::set_sleep(bool sleep)
{
    uint8_t data[] = {0x1, !sleep};
    _send_cmd(CMD_SLEEP, data, 2);
    _ignore_response();
}

void Sds011::query_data(int *pm25, int *pm10)
{
    _send_cmd(CMD_QUERY_DATA, NULL, 0);
    _read_response();

    *pm25 = _buf[2] | _buf[3]<<8;
    *pm10 = _buf[4] | _buf[5]<<8;
}

void Sds011::query_data_auto(int *pm25, int *pm10, int n)
{
    int pm25_table[n];
    int pm10_table[n];
    int ok;

    for (int i = 0; i<n; i++) {
	query_data(&pm25_table[i], &pm10_table[i]);
	ok = crc_ok();
	if (!ok) {
	    n--, i--;
	    continue;
	}
	delay(1000);
    }

    _filter_data(n, pm25_table, pm10_table, pm25, pm10);
}

bool Sds011::crc_ok(void)
{
    uint8_t crc = 0 ;
    for (int i=2; i<8; i++) {
	crc+=_buf[i];
    }
    return crc==_buf[8];
}

void Sds011::_send_cmd(uint8_t cmd, uint8_t *data, uint8_t len)
{
    uint8_t i, crc;

    _buf[0] = 0xAA;
    _buf[1] = 0xB4;
    _buf[2] = cmd;
    _buf[15] = 0xff;
    _buf[16] = 0xff;
    _buf[18] = 0xAB;

    crc = cmd + _buf[15] + _buf[16];

    for (i=0; i<12; i++) {
	if (i<len) {
	    _buf[3+i] = data[i];
	} else {
	    _buf[3+i] = 0;
	}
	crc += _buf[3+i];
    }

    _buf[17] = crc;

    for (i = 0; i < 19; i++) {
	_out.write(_buf[i]);
    }
    _out.flush();
}

uint8_t Sds011::_read_byte(void)
{
    while (!_out.available())
	delay(1);
    return _out.read();
}

void Sds011::_ignore_response(void)
{
    delay(1000);
    while (_out.available())
	_out.read();
}

void Sds011::_read_response(void)
{
    uint8_t i = 1, b;

    while ((b=_read_byte())!=0xAA)
	;

    _buf[0] = b;

    for(i = 1; i<10; i++) {
	_buf[i] = _read_byte();
    }
}

String Sds011::_buf_to_string(void)
{
    String ret = "";
    uint8_t i = 0;

    for(i = 0; i<19; i++) {
	char c = (_buf[i]>>4) + '0';
	if (c > '9') {
	    c += 'A'-'9'-1;
	}
	ret += c;

	c = (_buf[i] & 0xf) + '0';
	if (c > '9') {
	    c += 'A'-'9'-1;
	}
	ret += c;
    }

    return ret;
}

void Sds011::_filter_data(int n, int *pm25_table, int *pm10_table, int *pm25, int *pm10)
{
    int pm25_min, pm25_max, pm10_min, pm10_max, pm25_sum, pm10_sum;

    pm10_sum = pm10_min = pm10_max = pm10_table[0];
    pm25_sum = pm25_min = pm25_max = pm25_table[0];

    for (int i=1; i<n; i++) {
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
	*pm10 = (pm10_sum - pm10_max - pm10_min)/(n-2);
	*pm25 = (pm25_sum - pm25_max - pm25_min)/(n-2);
    } else if (n > 1) {
	*pm10 = (pm10_sum - pm10_min)/(n-1);
	*pm25 = (pm25_sum - pm25_min)/(n-1);
    } else {
	*pm10 = pm10_sum/n;
	*pm25 = pm25_sum/n;
    }
}
