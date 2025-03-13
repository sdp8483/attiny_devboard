/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>

#include "clock.h"  /* set main clock frequency using platformio.ini fcpu value */

/* Define -------------------------------------------------------------------- */
#define LED_BUILTIN_PORT  PORTA
#define LED_BUILTIN_PIN   PIN3_bm 

/* Macro --------------------------------------------------------------------- */

/* Typedef ------------------------------------------------------------------- */

/* Global -------------------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Main ---------------------------------------------------------------------- */
int main(void) {
    clock_init();   /* setup main clock frequency */

    /* setup LED pin */
    LED_BUILTIN_PORT.OUTCLR = LED_BUILTIN_PIN;
    LED_BUILTIN_PORT.DIRSET = LED_BUILTIN_PIN;

    while(1) {
        LED_BUILTIN_PORT.OUTTGL = LED_BUILTIN_PIN;
        _delay_ms(500);
    }
}