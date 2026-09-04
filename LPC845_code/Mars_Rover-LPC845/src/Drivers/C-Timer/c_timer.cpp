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
#define	__CTIMER0_CCR_CHANNELS_OFFSET	3
#define	__CTIMER0_MCR_CHANNELS_OFFSET	3

// # Conversión pin/puerto a número para SWM  #
typedef enum __SWM_Port_Offset_e {
	Port0 = 0,
	Port1 = 1
} __SWM_Port_Offset_t;


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */

// # Inicialización de objetos estáticos (canales MAT/CAP) #
int8_t CTimer::__MATchannelsAvailable = MAT_CHANNEL_LIMIT;
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
				__prescalerFrequency(prescalerFrequency) {
	// # Habilitación del periférico C-Timer #
	SYSCON->SYSAHBCLKCTRL0 |= (__CTIMER0_SYSCON_MASK);

	// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
	SYSCON->PRESETCTRL1 &= (uint8_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
	SYSCON->PRESETCTRL1 |= (uint8_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.


	// # Seteo de los canales MAT/CAP a utilizar por objeto tipo CTimer #
	--__MATchannelsAvailable;
	--__CAPchannelsAvailable;

	if ( (__MATchannelsAvailable < 0) || (__CAPchannelsAvailable < 0) ) {
		return;
	}

	__MATchannel = MAT_CHANNEL_LIMIT - __MATchannelsAvailable;
	__CAPchannel = CAP_CHANNEL_LIMIT - __CAPchannelsAvailable;

	this->SwitchMatrix_Config_MAT_CAP();

	// ## Prescale register (PR) ##
	CTIMER->PR 	  =   FREQ_CLOCK / __prescalerFrequency - 1;	// Cada 30 ciclos del APB (FRO = 30 M Hz), se incrementa en 1 el TC.
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

	// ## Configuración de MCR/CCR (Match/Capture Control Register) ##
	CTIMER->MCR   =   0x00;			// Limpieza del MCR y del CCR.
	CTIMER->CCR   =   0x00;
	this->Config_MatchOutput(  STOP_MCR, 0x00, 10 );	// Limpieza para que NO frene el contador.
	this->Config_CaptureInput( FALLING_CCR, 0x01 );
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
void CTimer::Config_MatchOutput( MCRvalues_t 	inputMCRmode,
								 uint8_t		bitValueMCR,
								 uint32_t 		microSecondsMATCH ) {
	// # Protección contra límites físicos (HW) #
	if ( (__MATchannel > 3) || (bitValueMCR > 1) ) {
		return;
	}

	// # Limpieza del registro #
//	CTIMER->MCR  =   0x00;
	CTIMER->MCR &= ~(0x01 << (__CTIMER0_MCR_CHANNELS_OFFSET * __MATchannel + inputMCRmode));
//	CTIMER->MCR |=  (0x01 << 1);
	CTIMER->MCR |=  ((0x01 * bitValueMCR) << (__CTIMER0_MCR_CHANNELS_OFFSET * __MATchannel + inputMCRmode));

	// # Asignación del tiempo deseado para el MATCH en microsegundos (x 10^(-6)) #
	if ( bitValueMCR == 0x01 )
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
void CTimer::Config_CaptureInput( CCRvalues_t 	inputCCRmode,
								  uint8_t 		bitValueCCR ) {
	// # Protección contra límites físicos (HW) #
	if ( (__CAPchannel > 3) || (bitValueCCR > 1) ) {
		return;
	}

	// # Limpieza del registro #
//	CTIMER->CCR  =   0x00;
	CTIMER->CCR &= ~(0x01 << (__CTIMER0_CCR_CHANNELS_OFFSET * __CAPchannel + inputCCRmode));

	// # Configuración de comportamiento de CAPx #
//	CTIMER->CCR |=  (0x01 << 1);
	CTIMER->CCR |=  ((0x01 * bitValueCCR) << (__CTIMER0_CCR_CHANNELS_OFFSET * __CAPchannel + inputCCRmode));
}


/*********************************************
 * GetCAPxValue
 *********************************************
 * \brief: 	Devuelve el valor de CAPx pedido.
 */
uint32_t CTimer::GetCAPxValue() {
	return __CR[__CAPchannel];
}


/*********************************************
 * SetMATxValue
 *********************************************
 * \brief: 	Escribe el valor de MATx elegido.
 */
void CTimer::SetMATxValue( uint32_t	inputMATvalue ) {
	__MR[__MATchannel] = inputMATvalue;
}





// ====================================================================================
// >> EN DESUSO: CTimer programado a lo C (sin clases).
// ====================================================================================


//
///*********************************************
// * CTimer_Config
// *********************************************
// * \brief: 	Inicializa el C-Timer para el HC_SR04.
// * 			Requiere precisión de al menos 1 us.
// *
// * \input:
// * 	 \--->	inputPort_CAP:		Port elegido para el Capture Input.
// * 	 \--->	inputPin_CAP:		Pin elegido para el Capture Input.
// * 	 \--->	outputPort_MATCH:	Port elegido para el Match Output.
// * 	 \--->	outputPin_MATCH:	Pin elegido para el Match Output.
// *
// */
////void CTimer_Config( uint8_t inputPeriod, __SWM_PIO_NUMBER inputTRIG_PinAssign ) {
//void CTimer_Config( uint8_t 	inputPort_MAT,
//					uint8_t 	inputPin_MAT,
//					uint8_t 	inputPort_CAP,
//					uint8_t 	inputPin_CAP,
//					uint32_t 	prescalerFrequency ) {
//
//	// # Habilitación del periférico C-Timer #
//	SYSCON->SYSAHBCLKCTRL0 |= (__CTIMER0_SYSCON_MASK);
//
//	// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
//	SYSCON->PRESETCTRL1 &= (uint8_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
//	SYSCON->PRESETCTRL1 |= (uint8_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.
//
//	SwitchMatrix_Config_MAT_CAP( inputPort_MAT, inputPin_MAT, inputPort_CAP, inputPin_CAP );
//
//	// ## Prescale register (PR) ##
//	CTIMER->PR 	  =   FREQ_CLOCK / prescalerFrequency - 1;	// Cada 30 ciclos del APB (FRO = 30 M Hz), se incrementa en 1 el TC.
//											// Con este método, 1 tick = 1 us = 1 x 10^(-6)s.
//	// ## Count Control register (CTCR) ##
//	CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.
//	// # Counter/Timer Mode (CTMODE) #
//	CTIMER->CTCR &= ~(0x01);		// Timer Mode.
//	// # Reset (ENCC) #
//	CTIMER->CTCR |=  (0x01 << 4);	// Habilitamos el reset por Capture Input x.
//	// # Reset (SELCC) #
//	CTIMER->CTCR &= ~(0x07 << 5);	// El reset es por Capture Input 0, rising edge.
//	// \--> Esto se hace así para que, cuando se detecte una subida
//	//		por el pin de ECHO, reinicie la cuenta.
//
//	// ## Timer Control register (TCR) ##
//	CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.
//	// # Counter enable (CEN) #
//	CTIMER->TCR  |=	 (0x01);
//	// # Counter reset (CRST) #
//	CTIMER->TCR  |=	 (0x01 << 1);	// Reseteamos el TC y el PC.
//	CTIMER->TCR  &=	~(0x01 << 1);	// Los volvemos a habilitar.
//
//
//	CTIMER->MCR   =   0x00;			// Limpieza del MCR y del CCR.
//	CTIMER->CCR   =   0x00;
//	CTimer_Config_MatchOutput(  __MAT_CHANNEL, STOP_MCR, 0, 10 );
//	CTimer_Config_CaptureInput( __CAP_CHANNEL, FALLING_CCR, 1 );
//}
//
//
///*********************************************
// * SwitchMatrix_Config_MAT_CAP
// *********************************************
// * \brief: 	Cambia de funcionalidad los pines seleccionados según SW.
// * 			Se configura en los canales 0; es decir:
// * 			MAT0, CAP0.
// *
// * \input:
// * 	 \--->	inputPortMAT:	Puerto de entrada para configurar como Match Output.
// * 	 \--->	inputPinMAT:	Pin de entrada para configurar como Match Output.
// * 	 \--->	inputPortCAP:	Puerto de entrada para configurar como Capture Input.
// * 	 \--->	inputPinCAP:	Pin de entrada para configurar como Capture Input.
// *
// */
//void SwitchMatrix_Config_MAT_CAP( uint8_t inputPort_MAT,
//						  	   	  uint8_t inputPin_MAT,
//								  uint8_t inputPort_CAP,
//								  uint8_t inputPin_CAP ) {
//
//	SYSCON->SYSAHBCLKCTRL0 |=  (__SWM_SYSCON_MASK );	// Habilitación del SW.
//
//
//	// # Protección contra límites físicos (HW) #
//	switch ( inputPort_MAT ) {
//		case Port0:
//			if ( inputPin_MAT >= __LPC845_PORT0_MAX_PINS )
//				return;
//		break;
//
//		case Port1:
//			if ( inputPin_MAT >= __LPC845_PORT1_MAX_PINS )
//				return;
//		break;
//
//		default:
//			return;
//	}
//
//	switch ( inputPort_CAP ) {
//		case Port0:
//			if ( inputPin_CAP >= __LPC845_PORT0_MAX_PINS )
//				return;
//		break;
//
//		case Port1:
//			if ( inputPin_CAP >= __LPC845_PORT1_MAX_PINS )
//				return;
//		break;
//
//		default:
//			return;
//	}
//
//	// # Habilitación de los pines MATCH #
//	SWM0->PINASSIGN.PINASSIGN13 |= ((inputPort_MAT * __PINASSIGN_PORT_OFFSET + inputPin_MAT)
//									<< __PINASSIGN13_TO_MAT_0_OFFSET);	// MAT0
//
//	// # Habilitación de los pines CAP #
//	SWM0->PINASSIGN.PINASSIGN14 |= ((inputPort_CAP * __PINASSIGN_PORT_OFFSET + inputPin_CAP)
//									<< __PINASSIGN14_TO_CAP_0_OFFSET);	// CAP0
//
//
//	SYSCON->SYSAHBCLKCTRL0 &= ~(__SWM_SYSCON_MASK );	// Deshabilitación del SW.
//}
//
//
///*********************************************
// * CTimer_Config_MatchOutput
// *********************************************
// * \brief: 	Configura correctamente los pines para Match Output.
// * 			Esto sirve para que un pin cambie de estado cuando TC = MATx.
// *
// * \input:
// * 	 \--->	inputMATchannel:	Canal del MAT (0 ~ 3).
// * 	 \--->	inputMCRmode:		Modo a configurar entre INTERRUPT, RESET, STOP.
// * 	 \--->	bitValueMCR:		Valor en binario a establecer {0; 1}.
// *
// */
//void CTimer_Config_MatchOutput( uint8_t 	inputMATchannel,
//								MCRvalues_t inputMCRmode,
//								uint8_t		bitValueMCR,
//								uint32_t 	microSecondsMATCH ) {
//	// # Protección contra límites físicos (HW) #
//	if ( (inputMATchannel > 3) || (bitValueMCR > 1) ) {
//		return;
//	}
//
//	// # Limpieza del registro #
////	CTIMER->MCR  =   0x00;
//	CTIMER->MCR &= ~(0x01 << (__CTIMER0_MCR_CHANNELS_OFFSET * inputMATchannel + inputMCRmode));
////	CTIMER->MCR |=  (0x01 << 1);
//	CTIMER->MCR |=  ((0x01 * bitValueMCR) << (__CTIMER0_MCR_CHANNELS_OFFSET * inputMATchannel + inputMCRmode));
//
//	// # Asignación del tiempo deseado para el MATCH en microsegundos (x 10^(-6)) #
//	CTIMER->MR[inputMATchannel] = microSecondsMATCH;
//}
//
//
///*********************************************
// * CTimer_Config_CaptureInput
// *********************************************
// * \brief: 	Configura correctamente los pines para Capture Input.
// * 			Por HW: CAPx copia el valor de TC cuando el pin de
// * 			CAPTURE INPUT realize la acción configurada.
// *
// * \input:
// * 	 \--->	inputCAPchannel:	Canal del CAP (0 ~ 3).
// * 	 \--->	inputCCRmode:		Modo a configurar entre RISING, FALLING, INTERRUPT.
// * 	 \--->	bitValueCCR:		Valor en binario a establecer {0; 1}.
// *
// */
//void CTimer_Config_CaptureInput( uint8_t 	 inputCAPchannel,
//								 CCRvalues_t inputCCRmode,
//								 uint8_t 	 bitValueCCR ) {
//	// # Protección contra límites físicos (HW) #
//	if ( (inputCAPchannel > 3) || (bitValueCCR > 1) ) {
//		return;
//	}
//
//	// # Limpieza del registro #
////	CTIMER->CCR  =   0x00;
//	CTIMER->CCR &= ~(0x01 << (__CTIMER0_CCR_CHANNELS_OFFSET * inputCAPchannel + inputCCRmode));
//
//	// # Configuración de comportamiento de CAPx #
////	CTIMER->CCR |=  (0x01 << 1);
//	CTIMER->CCR |=  ((0x01 * bitValueCCR) << (__CTIMER0_CCR_CHANNELS_OFFSET * inputCAPchannel + inputCCRmode));
//}


