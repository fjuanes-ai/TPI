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
//


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
DigitalOutputs startBut( GPIO::PORT0, 4, 0, GPIO::BAJO, false );		// TODO: revisar pinout de pulsadores.
DigitalOutputs stopBut( GPIO::PORT0, 12, 0, GPIO::BAJO, false );
Intext emergencyBut( 2, GPIO::PORT0, 6, Emergency_Stop );


/*********************************************
 * Inicializar
 *********************************************
 * Inicializa el SysTick a 1 ms e inicia las secuencias.
 * Configura el modo de funcionamiento de la interrupción externa.
 */
void Inicializar() {
	// # Timers #
	SysTick_Config(1);
	SetupTimers();

	// # Interrupción externa #
	emergencyBut.set_flanco(Intext::ASCENDENTE);
}



