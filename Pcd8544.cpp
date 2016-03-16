#include "Pcd8544.h"

Pcd8544::Pcd8544(void)
{
    if (PIN_SCE > 0) {
        pinMode(PIN_SCE, OUTPUT);
    }
    if (PIN_RESET > 0) {
        pinMode(PIN_RESET, OUTPUT);
    }

    pinMode(PIN_DC, OUTPUT);
    pinMode(PIN_SDIN, OUTPUT);
    pinMode(PIN_SCLK, OUTPUT);

    if (PIN_RESET > 0) {
        digitalWrite(PIN_RESET, LOW);
        digitalWrite(PIN_RESET, HIGH);
    }

    _write_cmd(LCD_C, 0x21 );  // LCD Extended Commands.
    _write_cmd(LCD_C, 0xB1 );  // Set LCD Vop (Contrast).
    _write_cmd(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
    _write_cmd(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
    _write_cmd(LCD_C, 0x20 );  // LCD Basic Commands
    _write_cmd(LCD_C, 0x0C );  // LCD in normal mode.
}

void Pcd8544::clear(void)
{
    for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
    {
        _write_cmd(LCD_D, 0x00);
    }
}

void Pcd8544::print(const char *data)
{
    while (*data)
    {
        _send_char(*data++);
    }
}

// x - range: 0 to 84
// y - range: 0 to 5
void Pcd8544::setCursor(int x, int y)
{
    _write_cmd( 0, 0x80 | x);  // Column
    _write_cmd( 0, 0x40 | y);  // Row
}

void Pcd8544::_write_cmd(byte dc, byte data)
{
    digitalWrite(PIN_DC, dc);
    if (PIN_SCE > 0) {
        digitalWrite(PIN_SCE, LOW);
    }

    shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);

    if (PIN_SCE > 0) {
        digitalWrite(PIN_SCE, HIGH);
    }
}

void Pcd8544::_send_char(char c)
{
    _write_cmd(LCD_D, 0x00);
    for (int index = 0; index < 5; index++)
    {
        _write_cmd(LCD_D, ASCII[c - 0x20][index]);
    }
    _write_cmd(LCD_D, 0x00);
}

