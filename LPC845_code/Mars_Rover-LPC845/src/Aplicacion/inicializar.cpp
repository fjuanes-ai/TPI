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
//


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
CTimer ctimerObject( 1000000 );
Ultrasonido sensor_hc_sr04( 1, 0, 1, 5, &ctimerObject );
//GPIO lpcLED( GPIO::puertos_e::PORT1, 1, GPIO::direccion_e::SALIDA, GPIO::actividad_e::BAJO );

//void (**SecuenciaTRIG[])() = {
//		TRIGstep_01
//};


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
	// # Timers #
	// TODO: hacer una manera más modular para meter la información de TRIG y ECHO...
//	CTimer_Config( puertoTRIG, pinTRIG, puertoECHO, pinECHO, PRESCALER_DEFAULT );
	SysTick_Config( 1 );
//	lpcLED.SetPin();

//	sensor_hc_sr04.
}


/*********************************************
 * SecuenciaTRIG
 *********************************************
 * A
 */
void SecuenciaTRIG() {
//	sensor_hc_sr04.
}



