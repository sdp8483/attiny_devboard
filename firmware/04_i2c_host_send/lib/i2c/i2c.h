#ifndef INC_I2C_H
#define INC_I2C_H

/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Defines ------------------------------------------------------------------- */
#if (BOARD == ATtiny212) || (BOARD == ATtiny212)
/* ATtiny212 and ATtiny412 use PA1 and PA2 as I2C pins*/
#define I2C_PORT            PORTA
#define SDA_PIN             PIN1_bm
#define SCL_PIN             PIN2_bm
#else
#define I2C_PORT            PORTB
#define SDA_PIN             PIN1_bm
#define SCL_PIN             PIN0_bm
#endif

#define I2C_100kHz          100000
#define I2C_400kHz          400000

/* Macros -------------------------------------------------------------------- */
#define I2C_BAUD(F_SCL)     (((float) F_CPU / (2 * (float) F_SCL)) - 5)

/* typedef ------------------------------------------------------------------- */

/* Globals ------------------------------------------------------------------- */

/* External Globals ---------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Class --------------------------------------------------------------------- */
class I2C_HOST {
    public:
        enum i2c_pullup {
            I2C_ENABLE_INTERNAL_PULLUP,
            I2C_DISABLE_INTERNAL_PULLUP
        };

        void begin(uint32_t baud_rate = I2C_100kHz, i2c_pullup pullup_enable =  I2C_DISABLE_INTERNAL_PULLUP);

        void end(void);

        void write(uint8_t address, uint8_t data);
        void write(uint8_t address, char *buffer);
        void write(uint8_t address, uint8_t *buffer, size_t len);

    private:
        void _write(uint8_t address, uint8_t *buffer, size_t len);
};

static I2C_HOST i2c_host;

#endif