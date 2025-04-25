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

    _wait_wif();
    _rxack();
    _wait_clkhold();

    for (uint8_t i=0; i<len; i++) {
        TWI0.MDATA = *buffer++;
        _wait_wif();
    }
    
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

I2C_HOST::i2c_return_t I2C_HOST::_wait_wif(void) {
    i2c_return_t ret = I2C_WIF;

    uint16_t timeout = 0;

    /* poll for write interrupt flag, timeout if not set */
    while(!(TWI0.MSTATUS & TWI_WIF_bm)) {
        timeout++;

        #ifdef I2C_DEBUG_PRINT
        Serial.print(timeout);
        Serial.println(" wait for WIF");
        #endif

        if (timeout > I2C_TIMEOUT_LIMIT) {
            ret = I2C_TIMEOUT;

            #ifdef I2C_DEBUG_PRINT
            Serial.println("WIF timeout");
            #endif

            break;
        }
    }

    return ret;
}

I2C_HOST::i2c_return_t I2C_HOST::_wait_rif(void) {
    i2c_return_t ret = I2C_RIF;

    uint16_t timeout = 0;

    /* poll for read interrupt flag, timeout if not set */
    while(!(TWI0.MSTATUS & TWI_RIF_bm)) {
        timeout++;

        #ifdef I2C_DEBUG_PRINT
        Serial.print(timeout);
        Serial.println(" wait for RIF");
        #endif

        if (timeout > I2C_TIMEOUT_LIMIT) {
            ret = I2C_TIMEOUT;

            #ifdef I2C_DEBUG_PRINT
            Serial.println("RIF timeout");
            #endif

            break;
        }
    }

    return ret;
}

I2C_HOST::i2c_return_t I2C_HOST::_rxack(void) {
    i2c_return_t ret = I2C_ACK;

    /* if ACKACT is 1 client signals NACK*/
    if (TWI0.MSTATUS & TWI_RXACK_bm) {
        ret = I2C_NACK;

        #ifdef I2C_DEBUG_PRINT
        Serial.println("NACK");
        #endif
    }

    return ret;
}

I2C_HOST::i2c_return_t I2C_HOST::_wait_clkhold(void) {
    i2c_return_t ret = I2C_CLKHOLD;

    uint16_t timeout = 0;

    /* poll for write interrupt flag, timeout if not set */
    while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm)) {
        timeout++;

        #ifdef I2C_DEBUG_PRINT
        Serial.print(timeout);
        Serial.println(" wait for CLKHOLD");
        #endif

        if (timeout > I2C_TIMEOUT_LIMIT) {
            ret = I2C_TIMEOUT;

            #ifdef I2C_DEBUG_PRINT
            Serial.println("CLKHOLD timeout");
            #endif

            break;
        }
    }

    return ret;
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

uint8_t I2C_HOST::read(uint8_t address) {
    TWI0.MADDR = (address << 1) + 1;                       /* shift and set read/write bit */
    _rxack();

    if (_wait_rif() == I2C_TIMEOUT) {
        TWI0.MCTRLB |= (0 << TWI_ACKACT_bp | TWI_MCMD_STOP_gc);
        return;
    }

    _wait_clkhold();

    uint8_t data = TWI0.MDATA;

    TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
    
    return data;
}