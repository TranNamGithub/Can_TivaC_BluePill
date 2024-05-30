/*
 * i2c_slave.h
 *
 *  Created on: Nov 27, 2022
 *      Author: Nam
 */

#ifndef DRIVES_I2C_SLAVE_H_
#define DRIVES_I2C_SLAVE_H_

//extern void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...);
//extern void I2CSendString(uint32_t slave_addr, char array[]);
//extern uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg);
extern void I2C0_Init(void);
extern void I2C0_Send(uint8_t device_address, uint8_t device_data);
extern void I2C0_sendchar(uint8_t device_address, uint8_t *device_data, uint8_t lenght);
extern void I2C0_Read(uint8_t device_address, uint8_t *received_data);
extern void I2C0_SendTwoBytes(uint8_t device_address, uint8_t device_data1, uint8_t device_data2);
#endif /* DRIVES_I2C_SLAVE_H_ */
