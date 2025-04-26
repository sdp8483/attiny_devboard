/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>

#include "clock.h"  /* set main clock frequency using platformio.ini fcpu value */
#include "usart.h"
#include "i2c.h"

/* Define -------------------------------------------------------------------- */
#define LED_PORT            PORTA
#define LED_PIN             PIN3_bm

#define I2C_CLIENT_ADDRESS  0x38    /* PCF8574A 8 I/O expander */

/* Macro --------------------------------------------------------------------- */

/* Typedef ------------------------------------------------------------------- */

/* Global -------------------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Main ---------------------------------------------------------------------- */
int main(void) {
    /* setup main clock frequency */
    clock_init();

    /* setup UART for debug printing */
    Serial.begin(BAUDRATE);

    /* setup LED pin */
    LED_PORT.OUTCLR = LED_PIN;
    LED_PORT.DIRSET = LED_PIN;

    i2c_host.init();

    uint8_t data;

    while(1) {
        LED_PORT.OUTSET = LED_PIN;
        i2c_host.start(I2C_CLIENT_ADDRESS, I2C_HOST::i2c_host_direction::I2C_HOST_WRITE);
        data = 0x7F;
        i2c_host.write(&data);
        i2c_host.stop();

        i2c_host.start(I2C_CLIENT_ADDRESS, I2C_HOST::i2c_host_direction::I2C_HOST_READ);
        data = i2c_host.read(I2C_HOST::i2c_host_read_response::I2C_HOST_READ_NACK);
        i2c_host.stop();

        Serial.println(data, HEX);
        _delay_ms(100);

        LED_PORT.OUTCLR = LED_PIN;
        i2c_host.start(I2C_CLIENT_ADDRESS, I2C_HOST::I2C_HOST_WRITE);
        data = 0xFD;
        i2c_host.write(&data);
        i2c_host.stop();

        i2c_host.start(I2C_CLIENT_ADDRESS, I2C_HOST::I2C_HOST_READ);
        data = i2c_host.read(I2C_HOST::i2c_host_read_response::I2C_HOST_READ_NACK);
        i2c_host.stop();
        Serial.println(data, HEX);

        _delay_ms(100);
    }
}