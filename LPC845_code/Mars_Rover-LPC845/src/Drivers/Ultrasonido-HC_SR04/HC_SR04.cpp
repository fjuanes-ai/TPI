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
#include "Drivers/Ultrasonido-HC_SR04/HC_SR04.h"
#include "Drivers/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


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
//		static void SecuenciaTRIG();
//		static void SecuenciaECHO();
//	}
//#endif


// ====================================================================================
// ====================================================================================


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */

/* #############################################
 * SecuenciaTRIG (ASYNC)
 * #############################################
 * Función asíncrona de secuencia para TRIG.
 */
//void SecuenciaTRIG() {
//
//}


/* #############################################
 * SecuenciaECHO (ASYNC)
 * #############################################
 * Función asíncrona de secuencia para ECHO.
 */
//void SecuenciaECHO() {
//
//}


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
Ultrasonido::Ultrasonido( uint8_t portTrig, uint8_t pinTrig,
						  uint8_t portEcho, uint8_t pinEcho,
						  CTimer *inputCTimerObject ) :
					__TRIGsequenceStep_callback(0),
					__ECHOsequenceStep_callback(0),
					__CTimerFeatures( inputCTimerObject ) {

	int8_t	__tempErrorBuffer;

	__ticksCount_microSeconds = __MAX_TICKS_MEASUREMENT;

	// ## Obtención de los canales necesarios de MAT y CAP, para TRIG y ECHO respectivamente. ##

	// # MAT = TRIG #
	__MATchannelTRIG = __CTimerFeatures->Get_available_MAT_channel();
	if ( __MATchannelTRIG < 0 ) {
		return;		// < ERROR >
	}

	__tempErrorBuffer = __CTimerFeatures->SwitchMatrix_Config_MAT( portTrig, pinTrig, __MATchannelTRIG );
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__CTimerFeatures->Config_ExternalMatchOutput( __MATchannelTRIG, CTimer::EMR_Mode_t::EMR_SET );


	// # CAP = ECHO #
	__CAPchannelECHO = __CTimerFeatures->Get_available_CAP_channel();
	if ( __CAPchannelECHO < 0 ) {
		return;		// < ERROR >
	}

	__tempErrorBuffer = __CTimerFeatures->SwitchMatrix_Config_CAP( portEcho, pinEcho, __CAPchannelECHO );
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__CTimerFeatures->Config_CountControlRegister( __CAPchannelECHO, CTimer::CTCR_TimerCounter_Mode_t::TIMER_MODE,
												   true, CTimer::CTCR_Edge_t::CAP_RISING_EDGE );

	this->Set_TRIG_Callback_Sequence( nullptr );
	this->Set_ECHO_Callback_Sequence( nullptr );

	this->InstalarPerifericoTemporizado( this );
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
uint32_t Ultrasonido::Measure_Time() {
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
void Ultrasonido::Time_microSec_to_Distance_millimeters() {

	uint32_t measuredTime_microSec = this->Measure_Time();

	switch ( measuredTime_microSec ) {
		case Ultrasonido::TIME_MIN:
			__distance_millimeters = Ultrasonido::DISTANCE_MIN;
			break;

		case Ultrasonido::TIME_MAX:
			__distance_millimeters = Ultrasonido::DISTANCE_MAX;
			break;

		case Ultrasonido::TIME_NO_OBSTACLE:
			__distance_millimeters = Ultrasonido::DISTANCE_NO_OBSTACLE;
			break;
	
		default:
			__distance_millimeters = measuredTime_microSec * 10 / 58.0;
	}
}


/* #############################################
 * Set_TRIG_Callback_Sequence
 * #############################################
 * \brief:		Asigna el callback de TRIG a una secuencia.
 *
 * \input:
 * 	 \--->	inputCallback:	Secuencia de funciones callback
 * 	 						a ejecutar por cada interrupción.
 */
void Ultrasonido::Set_TRIG_Callback_Sequence( void (**inputCallback)(void) ) {
	for ( uint8_t index = 0; index < __CTimer_MAX_MR; index++ ) {
		if ( inputCallback != nullptr ) {
			__SecuenciaTRIGcallback[index] = inputCallback[index];
		} else {
			__SecuenciaTRIGcallback[index] = nullptr;
		}
	}

	__CTimerFeatures->Set_Callback( CTimer::registerSelection_MAT_CAP_t::MAT_REGISTER,
									__MATchannelTRIG, __SecuenciaTRIGcallback[__TRIGsequenceStep_callback] );
}


/* #############################################
 * Set_ECHO_Callback_Sequence
 * #############################################
 * \brief:			Asigna el callback de ECHO.
 *
 * \input:
 * 	 \--->	inputCallback:	Secuencia de funciones callback
 * 	 						a ejecutar por cada interrupción.
 */
void Ultrasonido::Set_ECHO_Callback_Sequence( void (**inputCallback)(void) ) {
	for ( uint8_t index = 0; index < __CTimer_MAX_CR; index++ ) {
		if ( inputCallback != nullptr ) {
			__SecuenciaECHOcallback[index] = inputCallback[index];
		} else {
			__SecuenciaECHOcallback[index] = nullptr;
		}
	}

	__CTimerFeatures->Set_Callback( CTimer::registerSelection_MAT_CAP_t::CAP_REGISTER,
									__CAPchannelECHO, __SecuenciaECHOcallback[__ECHOsequenceStep_callback] );
}


/* #############################################
 * HandlerDelPeriferico
 * #############################################
 * \brief:			Handler para actualizar información de distancia.
 */
void Ultrasonido::HandlerDelPeriferico() {

	--__ticksUpdateCount;

	if ( !__ticksUpdateCount ) {
		__ticksUpdateCount = __MAX_TICKS_UPDATE;
		// TODO: implementar...
	}
}


// ====================================================================================
// >> EN DESUSO: Implementación vieja previa al CTimer.
// ====================================================================================



///* #############################################
// * Trig_Pulse
// * #############################################
// * Manda un pulso de 10 us a "TRIG".
// */
//void Ultrasonido::Trig_Pulse() {
//
////	__trigHW.SetPin();
//	__pulseSent = Y_PULSE;
//}
//
//
///* #############################################
// * Check_Echo
// * #############################################
// * Checkea si recibió un pulso como eco.
// */
//void Ultrasonido::Check_Echo() {
//
//}
//
//
///* #############################################
// * Stop_Trig_Pulse
// * #############################################
// * Para el pulso del TRIG.
// */
//void Ultrasonido::Stop_Trig_Pulse() {
//
////	__trigHW.ClrPin();
//	__pulseSent = N_PULSE;
//}


