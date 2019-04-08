/*
	Name:       measure.ino
	Created:	2019-04-06 12:16:00
	Author:     Dirk O. Kaar <dok@dok-net.net>
*/

#include <SoftwareSerial.h>
#include <Sds011.h>

#define SDS_PIN_RX D3
#define SDS_PIN_TX D4

SoftwareSerial serialSDS(SDS_PIN_RX, SDS_PIN_TX, false, 192);
Sds011Async< SoftwareSerial > sds011(serialSDS);

bool is_SDS_running = true;

void start_SDS() {
	Serial.println("Start wakeup SDS011");

	if (sds011.set_sleep(false)) { is_SDS_running = true; }

	Serial.println("End wakeup SDS011");
}

void stop_SDS() {
	Serial.println("Start sleep SDS011");

	if (sds011.set_sleep(true)) { is_SDS_running = false; }

	Serial.println("End sleep SDS011");
}

// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(115200);
	serialSDS.begin(9600);

	Serial.println("SDS011 start/stop and reporting sample");

	Sds011::Report_mode report_mode;
	if (!sds011.get_data_reporting_mode(report_mode)) {
		Serial.println("Sds011::get_data_reporting_mode() failed");
	}
	if (Sds011::REPORT_ACTIVE != report_mode) {
		Serial.println("Turning on Sds011::REPORT_ACTIVE reporting mode");
		if (!sds011.set_data_reporting_mode(Sds011::REPORT_ACTIVE)) {
			Serial.println("Sds011::set_data_reporting_mode(Sds011::REPORT_ACTIVE) failed");
		}
	}
}

// Add the main program code into the continuous loop() function
void loop()
{
	constexpr uint32_t down_s = 30;

	stop_SDS();
	Serial.print("stopped SDS011 (is running = ");
	Serial.print(is_SDS_running);
	Serial.println(")");
	delay(down_s * 1000);

	// Per manufacturer specification, place the sensor in standby to prolong service life.
	// At an user-determined interval (here 30s down plus 30s duty = 1m), run the sensor for 30s.
	// Quick response time is given as 10s by the manufacturer, thus omit the measurements
	// obtained during the first 10s of each run.

	constexpr uint32_t duty_s = 30;
	constexpr uint32_t duty_invalid_s = 10;

	start_SDS();
	Serial.print("started SDS011 (is running = ");
	Serial.print(is_SDS_running);
	Serial.println(")");

	const uint32_t deadline_invalid = duty_invalid_s * ESP.getCycleCount() + ESP.getCpuFreqMHz() * 1000000;
	const uint32_t deadline = duty_s * ESP.getCycleCount() + ESP.getCpuFreqMHz() * 1000000;

	sds011.on_query_data_auto([deadline_invalid](int pm25_serial, int pm10_serial) {
		Serial.println("Begin Handling SDS011 data");
		if (static_cast<int32_t>(deadline_invalid - ESP.getCycleCount()) > 0) {
			Serial.println("Discarding measurements within 10s after start");
		}
		else if ((!isnan(pm10_serial)) && (!isnan(pm25_serial))) {
			Serial.print("PM10: ");
			Serial.println(float(pm10_serial) / 10);
			Serial.print("PM2.5: ");
			Serial.println(float(pm25_serial) / 10);
		}
		Serial.println("End Handling SDS011 data");
		});

	while (static_cast<int32_t>(deadline - ESP.getCycleCount()) > 0) {
		optimistic_yield(1000000);
		serialSDS.perform_work();
	}

	sds011.on_query_data_auto(0);
}
