/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdint.h>

#include "clock.h"  /* set main clock frequency using platformio.ini fcpu value */
#include "usart.h"

/* Define -------------------------------------------------------------------- */
#define LED_PORT        PORTA
#define LED_PIN         PIN3_bm 

#define BLINK_DELAY_ms  500

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

    Serial.begin(BAUD); /* setup uart */
    Serial.println("Hello World!");

    uint16_t blink_count = 0;

    while(1) {
        LED_PORT.OUTSET = LED_PIN;

        Serial.print("blink ");
        Serial.println(++blink_count);

        _delay_ms(BLINK_DELAY_ms);

        LED_PORT.OUTCLR = LED_PIN;
        _delay_ms(BLINK_DELAY_ms);
    }
}