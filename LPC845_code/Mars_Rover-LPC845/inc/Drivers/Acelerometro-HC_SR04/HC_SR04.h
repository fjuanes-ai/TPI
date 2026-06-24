/*********************************************
 *
 * ### HC_SR04.h ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 21, 2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         DRIVERS_ACELEROMETRO_HC_SR04_HC_SR04_H_
    #define     DRIVERS_ACELEROMETRO_HC_SR04_HC_SR04_H_


    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Modulos/02-GPIO/GPIO.h"
	#include "Modulos/06-PerifericosTemporizados/perifericotemporizado.h"
	#include "Modulos/03-Timer/Timer.h"
	#include "Drivers/Acelerometro-HC_SR04/HC_SR04-IRQ.h"


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

	class Acelerometro : protected PerifericoTemporizado {
		private:
			GPIO 		__trigHW;	// PINES en HARDWARE de Trigger + Echo.
			GPIO 		__echoHW;	// PINES en HARDWARE de Trigger + Echo.;


		public:
			Timer 		measureEvent;
			Timer 		trigDuration;
			Timer 		echoDuration;


		public:
						Acelerometro( uint8_t portTrig, uint8_t pinTrig, uint8_t portEcho, uint8_t pinEcho );
			void 		Trig_Pulse();
			void 		Check_Echo();
			void 		Stop_Trig_Pulse();
			void 		Measure_Time();
			uint32_t 	Time_to_Distance( uint32_t microSec );
//		~Acelerometro();
	};


#endif          /* DRIVERS_ACELEROMETRO_HC_SR04_HC_SR04_H_ */
