#include "Sds011.h"

Sds011::Sds011(SoftwareSerial &out) : _out(out)
{
}

String Sds011::firmware_version(void)
{
    _construct_cmd(CMD_FIRMWARE, NULL, 0);
    _send_cmd();
    _read_response();

    return String(_buf[3])+"_"+String(_buf[4])+"_"+String(_buf[5]);
}

void Sds011::set_mode(Report_mode mode)
{
    uint8_t data[] = {0x1, mode};
    _construct_cmd(CMD_MODE, data, 2);
    _send_cmd();
    _ignore_response();
}

void Sds011::set_sleep(bool sleep)
{
    uint8_t data[] = {0x1, !sleep};
    _construct_cmd(CMD_SLEEP, data, 2);
    _send_cmd();
    _ignore_response();
}

void Sds011::query_data(int *pm25, int *pm10)
{
    _construct_cmd(CMD_QUERY_DATA, NULL, 0);
    _send_cmd();
    _read_response();

    *pm25 = _buf[2] | _buf[3]<<8;
    *pm10 = _buf[4] | _buf[5]<<8;
}

bool Sds011::crc_ok(void)
{
    uint8_t crc = 0 ;
    for (int i=2; i<8; i++) {
	crc+=_buf[i];
    }
    return crc==_buf[8];
}

void Sds011::_construct_cmd(uint8_t cmd, uint8_t *data, uint8_t len)
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
}

uint8_t Sds011::_read_byte(void)
{
    while (!_out.available())
	delay(1);
    return _out.read();
}

void Sds011::_send_cmd(void)
{
    _out.write(_buf, 19);
    _out.flush();
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
