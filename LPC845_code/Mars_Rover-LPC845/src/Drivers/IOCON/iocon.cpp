/*
 * iocon.cpp
 *
 *  Created on: Sep 29, 2026
 *      Author: iyopolo
 */



/* ###########################################
 * ### INCLUDES ###
 * ########################################### */
#include    "Drivers/IOCON/iocon.h"


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
static const int8_t __IOCON_PIO_Register_Table[__LPC845_MAX_PORTS][__LPC845_PORT0_MAX_PINS] = {
		// El desplazamiento de bit a la derecha es una división por 4 (2^2 = 4), acomodando los
		// offsets de memoria a desplazamientos en términos de bytes ocupados por un uint32_t.
		{
			0x44 >> 2, 0x2C >> 2, 0x18 >> 2, 0x14 >> 2, 0x10 >> 2, 0x0C >> 2, 0x40 >> 2, 0x3C >> 2,	// PIO0.0~7
			0x38 >> 2, 0x34 >> 2, 0x20 >> 2, 0x1C >> 2, 0x08 >> 2, 0x04 >> 2, 0x48 >> 2, 0x28 >> 2,	// PIO0.8~15
			0x24 >> 2, 0x00 >> 2, 0x78 >> 2, 0x74 >> 2, 0x70 >> 2, 0x6C >> 2, 0x68 >> 2, 0x64 >> 2,	// PIO0.16~23
			0x60 >> 2, 0x5C >> 2, 0x58 >> 2, 0x54 >> 2, 0x50 >> 2, 0xC8 >> 2, 0xCC >> 2, 0x8C >> 2	// PIO0.24~31
		},
		{
			0x90 >> 2, 0x94 >> 2, 0x98 >> 2, 0xA4 >> 2, 0xA8 >> 2, 0xAC >> 2, 0xB8 >> 2, 0xC4 >> 2, // PIO1.0~7
			0x7C >> 2, 0x80 >> 2, 0xDC >> 2, 0xD8 >> 2, 0x84 >> 2, 0x88 >> 2, 0x9C >> 2, 0xA0 >> 2, // PIO1.8~15
			0xB0 >> 2, 0xB4 >> 2, 0xBC >> 2, 0xC0 >> 2, 0xD0 >> 2, 0xD4 >> 2, -1,		 -1			// PIO1.16~21 (22 y 23 no existen).
			-1,		   -1,		  -1,		 -1,        -1,        -1,        -1,        -1			// < EN DESUSO >
		}
};


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
/*  
#if defined (__cplusplus)
	extern "C" {
    	void Funcion_ASYNC();
    }
#endif
*/
//


// ====================================================================================
// ====================================================================================


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */

/*********************************************
 * NombreFuncion
 *********************************************
 * @brief:	resumen de la funcionalidad.
 *
 * @return:	valores de retorno.
 */
// void NombreFuncion() {...}


// ====================================================================================
// ====================================================================================


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */

/*********************************************
 * IOCON_Config_PIO
 *********************************************
 * \brief:	Configura el puerto/pin seleccionado con las funcionalidades de HW
 * 			establecidas.
 *
 */
void IOCON_Config_PIO( uint8_t input_port, uint8_t input_pin,
					   uint32_t input_mask, bool enable ) {

	SYSCON->SYSAHBCLKCTRL0 |=   SYSCON_SYSAHBCLKCTRL0_IOCON_MASK;


	if ( enable )
		IOCON->PIO[__IOCON_PIO_Register_Table[input_port][input_pin]] |=   input_mask;
	else
		IOCON->PIO[__IOCON_PIO_Register_Table[input_port][input_pin]] &= ~(input_mask); // Sin histéresis.


	SYSCON->SYSAHBCLKCTRL0 &= ~(SYSCON_SYSAHBCLKCTRL0_IOCON_MASK);
}

