#include "i2c.h"

void I2C_HOST::init(uint32_t baud_rate = I2C_DEFAULT_SCK) {
    TWI0.MBAUD = (uint8_t)I2C_BAUD(baud_rate);
    TWI0.MCTRLA = (1 << TWI_ENABLE_bp);
    TWI0.MCTRLB  = (1 << TWI_FLUSH_bp);
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

uint8_t I2C_HOST::start(uint8_t address, i2c_host_direction direction) {
    TWI0.MADDR = (address << 1) + direction;

    _wait_wif_rif();

    if ((TWI0.MSTATUS & TWI_ARBLOST_bm)) {
        return 0;
    }

    return !(TWI0.MSTATUS & TWI_RXACK_bm);
}

uint8_t I2C_HOST::write(uint8_t data) {
    _wait_wif();
    TWI0.MDATA = data;
    TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
    return (!(TWI0.MSTATUS & TWI_RXACK_bm));
}

void I2C_HOST::stop(void) {
    TWI0.MCTRLB = (TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc);
}

uint8_t I2C_HOST::read(I2C_HOST::i2c_host_read_response read_response) {
    _wait_rif();
    uint8_t data = TWI0.MDATA;

    switch (read_response) {
        case I2C_HOST::i2c_host_read_response::I2C_HOST_READ_ACK:
            TWI0.MSTATUS = (TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc);
            break;

        case I2C_HOST::i2c_host_read_response::I2C_HOST_READ_NACK:
            TWI0.MSTATUS = (TWI_ACKACT_NACK_gc | TWI_MCMD_RECVTRANS_gc);
            break;

        default:
            TWI0.MSTATUS = (TWI_ACKACT_NACK_gc | TWI_MCMD_RECVTRANS_gc);
            break;
    }

    return data;
}

uint8_t I2C_HOST::_wait_wif_rif(void) {
    uint16_t timeout = 0;

    while(!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))) {
        timeout++;

        if (timeout > I2C_TIMEOUT_LIMIT) {
            return 0;
        }
    }

    return 1;
}

uint8_t I2C_HOST::_wait_wif(void) {
    uint16_t timeout = 0;

    while(!(TWI0.MSTATUS & (TWI_WIF_bm))) {
        timeout++;

        if (timeout > I2C_TIMEOUT_LIMIT) {
            return 0;
        }
    }

    return 1;
}

uint8_t I2C_HOST::_wait_rif(void) {
    uint16_t timeout = 0;

    while(!(TWI0.MSTATUS & (TWI_RIF_bm))) {
        timeout++;

        if (timeout > I2C_TIMEOUT_LIMIT) {
            return 0;
        }
    }

    return 1;
}