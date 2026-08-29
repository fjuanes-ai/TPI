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
#if defined (__cplusplus)
	extern "C" {
		void CTIMER0_IRQHandler();
	}
#endif


/**********************************************/


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
/*********************************************
 * CTIMER0_IRQHandler
 *********************************************
 * \brief: 	función IRQ handler del CTIMER.
 * 			Interrumpe en caso de ser configurado
 * 			por los registros correspondientes.
 *
 * Debe de checkear por qué medio llegó la interrupción y
 * decidir en base a esa información.
 */
void CTIMER0_IRQHandler() {
	uint8_t		__tempRead;

	for ( uint8_t index = 0; index < 7; index++ ) {
		__tempRead = (uint8_t) ( CTIMER->IR & (0x01 << index) );

		if ( __tempRead != 0x00 ) {
			CTIMER->IR |= (0x01 << index);		// Reiniciamos el IR.

			// # TODO: elegir la acción a realizar según interrupción #
			switch ( __tempRead ) {

			}
		}
	}
}


/**********************************************/


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
void CTimer_Config( uint8_t inputPort_PINASSIGN,
					uint8_t inputPin_PINASSIGN,
					uint8_t outputPort_PINASSIGN,
					uint8_t outputPin_PINASSIGN ) {
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

	// ## Prescale register (PR) (N/USED) ##
	CTIMER->PR 	  =   0x00000000;	// En cada ciclo del APB, se incrementa en 1 el TC.

	// ## Count Control register (CTCR) ##
	CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.
	// # Counter/Timer Mode (CTMODE) #
	CTIMER->CTCR &= ~(0x01);		// Timer Mode.
	// # Count Input Select (CINSEL) (N/USED) #
	CTIMER->CTCR &= ~(0x03 << 2);	// Por desuso, dejamos estos bits en 0.

	// ## Timer Control register (TCR) ##
	CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.
	// # Counter enable (CEN) #
	CTIMER->TCR  |=	 (0x01 << 0);
	// # Counter reset (CRST) #
	CTIMER->TCR  &=	~(0x01 << 1);

	// ## Match Control register (MCR) ##
	CTIMER->MCR   =   0x00000000;	// Limpiamos el registro con 0s.
//	CTIMER->MCR   =   0x01;

	// Activar EXTERNAL MATCH para que decida qué hacer en cada MATCH.
	// Configurar el pin y puerto a utilizar para el EXTERNAL MATCH.


	// Configurar pin de ECHO como CAPTURE INPUT.

}

