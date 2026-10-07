/*
 * iocon.h
 *
 *  Created on: Sep 29, 2026
 *      Author: iyopolo
 */



#ifndef         DRIVERS_IOCON_IOCON_H_     
    #define     DRIVERS_IOCON_IOCON_H_

    // #include    "inc/Drivers/IOCON/iocon.h"

    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Drivers/tipos.h"
	#include "Drivers/LPC845.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
	// ### Máscaras de bit de funcionalidades ###
	#define __IOCON_MODE_SHIFT				3
	#define __IOCON_MODE_INACTIVE_MASK		(0x0 << __IOCON_MODE_SHIFT)
	#define __IOCON_MODE_PULL_DOWN_MASK		(0x1 << __IOCON_MODE_SHIFT)
	#define __IOCON_MODE_PULL_UP_MASK		(0x2 << __IOCON_MODE_SHIFT)
	#define __IOCON_MODE_REPEATER_MASK		(0x3 << __IOCON_MODE_SHIFT)

	#define __IOCON_HYS_SHIFT				5
	#define __IOCON_HYS_MASK				(0x1 << __IOCON_HYS_SHIFT)

	#define __IOCON_INV_SHIFT				6
	#define __IOCON_INV_MASK				(0x1 << __IOCON_INV_SHIFT)

	#define __IOCON_I2C_SHIFT				8
	#define __IOCON_I2C_MASK				(0x1 << __IOCON_I2C_SHIFT)

	#define __IOCON_OD_SHIFT				10
	#define __IOCON_OD_MASK					(0x1 << __IOCON_OD_SHIFT)

	#define __IOCON_S_MODE_SHIFT			11
	#define __IOCON_S_MODE_MASK(x)			((uint32_t) x << __IOCON_S_MODE_SHIFT)	// Values: 0 ~ 3

	#define __IOCON_CLK_DIV_SHIFT			13
	#define __IOCON_CLK_DIV_MASK			(0x1 << __IOCON_CLK_DIV_SHIFT)

	#define __IOCON_DAC_MODE_SHIFT			16
	#define __IOCON_DAC_MODE_MASK			(0x1 << __IOCON_DAC_MODE_SHIFT)


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
    //


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
    void IOCON_Config_PIO( uint8_t input_port, uint8_t input_pin,
    					   uint32_t input_mask, bool enable );


    /* ###########################################
     * ### DEFINICIONES DE CLASES ###
     * ########################################### */
    //


#endif          /* DRIVERS_IOCON_IOCON_H_ */
