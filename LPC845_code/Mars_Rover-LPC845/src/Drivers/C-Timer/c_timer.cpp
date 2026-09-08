/*********************************************
 *
 * ### c_timer.cpp ###
 * 
 * @brief           Drivers del periférico CTimer del LPC845.
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
#define	__CTIMER0_CCR_CHANNELS_OFFSET	3
#define	__CTIMER0_MCR_CHANNELS_OFFSET	3
#define __CTIMER0_EMR_EMC0_OFFSET		4
#define __CTIMER0_EMR_CHANNELS_OFFSET	2
#define __CTIMER0_CTCR_ENCC_OFFSET		4
#define __CTIMER0_CTCR_SELCC_OFFSET		5
#define __CTIMER0_CTCR_CHANNELS_OFFSET	2
#define	__CTIMER0_TCR_CRST_OFFSET		1
#define	__CTIMER0_TCR_CEN_OFFSET		0

// # Conversión pin/puerto a número para SWM  #
typedef enum __SWM_Port_Offset_e {
	Port0 = 0,
	Port1 = 1
} __SWM_Port_Offset_t;


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */

// # Inicialización de objetos estáticos (canales MAT/CAP) #
int8_t CTimer::__MATchannelsAvailable = MAT_CHANNEL_LIMIT;	 // Valor neg (< 0) = sin espacio para canales MAT.
int8_t CTimer::__CAPchannelsAvailable = CAP_CHANNEL_LIMIT;


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */

#if defined (__cplusplus)
	extern "C" {
		void CTIMER0_IRQHandler();
	}
#endif


// ====================================================================================
// ====================================================================================


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


/*********************************************
 * SwitchMatrix_Config_MAT_CAP
 *********************************************
 * \brief: 	Cambia de funcionalidad los pines seleccionados según SW.
 * 			Se configura en los canales 0; es decir:
 * 			MAT0, CAP0.
 */
void CTimer::SwitchMatrix_Config_MAT_CAP() {
	SYSCON->SYSAHBCLKCTRL0 |=  (__SWM_SYSCON_MASK );	// Habilitación del SW.

	// # Protección contra límites físicos (HW) #
	switch ( __MATport ) {
		case Port0:
			if ( __MATpin >= __LPC845_PORT0_MAX_PINS )
				return;
		break;

		case Port1:
			if ( __MATpin >= __LPC845_PORT1_MAX_PINS )
				return;
		break;

		default:
			return;
	}

	switch ( __CAPport ) {
		case Port0:
			if ( __CAPpin >= __LPC845_PORT0_MAX_PINS )
				return;
		break;

		case Port1:
			if ( __CAPpin >= __LPC845_PORT1_MAX_PINS )
				return;
		break;

		default:
			return;
	}

	// # Habilitación de los pines MATCH #
	SWM0->PINASSIGN.PINASSIGN13 |= ((__MATport * __PINASSIGN_PORT_OFFSET + __MATpin)
									<< __PINASSIGN13_TO_MAT_0_OFFSET);	// MAT0

	// # Habilitación de los pines CAP #
	SWM0->PINASSIGN.PINASSIGN14 |= ((__CAPport * __PINASSIGN_PORT_OFFSET + __CAPpin)
									<< __PINASSIGN14_TO_CAP_0_OFFSET);	// CAP0


	SYSCON->SYSAHBCLKCTRL0 &= ~(__SWM_SYSCON_MASK );	// Deshabilitación del SW.
}


// ====================================================================================
// ====================================================================================


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */


/*********************************************
 * CTimer
 *********************************************
 * \brief: 	Constructor de la clase.
 *
 * \input:
 * 	 \--->	inputPort_MAT:		Puerto para el MAT.
 * 	 \--->	inputPin_MAT:		Pin para el MAT.
 * 	 \--->	inputPort_CAP:		Puerto para el CAP.
 * 	 \--->	inputPin_CAP:		Pin para el CAP.
 * 	 \--->	prescalerFrequency:	Frecuencia para el prescaler.
 */
CTimer::CTimer( uint8_t 	inputPort_MAT,
				uint8_t 	inputPin_MAT,
				uint8_t 	inputPort_CAP,
				uint8_t 	inputPin_CAP,
				uint32_t 	prescalerFrequency ) :
				__MATport(inputPort_MAT),
				__MATpin(inputPin_MAT),
				__MATperiod(0),
				__CAPport(inputPort_CAP),
				__CAPpin(inputPin_CAP),
				__ticksFrequency(prescalerFrequency) {

	if ( this->Set_MAT_CAP_Channels() < 0 )
		return;		// < ERROR >

	// # Habilitación del periférico C-Timer #
	SYSCON->SYSAHBCLKCTRL0 |= (__CTIMER0_SYSCON_MASK);

	// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
	SYSCON->PRESETCTRL1 &= (uint8_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
	SYSCON->PRESETCTRL1 |= (uint8_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.

	this->SwitchMatrix_Config_MAT_CAP();

	this->Config_PrescalerFrequency( prescalerFrequency );

	CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.
//	this->Config_CountControlRegister( TIMER_MODE, true, CAP_RISING_EDGE );

	// ## Timer Control register (TCR) ##
	CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.
	// # Counter enable (CEN) #
	this->EnableDisable_Timer_Prescale( true );
	this->Reset_Timer_Prescale();

	// ## Configuración de MCR/CCR (Match/Capture Control Register) ##
	CTIMER->MCR   =   0x00;			// Limpieza del MCR y del CCR.
	CTIMER->CCR   =   0x00;
	this->Config_MatchOutput(  STOP_MCR, 0x00, 10 );	// Limpieza para que NO frene el contador.
	this->Config_CaptureInput( FALLING_CCR, 0x01 );
}


/*********************************************
 * Set_MAT_CAP_Channels
 *********************************************
 * \brief: 	Hace el conteo de los valores disponibles para canales de
 * 			MAT o CAP.
 */
int8_t CTimer::Set_MAT_CAP_Channels() {
	// # Seteo de los canales MAT/CAP a utilizar por objeto tipo CTimer #
	--__MATchannelsAvailable;
	--__CAPchannelsAvailable;

	if ( (__MATchannelsAvailable < 0) || (__CAPchannelsAvailable < 0) )
		return -1;

	__MATchannel = MAT_CHANNEL_LIMIT - __MATchannelsAvailable - 1;
	__CAPchannel = CAP_CHANNEL_LIMIT - __CAPchannelsAvailable - 1;

	return 0;
}


/*********************************************
 * EnableDisable_Timer_Prescale
 *********************************************
 * \brief: 	Habilita o deshabilita el Timer y el Prescale.
 *
 * \input:
 * 	 \--->	inputEnableValue:	Valor para habilitar o deshabilitar
 * 	 							el TC y el PC.
 */
void CTimer::EnableDisable_Timer_Prescale( bool inputEnableValue ) {
	CTIMER->TCR |= inputEnableValue << __CTIMER0_TCR_CEN_OFFSET;
}


/*********************************************
 * Reset_Timer_Prescale
 *********************************************
 * \brief: 	Resetea el Timer y el Prescale counters.
 */
void CTimer::Reset_Timer_Prescale() {
	CTIMER->TCR |=   0x01 << __CTIMER0_TCR_CRST_OFFSET;
	CTIMER->TCR &= ~(0x01 << __CTIMER0_TCR_CRST_OFFSET);
}


/*********************************************
 * Config_CountControlRegister
 *********************************************
 * \brief: 	Configura el Registro del Count Control (CTCR)
 * 			para elegir entre modo Timer o Counter, más si se quiere
 * 			resetear TC y PC con una señal de CAPx, entre otras.
 *
 * \input:
 * 	 \--->	inputMode:					Modo del CTCR (Timer, Counter Rising,
 * 	 |									Counter Falling, Counter Double Edge).
 * 	 \--->	clearTCwithCaptureEdge:		Booleano para limpiar o no el valor de
 * 	 |									TC y PR en evento de CAPx.
 * 	 \--->	inputEdge:					Evento elegido para el CAPx.
 */
void CTimer::Config_CountControlRegister( CTCR_TimerCounter_Mode_t 	inputMode,
									      bool 						clearTCwithCaptureEdge,
									      CTCR_Edge_t 				inputEdge ) {
	// # Counter/Timer Mode (CTMODE) #
//	CTIMER->CTCR &= ~(0x01);		// Timer Mode.
	CTIMER->CTCR |=   inputMode;

	// # Reset (ENCC) #
//	CTIMER->CTCR |=  (0x01 << 4);	// Habilitamos el reset por Capture Input x.
	CTIMER->CTCR |=  (clearTCwithCaptureEdge << __CTIMER0_CTCR_ENCC_OFFSET);

	// # Reset (SELCC) #
//	CTIMER->CTCR &= ~(0x07 << 5);	// El reset es por Capture Input 0, rising edge.
	CTIMER->CTCR &= ~((__CTIMER0_CTCR_CHANNELS_OFFSET * __CAPchannel + inputEdge)
						<< __CTIMER0_CTCR_SELCC_OFFSET);
	// \--> Esto se hace así para que, cuando se detecte una subida
	//		por el pin de ECHO, reinicie la cuenta.
}

/*********************************************
 * Config_PrescalerFrequency
 *********************************************
 * \brief: 	Configura el Prescaler (divide frecuencia).
 *
 * \input:
 * 	 \--->	prescalerFrequency:		Frecuencia para el Prescaler en Hz.
 */
void CTimer::Config_PrescalerFrequency( uint32_t prescalerFrequency ) {
	__ticksFrequency = prescalerFrequency;

	CTIMER->PR 	  =   FREQ_CLOCK / __ticksFrequency - 1;
	// Cada 30 ciclos del APB (FRO = 30 M Hz), se incrementa en 1 el TC.
	// Con este método, 1 tick = 1 us = 1 x 10^(-6)s.
}



/*********************************************
 * Config_ExternalMatchOutput
 *********************************************
 * \brief: 	Configura la funcionalidad de los pines de External Match.
 *
 * \input:
 * 	 \--->	inputMatchDemeanor:		Comportamiento del EMx elegido.
 */
void CTimer::Config_ExternalMatchOutput( EMR_Mode_t inputMatchDemeanor ) {
	CTIMER->EMR |= inputMatchDemeanor <<
					(__CTIMER0_EMR_EMC0_OFFSET + __CTIMER0_EMR_CHANNELS_OFFSET * __MATchannel);
}


/*********************************************
 * Config_MatchOutput
 *********************************************
 * \brief: 	Configura los pines a utilizar para Match Output.
 *
 * \input:
 * 	 \--->	inputMCRmode:		Modo del MCR (0 ~ 3).
 * 	 \--->	bitValueMCR:		Valor binario para habilitar o deshabilitar
 * 	 							la función elegida.
 * 	 \--->	microSecondsMATCH:	Período configurado del registro MATx.
 */
void CTimer::Config_MatchOutput( MCRtriggers_t 	inputMCRmode,
								 bool			bitValueMCR,
								 uint32_t 		microSecondsMATCH ) {
	// # Protección contra límites físicos (HW) #
	if ( __MATchannel > 3 ) {
		return;
	}

	// # Limpieza del registro #
	CTIMER->MCR &= ~(0x01 << (__CTIMER0_MCR_CHANNELS_OFFSET * __MATchannel + inputMCRmode));

	// # Configuración de comportamiento de MATx #
	CTIMER->MCR |=   bitValueMCR << (__CTIMER0_MCR_CHANNELS_OFFSET * __MATchannel + inputMCRmode);

	// # Asignación del tiempo deseado para el MATCH en microsegundos (x 10^(-6)) #
	if ( bitValueMCR )
		CTIMER->MR[__MATchannel] = microSecondsMATCH;
}


/*********************************************
 * Config_CaptureInput
 *********************************************
 * \brief: 	Configura los pines a utilizar para Capture Input.
 *
 * \input:
 * 	 \--->	inputCCRmode:		Modo del CCR (0 ~ 3).
 * 	 \--->	bitValueCCR:		Valor binario para habilitar o deshabilitar
 * 	 							la función elegida.
 */
void CTimer::Config_CaptureInput( CCRtriggers_t 	inputCCRmode,
								  bool 				bitValueCCR ) {
	// # Protección contra límites físicos (HW) #
	if ( __CAPchannel > 3 ) {
		return;
	}

	// # Limpieza del registro #
	CTIMER->CCR &= ~(0x01 << (__CTIMER0_CCR_CHANNELS_OFFSET * __CAPchannel + inputCCRmode));

	// # Configuración de comportamiento de CAPx #
	CTIMER->CCR |=  bitValueCCR << (__CTIMER0_CCR_CHANNELS_OFFSET * __CAPchannel + inputCCRmode);
}


/*********************************************
 * GetCAPxValue
 *********************************************
 * \brief: 	Devuelve el valor de CAPx pedido.
 */
__I uint32_t CTimer::GetCAPxValue() const {
	return CTIMER->CR[__CAPchannel];
}


/*********************************************
 * SetMATxValue
 *********************************************
 * \brief: 	Escribe el valor de MATx elegido.
 */
void CTimer::SetMATxValue( uint32_t	inputMATvalue ) {
	CTIMER->MR[__MATchannel] = inputMATvalue;
}



