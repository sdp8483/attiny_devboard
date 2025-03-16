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

#define PWM_PER         1024

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

    /* setup TCA0 */
    TCA0.SINGLE.CTRLA |= (TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm);
    TCA0.SINGLE.CTRLB |= (TCA_SINGLE_CMP0_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc);
    TCA0.SINGLE.PER = PWM_PER;
    TCA0.SINGLE.CMP0 = 0;

    /* setup uart */
    Serial.begin(BAUDRATE); 
    Serial.println("Hello World!");

    while(1) {
        Serial.println("Shine");
        for (int16_t i=100; i<PWM_PER; i+=10) {
            TCA0.SINGLE.CMP0 = i;
            _delay_ms(10);
        }
        
        Serial.println("Fade");
        for (int16_t i=PWM_PER; i>100; i-=10) {
            TCA0.SINGLE.CMP0 = i;
            _delay_ms(10);
        }
    }
}