/*
 * configure_co2.c
 *
 *  Created on: Aug 20, 2023
 *      Author: acer
 */
#include "SSDD1306/global_include.h"
#include "switches.h"
#include "configure_co2.h"
#include "SSDD1306/OLED_LCD_SSD1306.h"
#include "SSDD1306/fonts.h"
volatile uint8_t S_mode = No_configure, sw0_m = No_configure, sw1_m= No_configure, sw2_m=No_configure;

volatile uint16_t Value_Conf =10;
uint8_t Position_move= 0;
char Value_char[4];
SSD1306_Name SSD1306s;
bool pass =true; // if pass 1 then configure range
void DEFAULT(void){
    S_mode = No_configure;
    sw0_m = No_configure;
    sw1_m= No_configure;
    sw2_m=No_configure;
}
void convertAndStore(uint16_t number, char *outputArray) {

    uint8_t i;
    for(i =0; i<4; i++){
        uint8_t digit = number % 10;
        outputArray[3-i] = '0' + digit;
        number /= 10;
    }
}

void BUTTON_CHANGEMODE_ISR(void)
{   GPIOIntClear(GPIO_PORTE_BASE,GPIO_INT_PIN_1|GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    uint32_t S_status = GPIOIntStatus(GPIO_PORTE_BASE, true);
    switch(S_status){
        case 2:
            if(S_mode == Configuring){
                sw0_m = MOVE;
                Position_move ++;
                SysCtlDelay(200*(SysCtlClockGet()/3000));
                if(Position_move >3){
                   Position_move = 1;
                }
            }
            else{
                S_mode =Configure;
                sw0_m = MOVE;
                Position_move =1;
            }
            break;
        case 4:
            S_status = 0;
            if(S_mode == Configuring && Position_move ==1){
                sw1_m = INC;
                Value_Conf ++;
                if(Value_Conf >=5000){
                    Value_Conf = 10;
                }
                SysCtlDelay(200*(SysCtlClockGet()/3000));
                while((S_status = GPIOIntStatus(GPIO_PORTE_BASE, true))==4){
                    IntMasterDisable();
                    Value_Conf =Value_Conf+10;
                    if(Value_Conf >=5000){
                        Value_Conf = 5000;
                    }
                    SysCtlDelay(10*(SysCtlClockGet()/3000));
                    convertAndStore(Value_Conf, Value_char);
                    SSD1306_GotoXY(&SSD1306s, 44, 30);
                    SSD1306_Puts(&SSD1306s, Value_char, &Font_7x10, SSD1306_COLOR_WHITE);
                    SSD1306_UpdateScreen(&SSD1306s);
                    IntMasterEnable();

                }
                IntMasterDisable();
                convertAndStore(Value_Conf, Value_char);
                SSD1306_GotoXY(&SSD1306s, 44, 30);
                SSD1306_Puts(&SSD1306s, Value_char, &Font_7x10, SSD1306_COLOR_WHITE);
                SSD1306_UpdateScreen(&SSD1306s);
                IntMasterEnable();

            }
            break;
        case 8:
            S_status = 0;
            if(Position_move ==3){
                sw2_m = ENTER_E;
            }
            if(Position_move ==2){
                sw2_m = ENTER_B;
            }
            if(S_mode == Configuring && Position_move ==1){
                sw2_m = DEC;
                Value_Conf --;
                if(Value_Conf >=5000){
                    Value_Conf = 5000;
                }
                SysCtlDelay(200*(SysCtlClockGet()/3000));
                while((S_status = GPIOIntStatus(GPIO_PORTE_BASE, true))==8){
                    IntMasterDisable();
                    Value_Conf =Value_Conf -10;
                    if(Value_Conf >=5000){
                        Value_Conf = 5000;
                    }
                    SysCtlDelay(10*(SysCtlClockGet()/3000));
                    convertAndStore(Value_Conf, Value_char);
                    SSD1306_GotoXY(&SSD1306s, 44, 30);
                    SSD1306_Puts(&SSD1306s, Value_char, &Font_7x10, SSD1306_COLOR_WHITE);
                    SSD1306_UpdateScreen(&SSD1306s);
                    IntMasterEnable();
                }
                IntMasterDisable();
                convertAndStore(Value_Conf, Value_char);
                SSD1306_GotoXY(&SSD1306s, 44, 30);
                SSD1306_Puts(&SSD1306s, Value_char, &Font_7x10, SSD1306_COLOR_WHITE);
                SSD1306_UpdateScreen(&SSD1306s);
                IntMasterEnable();
            }
            break;

    }

}

void Switch_INT_init(void){
    switchInit();
    GPIOIntRegister(GPIO_PORTE_BASE, BUTTON_CHANGEMODE_ISR);
    GPIOIntTypeSet(GPIO_PORTE_BASE,GPIO_INT_PIN_1|GPIO_INT_PIN_2 | GPIO_INT_PIN_3, GPIO_LOW_LEVEL);
    GPIOIntEnable(GPIO_PORTE_BASE,GPIO_INT_PIN_1 |GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    IntEnable(INT_GPIOE);
}



uint8_t COnfigure_CO2(uint8_t *pasco2_set_measurement_and_limit){
    uint8_t conf_sucess;
    while(S_mode == Configure){
        SSD1306_Clear(&SSD1306s);
        SSD1306_GotoXY(&SSD1306s, 0, 10);
        if(pass == true){
            SSD1306_Puts(&SSD1306s, "RANGE 10-5000 (s)", &Font_7x10, SSD1306_COLOR_WHITE);}
        else{
            SSD1306_Puts(&SSD1306s, "Limited CO2", &Font_7x10, SSD1306_COLOR_WHITE);
        }
        convertAndStore(Value_Conf, Value_char);
        SSD1306_GotoXY(&SSD1306s, 44, 30);
        SSD1306_Puts(&SSD1306s, Value_char, &Font_7x10, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(&SSD1306s, 0, 53);
        SSD1306_Puts(&SSD1306s, "BACK", &Font_7x10, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(&SSD1306s, 75, 53);
        SSD1306_Puts(&SSD1306s, "ENTER", &Font_7x10, SSD1306_COLOR_WHITE);
        SSD1306_UpdateScreen(&SSD1306s);
        S_mode = Configuring;

        while(S_mode == Configuring){
            if(sw0_m == MOVE){
                switch(Position_move){
                    case 1:
                        SSD1306_DrawRectangle(&SSD1306s, 0, 50, 30, 12, SSD1306_COLOR_BLACK);// back rectangle
                        SSD1306_DrawRectangle(&SSD1306s, 75, 50, 40, 12, SSD1306_COLOR_BLACK);// enter rectangle
                        SSD1306_DrawRectangle(&SSD1306s, 44, 28, 30, 12, SSD1306_COLOR_WHITE);// number retanggle
                        SSD1306_UpdateScreen(&SSD1306s);
                        sw0_m = DEFAULT_ALL;
                        break;
                    case 2:
                        SSD1306_DrawRectangle(&SSD1306s, 75, 50, 40, 12, SSD1306_COLOR_BLACK);// enter rectangle
                        SSD1306_DrawRectangle(&SSD1306s, 44, 28, 30, 12, SSD1306_COLOR_BLACK);// number retanggle
                        SSD1306_DrawRectangle(&SSD1306s, 0, 50, 30, 12, SSD1306_COLOR_WHITE);// back rectangle
                        SSD1306_UpdateScreen(&SSD1306s);
                        sw0_m = DEFAULT_ALL;
                        break;
                    case 3:
                        SSD1306_DrawRectangle(&SSD1306s, 44, 28, 30, 12, SSD1306_COLOR_BLACK);// number retanggle
                        SSD1306_DrawRectangle(&SSD1306s, 0, 50, 30, 12, SSD1306_COLOR_BLACK);// back rectangle
                        SSD1306_DrawRectangle(&SSD1306s, 75, 50, 40, 12, SSD1306_COLOR_WHITE);// enter rectangle
                        SSD1306_UpdateScreen(&SSD1306s);
                        sw0_m = DEFAULT_ALL;
                        break;

                }


            }
            if(sw2_m == ENTER_E && pass ==true){
                pasco2_set_measurement_and_limit[1] =Value_Conf &0xff;
                pasco2_set_measurement_and_limit[0] =(Value_Conf>>8) &0xff;
                Value_Conf = 0;
                Position_move= 1;
                pass =false;
                DEFAULT();
                S_mode = Configure;
                sw0_m = MOVE;
                SSD1306_Clear(&SSD1306s);
                break;
            }
            if(sw2_m == ENTER_E && pass ==false){break;}
            if(sw2_m == ENTER_B && pass ==false){break;}
            if(sw2_m == ENTER_B && pass ==true){
                Value_Conf = 10;
                DEFAULT();
                S_mode =No_configure;
                SSD1306_Clear(&SSD1306s);
                break;
            }
        }
        if(sw2_m == ENTER_B && pass ==false){
            SSD1306_Clear(&SSD1306s);
            DEFAULT();
            pass =true;
            Value_Conf = 10;
            Position_move= 1;
            sw0_m = MOVE;
            S_mode = Configure;
        }
        if(sw2_m == ENTER_E && pass ==false){
            pasco2_set_measurement_and_limit[3] =Value_Conf &0xff;
            pasco2_set_measurement_and_limit[2] =(Value_Conf>>8) &0xff;
            SSD1306_Clear(&SSD1306s);
            DEFAULT();
            Value_Conf = 10;
            Position_move= 1;
            pass =true;
            S_mode = No_configure;
            conf_sucess =1;
        }




    }

    DEFAULT();
    IntEnable(INT_CAN0);
    IntEnable(INT_UART1);
    if(conf_sucess ==1){
        return 1;
    }
    else{
        return 0;
    }


}
