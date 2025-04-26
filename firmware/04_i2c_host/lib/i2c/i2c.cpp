#include "i2c.h"

/* ---------------------------------------------------------------------------
 * @brief TWI peripheral as a host I2C bus
 *   @param sck_frq I2C bus clock speed
 *   @return void                                                              */
void I2C_HOST::init(i2c_sck_freq sck_freq = i2c_sck_freq::I2C_SCK_100kHz) {
    /* setup pins for I2C */
    I2C_PORT.DIRCLR = SCL_PIN;      /* set SCL as tristate input */
    I2C_PORT.OUTCLR = SCL_PIN;

    I2C_PORT.DIRSET = SDA_PIN;      /* set SDA as output */
    I2C_PORT.OUTCLR = SDA_PIN;

    /* disable special modes on SCL and SDA pins */
    #if (BOARD == ATtiny212) || (BOARD == ATtiny212)
    I2C_PORT.PIN1CTRL &= !(1 << PORT_INVEN_bp | 1 << PORT_PULLUPEN_bp | !PORT_ISC_INTDISABLE_gc);
    I2C_PORT.PIN2CTRL &= !(1 << PORT_INVEN_bp | 1 << PORT_PULLUPEN_bp | !PORT_ISC_INTDISABLE_gc);
    #else
    I2C_PORT.PIN0CTRL &= !(1 << PORT_INVEN_bp | 1 << PORT_PULLUPEN_bp | !PORT_ISC_INTDISABLE_gc);
    I2C_PORT.PIN1CTRL &= !(1 << PORT_INVEN_bp | 1 << PORT_PULLUPEN_bp | !PORT_ISC_INTDISABLE_gc);
    #endif

    /* set baud rate using equation 26-3 from datasheet */
    switch(sck_freq) {
        case i2c_sck_freq::I2C_SCK_100kHz:
            /* disable fast mode plus and set clock to 100kHz */
            TWI0.CTRLA &= !(1 << TWI_FMPEN_bp);
            TWI0.MBAUD = (uint8_t)I2C_BAUD(100000);
            break;
        
        case i2c_sck_freq::I2C_SCK_400kHz:
            /* disable fast mode plus and set clock to 400kHz */
            TWI0.CTRLA &= !(1 << TWI_FMPEN_bp);
            TWI0.MBAUD = (uint8_t)I2C_BAUD(400000);
            break;
        
        case i2c_sck_freq::I2C_SCK_1MHz:
            /* enable fast mode plus and set clock to 1MHz */
            TWI0.CTRLA |= (1 << TWI_FMPEN_bp);
            TWI0.MBAUD = (uint8_t)I2C_BAUD(1000000);
            break;

        default:
            break;
    }


    // TWI0.MBAUD = (uint8_t)I2C_BAUD(sck_hz);

    /* clear internal state of TWI peripheral */
    TWI0.MCTRLB  |= (1 << TWI_FLUSH_bp);

    /* enable TWI peripheral */
    TWI0.MCTRLA |=  1 << TWI_ENABLE_bp        /* enable TWI Master */
                  | 0 << TWI_QCEN_bp          /* disable quick command */
                  | 0 << TWI_RIEN_bp          /* disable read interrupt */
                  | 0 << TWI_SMEN_bp          /* disable smart mode */
                  | TWI_TIMEOUT_DISABLED_gc   /* disable bus timeout */
                  | 0 << TWI_WIEN_bp;         /* disable write interrupt */
    
    /* set bus state to idle */
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

/* ---------------------------------------------------------------------------
 * @brief Host puts client address with read/write bit on I2C bus
 *   @param address         7 bit address of client
 *   @param direction       read/write bit
 *   @return I2C bus state                                                     */
I2C_HOST::i2c_state I2C_HOST::start(uint8_t address, i2c_host_direction direction) {
    /* check for bus error */
    if (TWI0.MSTATUS & TWI_BUSERR_bm) {
        return I2C_HOST::i2c_state::I2C_HOST_ERROR_BUS;
    }

    /* place address and read/write bit on bus */
    TWI0.MADDR = (address << 1) + direction;

    /* wait for read or write interrupt flag, return if timeout */
    if (_wait_wif_rif() == I2C_HOST::i2c_state::I2C_WIF_RIF_TIMEOUT) {
        return I2C_HOST::i2c_state::I2C_WIF_RIF_TIMEOUT;
    }

    /* check if client responded with nack */
    if (TWI0.MSTATUS & TWI_RXACK_bm) {
        return I2C_HOST::i2c_state::I2C_CLIENT_NACK;
    }

    /* check if bus arbitration was lost */
    if ((TWI0.MSTATUS & TWI_ARBLOST_bm)) {
        return I2C_HOST::i2c_state::I2C_HOST_ERROR_ARBLOST;
    }

    /* return client response to address */
    if (TWI0.MSTATUS & TWI_RXACK_bm) {
        return I2C_HOST::i2c_state::I2C_CLIENT_NACK;
    } else {
        return I2C_HOST::i2c_state::I2C_CLIENT_ACK;
    }

    /* this line should never execute */
    return I2C_HOST::i2c_state::I2C_UKNOWN_ERROR;
}

/* ---------------------------------------------------------------------------
 * @brief Host puts a byte on I2C bus
 *   @param data        byte of data sent to client
 *   @return I2C bus state                                                     */
I2C_HOST::i2c_state I2C_HOST::write(uint8_t *data) {
    /* check if host has control of the bus */
    if ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_OWNER_gc) {
        return I2C_HOST::i2c_state::I2C_HOST_ERROR_NOT_OWNER;
    }

    /* wait for write interrupt flag, return if timeout */
    if (_wait_wif() == I2C_HOST::i2c_state::I2C_WIF_TIMEOUT) {
        return I2C_HOST::i2c_state::I2C_WIF_TIMEOUT;
    }

    /* place 1 byte of data into host data register */
    TWI0.MDATA = *data;

    /* wait for write interrupt flag, return if timeout */
    if (_wait_wif() == I2C_HOST::i2c_state::I2C_WIF_TIMEOUT) {
        return I2C_HOST::i2c_state::I2C_WIF_TIMEOUT;
    }

    /* return client response to data */
    if (TWI0.MSTATUS & TWI_RXACK_bm) {
        return I2C_HOST::i2c_state::I2C_CLIENT_NACK;
    } else {
        return I2C_HOST::i2c_state::I2C_CLIENT_ACK;
    }

    /* this line should never execute */
    return I2C_HOST::i2c_state::I2C_UKNOWN_ERROR;
}

/* ---------------------------------------------------------------------------
 * @brief Host waits to read a byte on I2C bus
 *   @param read_response   hosts response with ack or nack after receiving a byte of data 
 *   @return I2C bus state                                                     */
// uint8_t I2C_HOST::read(I2C_HOST::i2c_host_response acknack) {
I2C_HOST::i2c_state I2C_HOST::read(uint8_t *data, i2c_host_response acknack = i2c_host_response::I2C_HOST_NACK) {
    /* check if host has control of the bus */
    if ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_OWNER_gc) {
        return I2C_HOST::i2c_state::I2C_HOST_ERROR_NOT_OWNER;
    }

    /* wait for read interrupt flag, return if timeout */
    if (_wait_rif() == I2C_HOST::i2c_state::I2C_RIF_TIMEOUT) {
        return I2C_HOST::i2c_state::I2C_RIF_TIMEOUT;
    }

    /* get data sent from client */
    *data = TWI0.MDATA;

    /* respond to client data */
    switch (acknack) {
        case I2C_HOST::i2c_host_response::I2C_HOST_ACK:
            /* ack client in preparation for more data */
            TWI0.MCTRLB |= (TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc);
            break;

        case I2C_HOST::i2c_host_response::I2C_HOST_NACK:
            /* nack client to prevent more data being sent */
            TWI0.MCTRLB |= (TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc);
            break;

        default:
            /* nack client if switch case is setup wrong */
            TWI0.MCTRLB |= (TWI_ACKACT_NACK_gc | TWI_MCMD_RECVTRANS_gc);
            break;
    }

    // return data;
    return i2c_state::I2C_HOST_OK;
}

/* ---------------------------------------------------------------------------
 * @brief Host issues stop command on I2C bus
 *   @param void 
 *   @return void                                                              */
void I2C_HOST::stop(void) {
    TWI0.MCTRLB |= (TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc);
}

/* ---------------------------------------------------------------------------
 * @brief wait for read or write interrupt flag to be set, timeout if count is exceeded
 *   @param void 
 *   @return i2c bus state                                                     */
I2C_HOST::i2c_state I2C_HOST::_wait_wif_rif(void) {
    /* initialize timeout count */
    uint16_t timeout_count = 0;

    /* poll for read/write interrupt flags to be set */
    while(!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))) {
        /* increment timeout count */
        timeout_count++;

        /* if timeout count exceeds limit break and return */
        if (timeout_count > _max_timeout_count) {
            return I2C_HOST::i2c_state::I2C_WIF_RIF_TIMEOUT;
        }
    }

    /* return with no timeout */
    return I2C_HOST::i2c_state::I2C_NO_TIMEOUT;
}

/* ---------------------------------------------------------------------------
 * @brief Wait for write interrupt flag to be set, timeout if count is exceeded
 *   @param void 
 *   @return i2c bus state                                                     */
I2C_HOST::i2c_state I2C_HOST::_wait_wif(void) {
    /* initialize timeout count */
    uint16_t timeout_count = 0;

    /* poll for write interrupt flags to be set */
    while(!(TWI0.MSTATUS & TWI_WIF_bm)) {
        /* increment timeout count */
        timeout_count++;

        /* if timeout count exceeds limit break and return */
        if (timeout_count > _max_timeout_count) {
            return I2C_HOST::i2c_state::I2C_WIF_TIMEOUT;
        }
    }

    /* return with no timeout */
    return I2C_HOST::i2c_state::I2C_NO_TIMEOUT;
}

/* ---------------------------------------------------------------------------
 * @brief Wait for read interrupt flag to be set, timeout if count is exceeded
 *   @param void 
 *   @return i2c bus state                                                     */
I2C_HOST::i2c_state I2C_HOST::_wait_rif(void) {
    /* initialize timeout count */
    uint16_t timeout_count = 0;

    /* poll for read interrupt flags to be set */
    while(!(TWI0.MSTATUS & TWI_RIF_bm)) {
        /* increment timeout count */
        timeout_count++;

        /* if timeout count exceeds limit break and return */
        if (timeout_count > _max_timeout_count) {
            return I2C_HOST::i2c_state::I2C_RIF_TIMEOUT;
        }
    }

    /* return with no timeout */
    return I2C_HOST::i2c_state::I2C_NO_TIMEOUT;
}