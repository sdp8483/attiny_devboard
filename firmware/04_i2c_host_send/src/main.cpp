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

#define I2C_CLIENT_ADDRESS  0x8

/* Macro --------------------------------------------------------------------- */

/* Typedef ------------------------------------------------------------------- */

/* Global -------------------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Main ---------------------------------------------------------------------- */
int main(void) {
    /* setup main clock frequency */
    clock_init();

    /* setup LED pin */
    LED_PORT.OUTCLR = LED_PIN;
    LED_PORT.DIRSET = LED_PIN;

    i2c_host.begin();

    uint32_t val = 0xBA5EBA11;
    uint8_t v = 0x42;
    char str[] = "Hello World!\n";

    while(1) {
        LED_PORT.OUTSET = LED_PIN;

        i2c_host.write(I2C_CLIENT_ADDRESS, (uint8_t *) &val, sizeof(val));

        i2c_host.write(I2C_CLIENT_ADDRESS, v++);

        i2c_host.write(I2C_CLIENT_ADDRESS, str);

        LED_PORT.OUTCLR = LED_PIN;
        _delay_ms(500);
    }
}