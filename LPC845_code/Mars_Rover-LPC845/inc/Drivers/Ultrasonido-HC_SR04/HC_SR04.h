/*********************************************
 *
 * ### HC_SR04.h ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 21, 2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_
    #define     DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_


    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Modulos/02-GPIO/GPIO.h"
	#include "Modulos/06-PerifericosTemporizados/perifericotemporizado.h"
	#include "Modulos/03-Timer/Timer.h"
	#include "Drivers/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
    //


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
    //


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
    //


    /* ###########################################
     * ### DEFINICIONES DE CLASES ###
     * ########################################### */

	class Ultrasonido : protected PerifericoTemporizado {
		private:
			GPIO 		__trigHW;	// PINES en HARDWARE de Trigger + Echo.
			GPIO 		__echoHW;	// PINES en HARDWARE de Trigger + Echo.
			uint8_t		__pulseSent;
			uint8_t		__ticksCount;

			typedef enum pulse_e {
				N_PULSE	= 0,
				Y_PULSE	= 1
			} pulse_t;

//		public:
//			Timer 		measureEvent;
//			Timer 		trigDuration;
//			Timer 		echoDuration;


		public:
						Ultrasonido( uint8_t portTrig, uint8_t pinTrig, uint8_t portEcho, uint8_t pinEcho );
			void 		Trig_Pulse();
			void 		Check_Echo();
			void 		Stop_Trig_Pulse();
			void 		Measure_Time();
			void 		HandlerDelPeriferico ( void );
			uint32_t 	Time_to_Distance( uint32_t microSec );
//						~Ultrasonido();
	};


#endif          /* DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_ */
