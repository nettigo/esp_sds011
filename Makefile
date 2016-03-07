# BOARD_TAG = uno
BOARD_TAG = pro
BOARD_SUB = 16MHzatmega328
ARDUINO_DIR = $(HOME)/Downloads/arduino-1.6.5-r5/
USER_LIB_PATH = $(PWD)/libraries/
ARDUINO_LIBS = Adafruit_GFX Adafruit_PCD8544 SPI SoftwareSerial
include ../Arduino-Makefile/Arduino.mk
