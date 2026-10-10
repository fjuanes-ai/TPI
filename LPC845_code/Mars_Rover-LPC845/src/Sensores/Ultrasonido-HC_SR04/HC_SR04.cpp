/*********************************************
 *
 * ### HC_SR04.cpp ###
 * 
 * @brief           Drivers del sensor ultrasónico HC-SR04.
 * @date            Jun 21, 2026
 * @author          iyopolo
 *
 *********************************************/

/* ### Canales MAT/CAP utilizados ###
 *
 * # MAT #
 * ------------------------------------------------
 * Ut?	N°	Descripción
 * ------------------------------------------------
 * [X]	0	Canal de pulsos PWM para alimentar al TRIG.
 * [X]	1	Canal de delay/retardo entre pulso en TRIG y lectura en ECHO. CON INTERRUPCIÓN.
 * [ ]	2
 * [ ]	3
 * ------------------------------------------------
 *
 * # CAP #
 * ------------------------------------------------
 * Ut?	N°	Descripción
 * ------------------------------------------------
 * [X]	0	Canal de lectura de TC en flanco ascendente.
 * [X]	1 	Canal de lectura de TC en flanco descendente. CON INTERRUPCIÓN.
 * [ ]	2
 * [ ]	3
 * ------------------------------------------------
 */


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */
#include "Sensores/Ultrasonido-HC_SR04/HC_SR04.h"


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
extern Uart serialCOMS_LPC_ESP;


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
#define		__PWM_MAT_CHANNEL_DEFAULT	3
#define 	__PWM_PERIOD_DEFAULT		100e3				// us = 10^(-6) s.
#define 	__PWM_DUTY_CYCLE_DEFAULT	10					// us = 10^(-6) s.
#define		__MAX_TICKS_UPDATE			1000				// ms = 10^(-3) s.
#define		__MSG_SIZE_IN_BYTES			4


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
extern Us_HC_SR04 sensor_hc_sr04;


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */


// ====================================================================================
// ====================================================================================


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
#if defined (__cplusplus)
	extern "C" {
    	volatile void Callback_CAP_Save_Rising_Edge_Value();
    	volatile void Callback_CAP_Save_Falling_Edge_Value();
    	volatile void Callback_CAP_Save_Time();
	}
#endif


// ====================================================================================
// ====================================================================================



/*********************************************
 * *** FUNCIONES PÚBLICAS ***
 *********************************************/

/* #############################################
 * Callback_CAP_Save_Rising_Edge_Value (IRQ)
 * #############################################
 * Función asíncrona que guarda el tiempo entre lecturas del CAP.
 */
volatile void Callback_CAP_Save_Rising_Edge_Value() {
	sensor_hc_sr04.CAP_Save_Rising_Edge_Value();
}


/* #############################################
 * Callback_CAP_Save_Falling_Edge_Value (IRQ)
 * #############################################
 * Función asíncrona que guarda el tiempo entre lecturas del CAP.
 */
volatile void Callback_CAP_Save_Falling_Edge_Value() {
	sensor_hc_sr04.CAP_Save_Falling_Edge_Value();
}


/* #############################################
 * Callback_CAP_Save_Time (IRQ)
 * #############################################
 * Función asíncrona que guarda el tiempo entre lecturas del CAP.
 */
volatile void Callback_CAP_Save_Time() {
	sensor_hc_sr04.Measure_Time();
}


/* #############################################
 * Ultrasonido (CONSTRUCTOR)
 * #############################################
 * Establece puertos y pines del HW.
 */
Us_HC_SR04::Us_HC_SR04( uint8_t portTrig, uint8_t pinTrig,
						uint8_t portEcho, uint8_t pinEcho,
						const char inputTrama[],
						CTimer *inputCTimerObject ) :
			__ticksUpdateCount( 0 ),
			__MATchannelPWM( __PWM_MAT_CHANNEL_DEFAULT ),
			__PWM_totalPeriod( __PWM_PERIOD_DEFAULT ),
			__PWM_onPeriod( __PWM_DUTY_CYCLE_DEFAULT ),
			__initialCAPvalue( 0 ),
			__finalCAPvalue( 0 ),
			__CTimerFeatures( inputCTimerObject ) {

	// ## CTimer + drivers ##
	if ( __CTimerFeatures == nullptr ) {
		return;		// < ERROR >
	}

	__CTimerFeatures->Enable_Timer_Prescale( false );

	this->Config_TRIG( portTrig, pinTrig );
	this->Config_ECHO( portEcho, pinEcho );

	__CTimerFeatures->Enable_Timer_Prescale( true );
	__CTimerFeatures->Reset_Timer_Prescale();

	// ## Tx ##
	this->ArmadoDeTrama( inputTrama );

	// ## Callback para Tx + censado de datos ##
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

	// # MAT3 = PWM (totalPeriod) #
	__tempErrorBuffer = __CTimerFeatures->Get_available_MAT_channel();	// No lo usamos, pero dejamos en claro que tomamos un canal.
	if ( __tempErrorBuffer < 0 ) {
		return;		// < ERROR >
	}
//	__MATchannelPWM = 3;		// Se toma el canal 3 por defecto.
								// El constructor asigna dicho valor en la lista inicializadora.
								// No hace falta SWM porque no es de salida.

	// El NO configurado como PWM define Período total.
	__CTimerFeatures->Set_PWM_MAT_channel( __MATchannelPWM, false );
	__CTimerFeatures->Set_PWM_totalPeriod( __MATchannelPWM, __PWM_totalPeriod );

	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::INTERRUPT_MCR, false );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::RESET_MCR, true );
	__CTimerFeatures->Config_MatchControlRegister( __MATchannelPWM, CTimer::MCRtriggers_t::STOP_MCR, false );

	// El configurado como PWM define Duty Cycle (período en alto) individual.
	__CTimerFeatures->Set_PWM_MAT_channel( __MATchannelTRIG, true );
	__CTimerFeatures->Set_PWM_onPeriod( __MATchannelTRIG, __MATchannelPWM, __PWM_onPeriod );

	__CTimerFeatures->Config_MatchControlRegister( __MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, true );
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
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_risingEdge, CTimer::CCRtriggers_t::INTERRUPT_CCR, true );


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
	__CTimerFeatures->Config_CaptureControlRegister( __CAPchannelECHO_fallingEdge, CTimer::CCRtriggers_t::INTERRUPT_CCR, true );


	// ## CALLBACKS ##

	// # CAPx = ECHO 1 (rising edge) #
	__CTimerFeatures->Set_Callback( CTimer::registerSelection_MAT_CAP_t::CAP_REGISTER, __CAPchannelECHO_risingEdge, Callback_CAP_Save_Rising_Edge_Value );

	// # CAPy = ECHO 2 (falling edge) #
	__CTimerFeatures->Set_Callback( CTimer::registerSelection_MAT_CAP_t::CAP_REGISTER, __CAPchannelECHO_fallingEdge, Callback_CAP_Save_Falling_Edge_Value );


	// ## IOCON: Para reducir ruido ##
	// # Resistencia a Pulldown #
	IOCON_Config_PIO( portEcho, pinEcho, 0xFFFFFFFF, false );
	IOCON_Config_PIO( portEcho, pinEcho, __IOCON_MODE_PULL_DOWN_MASK, true );
}


/* #############################################
 * CAP_Save_Rising_Edge_Value (IRQ)
 * #############################################
 * \brief:			Copia automáticamente el valor de CAP0 (TC) al momento
 * 					en el cual CTimer detecta un flanco ascendente por el
 * 					pin de ECHO.
 *
 * Entra por interrupción en CAP0 (canal por flanco ascendente).
 */
void Us_HC_SR04::CAP_Save_Rising_Edge_Value() {
	__initialCAPvalue = __CTimerFeatures->CTimer::GetCAPxValue( __CAPchannelECHO_risingEdge );
}


/* #############################################
 * CAP_Save_Falling_Edge_Value (IRQ)
 * #############################################
 * \brief:			Copia automáticamente el valor de CAP0 (TC) al momento
 * 					en el cual CTimer detecta un flanco descendente por el
 * 					pin de ECHO.
 *
 * Entra por interrupción en CAP1 (canal por flanco descendente).
 */
void Us_HC_SR04::CAP_Save_Falling_Edge_Value() {
	__finalCAPvalue = __CTimerFeatures->CTimer::GetCAPxValue( __CAPchannelECHO_fallingEdge );
}


/* #############################################
 * Measure_Time (IRQ)
 * #############################################
 * \brief:			Mide el tiempo de pulso recibido en "ECHO".
 *
 * # Valores típicos #
 * 	MIN: 100 	uS.
 * 	MAX: 18000  uS.
 * 	N/O: 36000  uS.	(No Obstacle)
 */
void Us_HC_SR04::Measure_Time() {
	__finalCAPvalue = (__finalCAPvalue > __initialCAPvalue) ? (__finalCAPvalue - __initialCAPvalue) : 0;
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
 * Se convierte:
 * |-> 'm' 		-> 	'mm 		= 10^(-3) m'
 * |-> 'S^(-1)'	->	'(uS)^(-1) 	= (10^(-6) S)^(-1)	= 10^6 S^(-1)'
 */
void Us_HC_SR04::Save_Distance_millimeters_from_Time_microSec() {
	this->Measure_Time();

	__distance_millimeters = (__measuredTime_microSec * 343) / 2000;
//	__distance_millimeters = __measuredTime_microSec * (343.0 / 1000.0) / 2;
//	__distance_millimeters = __measuredTime_microSec * 100 / 583;	// Aproximación de la mitad de la velocidad del sonido.
//	__distance_millimeters = __measuredTime_microSec * 10 / 58.0;

	if ( __measuredTime_microSec <= (uint32_t) Us_HC_SR04::TIME_MIN )
		__distance_millimeters = Us_HC_SR04::DISTANCE_MIN;

	if ( (__measuredTime_microSec >= (uint32_t) Us_HC_SR04::TIME_MAX) && (__measuredTime_microSec < (uint32_t) Us_HC_SR04::TIME_NO_OBSTACLE) )
		__distance_millimeters = Us_HC_SR04::DISTANCE_MAX;

	if ( __measuredTime_microSec >= (uint32_t) Us_HC_SR04::TIME_NO_OBSTACLE )
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

		this->Save_Distance_millimeters_from_Time_microSec();

		// TODO: Armar trama con datos de sensado...
		this->EnvioDeDatos_UART();
	}
}


/* #############################################
 * ArmadoDeTrama
 * #############################################
 * \brief:			Arma la trama para envío de datos del sensor (SERIAL/UART).
 */
void Us_HC_SR04::ArmadoDeTrama( const char inputTrama[] ) {
	__datosTx[0] = inputTrama[0];
	__datosTx[3] = inputTrama[1];
}


/* #############################################
 * GuardarDatosSerial
 * #############################################
 * \brief:			Guardado de datos registrados para ser enviados
 * 					serialmente (UART).
 *
 * 	# Opción A #
 *  Toma la dirección de memoria de la variable como puntero a char,
 *  luego guarda ambos Bytes.
 *
 *  # Opción B #
 *  Utiliza uniones para poder transformar datos de tipo uint16_t a
 *  2 x uint8_t (serialización).
 */
void Us_HC_SR04::GuardarDatosSerial() {
	// # Opción A #
//	for ( uint8_t index = 0; index < sizeof(__distance_millimeters); index ++ )
//		__datosTx[index + 1] = ((uint8_t *) &__distance_millimeters)[index];


	// # Opción B #
//	typedef union SERIAL_PROTOCOL_DATA_u {
//		uint16_t	dataInput;
//		uint8_t	 	dataOutput[2];
//	} SERIAL_PROTOCOL_DATA_t;

	SERIAL_PROTOCOL_DATA_t temp_dataTx;
	temp_dataTx.dataInput = __distance_millimeters;

	for ( uint8_t index = 0; index < sizeof(temp_dataTx.dataInput); ++index )
		__datosTx[index + 1] = temp_dataTx.dataOutput[index];
}


/* #############################################
 * EnvioDeDatos_UART
 * #############################################
 * \brief:			Envío de datos CRUDOS a la base de datos (PC) mediante
 * 					datos seriales UART (ESP).
 */
void Us_HC_SR04::EnvioDeDatos_UART() {
	// TODO: Armar trama con datos de sensado...
//	this->ArmadoDeTrama();
	this->GuardarDatosSerial();

	serialCOMS_LPC_ESP.Transmit( (void *) __datosTx,
//								 sizeof(__datosTx) + 2 * sizeof(unsigned const char) );
//			 	 	 	 	 	 __MSG_SIZE_IN_BYTES );
								 sizeof(__datosTx) );
}


/* #############################################
 * Debug_HC_SR04
 * #############################################
 * \brief:			DEBUGEO del sensor (ECHO + TRIG).
 */
void Us_HC_SR04::Debug_HC_SR04() {

}


/* #############################################
 * Debug_HC_SR04
 * #############################################
 * \brief:			DEBUGEO del sensor (ECHO + TRIG).
 */
//void Debug_HC_SR04() {
//	...
//}
