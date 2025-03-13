/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>

#include "clock.h"  /* set main clock frequency using platformio.ini fcpu value */

/* Define -------------------------------------------------------------------- */
#define LED_PORT  PORTA
#define LED_PIN   PIN3_bm 

/* Macro --------------------------------------------------------------------- */

/* Typedef ------------------------------------------------------------------- */

/* Global -------------------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Main ---------------------------------------------------------------------- */
int main(void) {
    clock_init();   /* setup main clock frequency */

    /* setup LED pin */
    LED_PORT.OUTCLR = LED_PIN;
    LED_PORT.DIRSET = LED_PIN;

    while(1) {
        LED_PORT.OUTTGL = LED_PIN;
        _delay_ms(500);
    }
}