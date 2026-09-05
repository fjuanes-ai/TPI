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
#define		__MAX_TICKS					10					// us = 10^(-6) s.
#define		__DISTANCE_MIN				20U					// mm = 10^(-3) m.
#define		__DISTANCE_MAX				4000U				// mm = 10^(-3) m.


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
//


/**********************************************/


/*********************************************
 * *** FUNCIONES PÚBLICAS ***
 *********************************************/


/* #############################################
 * Ultrasonido (CONSTRUCTOR)
 * #############################################
 * Establece puertos y pines del HW.
 */
Ultrasonido::Ultrasonido( uint8_t portTrig, uint8_t pinTrig, uint8_t portEcho, uint8_t pinEcho ) :
				CTimer( portTrig, pinTrig, portEcho, pinEcho, PRESCALER_DEFAULT_FREQ ) {

	__ticksCount_microSeconds = __MAX_TICKS;
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
	return this->CTimer::GetCAPxValue();
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
 * A
 * #############################################
 * \brief:			A.
 *
 * A
 */
void Ultrasonido::A() {
//	return this->CTimer::GetCAPxValue();
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


