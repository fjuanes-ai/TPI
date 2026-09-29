/*********************************************
 *
 * ### HC_SR04.cpp ###
 * 
 * @brief           Drivers del sensor ultrasónico HC-SR04.
 * @date            Jun 21, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */
#include "Sensores/Ultrasonido-HC_SR04/HC_SR04.h"
#include "Sensores/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
#define		__MAX_TICKS_MEASUREMENT		10					// us = 10^(-6) s.
#define		__MAX_TICKS_UPDATE			10					// ms = 10^(-3) s.
#define		__DISTANCE_MIN				20U					// mm = 10^(-3) m.
#define		__DISTANCE_MAX				4000U				// mm = 10^(-3) m.


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//#if defined (__cplusplus)
//	extern "C" {
//		static void MdE_Ultrasonido_MidiendoTiempoECHO();
//		static void MdE_Ultrasonido_DelayReinicio();
//		static void MdE_Ultrasonido_PulsoTRIGalto();
//		static void MdE_Ultrasonido_EsperandoECHO();
//	}
//#endif


// ====================================================================================
// ====================================================================================


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
//


// ====================================================================================
// ====================================================================================



/*********************************************
 * *** FUNCIONES PÚBLICAS ***
 *********************************************/


/* #############################################
 * Ultrasonido (CONSTRUCTOR)
 * #############################################
 * Establece puertos y pines del HW.
 */
Us_HC_SR04::Us_HC_SR04( uint8_t portTrig, uint8_t pinTrig,
						uint8_t portEcho, uint8_t pinEcho,
						CTimer *inputCTimerObject ) :
			__CTimerFeatures( inputCTimerObject ) {

	int8_t	__tempErrorBuffer;

//	__ticksCount_microSeconds = __MAX_TICKS_MEASUREMENT;

	if ( __CTimerFeatures == nullptr ) {
		return;		// < ERROR >
	}

	// ## Obtención de los canales necesarios de MAT y CAP, para TRIG y ECHO respectivamente. ##

	// # MAT = TRIG #
	__tempErrorBuffer = __CTimerFeatures->Get_available_MAT_channel();
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__MATchannelTRIG = __tempErrorBuffer;

	__tempErrorBuffer = __CTimerFeatures->SwitchMatrix_Config_MAT( portTrig, pinTrig, __MATchannelTRIG );
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}


	// # CAP = ECHO #
	__tempErrorBuffer = __CTimerFeatures->Get_available_CAP_channel();
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__CAPchannelECHO = __tempErrorBuffer;

	__tempErrorBuffer = __CTimerFeatures->SwitchMatrix_Config_CAP( portEcho, pinEcho, __CAPchannelECHO );
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}


	// Resetea TC a flanco ascendente del CAP (ECHO).
//	__CTimerFeatures->Config_CountControlRegister( __CAPchannelECHO, CTimer::CTCR_TimerCounter_Mode_t::TIMER_MODE,
//												   true, CTimer::CTCR_Edge_t::CAP_RISING_EDGE );

	// ## DEBUG ##
	__CTimerFeatures->Config_CountControlRegister( __CAPchannelECHO, CTimer::CTCR_TimerCounter_Mode_t::TIMER_MODE, false, CTimer::CTCR_Edge_t::CAP_RISING_EDGE );
	// ## DEBUG ##

	__CTimerFeatures->Reset_Timer_Prescale();


	this->InstalarPerifericoTemporizado( this );
}


/* #############################################
 * InicioDeSecuencia
 * #############################################
 * \brief:			Inicia la secuencia por máquina de estados.
 */
void Us_HC_SR04::InicioDeSecuencia() {
	HC_SR04_InicioDeSecuencia();
}


/* #############################################
 * Measure_Time
 * #############################################
 * \brief:			Mide el tiempo de pulso recibido en "ECHO".
 *
 * # Valores típicos #
 * 	MIN: 100 uS.
 * 	MAX: 18  mS.
 * 	N/O: 36  mS.	(No Obstacle)
 */
uint32_t Us_HC_SR04::Measure_Time() {
	return __CTimerFeatures->CTimer::GetCAPxValue( __CAPchannelECHO );
}


/* #############################################
 * Time_microSec_to_Distance_millimeters
 * #############################################
 * \brief:			Convierte la duración del pulso de "ECHO" a
 * 					centímetros (mm).
 *
 * # FÓRMULA #
 * uS * 10 / 58 = mm
 *
 * Viene de la distancia recorrida por el sonido (343 m/s) en una
 * distancia desconocida "d" 2 veces (por rebote), en un tiempo "t" medido.
 */
//uint32_t Ultrasonido::Time_microSec_to_Distance_millimeters( uint32_t inputTime_microSec ) {
void Us_HC_SR04::Time_microSec_to_Distance_millimeters() {

	uint32_t measuredTime_microSec = this->Measure_Time();

	switch ( measuredTime_microSec ) {
		case Us_HC_SR04::TIME_MIN:
			__distance_millimeters = Us_HC_SR04::DISTANCE_MIN;
			break;

		case Us_HC_SR04::TIME_MAX:
			__distance_millimeters = Us_HC_SR04::DISTANCE_MAX;
			break;

		case Us_HC_SR04::TIME_NO_OBSTACLE:
			__distance_millimeters = Us_HC_SR04::DISTANCE_NO_OBSTACLE;
			break;
	
		default:
			__distance_millimeters = measuredTime_microSec * 10 / 58.0;
	}
}


/* #############################################
 * Set_Callback_Sequence
 * #############################################
 * \brief:		Asigna la secuencia de la máquina de estados.
 *
 * \input:
 * 	 \--->	inputCallback:	Secuencia de funciones callback
 * 	 						a ejecutar por cada interrupción.
 */
void Us_HC_SR04::Set_Callback_Sequence( volatile void (**inputCallback)(void) ) {
	if ( inputCallback != nullptr ) {
		for ( uint8_t index = 0; index < __HC_SR04_MDE_STEPS; index++ ) {
			if ( inputCallback[index] != nullptr ) {
				__sequenceCallbacks[index] = inputCallback[index];
			} else {
				__sequenceCallbacks[index] = nullptr;
			}
		}
	} else {
		for ( uint8_t index = 0; index < __HC_SR04_MDE_STEPS; index++ ) {
			__sequenceCallbacks[index] = nullptr;
		}
	}
}

/* #############################################
 * HandlerDelPeriferico
 * #############################################
 * \brief:			Handler para actualizar información de distancia.
 */
void Us_HC_SR04::HandlerDelPeriferico() {

	--__ticksUpdateCount;

	if ( !__ticksUpdateCount ) {
		__ticksUpdateCount = __MAX_TICKS_UPDATE;

		// ## DEBUG ##
//		this->Measure_Time();
		this->Time_microSec_to_Distance_millimeters();
		__distance_millimeters = __distance_millimeters;
		// ## DEBUG ##
	}
}

