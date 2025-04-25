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

    i2c_host.begin();

    while(1) {
        LED_PORT.OUTSET = LED_PIN;
        i2c_host.write(I2C_CLIENT_ADDRESS, 0x7F);

        uint8_t data = i2c_host.read(I2C_CLIENT_ADDRESS);
        Serial.println(data, HEX);
        _delay_ms(100);

        LED_PORT.OUTCLR = LED_PIN;
        i2c_host.write(I2C_CLIENT_ADDRESS, 0xFD);
        // Serial.println(i2c_host.read(I2C_CLIENT_ADDRESS), HEX);
        _delay_ms(100);
    }
}