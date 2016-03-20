# BOARD_TAG = uno
BOARD_TAG = pro
BOARD_SUB = 16MHzatmega328
#ARDUINO_DIR = $(HOME)/Downloads/arduino-1.6.5-r5/
USER_LIB_PATH = $(PWD)/libraries/
ARDUINO_LIBS = SoftwareSerial Wire
include ../Arduino-Makefile/Arduino.mk
