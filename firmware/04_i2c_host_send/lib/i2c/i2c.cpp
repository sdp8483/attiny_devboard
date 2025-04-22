#include "i2c.h"

void I2C_HOST::begin(uint32_t baud_rate) {
    /* setup i2c pins */
    I2C_PORT.OUTCLR = (SDA_PIN | SCL_PIN);
    I2C_PORT.DIRSET = (SDA_PIN | SCL_PIN);
    // I2C_PORT.PIN1CTRL |= PORT_PULLUPEN_bm;
    // I2C_PORT.PIN2CTRL |= PORT_PULLUPEN_bm;

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

void I2C_HOST::begin(void) {
    begin(I2C_DEFAULT_BAUD);
}

void I2C_HOST::end(void) {
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    TWI0.MCTRLA &= !(1 << TWI_ENABLE_bp);

    I2C_PORT.DIRCLR = (SDA_PIN | SCL_PIN);
}

void I2C_HOST::write(uint8_t address, uint8_t data) {
    TWI0.MADDR = address << 1;                              /* shift and set read/write bit */
    while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;

    TWI0.MDATA = data;
    while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

void I2C_HOST::write(uint8_t address, uint16_t data) {
    TWI0.MADDR = address << 1;                              /* shift and set read/write bit */
    while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;

    TWI0.MDATA = data >> 8;
    while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    TWI0.MDATA = data & 0xff;
    while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    
    // if (!(TWI0.MSTATUS & TWI_RXACK_bm))
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

void I2C_HOST::write(uint8_t address, char *data) {
    TWI0.MADDR = address << 1;                              /* shift and set read/write bit */
    while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
    TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;

    uint8_t i=0;
    while (data[i] != 0) {
        TWI0.MDATA = data[i];
        while(!(TWI0.MSTATUS & (TWI_WIEN_bm | TWI_RIEN_bm)));   /* wait for write or read interrupt flag */
        i++;
    }
    
    // if (!(TWI0.MSTATUS & TWI_RXACK_bm))
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}