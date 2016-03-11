#ifndef _SDS011_H
#define _ASDS011_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#include <SoftwareSerial.h>

#define CMD_MODE 2
#define CMD_QUERY_DATA 4
#define CMD_DEVICE_ID  5
#define CMD_SLEEP 6
#define CMD_FIRMWARE 7
#define CMD_WORKING_PERIOD 8

enum Report_mode {
    ACTIVE = 0,
    QUERY = 1
};

class Sds011
{
    public:
	Sds011(SoftwareSerial &out);
	String firmware_version();
	void set_mode(Report_mode mode);
	void set_sleep(bool sleep);
	void query_data(int *pm25, int *pm10);
	bool crc_ok();

    private:
	void _construct_cmd(uint8_t cmd, uint8_t *buf, uint8_t len);
	void _send_cmd();
	uint8_t _read_byte();
	String _buf_to_string();
	void _ignore_response();
	void _read_response();

	SoftwareSerial &_out;
	uint8_t _buf[19];
};

#endif
