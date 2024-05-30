/*
 * pasco2.h
 *
 *  Created on: Aug 9, 2023
 *      Author: acer
 */

#ifndef DRIVERS_PASCO2_H_
#define DRIVERS_PASCO2_H_

extern uint8_t pasco2_uart_read(uint32_t UART_BASE ,uint8_t reg_addr, uint8_t *data);
extern int32_t pasco2_uart_write(uint32_t UART_BASE, uint8_t reg_addr,uint16_t value, uint8_t * data);
extern void pasco2_set_measurement(uint32_t UART_BASE,uint8_t value1, uint8_t value2, uint8_t * data);
extern void pasco2_uart_init(uint32_t UART_BASE, uint8_t * data);
#endif /* DRIVERS_PASCO2_H_ */
