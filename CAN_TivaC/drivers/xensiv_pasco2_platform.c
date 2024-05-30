/*
 * xensiv_pasco2_platform.c
 *
 *  Created on: Aug 4, 2023
 *      Author: acer
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "drivers/xensiv_pasco2.h"

int32_t xensiv_pasco2_plat_i2c_transfer(void *ctx, uint16_t dev_addr, const uint8_t *tx_buffer, size_t tx_len, uint8_t *rx_buffer, size_t rx_len) {
    uint32_t i2cBase = (uint32_t)ctx;

    I2CMasterSlaveAddrSet(i2cBase, dev_addr, false);

    I2CMasterDataPut(i2cBase, tx_buffer[0]);
    I2CMasterControl(i2cBase, I2C_MASTER_CMD_BURST_SEND_START);
    while (I2CMasterBusy(i2cBase)) {}
    size_t i = 1;
    for (i = 1; i < tx_len; i++) {
        I2CMasterDataPut(i2cBase, tx_buffer[i]);
        I2CMasterControl(i2cBase, I2C_MASTER_CMD_BURST_SEND_CONT);
        while (I2CMasterBusy(i2cBase)) {}
    }

    if (rx_buffer && rx_len > 0) {
        I2CMasterSlaveAddrSet(i2cBase, dev_addr, true);

        I2CMasterControl(i2cBase, I2C_MASTER_CMD_BURST_RECEIVE_START);
        while (I2CMasterBusy(i2cBase)) {}
        size_t i = 0;
        for (i = 0; i < rx_len - 1; i++) {
            I2CMasterControl(i2cBase, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            while (I2CMasterBusy(i2cBase)) {}
            rx_buffer[i] = I2CMasterDataGet(i2cBase);
        }

        I2CMasterControl(i2cBase, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        while (I2CMasterBusy(i2cBase)) {}
        rx_buffer[rx_len - 1] = I2CMasterDataGet(i2cBase);
    }

    return XENSIV_PASCO2_OK;
}

int32_t xensiv_pasco2_plat_uart_read(void *ctx, uint8_t *data, size_t len) {
    uint32_t uartBase = (uint32_t)ctx;
    size_t i = 0;
   uint32_t timeout = 0;
    for (i = 0; i < len; i++) {
        while (!UARTCharsAvail(UART1_BASE)) {
            // Wait until a character is available

            ++timeout;
            if(timeout>500) {
                break;
            }
        }
        data[i] = UARTCharGetNonBlocking(UART1_BASE);
    }

    return XENSIV_PASCO2_OK;
}

int32_t xensiv_pasco2_plat_uart_write(void *ctx, uint8_t *data, size_t len) {
    uint32_t uartBase = (uint32_t)ctx;
    size_t i = 0;
    for (i = 0; i < len; i++) {
        while (!UARTSpaceAvail(UART1_BASE)) {
            // Wait until there is space in the transmit buffer
        }
        UARTCharPutNonBlocking(UART1_BASE, data[i]);
    }

    return XENSIV_PASCO2_OK;
}

void xensiv_pasco2_plat_delay(uint32_t ms)
{
    SysCtlDelay(10*ms * (SysCtlClockGet() / 3000)); // Adjust the divider based on your clock frequency
}

uint16_t xensiv_pasco2_plat_htons(uint16_t x)
{
    return ((uint16_t)(((x & 0x00ffU) << 8) | ((x & 0xff00U) >> 8)));
}

void xensiv_pasco2_plat_assert(int expr)
{
        // Handle error, e.g., print an error message and halt
    (void)expr;
}
