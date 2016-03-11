#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SoftwareSerial.h>

static const int PM25_NORM=25;
static const int PM10_NORM=40;

// RX, TX
SoftwareSerial mySerial(8,9);

// SCLK, DIN, DC, CS, RST
/* Adafruit_PCD8544 display = Adafruit_PCD8544(A5, A4, A3, A2, A1); */
Adafruit_PCD8544 display = Adafruit_PCD8544(A3, A2, A1, A0, 13);

void display_data(int pm25, int pm10, bool crc)
{
    static int count = 0;

    display.clearDisplay();

    display.setRotation(0);
    display.setTextSize(1);
    display.setTextColor(BLACK);

    display.setCursor(0,0);
    display.println("   2.5  10");
    display.print("ug ");
    display.print(pm25/10);
    display.print(".");
    display.print(pm25%10);

    display.setCursor(8*6, 8);
    display.print(pm10/10);
    display.print(".");
    display.println(pm10%10);

    display.print("%  ");
    display.print(10*pm25/PM25_NORM);
    display.setCursor(8*6, 8*2);
    display.println(10*pm10/PM10_NORM);

    display.println();
    display.print("CRC ");
    if (crc) {
	display.println("OK");
    } else {
	display.println("ERR");
    }

    display.print("Count=");
    display.println(count++);
    display.display();
}

void setup()
{
    display.begin();
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("START");
    display.display();

    mySerial.begin(9600);
    Serial.begin(9600);
}

int readByte()
{
    while (!mySerial.available())
	delay(1);
    return mySerial.read();
}

bool getData(int *pm25, int *pm10)
{
    int b, i;
    byte data[11];

    while (true) {
	while ((b=readByte())!=0xAA)
	    Serial.println(b);

	data[0] = b;
	data[1] = readByte();
	if (data[1] != 0xC0) {
	    continue;
	}

	for (i=2; i<11; i++)
	{
	    data[i] = readByte();
	}
	break;
    }

    *pm25 = data[2]+data[3]*256;
    *pm10 = data[4]+data[5]*256;

    int crc=0;
    for (int i=2; i<8; i++) {
	crc+=data[i];
    }

    return crc%256==data[8];
}

int count=1;

void loop()
{
    int pm25, pm10;
    bool ok;

    ok = getData(&pm25, &pm10);

    display_data(pm25, pm10, ok);
    count++;
}
