#esp_sds011

Arduino library for the SDS011 particulate matter sensor attached to the
ESP8266 or ESP32 SOC micro controllers.

The SDS011 is a particulate matter sensor by Nova Fitness.

This library is designed to work with the EspSoftwareSerial library to
support the reception and evaluation of the asynchronous measurements
emitted by the sensor. Working at 9600bps, using a simulated UART
poses no difficulty to either of the supported SOCs.
