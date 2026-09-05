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
	#define		__MY_FLOAT_POS_INFINITY		0x7F800000
	#define		__MY_FLOAT_NEG_INFINITY		0xFF800000
	#define		__MY_DOUBLE_POS_INFINITY	0x7FF0000000000000
	#define		__MY_DOUBLE_NEG_INFINITY	0xFFF0000000000000


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
		// # Tipos de datos #
		public:
			typedef enum HC_SR04_TimeValues_microSeconds_e {
				TIME_MIN		 =	100,
				TIME_MAX		 =	18000,
				TIME_NO_OBSTACLE =  36000
			} HC_SR04_TimeValues_microSeconds_t;

			typedef enum HC_SR04_DistanceValues_millimeters_e {
							DISTANCE_MIN		 =	20,
							DISTANCE_MAX		 =	4000,
							DISTANCE_NO_OBSTACLE =  __MY_DOUBLE_POS_INFINITY
			} HC_SR04_DistanceValues_millimeters_t;


		// # Variables #
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
			double		__distance_millimeters;
//			uint32_t	__distance_millimeters;


		// # Métodos #
		public:
						Ultrasonido( uint8_t portTrig, uint8_t pinTrig, uint8_t portEcho, uint8_t pinEcho );
//			void 		Trig_Pulse();
//			void 		Check_Echo();
//			void 		Stop_Trig_Pulse();
			uint32_t 	Measure_Time();
//			uint32_t 	Time_microSec_to_Distance_millimeters( uint32_t inputTime_microSec );
			void 		Time_microSec_to_Distance_millimeters();
//						~Ultrasonido();
	};


#endif          /* DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_ */
