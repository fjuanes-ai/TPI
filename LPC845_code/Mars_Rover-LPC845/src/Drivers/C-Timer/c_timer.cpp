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

//#define __SYSCON_SYSAHBCLKCTRL0_IOCON_MASK	(0x01 << 18)
//#define __IOCON_PIOX_Y_MODE_OFFSET		3
//#define __IOCON_PIOX_Y_HYS_OFFSET		5
//#define __IOCON_PIOX_Y_INV_OFFSET		6
//#define __IOCON_PIOX_Y_I2C_MODE_OFFSET	8
//#define __IOCON_PIOX_Y_OD_OFFSET		10
//#define __IOCON_PIOX_Y_S_MODE_OFFSET	11
//#define __IOCON_PIOX_Y_CLK_DIV_OFFSET	13
//#define __IOCON_PIOX_Y_DAC_MODE_OFFSET	16
//#define	__SYSCON_SYSAHBCLKCTRL0_SWM_MASK	(0x01 << 7)
#define __SYSCON_PRESETCTRL1_FRG0_MASK		(0x01 << 3)
#define __SYSCON_PRESETCTRL1_FRG1_MASK		(0x01 << 4)
//#define	__CTIMER0_SYSCON_MASK				(0x01 << 25)
#define	__CTIMER0_CCR_CHANNELS_OFFSET	3
#define	__CTIMER0_MCR_CHANNELS_OFFSET	3
#define __CTIMER0_MCR_MR0RL_OFFSET   	24
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
		.port = -1,
		.pin = -1,
		.period = 0,
		.EMRx = &(CTIMER->EMR),
		.MCRx = &(CTIMER->MCR),
		.MRx = CTIMER->MR,
		.__callback = nullptr
	},
	{
		.port = -1,
		.pin = -1,
		.period = 0,
		.EMRx = &(CTIMER->EMR) + 1,
		.MCRx = &(CTIMER->MCR) + 1,
		.MRx = CTIMER->MR + 1,
		.__callback = nullptr
	},
	{
		.port = -1,
		.pin = -1,
		.period = 0,
		.EMRx = &(CTIMER->EMR) + 2,
		.MCRx = &(CTIMER->MCR) + 2,
		.MRx = CTIMER->MR + 2,
		.__callback = nullptr
	},
	{
		.port = -1,
		.pin = -1,
		.period = 0,
		.EMRx = &(CTIMER->EMR) + 3,
		.MCRx = &(CTIMER->MCR) + 3,
		.MRx = CTIMER->MR + 3,
		.__callback = nullptr
	}
};

CTimer::CAP_data_t  CTimer::__CAP[__CTimer_MAX_CR] = {
	{	// # Channel 0 #
		.port = -1,
		.pin = -1,
		.CCRmode = &(CTIMER->CCR),
		.CTCRedge = &(CTIMER->CTCR),
		.CRx = CTIMER->CR,
		.__callback = nullptr
	},
	{
		.port = -1,
		.pin = -1,
		.CCRmode = &(CTIMER->CCR) + 1,
		.CTCRedge = &(CTIMER->CTCR) + 1,
		.CRx = CTIMER->CR + 1,
		.__callback = nullptr
	},
	{
		.port = -1,
		.pin = -1,
		.CCRmode = &(CTIMER->CCR) + 2,
		.CTCRedge = &(CTIMER->CTCR) + 2,
		.CRx = CTIMER->CR + 2,
		.__callback = nullptr
	},
	{
		.port = -1,
		.pin = -1,
		.CCRmode = &(CTIMER->CCR) + 3,
		.CTCRedge = &(CTIMER->CTCR) + 3,
		.CRx = CTIMER->CR + 3,
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
	uint8_t		__bitValueInterruption, __tempChannel;
	uint32_t	interruptRegister_Read = CTIMER->IR;

	// # MAT #
	for ( uint8_t index = 0; index < __CTimer_MAX_MR; index++ ) {	// Separación canales.
		__bitValueInterruption = (uint8_t) ( interruptRegister_Read & (0x01 << index) );

		if ( __bitValueInterruption != 0x00 ) {
//			interruptRegister_Read &= ~(0x01 << index);
			interruptRegister_Read |=  (0x01 << index);		// Reiniciamos el IR con un 1.

			__tempChannel = index;

			if ( CTimer::__MAT[__tempChannel].__callback != nullptr )
				CTimer::__MAT[__tempChannel].__callback();
		}
	}

	// # CAP #
	for ( uint8_t index = 0; index < __CTimer_MAX_CR; index++ ) {	// Separación canales.
		__bitValueInterruption = (uint8_t) ( interruptRegister_Read & (0x01 << (index + __CTIMER0_IRQ_CR_OFFSET)) );

		if ( __bitValueInterruption != 0x00 ) {
//			interruptRegister_Read &= ~(0x01 << (index + __CTIMER0_IRQ_CR_OFFSET));
			interruptRegister_Read |=  (0x01 << (index + __CTIMER0_IRQ_CR_OFFSET));		// Reiniciamos el IR con un 1.

			__tempChannel = index;

			if ( CTimer::__CAP[__tempChannel].__callback != nullptr )
				CTimer::__CAP[__tempChannel].__callback();
		}
	}

	CTIMER->IR = interruptRegister_Read;
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
		SYSCON->SYSAHBCLKCTRL0 |= (SYSCON_SYSAHBCLKCTRL0_CTIMER_MASK);

		// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
		SYSCON->PRESETCTRL1 &= (uint8_t) ~(__SYSCON_PRESETCTRL1_FRG0_MASK | __SYSCON_PRESETCTRL1_FRG1_MASK);	// Apaga.
		SYSCON->PRESETCTRL1 |= (uint8_t)  (__SYSCON_PRESETCTRL1_FRG0_MASK | __SYSCON_PRESETCTRL1_FRG1_MASK);	// Prende.

		this->Config_PrescalerFrequency( FREQ_CLOCK );

		// # Counter/Timer Mode (CTMODE) #
		CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.

		// ## Timer Control register (TCR) ##
		CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.

		// ## Configuración de MCR/CCR (Match/Capture Control Register) ##
		CTIMER->MCR   =   0x00;			// Limpieza del MCR y del CCR.
		CTIMER->CCR   =   0x00;

		// ## Habilitación del Vector de Interrupciones (NVIC) ##
		NVIC->ISER[0] |=  (0x01 << 23);		// Se habilita la interrupción en el vector.

		// # Counter enable (CEN) #
		this->Enable_Timer_Prescale( true );
		this->Reset_Timer_Prescale();

		CTIMER0_IRQHandler();	// Limpia las banderas por ruido.
	}
}

CTimer::CTimer( uint32_t prescalerFrequency ) :
				__ticksFrequency(prescalerFrequency) {

	if ( __isSetup == false ) {		// Setup único del periférico.
		__isSetup = true;

		// # Habilitación del periférico C-Timer #
		SYSCON->SYSAHBCLKCTRL0 |= (SYSCON_SYSAHBCLKCTRL0_CTIMER_MASK);

		// # Reseto del periférico "Fractional Baud Rate Generator" 0 y 1 #
		SYSCON->PRESETCTRL1 &= (uint32_t) ~((0x01 << 3) | (0x01 << 4));	// Apaga.
		SYSCON->PRESETCTRL1 |= (uint32_t)  ((0x01 << 3) | (0x01 << 4));	// Prende.

		this->Config_PrescalerFrequency( prescalerFrequency );

		// # Counter/Timer Mode (CTMODE) #
		CTIMER->CTCR  =   0x00000000;	// Limpiamos el registro con 0s.

		// ## Timer Control register (TCR) ##
		CTIMER->TCR   =   0x00000000;	// Limpiamos el registro con 0s.

		// ## Configuración de MCR/CCR (Match/Capture Control Register) ##
		CTIMER->MCR   =   0x00;			// Limpieza del MCR y del CCR.
		CTIMER->CCR   =   0x00;

		// ## Habilitación del Vector de Interrupciones (NVIC) ##
		NVIC->ISER[0] |=  (0x01 << 23);		// Se habilita la interrupción en el vector.

		// # Counter enable (CEN) #
		this->Enable_Timer_Prescale( true );
		this->Reset_Timer_Prescale();

		CTIMER0_IRQHandler();	// Limpia las banderas por ruido.
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
						   uint8_t channel, volatile void (* inputCallback)(void) ) {

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
int8_t CTimer::SwitchMatrix_Config_MAT( uint8_t input_MATport, uint8_t input_MATpin, uint8_t channel ) {

	// # Protección contra límites físicos (HW) #
	if ( channel >= __CTimer_MAX_MR )
		return -1;

	switch ( input_MATport ) {
		case Port0:
			if ( input_MATpin >= __LPC845_PORT0_MAX_PINS )
				return -1;
		break;

		case Port1:
			if ( input_MATpin >= __LPC845_PORT1_MAX_PINS )
				return -1;
		break;

		default:
			return -1;
	}

	this->__MAT[channel].port = input_MATport;
	this->__MAT[channel].pin  = input_MATpin;

	// # Habilitación de los pines MATCH #
	PINASSIGN_Config( PA_T0_MAT0 + channel, input_MATport, input_MATpin );


	// ## EXTRA ##
	// # Configuración de IOCON #
//	IOCON_Config_PIO( input_MATport, input_MATpin, 0xFFFFFFFF, false );
//	IOCON_Config_PIO( input_MATport, input_MATpin, __IOCON_MODE_PULL_DOWN_MASK, true );
//	IOCON_Config_PIO( input_MATport, input_MATpin, __IOCON_HYS_MASK, false );

	return 0;
}


/*********************************************
 * SwitchMatrix_Config_CAP
 *********************************************
 * \brief: 	Cambia de funcionalidad los pines seleccionados según SW
 * 			para habilitar la función de CAP.
 */
int8_t CTimer::SwitchMatrix_Config_CAP( uint8_t input_CAPport, uint8_t input_CAPpin, uint8_t channel ) {

//	SYSCON->SYSAHBCLKCTRL0 |=  (__SYSCON_SYSAHBCLKCTRL0_SWM_MASK );	// Habilitación del SW.

	// # Protección contra límites físicos (HW) #
	if ( channel >= __CTimer_MAX_CR - 1 )	// 1 CANAL MENOS DISPONIBLE POR HW.
		return -1;

	switch ( input_CAPport ) {
		case Port0:
			if ( input_CAPpin >= __LPC845_PORT0_MAX_PINS )
				return -1;
		break;

		case Port1:
			if ( input_CAPpin >= __LPC845_PORT1_MAX_PINS )
				return -1;
		break;

		default:
			return -1;
	}

	this->__CAP[channel].port = input_CAPport;
	this->__CAP[channel].pin  = input_CAPpin;


	// # Habilitación de los pines CAP #
	PINASSIGN_Config( PA_T0_CAP0 + channel, input_CAPport, input_CAPpin );


	// ## EXTRA ##
	// # Configuración de IOCON #
//	IOCON_Config_PIO( input_CAPport, input_CAPpin, 0xFFFFFFFF, false );
//	IOCON_Config_PIO( input_CAPport, input_CAPpin, __IOCON_MODE_PULL_DOWN_MASK, true );
//	IOCON_Config_PIO( input_CAPport, input_CAPpin, __IOCON_HYS_MASK, true );

	return 0;
}


/*********************************************
 * Config_PWM
 *********************************************
 * \brief: 	Configura los pines de PWM.
 *
 * \input:
 * 	 \--->	A:	A
 */
void CTimer::Config_PWM( uint8_t input_MATport, uint8_t input_MATpin, uint8_t channel, uint32_t valuePWM ) {

}


/*********************************************
 * Set_PWM_MAT_channel
 *********************************************
 * \brief: 	Configura el canal MAT utilizado para PWM (
 *
 * \input:
 * 	 \--->	A:	A
 */
void CTimer::Set_PWM_MAT_channel( uint8_t input_MATport, uint8_t input_MATpin, uint8_t channel, bool enable ) {

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
 * 	 \--->	input_EnableValue:	Valor para habilitar o deshabilitar
 * 	 							el TC y el PC.
 */
void CTimer::Enable_Timer_Prescale( bool input_EnableValue ) {
	if ( input_EnableValue )
		CTIMER->TCR |=   0x01 << __CTIMER0_TCR_CEN_OFFSET;
	else
		CTIMER->TCR &= ~(0x01 << __CTIMER0_TCR_CEN_OFFSET);
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
 * 	 \--->	input_CTCRmode:				Modo del CTCR (Timer, Counter Rising,
 * 	 |									Counter Falling, Counter Double Edge).
 * 	 \--->	clearTCwithCaptureEdge:		Booleano para limpiar o no el valor de
 * 	 |									TC y PR en evento de CAPx.
 * 	 \--->	input_Edge:					Evento elegido para el CAPx.
 */
void CTimer::Config_CountControlRegister( uint8_t					input_CAPchannel,
										  CTCR_TimerCounter_Mode_t 	input_CTCRmode,
									      bool 						clearTCwithCaptureEdge,
									      CTCR_Edge_t 				input_Edge ) {

//	CTIMER->CTCR &= ~(0x01);		// Timer Mode.
	CTIMER->CTCR |=    input_CTCRmode;

	// # Reset (ENCC) #
//	CTIMER->CTCR |=  (0x01 << 4);	// Habilitamos el reset por Capture Input x.
	if ( clearTCwithCaptureEdge )
		CTIMER->CTCR |=   (0x01 << __CTIMER0_CTCR_ENCC_OFFSET);
	else
		CTIMER->CTCR &=  ~(0x01 << __CTIMER0_CTCR_ENCC_OFFSET);

	// # Reset (SELCC) #
//	CTIMER->CTCR &= ~(0x07 << 5);	// El reset es por Capture Input 0, rising edge.
	// # Limpieza del registro #
	CTIMER->CTCR &= ~((0x7) << __CTIMER0_CTCR_SELCC_OFFSET);

	CTIMER->CTCR |=  ((__CTIMER0_CTCR_CHANNELS_OFFSET * input_CAPchannel + input_Edge)
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

	if ( prescalerFrequency == 0 )
		return;
	else {
		__ticksFrequency = prescalerFrequency;

		CTIMER->PR 	  	 =   FREQ_CLOCK / __ticksFrequency - 1;
	//	CTIMER->PR 	  	 =   FREQ_CLOCK / __ticksFrequency;
		// Cada 30 ciclos del APB (FRO = 30 M Hz), se incrementa en 1 el TC.
		// Con este método, 1 tick = 1 us = 1 x 10^(-6)s.
	}
}



/*********************************************
 * Config_ExternalMatchRegister
 *********************************************
 * \brief: 	Configura la funcionalidad de los pines
 * 			de External Match, de a 1 modo a la vez.
 *
 * \input:
 * 	 \--->	input_ExternalMatchDemeanor:		Comportamiento del EMx elegido.
 */
void CTimer::Config_ExternalMatchRegister( uint8_t input_MATchannel, EMR_Demeanor_t input_ExternalMatchDemeanor ) {
	// # Limpieza del registro #
	CTIMER->EMR &= (uint32_t) ~( 0x03 << ((uint32_t) (__CTIMER0_EMR_EMC0_OFFSET + (__CTIMER0_EMR_CHANNELS_OFFSET * input_MATchannel))) );

	CTIMER->EMR |= (uint32_t) ( input_ExternalMatchDemeanor << ((uint32_t) (__CTIMER0_EMR_EMC0_OFFSET + (__CTIMER0_EMR_CHANNELS_OFFSET * input_MATchannel))) );
}


/*********************************************
 * Config_MatchControlRegister
 *********************************************
 * \brief: 	Configura el comportamiento de
 * 			los Match Output, de a 1 modo a la vez.
 *
 * \input:
 * 	 \--->	input_MATchannel:	Canal MAT.
 * 	 \--->	input_MCRmode:		Modo del MCR (0 ~ 3).
 * 	 \--->	bitValueMCR:		Valor binario para habilitar o deshabilitar
 * 	 							la función elegida.
 * 	 \--->	reloadWithMatchShadow:	Booleano para habilitar o deshabilitar el MATCH SHADOW.
 */
void CTimer::Config_MatchControlRegister( uint8_t		input_MATchannel,
		  	  	  	  	  	  	 MCRtriggers_t 	input_MCRmode,
								 bool			bitValueMCR ) {
	// # Protección contra límites físicos (HW) #
	if ( input_MATchannel >= __CTimer_MAX_MR ) {
		return;
	}

	// # Limpieza del registro #
	CTIMER->MCR &= ~(0x01 << (__CTIMER0_MCR_CHANNELS_OFFSET * input_MATchannel + input_MCRmode));

	// # Configuración de comportamiento de MATx #
	if ( bitValueMCR )
		CTIMER->MCR |=   0x01 << (__CTIMER0_MCR_CHANNELS_OFFSET * input_MATchannel + input_MCRmode);
	else
		CTIMER->MCR &= ~(0x01 << (__CTIMER0_MCR_CHANNELS_OFFSET * input_MATchannel + input_MCRmode));
}



void CTimer::Config_MatchShadow( uint8_t		input_MATchannel,
								 bool			reloadWithMatchShadow ) {
	// # Protección contra límites físicos (HW) #
	if ( input_MATchannel >= __CTimer_MAX_MR ) {
		return;
	}

	if ( reloadWithMatchShadow ) {
		CTIMER->MCR |=   0x01 << (__CTIMER0_MCR_MR0RL_OFFSET + input_MATchannel);
	} else {
		CTIMER->MCR &= ~(0x01 << (__CTIMER0_MCR_MR0RL_OFFSET + input_MATchannel));
	}
}

/*********************************************
 * Config_CaptureControlRegister
 *********************************************
 * \brief: 	Configura el comportamiento de
 * 			los Capture Input, de a 1 modo a la vez.
 *
 * \input:
 * 	 \--->	inputCCRmode:		Modo del CCR (0 ~ 3).
 * 	 \--->	bitValueCCR:		Valor binario para habilitar o deshabilitar
 * 	 							la función elegida.
 */
void CTimer::Config_CaptureControlRegister( uint8_t			input_CAPchannel,
	  	  	  	 	 	 	 	  	CCRtriggers_t 	inputCCRmode,
									bool 			bitValueCCR ) {
	// # Protección contra límites físicos (HW) #
	if ( input_CAPchannel >= __CTimer_MAX_CR - 1 ) {		// 1 CANAL MENOS DISPONIBLE POR HW.
		return;
	}

	// # Limpieza del registro #
	CTIMER->CCR &= ~(0x01 << (__CTIMER0_CCR_CHANNELS_OFFSET * input_CAPchannel + inputCCRmode));

	// # Configuración de comportamiento de CAPx #
	if ( bitValueCCR )
		CTIMER->CCR |=   0x01 << (__CTIMER0_CCR_CHANNELS_OFFSET * input_CAPchannel + inputCCRmode);
	else
		CTIMER->CCR &= ~(0x01 << (__CTIMER0_CCR_CHANNELS_OFFSET * input_CAPchannel + inputCCRmode));

//	return input_CAPchannel;
}


/*********************************************
 * GetCAPxValue
 *********************************************
 * \brief: 	Devuelve el valor de CAPx pedido.
 */
uint32_t CTimer::GetCAPxValue( uint8_t channel ) const {
	return CTIMER->CR[channel];
}


/*********************************************
 * SetMATxValue
 *********************************************
 * \brief: 	Escribe el valor de MATx elegido.
 */
void CTimer::SetMATxValue( uint8_t channel, uint32_t input_MATvalue ) {
//	CTIMER->MR[channel] = input_MATvalue - 1;
	CTIMER->MR[channel] = input_MATvalue;
}


/*********************************************
 * SetMSRxValue
 *********************************************
 * \brief: 	Escribe el valor de MSRx elegido (Match Shadow).
 */
void CTimer::SetMSRxValue( uint8_t channel, uint32_t input_MSRvalue ) {
//	CTIMER->MSR[channel] = input_MSRvalue - 1;
	CTIMER->MSR[channel] = input_MSRvalue;
}



