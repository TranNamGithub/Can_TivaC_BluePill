#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/can.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "drivers/pasco2.h"
#include "SSDD1306/test.h"
#include "SSDD1306/OLED_LCD_SSD1306.h"
#include "drivers/i2c_slave.h"
#include "SSDD1306/fonts.h"
#include "SSDD1306/ConfCO2.h"
#include "drivers/configure_co2.h"
/*difine of CAN */
#define CAN0RXID                0x103
#define RXOBJECT                1
#define CAN0TXID                0x446
#define TXOBJECT                2
volatile uint32_t g_ui32RXMsgCount = 0;
volatile uint32_t g_ui32TXMsgCount = 0;
volatile bool g_bRXFlag = false;
volatile bool g_ErrFlag = 0;
uint8_t g_ui8RXMsgData[8];
uint8_t g_ui8TXMsgData[4];
tCANMsgObject g_sCAN0RxMessage;
tCANMsgObject g_sCAN0TxMessage;
bool state_fan;
bool state_door;

/*difine of led*/
#define LED_PORT_BASE GPIO_PORTF_BASE
#define LED_PIN GPIO_PIN_1

/* define of SSD1306 OlED */
SSD1306_Name SSD1306;

/* define of SENSOR CO2 UART*/
#define MAX_RX_BUFFER_SIZE 4
uint8_t rxBuffer[MAX_RX_BUFFER_SIZE];
uint32_t rxBufferIndex = 0;
uint8_t value1;
uint8_t value2;
uint16_t result;
uint8_t set_mes_and_limit[4];
char Value_Co2_Oled[4];
uint8_t check_conf_sucess;
/* End define*/
void LED_Toggle(void)
{
    GPIOPinWrite(LED_PORT_BASE, LED_PIN, GPIOPinRead(LED_PORT_BASE, LED_PIN) ^ LED_PIN);
    SysCtlDelay(1000000);
}

void LED_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(LED_PORT_BASE, LED_PIN);
}

void UARTIntHandler(void)
{
    uint32_t ui32Status;

    ui32Status = UARTIntStatus(UART1_BASE, true); //get interrupt status

    UARTIntClear(UART1_BASE, ui32Status); //clear the asserted interrupts

    if (ui32Status & (UART_INT_RX | UART_INT_RT))
      {
          while (UARTCharsAvail(UART1_BASE)) // Loop while there are chars
          {
              // Read and discard received characters
              char receivedChar = UARTCharGetNonBlocking(UART1_BASE);
              // Store received character in the buffer
              rxBuffer[rxBufferIndex++] = receivedChar;

              // Check if the buffer is full
              if (rxBufferIndex >= MAX_RX_BUFFER_SIZE)
              {
                  // Handle buffer overflow here, if needed
                  // For example, reset rxBufferIndex to 0
                  rxBufferIndex = 0;
              }
          }
          rxBufferIndex = 0;
      }
}

void CAN0IntHandler(void)
{
    uint32_t ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
    LED_Toggle();

    if (ui32Status == CAN_INT_INTID_STATUS)
    {
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        g_ErrFlag = true;
    }
    else if (ui32Status == RXOBJECT)
    {
        CANIntClear(CAN0_BASE, RXOBJECT);
        g_ui32RXMsgCount++;
        g_bRXFlag = true;
        g_ErrFlag = false;
    }
    else if (ui32Status == TXOBJECT)
    {
        CANIntClear(CAN0_BASE, TXOBJECT);
        g_ui32TXMsgCount++;
        g_ErrFlag = 0;
        // Xử lý ngắt TXOBJECT ở đây nếu cần thiết
    }
    else
    {
        //UARTprintf("Unexpected CAN bus interrupt\n");
    }
}


void ConfigureCAN(void)
{
    // Set up CAN0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // enable CAN1 GPIO peripheral
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);
    CANIntRegister(CAN0_BASE, CAN0IntHandler); // use dynamic vector table allocation
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    IntEnable(INT_CAN0);
    CANEnable(CAN0_BASE);

    // Set up the receive message object
    g_sCAN0RxMessage.ui32MsgID = 0;
    g_sCAN0RxMessage.ui32MsgIDMask = 0;
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = sizeof(g_ui8RXMsgData);
    CANMessageSet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);

    // Set up the transimit message object
    g_sCAN0TxMessage.ui32MsgID = 0;
    g_sCAN0TxMessage.ui32MsgIDMask = 0;
    g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    g_sCAN0TxMessage.ui32MsgLen = sizeof(g_ui8TXMsgData);
    g_sCAN0TxMessage.pui8MsgData = g_ui8TXMsgData;

}


Configure_Uart1()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTIntRegister(UART1_BASE, UARTIntHandler);

    IntEnable(INT_UART1); //enable the UART interrupt
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and RX  timeout interrupts
}


void pasco2set_measurement(uint8_t value_h, uint8_t value_l ){
    // check id
    pasco2_uart_read(UART1_BASE, 0x00, rxBuffer);
    // check status
    pasco2_uart_read(UART1_BASE, 0x01, rxBuffer);
    // idle mode
    pasco2_uart_write(UART1_BASE, 0x04, 0x0000, rxBuffer);
    //set measurement rate to 10s value hex
    pasco2_uart_write(UART1_BASE, 0x02, value_h, rxBuffer);
    pasco2_uart_write(UART1_BASE, 0x03, value_l, rxBuffer);
    SysCtlDelay(1000);
    //check to write successfully
    while(pasco2_uart_read(UART1_BASE, 0x03, rxBuffer)!=value_l){
        pasco2_uart_write(UART1_BASE, 0x02, 0x00, rxBuffer);
        pasco2_uart_write(UART1_BASE, 0x03, 0x0A, rxBuffer);
    }
    pasco2_uart_write(UART1_BASE, 0x04, 0x02, rxBuffer);
}

int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                   SYSCTL_OSC_MAIN);
    //i2c and OLED
    //I2C0_Init();
    //Switch_INT_init();
    //SSD1306_Init(&SSD1306);

    //init led
    LED_Init();

    // configure uart1 read and write senso co2
    Configure_Uart1();

    // configure can
    ConfigureCAN();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2
    IntMasterEnable(); //enable processor interrupts
    SysCtlDelay(2000000);
    // configure default senco2
    // check id
    pasco2_uart_read(UART1_BASE, 0x00, rxBuffer);
    LED_Toggle();
    // check status
    pasco2_uart_read(UART1_BASE, 0x01, rxBuffer);
    LED_Toggle();
    // idle mode
    pasco2_uart_write(UART1_BASE, 0x04, 0x0000, rxBuffer);
    //set measurement rate to 10s value hex
    pasco2_uart_write(UART1_BASE, 0x02, 0x00, rxBuffer);
    pasco2_uart_write(UART1_BASE, 0x03, 0x0A, rxBuffer);
    SysCtlDelay(1000);
    //check to write successfully
    while(pasco2_uart_read(UART1_BASE, 0x03, rxBuffer)!=0x0A){
        pasco2_uart_write(UART1_BASE, 0x02, 0x00, rxBuffer);
        pasco2_uart_write(UART1_BASE, 0x03, 0x0A, rxBuffer);
    }
    pasco2_uart_write(UART1_BASE, 0x04, 0x02, rxBuffer);
    // configure default senco2

    // show to oled
    SSD1306_DrawBitmap(&SSD1306, 0, 0, gImage_ConfCO2, 132, 64, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(&SSD1306, 45, 5);
    convertAndStore(result, Value_Co2_Oled);
    SSD1306_Puts(&SSD1306, Value_Co2_Oled, &Font_11x18, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(&SSD1306, 90, 5);
    SSD1306_Puts(&SSD1306, "ppm", &Font_11x18, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(&SSD1306, 0, 40);
    SSD1306_Puts(&SSD1306, "Fan State : OFF", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(&SSD1306, 0, 52);
    SSD1306_Puts(&SSD1306, "Door State : OFF", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_UpdateScreen(&SSD1306);
    // show to oled

    while (1)
    {
       // configure pasco2_set_measurement and check configure to sucess
       check_conf_sucess = COnfigure_CO2(set_mes_and_limit);
        if(check_conf_sucess ==1){
            g_ui8TXMsgData[0] = 0;
            g_ui8TXMsgData[1] = 0;
            CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_TX); // send as msg object 1
            pasco2set_measurement(set_mes_and_limit[0], set_mes_and_limit[1]);
            SSD1306_DrawBitmap(&SSD1306, 0, 0, gImage_ConfCO2, 132, 64, SSD1306_COLOR_WHITE);
            SSD1306_GotoXY(&SSD1306, 45, 5);
            convertAndStore(result, Value_Co2_Oled);
            SSD1306_Puts(&SSD1306, Value_Co2_Oled, &Font_11x18, SSD1306_COLOR_WHITE);
            SSD1306_GotoXY(&SSD1306, 90, 5);
            SSD1306_Puts(&SSD1306, "ppm", &Font_11x18, SSD1306_COLOR_WHITE);
            SSD1306_GotoXY(&SSD1306, 0, 40);
            SSD1306_Puts(&SSD1306, "Fan State : OFF", &Font_7x10, SSD1306_COLOR_WHITE);
            SSD1306_GotoXY(&SSD1306, 0, 52);
            SSD1306_Puts(&SSD1306, "Door State : OFF", &Font_7x10, SSD1306_COLOR_WHITE);
            SSD1306_UpdateScreen(&SSD1306);
        }

        // measurement status
        uint8_t check = pasco2_uart_read(UART1_BASE, 0x07, rxBuffer);
        // disable uart to delay 500ms
        SysCtlDelay(50*(SysCtlClockGet()/3000));


        // check measurement status ready to read value
        if(check==0x10){
            value1 = pasco2_uart_read(UART1_BASE, 0x05, rxBuffer);
            value2 = pasco2_uart_read(UART1_BASE, 0x06, rxBuffer);
            //UARTprintf("value\tr: %d\tg: %d\tb: \n", value1, value2);
            result = value1 << 8 | value2;
            //UARTprintf("value\tr: %d\tg \n", result);
            // Send value co2 and limit value co2... send byte h ->l
            g_ui8TXMsgData[0] = value1;
            g_ui8TXMsgData[1] = value2;
            g_ui8TXMsgData[2] = set_mes_and_limit[2];
            g_ui8TXMsgData[3] = set_mes_and_limit[3];

            CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_TX); // send as msg object 1

        }
        // Check if data has been received
        if (g_bRXFlag)
        {
            g_sCAN0RxMessage.pui8MsgData = g_ui8RXMsgData;
            CANMessageGet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_TX);
            // Print the received data to the UART terminal
            //UARTprintf("Received data:%d, %d %d \n", g_sCAN0RxMessage.ui32MsgID, g_ui8RXMsgData[0], g_ui8RXMsgData[1]);
            if(g_sCAN0RxMessage.ui32MsgID == 0x104){
                if(g_ui8RXMsgData[0] == 1){
                    state_fan =1;
                }
                else if(g_ui8RXMsgData[0] == 0){
                    state_fan =0;
                }
            }
            if(g_sCAN0RxMessage.ui32MsgID == 0x103){
                if(g_ui8RXMsgData[0] == 1){
                    state_door =1;
                }
                else if(g_ui8RXMsgData[0] == 0){
                    state_door =0;
                }
            }

            // Clear the RX flag
            g_bRXFlag = false;

        }
        SSD1306_GotoXY(&SSD1306, 45, 5);
        convertAndStore(result, Value_Co2_Oled);
        SSD1306_Puts(&SSD1306, Value_Co2_Oled, &Font_11x18, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(&SSD1306, 90, 5);
        SSD1306_Puts(&SSD1306, "ppm", &Font_11x18, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(&SSD1306, 0, 40);
        if(state_fan ==1){
        SSD1306_Puts(&SSD1306, "Fan State :  ON", &Font_7x10, SSD1306_COLOR_WHITE);
        SSD1306_UpdateScreen(&SSD1306);
        }
        else if(state_fan ==0){
            SSD1306_Puts(&SSD1306, "Fan State : OFF", &Font_7x10, SSD1306_COLOR_WHITE);
            SSD1306_UpdateScreen(&SSD1306);
        }
        SSD1306_GotoXY(&SSD1306, 0, 52);
        if(state_door ==1){
        SSD1306_Puts(&SSD1306, "Door State :  ON", &Font_7x10, SSD1306_COLOR_WHITE);
        SSD1306_UpdateScreen(&SSD1306);
        }
        else if(state_door ==0){
            SSD1306_Puts(&SSD1306, "Door State : OFF", &Font_7x10, SSD1306_COLOR_WHITE);
            SSD1306_UpdateScreen(&SSD1306);
        }


    }

}







//void Configure_Uart0()
//{
//    // Set up debugging UART
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//
//    /* Configure the pin muxing for UART0 functions on port A0 and A1.
//     * This step is not necessary if your part does not support pin muxing.
//     * TODO: change this to select the port/pin you are using. */
//    GPIOPinConfigure(GPIO_PA0_U0RX);
//    GPIOPinConfigure(GPIO_PA1_U0TX);
//
//    /* Enable UART0 so that we can configure the clock. */
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
//
//    /* Use the internal 16MHz oscillator as the UART clock source. */
//    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
//
//    /* Select the alternate (UART) function for these pins.
//     * TODO: change this to select the port/pin you are using. */
//    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
//
//    /* Initialize the UART for console I/O. */
//    UARTStdioConfig(0, 115200, 16000000);
//}
/*
 * can.c
 *
 *  Created on: Jul 6, 2023
 *      Author: acer
 */



/*
 * CAN bus LED controller master firmware
 * Written for TI Tiva TM4C1294NCPDT
 */

/*
 * CAN bus LED controller slave firmware
 * Written for TI Tiva TM4C123GH6PM
 */
