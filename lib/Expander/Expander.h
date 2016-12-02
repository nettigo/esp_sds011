#ifndef _EXPANDER_H
#define _EXPANDER_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

namespace expander {
    class Expander {
        public:
            Expander(uint8_t addr);
            void begin(void);
            uint8_t readByte(void);
            void writeByte(uint8_t b);
            void digitalWrite(uint8_t pin, bool val);
            uint8_t digitalRead(uint8_t pin);
            void attachInterrupt(void (*userFunc)(void));

        private:
            uint8_t _addr;
    };
}
#endif
