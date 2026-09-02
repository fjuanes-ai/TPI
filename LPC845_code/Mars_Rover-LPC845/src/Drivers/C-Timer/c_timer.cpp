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
 * 	 \--->	inputPort_CAP:		Port elegido para el Capture Input.
 * 	 \--->	inputPin_CAP:		Pin elegido para el Capture Input.
 * 	 \--->	outputPort_MATCH:	Port elegido para el Match Output.
 * 	 \--->	outputPin_MATCH:	Pin elegido para el Match Output.
 *
 */
//void CTimer_Config( uint8_t inputPeriod, __SWM_PIO_NUMBER inputTRIG_PinAssign ) {
void CTimer_Config( uint8_t inputPort_MAT,
					uint8_t inputPin_MAT,
					uint8_t inputPort_CAP,
					uint8_t inputPin_CAP ) {

	// # Habilitación del periférico C-Timer #
	SYSCON->SYSAHBCLKCTRL0 |= (__CTIMER0_SYSCON_MASK);

	// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
	SYSCON->PRESETCTRL1 &= (uint8_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
	SYSCON->PRESETCTRL1 |= (uint8_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.

	SwitchMatrix_Config_MAT_CAP( inputPort_MAT, inputPin_MAT, inputPort_CAP, inputPin_CAP );

	// ## Prescale register (PR) ##
	CTIMER->PR 	  =   FREQ_CLOCK / 1000000;	// Cada 30 ciclos del APB (FRO = 30 M Hz), se incrementa en 1 el TC.
											// Con este método, 1 tick = 1 us = 1 x 10^(-6)s.
	// ## Count Control register (CTCR) ##
	CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.
	// # Counter/Timer Mode (CTMODE) #
	CTIMER->CTCR &= ~(0x01);		// Timer Mode.
	// # Reset (ENCC) #
	CTIMER->CTCR |=  (0x01 << 4);	// Habilitamos el reset por Capture Input x.
	// # Reset (SELCC) #
	CTIMER->CTCR &= ~(0x07 << 5);	// El reset es por Capture Input 0, rising edge.
	// \--> Esto se hace así para que, cuando se detecte una subida
	//		por el pin de ECHO, reinicie la cuenta.

	// ## Timer Control register (TCR) ##
	CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.
	// # Counter enable (CEN) #
	CTIMER->TCR  |=	 (0x01);
	// # Counter reset (CRST) #
	CTIMER->TCR  |=	 (0x01 << 1);	// Reseteamos el TC y el PC.
	CTIMER->TCR  &=	~(0x01 << 1);	// Los volvemos a habilitar.


	CTimer_Config_MatchOutput(  inputPort_MAT, inputPin_MAT );

	CTimer_Config_CaptureInput( inputPort_CAP, inputPin_CAP );
}


/*********************************************
 * SwitchMatrix_Config_MAT_CAP
 *********************************************
 * \brief: 	Cambia de funcionalidad los pines seleccionados según SW.
 *
 * \input:
 * 	 \--->	inputPortMAT:	Puerto de entrada para configurar como Match Output.
 * 	 \--->	inputPinMAT:	Pin de entrada para configurar como Match Output.
 * 	 \--->	inputPortCAP:	Puerto de entrada para configurar como Capture Input.
 * 	 \--->	inputPinCAP:	Pin de entrada para configurar como Capture Input.
 *
 */
void SwitchMatrix_Config_MAT_CAP( uint8_t inputPort_MAT,
						  	   	  uint8_t inputPin_MAT,
								  uint8_t inputPort_CAP,
								  uint8_t inputPin_CAP ) {

	SYSCON->SYSAHBCLKCTRL0 |=  (__SWM_SYSCON_MASK );	// Habilitación del SW.


	// # Protección contra límites físicos (HW) #
	switch ( inputPort_MAT ) {
		case Port0:
			if ( inputPin_MAT >= __LPC845_PORT0_MAX_PINS )
				return;
		break;

		case Port1:
			if ( inputPin_MAT >= __LPC845_PORT1_MAX_PINS )
				return;
		break;

		default:
			return;
	}

	switch ( inputPort_CAP ) {
		case Port0:
			if ( inputPin_CAP >= __LPC845_PORT0_MAX_PINS )
				return;
		break;

		case Port1:
			if ( inputPin_CAP >= __LPC845_PORT1_MAX_PINS )
				return;
		break;

		default:
			return;
	}

	// # Habilitación de los pines MATCH #
	SWM0->PINASSIGN.PINASSIGN13 |= ((inputPort_MAT * __PINASSIGN_PORT_OFFSET + inputPin_MAT)
									<< __PINASSIGN13_TO_MAT_0_OFFSET);

	// # Habilitación de los pines CAP #
	SWM0->PINASSIGN.PINASSIGN14 |= ((inputPort_CAP * __PINASSIGN_PORT_OFFSET + inputPin_CAP)
									<< __PINASSIGN14_TO_CAP_0_OFFSET);


	SYSCON->SYSAHBCLKCTRL0 &= ~(__SWM_SYSCON_MASK );	// Deshabilitación del SW.
}


/*********************************************
 * CTimer_Config_MatchOutput
 *********************************************
 * \brief: 	Configura correctamente los pines para Match Output.
 * 			Esto sirve para que un pin cambie de estado cuando TC = MATx.
 *
 * \input:
 * 	 \--->	inputPort_MAT:		Port elegido.
 * 	 \--->	inputPin_MAT:		Pin elegido.
 *
 */
void CTimer_Config_MatchOutput( uint8_t inputPort_MAT,
						   	    uint8_t inputPin_MAT ) {

//	 A
}


/*********************************************
 * CTimer_Config_CaptureInput
 *********************************************
 * \brief: 	Configura correctamente los pines para Capture Input.
 * 			Copia el valor de TC en CAPx cuando el pin de CAPTURE INPUT
 * 			realize la acción configurada.
 *
 * \input:
 * 	 \--->	inputPort_CAP:		Port elegido.
 * 	 \--->	inputPin_CAP:		Pin elegido.
 *
 */
void CTimer_Config_CaptureInput( uint8_t inputPort_CAP,
			   	   	   	   		 uint8_t inputPin_CAP ) {

}
