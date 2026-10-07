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
						__trigHW( portTrig, pinTrig, GPIO::ENTRADA, GPIO::BAJO ),
						__echoHW( 7, portEcho, pinEcho, HC_SR04_IRQ ){

	__ticksCount = MAX_TICKS;
	InstalarPerifericoTemporizado(this);
}


/* #############################################
 * Trig_Pulse
 * #############################################
 * Manda un pulso de 10 us a "TRIG".
 */
void Ultrasonido::Trig_Pulse() {

//	__trigHW.SetPin();
	__pulseSent = Y_PULSE;
}


/* #############################################
 * Check_Echo
 * #############################################
 * Checkea si recibió un pulso como eco.
 */
void Ultrasonido::Check_Echo() {

}


/* #############################################
 * Stop_Trig_Pulse
 * #############################################
 * Para el pulso del TRIG.
 */
void Ultrasonido::Stop_Trig_Pulse() {

//	__trigHW.ClrPin();
	__pulseSent = N_PULSE;
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
void Ultrasonido::Measure_Time() {

}


/* #############################################
 * Time_to_Distance
 * #############################################
 * Convierte la duración del pulso de "ECHO" a
 * centímetros (cm).
 *
 * # FÓRMULA #
 * uS / 58 = centimetros
 */
uint32_t Ultrasonido::Time_to_Distance( uint32_t inputTime_uS ) {
	uint32_t outputDistance_CM = 0;

	if ( inputTime_uS != 0 ) {
		outputDistance_CM = inputTime_uS / 58;
	}

	return outputDistance_CM;
}


/* #############################################
 * HandlerDelPeriferico
 * #############################################
 * Checkea cada tick
 */
void Ultrasonido::HandlerDelPeriferico ( void ) {

	switch ( __pulseSent ) {

		case Y_PULSE:
			if ( __ticksCount )
				__ticksCount--;


			if ( !__ticksCount ) {

				__ticksCount = MAX_TICKS;
				__pulseSent = N_PULSE;

				// TODO: EJECUTAR EVENTO
//				Measure_Time();
			}
		break;


		case N_PULSE:
			Measure_Time();
		break;
	}

}


