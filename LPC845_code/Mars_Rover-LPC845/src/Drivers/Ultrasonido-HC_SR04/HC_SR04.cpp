/*********************************************
 *
 * ### HC_SR04.cpp ###
 * 
 * @brief           Drivers del sensor ultrasónico HC SR04.
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
#define		MAX_TICKS	10		// En us.


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

	__ticksCount_microSeconds = MAX_TICKS;
}


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
	// # TODO: Implementar método en CTimer para obtención de valores de CAP #
	uint32_t outputMeasuredTime = this->CTimer::GetCAPxValue();

	return outputMeasuredTime;
}


/* #############################################
 * Time_microSec_to_Distance_milimeters
 * #############################################
 * \brief:			Convierte la duración del pulso de "ECHO" a
 * 					centímetros (mm).
 *
 * # FÓRMULA #
 * uS * 10 / 58 = mm
 */
//uint32_t Ultrasonido::Time_microSec_to_Distance_milimeters( uint32_t inputTime_microSec ) {
void Ultrasonido::Time_microSec_to_Distance_milimeters() {
//	uint32_t outputDistance_milimeters = 0;

	uint32_t inputTime_microSec = this->Measure_Time();

	if ( inputTime_microSec != 0 ) {
//		outputDistance_milimeters = inputTime_microSec * 10 / 58.0;
		__distance_milimeters = inputTime_microSec * 10 / 58.0;
	}

//	return outputDistance_milimeters;
}


