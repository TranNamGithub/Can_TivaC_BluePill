/*
 * pasco2.c
 *
 *  Created on: Aug 9, 2023
 *      Author: acer
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
static inline uint8_t xensiv_pasco2_digit_to_ascii(uint8_t digit)
{
    if (digit < 10U)
    {
        return (uint8_t)(digit + 0x30U);
    }
    else
    {
        return (uint8_t)(digit + 0x37U);
    }
}


static inline uint8_t xensiv_pasco2_ascii_to_digit(uint8_t ascii)
{
    if (ascii < (uint8_t)'A')
    {
        return (uint8_t)(ascii - (uint8_t)'0');
    }
    else
    {
        return (uint8_t)(10u + (uint8_t)(ascii - (uint8_t)'A'));
    }
}



uint8_t pasco2_uart_read(uint32_t UART_BASE ,uint8_t reg_addr, uint8_t *data)
{
    uint8_t uart_buf[5] =
    {
        (uint8_t)'r',
        (uint8_t)',',
        xensiv_pasco2_digit_to_ascii((reg_addr & (uint8_t)0xF0) >> 4U),
        xensiv_pasco2_digit_to_ascii(reg_addr & (uint8_t)0x0F),
        (uint8_t)'\n'
    };
    uint8_t i;
    for(i = 0; i<5;i++)
    {
        while (!UARTSpaceAvail(UART_BASE)) {
            // Wait until there is space in the transmit buffer
        }
        UARTCharPutNonBlocking(UART_BASE, uart_buf[i]);
    }
    SysCtlDelay(100*(SysCtlClockGet()/3000));
    return (uint8_t)((xensiv_pasco2_ascii_to_digit(data[0])))*(0x10U)+(uint8_t)((xensiv_pasco2_ascii_to_digit(data[1])));
}


int32_t pasco2_uart_write(uint32_t UART_BASE, uint8_t reg_addr,uint8_t value, uint8_t * data)
{

    uint8_t uart_buf[8] =
    {
        (uint8_t)'w',
        (uint8_t)',',
        xensiv_pasco2_digit_to_ascii((reg_addr & 0xF0U) >> 4U), xensiv_pasco2_digit_to_ascii(reg_addr & 0x0FU),
        (uint8_t)',',
        xensiv_pasco2_digit_to_ascii((value & 0xF0U) >> 4U), xensiv_pasco2_digit_to_ascii(value & 0x0FU),
        (uint8_t)'\n'
    };

    uint8_t i;
    for(i = 0; i<8;i++)
    {
        while (!UARTSpaceAvail(UART_BASE)) {
            // Wait until there is space in the transmit buffer
        }
        UARTCharPutNonBlocking(UART_BASE, uart_buf[i]);
    }
    SysCtlDelay(100*(SysCtlClockGet()/3000));
    if(data[0] ==6 && data[1]==10){
        return 0;
    }
    else{
        return 1;
    }
}


void pasco2_uart_init(uint32_t UART_BASE, uint8_t * data){

    // check id
    pasco2_uart_read(UART_BASE, 0x00, data);
    // check status
    pasco2_uart_read(UART_BASE, 0x01, data);
    // idle mode
    pasco2_uart_write(UART_BASE, 0x04, 0x0000, data);

}

void pasco2_set_measurement(uint32_t UART_BASE,uint8_t value1, uint8_t value2, uint8_t * data){

    //set measurement rate to 10s value hex
    pasco2_uart_write(UART_BASE, 0x02, value1, data);
    pasco2_uart_write(UART_BASE, 0x03, value2, data);
    SysCtlDelay(1000);
    //check to write successfully
    if(pasco2_uart_read(UART_BASE, 0x03, data)!=value2){
        pasco2_uart_write(UART_BASE, 0x02, value1, data);
        pasco2_uart_write(UART_BASE, 0x03, value2, data);
    }

    //configure continous mode
    pasco2_uart_write(UART1_BASE, 0x04, 0x02, data);
}

