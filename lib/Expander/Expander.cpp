#ifdef ESP8266
#include <Wire.h>
#include "Expander.h"
using namespace expander;

Expander::Expander(uint8_t addr) : _addr(addr)
{
}

void Expander::begin(void)
{
    Wire.begin();
    writeByte(0xff);
}

uint8_t Expander::readByte(void)
{
    uint8_t b;
    Wire.beginTransmission(_addr);
    Wire.requestFrom(_addr, 1);
    b = Wire.read();
    Wire.endTransmission();

    return b;
}

void Expander::writeByte(uint8_t b)
{
    Wire.beginTransmission(_addr);
    Wire.write(b);
    Wire.endTransmission();
}

void Expander::digitalWrite(uint8_t pin, bool val)
{
    uint8_t b = readByte();

    if (val) {
        b |= (1 << pin);
    } else {
        b &= ~(1 << pin);
    }

    writeByte(b);
}

uint8_t Expander::digitalRead(uint8_t pin)
{
    uint8_t b = readByte();

    return (b & (1 << pin));
}

void Expander::attachInterrupt(void (*userFunc)(void))
{
    pinMode(2, INPUT_PULLUP);
    ::attachInterrupt(2, userFunc, FALLING);
}
#endif
