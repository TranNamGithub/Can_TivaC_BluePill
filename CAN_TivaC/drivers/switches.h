/*
 * switches.h
 *
 *  Created on: Mar 16, 2018
 *      Author: Quoc Bao
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#define SW_GPIO_PERIPH     SYSCTL_PERIPH_GPIOE
#define SW_GPIO_BASE       GPIO_PORTE_BASE
#define SW0_PIN            GPIO_PIN_1
#define SW1_PIN            GPIO_PIN_2
#define SW2_PIN            GPIO_PIN_3
typedef     enum {PRESSED,RELEASED}  sw_t;
void        switchInit(void);

sw_t        switchState(int SWnumber);
#endif /* SWITCHES_H_ */
