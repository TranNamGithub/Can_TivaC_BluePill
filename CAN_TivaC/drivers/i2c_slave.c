///*
// * i2c_slave.c
// *
// *  Created on: Nov 27, 2022
// *      Author: Nam
// */
//
//#include <stdarg.h>
//#include <stdbool.h>
//#include <stdint.h>
//#include "inc/hw_i2c.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_types.h"
//#include "inc/hw_gpio.h"
//#include "driverlib/i2c.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/gpio.h"
//#include "driverlib/pin_map.h"
//#include "i2c_slave.h"
//
////initialize I2C module 0
////Slightly modified version of TI's example code
//
////sends an I2C command to the specified slave
//void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...)
//{
//    // Tell the master module what address it will place on the bus when
//    // communicating with the slave.
//    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
//
//    //stores list of variable number of arguments
//    va_list vargs;
//
//    //specifies the va_list to "open" and the last fixed argument
//    //so vargs knows where to start looking
//    va_start(vargs, num_of_args);
//
//    //put data to be sent into FIFO
//    I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
//
//    //if there is only one argument, we only need to use the
//    //single send I2C function
//    if(num_of_args == 1)
//    {
//        //Initiate send of data from the MCU
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
//
//        // Wait until MCU is done transferring.
//        while(I2CMasterBusy(I2C0_BASE));
//
//        //"close" variable argument list
//        va_end(vargs);
//    }
//
//    //otherwise, we start transmission of multiple bytes on the
//    //I2C bus
//    else
//    {
//        //Initiate send of data from the MCU
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
//
//        // Wait until MCU is done transferring.
//        while(I2CMasterBusy(I2C0_BASE));
//
//        //send num_of_args-2 pieces of data, using the
//        //BURST_SEND_CONT command of the I2C module
//        uint8_t i;
//        for(i = 1; i < (num_of_args - 1); i++)
//        {
//            //put next piece of data into I2C FIFO
//            I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
//            //send next data that was just placed into FIFO
//            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
//
//            // Wait until MCU is done transferring.
//            while(I2CMasterBusy(I2C0_BASE));
//        }
//
//        //put last piece of data into I2C FIFO
//        I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
//        //send next data that was just placed into FIFO
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
//        // Wait until MCU is done transferring.
//        while(I2CMasterBusy(I2C0_BASE));
//
//        //"close" variable args list
//        va_end(vargs);
//    }
//}
//
////sends an array of data via I2C to the specified slave
//void I2CSendString(uint32_t slave_addr, char array[])
//{
//    // Tell the master module what address it will place on the bus when
//    // communicating with the slave.
//    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
//
//    //put data to be sent into FIFO
//    I2CMasterDataPut(I2C0_BASE, array[0]);
//
//    //if there is only one argument, we only need to use the
//    //single send I2C function
//    if(array[1] == '\0')
//    {
//        //Initiate send of data from the MCU
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
//
//        // Wait until MCU is done transferring.
//        while(I2CMasterBusy(I2C0_BASE));
//    }
//
//    //otherwise, we start transmission of multiple bytes on the
//    //I2C bus
//    else
//    {
//        //Initiate send of data from the MCU
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
//
//        // Wait until MCU is done transferring.
//        while(I2CMasterBusy(I2C0_BASE));
//
//        //initialize index into array
//        uint8_t i = 1;
//
//        //send num_of_args-2 pieces of data, using the
//        //BURST_SEND_CONT command of the I2C module
//        while(array[i + 1] != '\0')
//        {
//            //put next piece of data into I2C FIFO
//            I2CMasterDataPut(I2C0_BASE, array[i++]);
//
//            //send next data that was just placed into FIFO
//            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
//
//            // Wait until MCU is done transferring.
//            while(I2CMasterBusy(I2C0_BASE));
//        }
//
//        //put last piece of data into I2C FIFO
//        I2CMasterDataPut(I2C0_BASE, array[i]);
//
//        //send next data that was just placed into FIFO
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
//
//        // Wait until MCU is done transferring.
//        while(I2CMasterBusy(I2C0_BASE));
//    }
//}
//
////read specified register on slave device
//uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg)
//{
//    //specify that we are writing (a register address) to the
//    //slave device
//    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
//
//    //specify register to be read
//    I2CMasterDataPut(I2C0_BASE, reg);
//
//    //send control byte and register address byte to slave device
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
//
//    //wait for MCU to finish transaction
//    while(I2CMasterBusy(I2C0_BASE));
//
//    //specify that we are going to read from slave device
//    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);
//
//    //send control byte and read from the register we
//    //specified
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
//
//    //wait for MCU to finish transaction
//    while(I2CMasterBusy(I2C0_BASE));
//
//    //return data pulled from the specified register
//    return I2CMasterDataGet(I2C0_BASE);
//}



#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "i2c_slave.h"
#include "driverlib/interrupt.h"
void I2C0_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    //HWREG(I2C0_BASE + I2C_O_MCR) |= 0x01;
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
}

void I2C0_Send(uint8_t device_address, uint8_t device_data)
{
    // Determine Slave address
    // false: transmit Master --> Slave
    // true:  receive  Master <-- Slave
    I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);
    // Put data
    I2CMasterDataPut(I2C0_BASE, device_data);
    // Transmit data
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    // Wait until finish
    while(I2CMasterBusBusy(I2C0_BASE));
}
void I2C0_sendchar(uint8_t device_address, uint8_t *device_data, uint8_t lenght){

    I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);
    I2CMasterDataPut(I2C0_BASE, device_data[0]);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));

    uint8_t index;
    for(index = 1; index<lenght-1; index++){
        I2CMasterDataPut(I2C0_BASE, device_data[index]);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        while(I2CMasterBusy(I2C0_BASE));
    }

    I2CMasterDataPut(I2C0_BASE, device_data[lenght-1]);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));
}
void I2C0_Read(uint8_t device_address, uint8_t *received_data)
{
    // Send START condition
    I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusBusy(I2C0_BASE));

    // Change to receive mode (R/W bit = 1)
    I2CMasterSlaveAddrSet(I2C0_BASE, device_address, true);

    // Read data
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while (I2CMasterBusBusy(I2C0_BASE));
    *received_data = I2CMasterDataGet(I2C0_BASE);

    // Send STOP condition
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusBusy(I2C0_BASE));
}
void I2C0_SendTwoBytes(uint8_t device_address, uint8_t device_data1, uint8_t device_data2)
{
    // Determine Slave address
    // false: transmit Master --> Slave
    // true:  receive  Master <-- Slave
    I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);

    // Put data1
    I2CMasterDataPut(I2C0_BASE, device_data1);
    // Transmit data1
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    // Wait until finish
    while (I2CMasterBusBusy(I2C0_BASE));

    // Put data2
    I2CMasterDataPut(I2C0_BASE, device_data2);
    // Transmit data2
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    // Wait until finish
    while (I2CMasterBusBusy(I2C0_BASE));
}
void I2C0_Write(unsigned char uiSlave_add, unsigned char *ucData, uint16_t uiCount)
{
    unsigned int temp = 1;
    IntMasterDisable();
    // Set the slave address and setup for a transmit operation.
    I2CMasterSlaveAddrSet(I2C0_BASE, uiSlave_add, false);
    // Place the address to be written in the data register.
    I2CMasterDataPut(I2C0_BASE, ucData[0]);
    while (I2CMasterBusy(I2C0_BASE));
    if (uiCount == 0)
        // Initiate send of character from Master to Slave
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    else
    {
        // Start the burst cycle, writing the address as the first byte.
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        // Write the next byte to the data register.
        while (I2CMasterBusy(I2C0_BASE));
        I2CMasterDataPut(I2C0_BASE, ucData[0]);
        for( ; temp < uiCount; temp++)        //Loop to send data if not the last byte
        {
            // Continue the burst write.
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            // Write the next byte to the data register.
            while (I2CMasterBusy(I2C0_BASE));
            I2CMasterDataPut(I2C0_BASE, ucData[temp]);
        }
        // Finish the burst write.
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    }
    while (I2CMasterBusy(I2C0_BASE));
    IntMasterEnable();
}
