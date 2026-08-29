/*********************************************
 *
 * ### c_timer.cpp ###
 * 
 * @brief           Descripción del módulo...
 * @date            Aug 25, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */

#include "Drivers/C-Timer/c_timer.h"


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
#define	__SWM_SYSCON_MASK		( (uint16_t) (0x01 << 7)  )
#define	__CTIMER0_SYSCON_MASK	( (uint16_t) (0x01 << 25) )

// # Conversión pin/puerto a número para SWM  #
typedef enum __SWM_Port_Offset_e {
	Port0 = 0,
	Port1 = 1
} __SWM_Port_Offset_t;


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//


/**********************************************/


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */

/*********************************************
 * CTimer_Config
 *********************************************
 * \brief: 	Inicializa el C-Timer para el HC_SR04.
 * 			Requiere precisión de al menos 1 us.
 *
 * \input:
 * 	 \--->	inputPeriod:	Período de entrada en us (x 10^(-6) s).
 * 	 \--->	inputPinAssign:	Pin elegido según como lo lee SWM por tabla (se usa una enumeración).
 *
 */
//void CTimer_Config( uint8_t inputPeriod, __SWM_PIO_NUMBER inputTRIG_PinAssign ) {
void CTimer_Config( uint8_t inputPeriod, uint8_t inputPort_PINASSIGN, uint8_t inputPin_PINASSIGN ) {
	// # Habilitación de los periféricos C-Timer + Switch Matrix #
	SYSCON->SYSAHBCLKCTRL0 |= (__SWM_SYSCON_MASK | __CTIMER0_SYSCON_MASK);
	// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
	SYSCON->PRESETCTRL1 &= (uint8_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
	SYSCON->PRESETCTRL1 |= (uint8_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.

	// ### Asignación del pin para la Switch Matrix ###
	// # Protección contra límites físicos (HW) #
	switch ( inputPort_PINASSIGN ) {
		case Port0:
			if ( inputPin_PINASSIGN >= __LPC845_PORT0_MAX_PINS )
				break;
		break;

		case Port1:
			if ( inputPin_PINASSIGN >= __LPC845_PORT1_MAX_PINS )
				break;
		break;

		default:
			return;
	}
	// # Asignación concreta #
	SWM0->PINASSIGN.PINASSIGN13 |= (((inputPort_PINASSIGN * __PINASSIGN13_PORT_OFFSET) + inputPin_PINASSIGN)
									<< __PINASSIGN13_TO_MAT_0_OFFSET);
//	SWM0->PINASSIGN.PINASSIGN13 |= inputTRIG_PinAssign;		// Habilitamos la opción de MATCH OUTPUT para el pin de TRIG.

	CTIMER->CTCR &= ~(0x01);		// Timer Mode.
	// Activar EXTERNAL MATCH para que decida qué hacer en cada MATCH.
	// Configurar el pin y puerto a utilizar para el EXTERNAL MATCH.
}

