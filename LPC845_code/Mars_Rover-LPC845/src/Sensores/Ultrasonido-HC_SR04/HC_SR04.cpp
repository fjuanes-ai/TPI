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
//#include "Sensores/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
extern Uart test_UART;


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
#define		__PWM_MAT_CHANNEL_DEFAULT	3
#define 	__PWM_PERIOD_DEFAULT		100e3				// us = 10^(-6) s.
#define 	__PWM_DUTY_CYCLE_DEFAULT	10					// us = 10^(-6) s.
//#define 	__PWM_PERIOD_DEFAULT		1e6					// us = 10^(-6) s.
//#define 	__PWM_DUTY_CYCLE_DEFAULT	100e3				// us = 10^(-6) s.
#define		__MAX_TICKS_UPDATE			100					// ms = 10^(-3) s.


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */


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
			__MATchannelPWM( __PWM_MAT_CHANNEL_DEFAULT ),
			__PWM_period( __PWM_PERIOD_DEFAULT ),
			__PWM_dutyCicle( __PWM_DUTY_CYCLE_DEFAULT ),
			__CTimerFeatures( inputCTimerObject ) {

	if ( __CTimerFeatures == nullptr ) {
		return;		// < ERROR >
	}

	__CTimerFeatures->Enable_Timer_Prescale( false );

	this->Config_TRIG( portTrig, pinTrig );
	this->Config_ECHO( portEcho, pinEcho );

	__CTimerFeatures->Enable_Timer_Prescale( true );
	__CTimerFeatures->Reset_Timer_Prescale();


	this->InstalarPerifericoTemporizado( this );
}


/* #############################################
 * Config_TRIG
 * #############################################
 * \brief:			Configura puerto-pin del TRIG del ultrasonido para
 * 					que funcione con PWM.
 *
 * Una vez que esté todo configurado, funciona de manera ASINCRÓNICA.
 * No hace interrupciones más que por la clas de "PerifericoTemporizado" para
 * mostrar datos cada cierto tiempo.
 *
 * Los datos de puerto-pin son guardados en el objeto tipo CTimer, en los
 * canales MAT/CAP guardados en esta clase (HC_SR04).
 */
void Us_HC_SR04::Config_TRIG( uint8_t portTrig, uint8_t pinTrig ) {
	int8_t	__tempErrorBuffer;

	// ### Obtención de los canales necesarios de MAT y CAP, para TRIG y ECHO respectivamente ###

	// ## MAT CHANNEL ##
	// # MATx = TRIG (duty cycle) #
	__tempErrorBuffer = __CTimerFeatures->Get_available_MAT_channel();
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__MATchannelTRIG = __tempErrorBuffer;

	__tempErrorBuffer = __CTimerFeatures->SwitchMatrix_Config_MAT( portTrig, pinTrig, __MATchannelTRIG );
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	// # MAT3 = PWM (period) #
	__tempErrorBuffer = __CTimerFeatures->Get_available_MAT_channel();	// No lo usamos, pero dejamos en claro que tomamos un canal.
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}
//	__MATchannelPWM = 3;		// Se toma el canal 3 por defecto.
								// El constructor asigna dicho valor en la lista inicializadora.
								// No hace falta SWM porque no es de salida.

	__CTimerFeatures->Set_PWM_MAT_channel( __MATchannelPWM, false );	// El NO configurado como PWM define Período total.
	this->Set_PWM_period( __PWM_period );
//	__CTimerFeatures->Config_ExternalMatchRegister( __MATchannelPWM, CTimer::EMR_Demeanor_t::EMR_NOTHING );		// Como no es salida, lo dejamos para que no haga nada.
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::INTERRUPT_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::RESET_MCR, true );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::STOP_MCR, false );

	__CTimerFeatures->Set_PWM_MAT_channel( __MATchannelTRIG, true );	// El configurado como PWM define Duty Cycle individual.
	this->Set_PWM_dutyCycle( __PWM_dutyCicle );
//	__CTimerFeatures->Config_ExternalMatchRegister( __MATchannelTRIG, CTimer::EMR_Demeanor_t::EMR_NOTHING );	// Como no es salida, lo dejamos para que no haga nada.
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelTRIG, CTimer::MCRtriggers_t::RESET_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelTRIG, CTimer::MCRtriggers_t::STOP_MCR, false );

}


/* #############################################
 * Config_ECHO
 * #############################################
 * \brief:			Configura puerto-pin del ECHO del ultrasonido para
 * 					que registre datos de tiempo, luego siendo
 * 					convertidos a distancia.
 *
 * Notar que se utilizan 2 canales CAP en mismo puerto-pin del ECHO:
 * CAPx:	Copia TC en flanco ascendente.
 * CAPx+1:	Copia TC en flanco descendente.
 *
 * Además, NO se configuran para que resetee ninguno la cuenta (TC), así
 * se puede configurar un PWM fácil (con un período suficientemente largo),
 * no pisando lecturas del ECHO con pulsos del TRIG constantes.
 *
 * Los datos de puerto-pin son guardados en el objeto tipo CTimer, en los
 * canales MAT/CAP guardados en esta clase (HC_SR04).
 */
void Us_HC_SR04::Config_ECHO( uint8_t portEcho, uint8_t pinEcho ) {
	int8_t	__tempErrorBuffer;

	// # CAPx = ECHO 1 (rising edge) #
	__tempErrorBuffer = __CTimerFeatures->Get_available_CAP_channel();
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__CAPchannelECHO_risingEdge = __tempErrorBuffer;

	__tempErrorBuffer = __CTimerFeatures->SwitchMatrix_Config_CAP( portEcho, pinEcho, __CAPchannelECHO_risingEdge );
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_risingEdge, CTimer::CCRtriggers_t::RISING_CCR, true );
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_risingEdge, CTimer::CCRtriggers_t::FALLING_CCR, false );
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_risingEdge, CTimer::CCRtriggers_t::INTERRUPT_CCR, false );

	// # CAPy = ECHO 2 (falling edge) #
	__tempErrorBuffer = __CTimerFeatures->Get_available_CAP_channel();
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__CAPchannelECHO_fallingEdge = __tempErrorBuffer;

	__tempErrorBuffer = __CTimerFeatures->SwitchMatrix_Config_CAP( portEcho, pinEcho, __CAPchannelECHO_fallingEdge );
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}

	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_fallingEdge, CTimer::CCRtriggers_t::RISING_CCR, false );
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_fallingEdge, CTimer::CCRtriggers_t::FALLING_CCR, true );
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_fallingEdge, CTimer::CCRtriggers_t::INTERRUPT_CCR, false );
}


/* #############################################
 * Set_PWM_dutyCycle
 * #############################################
 * \brief:			Establece el ciclo de trabajo o actividad.
 *
 * Notar que funciona AL REVÉS. Es decir. debido a que los de NXP piensan todo al revés,
 * utilizar un canal MATx como PWM y ponerle un valor hace que:
 * 1)	Arranque en 0.
 * 2)	Cambie al estado ALTO (1) cuando TC = MATx.
 *
 * Para que no revierta el ciclo de actividad, se puede pensar que la señal arranca cuando
 * nuestro PWM pasó al estado BAJO (0). Entonces, hay que cargarle un valor de:
 * __PWM_period - __PWM_dutyCicle
 * para que pase ese período de actividad en alto, y cuando llegue al valor de __PWM_period,
 * se hace 0 y reinicia.
 */
void Us_HC_SR04::Set_PWM_dutyCycle( uint32_t dutyCycle ) {
	__PWM_dutyCicle = dutyCycle;
	__CTimerFeatures->SetMATxValue( __MATchannelTRIG, __PWM_period - __PWM_dutyCicle );
}


/* #############################################
 * Set_PWM_period
 * #############################################
 * \brief:			Configura el período total de la/s señal/es PWM.
 */
void Us_HC_SR04::Set_PWM_period( uint32_t period ) {
	__PWM_period = period;
	__CTimerFeatures->SetMATxValue( __MATchannelPWM, __PWM_period );
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
void Us_HC_SR04::Measure_Time() {
	uint32_t initialCAPvalue;
	uint32_t finalCAPvalue;

	initialCAPvalue = __CTimerFeatures->CTimer::GetCAPxValue( __CAPchannelECHO_risingEdge );
	finalCAPvalue = __CTimerFeatures->CTimer::GetCAPxValue( __CAPchannelECHO_fallingEdge );

	__measuredTime_microSec = (finalCAPvalue - initialCAPvalue);
}


/* #############################################
 * Save_Distance_millimeters_from_Time_microSec
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
//uint32_t Ultrasonido::Save_Distance_millimeters_from_Time_microSec( uint32_t inputTime_microSec ) {
void Us_HC_SR04::Save_Distance_millimeters_from_Time_microSec() {

	this->Measure_Time();

	__distance_millimeters = __measuredTime_microSec * 10 / 58.0;

	if ( __measuredTime_microSec <= Us_HC_SR04::TIME_MIN )
		__distance_millimeters = Us_HC_SR04::DISTANCE_MIN;

	if ( (__measuredTime_microSec >= Us_HC_SR04::TIME_MAX) && (__measuredTime_microSec <  Us_HC_SR04::TIME_NO_OBSTACLE) )
		__distance_millimeters = Us_HC_SR04::DISTANCE_MAX;

	if ( __measuredTime_microSec >=  Us_HC_SR04::TIME_NO_OBSTACLE )
		__distance_millimeters = Us_HC_SR04::DISTANCE_NO_OBSTACLE;
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
		this->Save_Distance_millimeters_from_Time_microSec();
		test_UART.Message( (void *) "Hola\n" );
		// ## DEBUG ##
	}
}


/* #############################################
 * Debug_HC_SR04
 * #############################################
 * \brief:			DEBUGEO del sensor (ECHO + TRIG).
 */
void Us_HC_SR04::Debug_HC_SR04() {
	__CTimerFeatures->Enable_Timer_Prescale( false );

	// ### Deshabilitación del MATx/EMRx ###
	// # MATx = TRIG (duty cycle) #
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelTRIG, CTimer::MCRtriggers_t::STOP_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelTRIG, CTimer::MCRtriggers_t::RESET_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, false );
	__CTimerFeatures->Config_ExternalMatchRegister( __MATchannelTRIG, CTimer::EMR_Demeanor_t::EMR_NOTHING );

	__CTimerFeatures->SetMATxValue( __MATchannelTRIG, __PWM_DUTY_CYCLE_DEFAULT );
	__CTimerFeatures->Config_MatchShadow( __MATchannelTRIG, false );


	// # MAT3 = PWM period #
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::STOP_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::RESET_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::INTERRUPT_MCR, false );
	__CTimerFeatures->SetMATxValue( __MATchannelPWM, __PWM_PERIOD_DEFAULT );


	// # Habilitación del CAPx #
	// # Interrupción por flanco ascendente #
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_risingEdge, CTimer::CCRtriggers_t::RISING_CCR, false );
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_risingEdge, CTimer::CCRtriggers_t::FALLING_CCR, true );
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_risingEdge, CTimer::CCRtriggers_t::INTERRUPT_CCR, true );


//	__CTimerFeatures->Set_Callback( CTimer::registerSelection_MAT_CAP_t::CAP_REGISTER, __CAPchannelECHO_risingEdge, Callback_Save_Time );

	__CTimerFeatures->Enable_Timer_Prescale( true );
	__CTimerFeatures->Reset_Timer_Prescale();
}


/* #############################################
 * Debug_HC_SR04
 * #############################################
 * \brief:			DEBUGEO del sensor (ECHO + TRIG).
 */
//void Debug_HC_SR04() {
//	...
//}

