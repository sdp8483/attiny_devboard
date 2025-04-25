#ifndef INC_I2C_H
#define INC_I2C_H

// This library is based on this example code
//    https://www.avrfreaks.net/s/topic/a5C3l000000UcBFEA0/t160557?comment=P-1319715

/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>
// #include <stdlib.h>
// #include <string.h>

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
#define I2C_DEFAULT_SCK     I2C_100kHz

#define I2C_TIMEOUT_LIMIT   1000

/* Macros -------------------------------------------------------------------- */
#define I2C_BAUD(F_SCL)     (((float) F_CPU / (2 * (float) F_SCL)) - 5)

/* typedef ------------------------------------------------------------------- */

/* Globals ------------------------------------------------------------------- */

/* External Globals ---------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Class --------------------------------------------------------------------- */
class I2C_HOST {
    public:
        enum i2c_host_direction {
            I2C_HOST_WRITE = 0,
            I2C_HOST_READ = 1
        };

        enum i2c_host_read_response {
            I2C_HOST_READ_ACK,
            I2C_HOST_READ_NACK
        };

        enum i2c_host_states {
            I2C_HOST_OK,
            I2C_HOST_ERROR_TIMEOUT
        };

        void init(uint32_t baud_rate = I2C_DEFAULT_SCK);

        uint8_t start(uint8_t address, i2c_host_direction direction);
        uint8_t write(uint8_t data);
        uint8_t read(i2c_host_read_response read_response);
        void stop(void);

    private:
        uint8_t _wait_wif_rif(void);    /* wait for read/write interrupt flag to be set with timeout */
        uint8_t _wait_wif(void);        /* wait for write interrupt flag to be set with timeout */
        uint8_t _wait_rif(void);        /* wait for read interrupt flag to be set with timeout */
};

static I2C_HOST i2c_host;

#endif