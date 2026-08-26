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
//typedef enum {
//	PIO0_0 = 0x00, PIO0_1, PIO0_2, PIO0_3, PIO0_4, PIO0_5, PIO0_6, PIO0_7, PIO0_8,
//	PIO0_9, PIO0_10, PIO0_11, PIO0_12, PIO0_13, PIO0_14, PIO0_15, PIO0_16,
//	PIO0_17, PIO0_18, PIO0_19, PIO0_20, PIO0_21, PIO0_22, PIO0_23, PIO0_24,
//	PIO0_25, PIO0_26, PIO0_27, PIO0_28, PIO0_29, PIO0_30, PIO0_31,
//
//	PIO1_0 = 0x20, PIO1_1, PIO1_2, PIO1_3, PIO1_4, PIO1_5, PIO1_6, PIO1_7, PIO1_8,
//	PIO1_9, PIO1_10, PIO1_11, PIO1_12, PIO1_13, PIO1_14, PIO1_15, PIO1_16,
//	PIO1_17, PIO1_18, PIO1_19, PIO1_20, PIO1_21
//} __SWM_PIO_NUMBER;
//
// # --> PARTE DE LPC845.h #


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
void CTimer_Config( uint8_t inputPeriod, __SWM_PIO_NUMBER inputTRIG_PinAssign ) {
	// # Habilitación de los periféricos C-Timer + Switch Matrix #
	SYSCON->SYSAHBCLKCTRL0 |= (__SWM_SYSCON_MASK | __CTIMER0_SYSCON_MASK);
	// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
	SYSCON->PRESETCTRL1 &= (uint8_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
	SYSCON->PRESETCTRL1 |= (uint8_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.

	// # Asignación del pin #
	SWM0->PINASSIGN.PINASSIGN13 |= inputTRIG_PinAssign;		// Habilitamos la opción de MATCH OUTPUT para el pin de TRIG.

	// Activar EXTERNAL MATCH para que decida qué hacer en cada MATCH.
	// Configurar el pin y puerto a utilizar para el EXTERNAL MATCH.
}

