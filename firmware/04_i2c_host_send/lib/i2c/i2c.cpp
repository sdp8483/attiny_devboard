#include "i2c.h"

void I2C_HOST::begin(uint32_t baud_rate = I2C_100kHz, i2c_pullup pullup_enable =  I2C_DISABLE_INTERNAL_PULLUP) {
    /* setup i2c pins */
    I2C_PORT.OUTCLR = (SDA_PIN | SCL_PIN);
    I2C_PORT.DIRSET = (SDA_PIN | SCL_PIN);

    if (pullup_enable == I2C_ENABLE_INTERNAL_PULLUP) {
        I2C_PORT.PIN1CTRL |= PORT_PULLUPEN_bm;

        #if (BOARD == ATtiny212) || (BOARD == ATtiny212)
        I2C_PORT.PIN2CTRL |= PORT_PULLUPEN_bm;
        #else
        I2C_PORT.PIN0CTRL |= PORT_PULLUPEN_bm;
        #endif

    }

    /* setup I2C */
    TWI0.MBAUD = (uint8_t) I2C_BAUD(baud_rate);

    if (baud_rate >= 1000000) {
        TWI0.CTRLA |= 1 << TWI_FMPEN_bp;        /* enable fast mode */
    }

    TWI0.MCTRLA |=  1 << TWI_ENABLE_bp          /* enable TWI Master */
                    | 0 << TWI_QCEN_bp          /* disable quick command */
                    | 0 << TWI_RIEN_bp          /* disable read interrupt */
                    | 0 << TWI_SMEN_bp          /* disable smart mode */
                    | TWI_TIMEOUT_DISABLED_gc   /* disable bus timeout */
                    | 0 << TWI_WIEN_bp;         /* disable write interrupt */
    
    TWI0.MCTRLB |= TWI_FLUSH_bm;
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
    TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);  /* reset module */
}

void I2C_HOST::end(void) {
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    TWI0.MCTRLA &= !(1 << TWI_ENABLE_bp);

    I2C_PORT.DIRCLR = (SDA_PIN | SCL_PIN);
}

void I2C_HOST::_write(uint8_t address, uint8_t *buffer, size_t len) {
    TWI0.MADDR = address << 1;                              /* shift and set read/write bit */
    while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;

    for (uint8_t i=0; i<len; i++) {
        TWI0.MDATA = *buffer++;
        while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    }
    
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

void I2C_HOST::write(uint8_t address, uint8_t data) {
    _write(address, &data, 1);
}

void I2C_HOST::write(uint8_t address, char *buffer) {
    _write(address, (uint8_t *) buffer, strlen(buffer));
}

void I2C_HOST::write(uint8_t address, uint8_t *buffer, size_t len) {
    _write(address, buffer, len);
}