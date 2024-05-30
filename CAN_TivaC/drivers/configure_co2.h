/*
 * configure_co2.h
 *
 *  Created on: Aug 20, 2023
 *      Author: acer
 */

#ifndef DRIVERS_CONFIGURE_CO2_H_
#define DRIVERS_CONFIGURE_CO2_H_
typedef enum {
        No_configure,
        Configure,
        Configuring,
        DEFAULT_ALL

} mode_t;

typedef enum {CONF, MOVE} sw0_t;
typedef enum {INC, INC_PRESS} sw1_t;
typedef enum {DEC,DEC_PRESS,ENTER_B, ENTER_E} sw2_t;
extern void convertAndStore(uint16_t number, char *outputArray);
extern void Switch_INT_init(void);
extern uint8_t COnfigure_CO2(uint8_t *pasco2_set_measurement_and_limit);
#endif /* DRIVERS_CONFIGURE_CO2_H_ */
