/*********************************************
 *
 * ### inicializar.cpp ###
 * 
 * @brief           Inicializa el SysTick a 1 ms e inicia las secuencias.
 * 					Configura el modo de funcionamiento de la interrupción externa.
 * @date            Jun 10, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */
#include "Aplicacion/inicializar.h"


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
#define	__CTIMER_PRESCALER_FREQ		((uint32_t) 1e6)
// ## DEBUG ##
//#define	__CTIMER_PRESCALER_FREQ		((uint32_t) 1)
// ## DEBUG ##
#define __HC_SR04_TRIG_PORT			1
#define __HC_SR04_TRIG_PIN			0
#define __HC_SR04_ECHO_PORT			1
#define __HC_SR04_ECHO_PIN			1


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
CTimer ctimerObject( __CTIMER_PRESCALER_FREQ );
Us_HC_SR04 sensor_hc_sr04( __HC_SR04_TRIG_PORT, __HC_SR04_TRIG_PIN,
							__HC_SR04_ECHO_PORT, __HC_SR04_ECHO_PIN,
							&ctimerObject );
//GPIO lpcLED( GPIO::puertos_e::PORT1, 1, GPIO::direccion_e::SALIDA, GPIO::actividad_e::BAJO );


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */

/*********************************************
 * Inicializar
 *********************************************
 * Inicializa el SysTick a 1 ms e inicia las secuencias.
 * Configura el modo de funcionamiento de la interrupción externa.
 */
void Inicializar() {
//	 ### Systick ###
	SysTick_Config( 1 );
//	lpcLED.SetPin();


//	### Sensor Ultrasónico HC-SR04 ###

	// ## DEBUG ##
//	PINENABLE_Config( PE_DACOUT0, false );
//	PINENABLE_Config( PE_ADC_9, false );
	// ## DEBUG ##

	sensor_hc_sr04.Set_Callback_Sequence( ultrasonido_secuencia );
	sensor_hc_sr04.InicioDeSecuencia();
}



