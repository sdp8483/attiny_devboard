/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>

#include "clock.h"  /* set main clock frequency using platformio.ini fcpu value */
#include "usart.h"
#include "i2c.h"

/* Define -------------------------------------------------------------------- */
#define LED_PORT        PORTA
#define LED_PIN         PIN3_bm

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

    /* send Ndata hint */
    uint16_t data = 0xDEAD;
    uint8_t *pt = (uint8_t *) &data;
    *pt++;

    while(1) {
        i2c_host.write(8, "BASE");
        _delay_ms(500);
    }
}