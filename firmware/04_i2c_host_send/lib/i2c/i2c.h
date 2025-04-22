#ifndef INC_I2C_H
#define INC_I2C_H

/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>
#include <stdlib.h>

/* Defines ------------------------------------------------------------------- */
#define I2C_PORT            PORTA
#define SDA_PIN             PIN1_bm
#define SCL_PIN             PIN2_bm
#define I2C_DEFAULT_BAUD    100000

/* Macros -------------------------------------------------------------------- */
#define I2C_BAUD(F_SCL)     (((float) F_CPU / (2 * (float) F_SCL)) - 5)

/* typedef ------------------------------------------------------------------- */

/* Globals ------------------------------------------------------------------- */

/* External Globals ---------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Class --------------------------------------------------------------------- */
class I2C_HOST {
    public:
        void begin(uint32_t baud_rate);
        void begin(void);

        void end(void);

        void write(uint8_t address, uint8_t data);
        void write(uint8_t address, uint16_t data);
        void write(uint8_t address, char *data);

    private:
};

static I2C_HOST i2c_host;

#endif