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

#define __SYSCON_PRESETCTRL1_FRG0_MASK		( (0x01 << 3) )
#define __SYSCON_PRESETCTRL1_FRG1_MASK		( (0x01 << 4) )
#define	__SYSCON_SYSAHBCLKCTRL0_SWM_MASK	( (uint16_t) (0x01 << 7)  )
#define	__CTIMER0_SYSCON_MASK				( (uint16_t) (0x01 << 25) )
#define	__CTIMER0_CCR_CHANNELS_OFFSET	3
#define	__CTIMER0_MCR_CHANNELS_OFFSET	3
#define __CTIMER0_EMR_EMC0_OFFSET		4
#define __CTIMER0_EMR_CHANNELS_OFFSET	2
#define __CTIMER0_CTCR_ENCC_OFFSET		4
#define __CTIMER0_CTCR_SELCC_OFFSET		5
#define __CTIMER0_CTCR_CHANNELS_OFFSET	2
#define	__CTIMER0_TCR_CRST_OFFSET		1
#define	__CTIMER0_TCR_CEN_OFFSET		0
#define __CTIMER0_IRQ_MR_OFFSET			0
#define __CTIMER0_IRQ_CR_OFFSET			4

// # Conversión pin/puerto a número para SWM  #
typedef enum __SWM_Port_Offset_e {
	Port0 = 0,
	Port1 = 1
} __SWM_Port_Offset_t;

typedef enum __IRQ_MAT_CAP_TABLE_e {
	IRQ_MAT = 0,
	IRQ_CAP = 4
} __IRQ_MAT_CAP_TABLE_t;


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */

bool CTimer::__isSetup = false;

// ## Datos estáticos para MAT/CAP ##

// # Canales MAT/CAP #
uint8_t CTimer::__availableMATchannels = __CTimer_MAX_MR;	 // 0 = sin espacio para canales MAT.
uint8_t CTimer::__availableCAPchannels = __CTimer_MAX_CR;

// # Estructuras con datos MAT/CAP #
CTimer::MAT_data_t  CTimer::__MAT[__CTimer_MAX_MR] = {
	{	// # Channel 0 #
		.port = 0,
		.pin = 0,
		.period = 0,
		.EMRx = &(CTIMER->EMR),
		.MCRx = &(CTIMER->MCR),
		.MRx = CTIMER->MR,
		.__callback = nullptr
	},
	{
		.port = 0,
		.pin = 0,
		.period = 0,
		.EMRx = &(CTIMER->EMR) + 1,
		.MCRx = &(CTIMER->MCR) + 1,
		.MRx = CTIMER->MR + 1,
		.__callback = nullptr
	},
	{
		.port = 0,
		.pin = 0,
		.period = 0,
		.EMRx = &(CTIMER->EMR) + 2,
		.MCRx = &(CTIMER->MCR) + 2,
		.MRx = CTIMER->MR + 2,
		.__callback = nullptr
	},
	{
		.port = 0,
		.pin = 0,
		.period = 0,
		.EMRx = &(CTIMER->EMR) + 3,
		.MCRx = &(CTIMER->MCR) + 3,
		.MRx = CTIMER->MR + 3,
		.__callback = nullptr
	}
};

CTimer::CAP_data_t  CTimer::__CAP[__CTimer_MAX_CR] = {
	{	// # Channel 0 #
		.port = 0,
		.pin = 0,
		.CCRmode = &(CTIMER->CCR),
		.CTCRedge = &(CTIMER->CTCR),
		.CRx = CTIMER->CR,
		.__callback = nullptr
	},
	{
		.port = 0,
		.pin = 0,
		.CCRmode = &(CTIMER->CCR) + 1,
		.CTCRedge = &(CTIMER->CTCR) + 1,
		.CRx = CTIMER->CR + 1,
		.__callback = nullptr
	},
	{
		.port = 0,
		.pin = 0,
		.CCRmode = &(CTIMER->CCR) + 2,
		.CTCRedge = &(CTIMER->CTCR) + 2,
		.CRx = CTIMER->CR + 2,
		.__callback = nullptr
	},
};


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//


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
	uint8_t		__tempChannelRead;

	// # MAT #
	for ( uint8_t index = 0; index < __CTimer_MAX_MR; index++ ) {	// Separación canales.
		__tempChannelRead = (uint8_t) ( CTIMER->IR & (0x01 << index) );

		if ( __tempChannelRead != 0x00 ) {
			CTIMER->IR |= (0x01 << index);		// Reiniciamos el IR con un 1.

			if ( CTimer::__MAT[__tempChannelRead].__callback != nullptr )
				CTimer::__MAT[__tempChannelRead].__callback();
		}
	}

	// # CAP #
	for ( uint8_t index = 0; index < __CTimer_MAX_CR; index++ ) {	// Separación canales.
		__tempChannelRead = (uint8_t) ( CTIMER->IR & (0x01 << (index + __CTIMER0_IRQ_CR_OFFSET)) );

		if ( __tempChannelRead != 0x00 ) {
			CTIMER->IR |= (0x01 << (index + __CTIMER0_IRQ_CR_OFFSET));		// Reiniciamos el IR con un 1.

			if ( CTimer::__CAP[__tempChannelRead].__callback != nullptr )
				CTimer::__CAP[__tempChannelRead].__callback();
		}
	}
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
 * 	 \--->	prescalerFrequency:	Frecuencia para el prescaler.
 */
CTimer::CTimer() {

	if ( __isSetup == false ) {		// Setup único del periférico.
		__isSetup = true;

		// # Habilitación del periférico C-Timer #
		SYSCON->SYSAHBCLKCTRL0 |= (__CTIMER0_SYSCON_MASK);

		// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
		SYSCON->PRESETCTRL1 &= (uint8_t) ~(__SYSCON_PRESETCTRL1_FRG0_MASK | __SYSCON_PRESETCTRL1_FRG1_MASK);	// Apaga.
		SYSCON->PRESETCTRL1 |= (uint8_t)  (__SYSCON_PRESETCTRL1_FRG0_MASK | __SYSCON_PRESETCTRL1_FRG1_MASK);	// Prende.

		this->Config_PrescalerFrequency( FREQ_CLOCK );

		// # Counter/Timer Mode (CTMODE) #
		CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.

		// ## Timer Control register (TCR) ##
		CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.
		// # Counter enable (CEN) #
		this->Enable_Timer_Prescale( true );
		this->Reset_Timer_Prescale();

		// ## Configuración de MCR/CCR (Match/Capture Control Register) ##
		CTIMER->MCR   =   0x00;			// Limpieza del MCR y del CCR.
		CTIMER->CCR   =   0x00;

		// ## Habilitación del Vector de Interrupciones (NVIC) ##
		NVIC->ISER[0] |=  (0x01 << 23);		// Se habilita la interrupción en el vector.
	}
}

CTimer::CTimer( uint32_t prescalerFrequency ) :
				__ticksFrequency(prescalerFrequency) {

	if ( __isSetup == false ) {		// Setup único del periférico.
		__isSetup = true;

		// # Habilitación del periférico C-Timer #
		SYSCON->SYSAHBCLKCTRL0 |= (__CTIMER0_SYSCON_MASK);

		// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
		SYSCON->PRESETCTRL1 &= (uint8_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
		SYSCON->PRESETCTRL1 |= (uint8_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.

		this->Config_PrescalerFrequency( prescalerFrequency );

		// # Counter/Timer Mode (CTMODE) #
		CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.

		// ## Timer Control register (TCR) ##
		CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.
		// # Counter enable (CEN) #
		this->Enable_Timer_Prescale( true );
		this->Reset_Timer_Prescale();

		// ## Configuración de MCR/CCR (Match/Capture Control Register) ##
		CTIMER->MCR   =   0x00;			// Limpieza del MCR y del CCR.
		CTIMER->CCR   =   0x00;

		// ## Habilitación del Vector de Interrupciones (NVIC) ##
		NVIC->ISER[0] |=  (0x01 << 23);		// Se habilita la interrupción en el vector.
	}
}


/*********************************************
 * Set_Callback
 *********************************************
 * \brief: 	Cambia el callback asociado al MAT[n] o CAP[n].
 *
 * \input:
 * 	 \--->	registerSelection:	Elección de MAT o CAP.
 * 	 \--->	channel:			Canal (0 ~ 4 usualmente, ver datasheet).
 * 	 \--->	inputCallback:		Función callback que se ejecuta si está
 * 	 							la interrupción habilitada.
 */
void CTimer::Set_Callback( registerSelection_MAT_CAP_t registerSelection,
						   uint8_t channel, void (* inputCallback)(void) ) {

	switch ( registerSelection ) {
		case MAT_REGISTER:
			__MAT[channel].__callback = inputCallback;
			break;

		case CAP_REGISTER:
			__CAP[channel].__callback = inputCallback;
			break;
	}

}


/*********************************************
 * SwitchMatrix_Config_MAT
 *********************************************
 * \brief: 	Cambia de funcionalidad los pines seleccionados según SW
 * 			para habilitar la función de MAT.
 */
int8_t CTimer::SwitchMatrix_Config_MAT( uint8_t inputMATport, uint8_t inputMATpin, uint8_t channel ) {

	SYSCON->SYSAHBCLKCTRL0 |=  (__SYSCON_SYSAHBCLKCTRL0_SWM_MASK );	// Habilitación del SW.

	// # Protección contra límites físicos (HW) #
	if ( channel >= __CTimer_MAX_MR )
		return -1;

	if ( this->Get_available_MAT_channel() == -1 )
		return -1;

	switch ( inputMATport ) {
		case Port0:
			if ( inputMATpin >= __LPC845_PORT0_MAX_PINS )
				return -1;
		break;

		case Port1:
			if ( inputMATpin >= __LPC845_PORT1_MAX_PINS )
				return -1;
		break;

		default:
			return -1;
	}

	this->__MAT[channel].port = inputMATport;
	this->__MAT[channel].pin  = inputMATpin;

	// # Habilitación de los pines MATCH #
	if ( channel < 3 )
		SWM0->PINASSIGN.PINASSIGN13 |= ((inputMATport * __PINASSIGN_PORT_OFFSET + inputMATpin)
									<< (__PINASSIGN13_TO_MAT_0_OFFSET * (channel + 1)));
	else
		SWM0->PINASSIGN.PINASSIGN14 |=  (inputMATport * __PINASSIGN_PORT_OFFSET + inputMATpin);


	SYSCON->SYSAHBCLKCTRL0 &= ~(__SYSCON_SYSAHBCLKCTRL0_SWM_MASK );	// Deshabilitación del SW.
	return 0;
}


/*********************************************
 * SwitchMatrix_Config_CAP
 *********************************************
 * \brief: 	Cambia de funcionalidad los pines seleccionados según SW
 * 			para habilitar la función de CAP.
 */
int8_t CTimer::SwitchMatrix_Config_CAP( uint8_t inputCAPport, uint8_t inputCAPpin, uint8_t channel ) {

	SYSCON->SYSAHBCLKCTRL0 |=  (__SYSCON_SYSAHBCLKCTRL0_SWM_MASK );	// Habilitación del SW.

	// # Protección contra límites físicos (HW) #
	if ( channel >= __CTimer_MAX_CR )
		return -1;

	if ( this->Get_available_CAP_channel() == -1 )
		return -1;

	switch ( inputCAPport ) {
		case Port0:
			if ( inputCAPpin >= __LPC845_PORT0_MAX_PINS )
				return -1;
		break;

		case Port1:
			if ( inputCAPpin >= __LPC845_PORT1_MAX_PINS )
				return -1;
		break;

		default:
			return -1;
	}

	this->__CAP[channel].port = inputCAPport;
	this->__CAP[channel].pin  = inputCAPpin;

	// # Habilitación de los pines CAP #
	SWM0->PINASSIGN.PINASSIGN14 |= ((inputCAPport * __PINASSIGN_PORT_OFFSET + inputCAPpin)
									<< (__PINASSIGN14_TO_CAP_0_OFFSET * (channel + 1)));


	SYSCON->SYSAHBCLKCTRL0 &= ~(__SYSCON_SYSAHBCLKCTRL0_SWM_MASK );	// Deshabilitación del SW.
	return 0;
}


/*********************************************
 * Get_available_MAT_channel
 *********************************************
 * \brief: 	Devuelve el valor del primer canal MAT disponible.
 *
 * \return:	-1 					= NO QUEDAN DISPONIBLES.
 *			outputMATchannel	= canal disponible.
 */
int8_t CTimer::Get_available_MAT_channel() {

	int8_t	outputMATchannel;

	outputMATchannel = (__availableMATchannels > 0) ? (__CTimer_MAX_MR - __availableMATchannels) : -1;

	--__availableMATchannels;

	return outputMATchannel;
}


/*********************************************
 * Get_available_CAP_channel
 *********************************************
 * \brief: 	Devuelve el valor del primer canal CAP disponible.
 *
 * \return:	-1 					= NO QUEDAN DISPONIBLES.
 *			outputCAPchannel	= canal disponible.
 */
int8_t CTimer::Get_available_CAP_channel() {

	int8_t	outputCAPchannel;

	outputCAPchannel = (__availableCAPchannels > 0) ? (__CTimer_MAX_CR - __availableCAPchannels) : -1;

	--__availableCAPchannels;

	return outputCAPchannel;
}


/*********************************************
 * Enable_Timer_Prescale
 *********************************************
 * \brief: 	Habilita o deshabilita el Timer y el Prescale.
 *
 * \input:
 * 	 \--->	inputEnableValue:	Valor para habilitar o deshabilitar
 * 	 							el TC y el PC.
 */
void CTimer::Enable_Timer_Prescale( bool inputEnableValue ) {
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
 * 	 \--->	inputPort_CAP:				Puerto para el CAP.
 * 	 \--->	inputPin_CAP:				Pin para el CAP.
 * 	 \--->	inputMode:					Modo del CTCR (Timer, Counter Rising,
 * 	 |									Counter Falling, Counter Double Edge).
 * 	 \--->	clearTCwithCaptureEdge:		Booleano para limpiar o no el valor de
 * 	 |									TC y PR en evento de CAPx.
 * 	 \--->	inputEdge:					Evento elegido para el CAPx.
 */
void CTimer::Config_CountControlRegister( uint8_t					inputCAPchannel,
										  CTCR_TimerCounter_Mode_t 	inputMode,
									      bool 						clearTCwithCaptureEdge,
									      CTCR_Edge_t 				inputEdge ) {

//	CTIMER->CTCR &= ~(0x01);		// Timer Mode.
	CTIMER->CTCR |=    inputMode;

	// # Reset (ENCC) #
//	CTIMER->CTCR |=  (0x01 << 4);	// Habilitamos el reset por Capture Input x.
	CTIMER->CTCR |=   (clearTCwithCaptureEdge << __CTIMER0_CTCR_ENCC_OFFSET);

	// # Reset (SELCC) #
//	CTIMER->CTCR &= ~(0x07 << 5);	// El reset es por Capture Input 0, rising edge.
	CTIMER->CTCR &= ~((__CTIMER0_CTCR_CHANNELS_OFFSET * inputCAPchannel + inputEdge)
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
 * 	 								Puede ir de 1 a 30 x 10^6 Hz (frec. máxima del reloj).
 */
void CTimer::Config_PrescalerFrequency( uint32_t prescalerFrequency ) {

	__ticksFrequency = prescalerFrequency;

	CTIMER->PR 	  	 =   FREQ_CLOCK / __ticksFrequency - 1;
	// Cada 30 ciclos del APB (FRO = 30 M Hz), se incrementa en 1 el TC.
	// Con este método, 1 tick = 1 us = 1 x 10^(-6)s.
}



/*********************************************
 * Config_ExternalMatchOutput
 *********************************************
 * \brief: 	Configura la funcionalidad de los pines
 * 			de External Match, de a 1 modo a la vez.
 *
 * \input:
 * 	 \--->	inputMatchDemeanor:		Comportamiento del EMx elegido.
 */
void CTimer::Config_ExternalMatchOutput( uint8_t inputMATchannel, EMR_Mode_t inputMatchDemeanor ) {
	CTIMER->EMR |= inputMatchDemeanor <<
					(__CTIMER0_EMR_EMC0_OFFSET + __CTIMER0_EMR_CHANNELS_OFFSET * inputMATchannel);
}


/*********************************************
 * Config_MatchOutput
 *********************************************
 * \brief: 	Configura el comportamiento de
 * 			los Match Output, de a 1 modo a la vez.
 *
 * \input:
 * 	 \--->	inputMCRmode:		Modo del MCR (0 ~ 3).
 * 	 \--->	bitValueMCR:		Valor binario para habilitar o deshabilitar
 * 	 							la función elegida.
 * 	 \--->	microSecondsMATCH:	Período configurado del registro MATx.
 */
void CTimer::Config_MatchOutput( uint8_t		inputMATchannel,
		  	  	  	  	  	  	 MCRtriggers_t 	inputMCRmode,
								 bool			bitValueMCR,
								 uint32_t 		microSecondsMATCH ) {
	// # Protección contra límites físicos (HW) #
	if ( inputMATchannel >= __CTimer_MAX_MR ) {
		return;
	}

	// # Limpieza del registro #
	CTIMER->MCR &= ~(0x01 << (__CTIMER0_MCR_CHANNELS_OFFSET * inputMATchannel + inputMCRmode));

	// # Configuración de comportamiento de MATx #
	CTIMER->MCR |=   bitValueMCR << (__CTIMER0_MCR_CHANNELS_OFFSET * inputMATchannel + inputMCRmode);

	// # Asignación del tiempo deseado para el MATCH en microsegundos (x 10^(-6)) #
	if ( bitValueMCR )
		CTIMER->MR[inputMATchannel] = microSecondsMATCH;
}


/*********************************************
 * Config_CaptureInput
 *********************************************
 * \brief: 	Configura el comportamiento de
 * 			los Capture Input, de a 1 modo a la vez.
 *
 * \input:
 * 	 \--->	inputCCRmode:		Modo del CCR (0 ~ 3).
 * 	 \--->	bitValueCCR:		Valor binario para habilitar o deshabilitar
 * 	 							la función elegida.
 */
int8_t CTimer::Config_CaptureInput( uint8_t			inputCAPchannel,
	  	  	  	 	 	 	 	  	CCRtriggers_t 	inputCCRmode,
									bool 			bitValueCCR ) {
	// # Protección contra límites físicos (HW) #
	if ( inputCAPchannel >= __CTimer_MAX_CR ) {
		return -1;
	}

	// # Limpieza del registro #
	CTIMER->CCR &= ~(0x01 << (__CTIMER0_CCR_CHANNELS_OFFSET * inputCAPchannel + inputCCRmode));

	// # Configuración de comportamiento de CAPx #
	CTIMER->CCR |=  bitValueCCR << (__CTIMER0_CCR_CHANNELS_OFFSET * inputCAPchannel + inputCCRmode);

	return inputCAPchannel;
}


/*********************************************
 * GetCAPxValue
 *********************************************
 * \brief: 	Devuelve el valor de CAPx pedido.
 */
__I uint32_t CTimer::GetCAPxValue( uint8_t channel ) const {
	return CTIMER->CR[channel];
}


/*********************************************
 * SetMATxValue
 *********************************************
 * \brief: 	Escribe el valor de MATx elegido.
 */
void CTimer::SetMATxValue( uint8_t channel, uint32_t	inputMATvalue ) {
	CTIMER->MR[channel] = inputMATvalue;
}



