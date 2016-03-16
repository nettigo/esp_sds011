#include "Pcd8544.h"
using namespace pcd8544;

Pcd8544::Pcd8544(uint8_t clk, uint8_t din, uint8_t dc, uint8_t ce, uint8_t rst)
    : pin_sclk(clk), pin_sdin(din), pin_dc(dc), pin_sce(ce), pin_reset(rst)
{
    if (pin_sce > 0) {
        pinMode(pin_sce, OUTPUT);
    }
    if (pin_reset > 0) {
        pinMode(pin_reset, OUTPUT);
    }

    pinMode(pin_dc, OUTPUT);
    pinMode(pin_sdin, OUTPUT);
    pinMode(pin_sclk, OUTPUT);

    if (pin_reset > 0) {
        digitalWrite(pin_reset, LOW);
        digitalWrite(pin_reset, HIGH);
    }

    _write_cmd(mode_c, 0x21 );  // LCD Extended Commands.
    _write_cmd(mode_c, 0xB1 );  // Set LCD Vop (Contrast).
    _write_cmd(mode_c, 0x04 );  // Set Temp coefficent. //0x04
    _write_cmd(mode_c, 0x14 );  // LCD bias mode 1:48. //0x13
    _write_cmd(mode_c, 0x20 );  // LCD Basic Commands
    _write_cmd(mode_c, 0x0C );  // LCD in normal mode.
}

void Pcd8544::clear(void)
{
    for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
    {
        _write_cmd(mode_d, 0x00);
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
    _write_cmd(mode_c, 0x80 | x);  // Column
    _write_cmd(mode_c, 0x40 | y);  // Row
}

void Pcd8544::_write_cmd(enum dc_mode dc, byte data)
{
    digitalWrite(pin_dc, dc);
    if (pin_sce > 0) {
        digitalWrite(pin_sce, LOW);
    }

    shiftOut(pin_sdin, pin_sclk, MSBFIRST, data);

    if (pin_sce > 0) {
        digitalWrite(pin_sce, HIGH);
    }
}

void Pcd8544::_send_char(char c)
{
    _write_cmd(mode_d, 0x00);
    for (int index = 0; index < 5; index++)
    {
        _write_cmd(mode_d, ASCII[c - 0x20][index]);
    }
    _write_cmd(mode_d, 0x00);
}

