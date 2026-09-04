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
	#include "Modulos/includeModulos.h"
	#include "Drivers/C-Timer/c_timer.h"
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

	class Ultrasonido : protected CTimer {
		private:
//			GPIO 		__trigHW;	// PINES en HARDWARE de Trigger + Echo.
//			Intext 		__echoHW;
			uint8_t		__pulseSent;
			uint8_t		__ticksCount_microSeconds;

			typedef enum pulse_e {
				N_PULSE	= 0,
				Y_PULSE	= 1
			} pulse_t;

		public:
//			Timer 		measureEvent;
//			Timer 		trigDuration;
//			Timer 		echoDuration;
			double		__distance_milimeters;
//			uint32_t	__distance_milimeters;


		public:
						Ultrasonido( uint8_t portTrig, uint8_t pinTrig, uint8_t portEcho, uint8_t pinEcho );
//			void 		Trig_Pulse();
//			void 		Check_Echo();
//			void 		Stop_Trig_Pulse();
			uint32_t 	Measure_Time();
//			uint32_t 	Time_microSec_to_Distance_milimeters( uint32_t inputTime_microSec );
			void 		Time_microSec_to_Distance_milimeters();
//						~Ultrasonido();
	};


#endif          /* DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_ */
