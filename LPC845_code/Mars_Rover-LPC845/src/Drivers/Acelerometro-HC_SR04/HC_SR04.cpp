/*********************************************
 *
 * ### HC_SR04.cpp ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 21, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */

#include "Drivers/Acelerometro-HC_SR04/HC_SR04.h"


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//void Stop_Trig_Pulse();


/**********************************************/


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */



/**********************************************/


/* #############################################
 * Acelerometro (CONSTRUCTOR)
 * #############################################
 * Establece puertos y pines del HW.
 */
Acelerometro::Acelerometro( uint8_t portTrig, uint8_t pinTrig, uint8_t portEcho, uint8_t pinEcho ) :
	__trigHW(portTrig, pinTrig, GPIO::SALIDA, GPIO::BAJO),
	__echoHW(portEcho, pinEcho, GPIO::ENTRADA, GPIO::BAJO),
	trigDuration(Timer::DEC, HC_SR04_IRQ)	// TODO: implementar HC_SR04_IRQ...
	{
}


/* #############################################
 * Trig_Pulse
 * #############################################
 * Manda un pulso de 10 uS a "TRIG".
 */
void Acelerometro::Trig_Pulse() {
	trigDuration.TimerStart( 10 );	// TODO: setear en uS.
	__trigHW.SetPin();
}


/* #############################################
 * Check_Echo
 * #############################################
 * Checkea si recibió un pulso como eco.
 */
void Acelerometro::Check_Echo() {

}


/* #############################################
 * Stop_Trig_Pulse
 * #############################################
 * Para el pulso del TRIG.
 */
void Acelerometro::Stop_Trig_Pulse() {
	__trigHW.ClrPin();
}


/* #############################################
 * Measure_Time
 * #############################################
 * Mide el tiempo de pulso recibido en "ECHO".
 *
 * # Valores típicos #
 * 	MIN: 100 uS.
 * 	MAX: 18  mS.
 * 	N/O: 36  mS.	(No Obstacle)
 */
void Acelerometro::Measure_Time() {

}


/* #############################################
 * Time_to_Distance
 * #############################################
 * Convierte la duración del pulso de "ECHO" a
 * centímetros (cm).
 *
 * # FÓRMULA #
 * uS / 58 = centimeters
 */
uint32_t Acelerometro::Time_to_Distance( uint32_t microSec ) {
	uint32_t o_DistanceCM = 0;	// Output distance in cm.

	if ( microSec != 0 ) {
		o_DistanceCM = microSec / 58;
	}

	return o_DistanceCM;
}


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */
//

