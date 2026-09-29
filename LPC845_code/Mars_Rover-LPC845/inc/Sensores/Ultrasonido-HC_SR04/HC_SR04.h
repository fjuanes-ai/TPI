/*********************************************
 *
 * ### HC_SR04.h ###
 * 
 * @brief           Archivo de cabecera del sensor ultrasónico HC-SR04.
 * @date            Jun 21, 2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_
    #define     DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_

//	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04.h"

    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Modulos/includeModulos.h"
	#include "Drivers/C-Timer/c_timer.h"
	#include "Sensores/Ultrasonido-HC_SR04/s_ultrasonico_plantilla.h"
	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
	#define		__MY_FLOAT_POS_INFINITY		0x7F800000
	#define		__MY_FLOAT_NEG_INFINITY		0xFF800000
	#define		__MY_DOUBLE_POS_INFINITY	0x7FF0000000000000
	#define		__MY_DOUBLE_NEG_INFINITY	0xFFF0000000000000
	#define		__SEQ_TRIG_STEPS			4
	#define		__SEQ_ECHO_STEPS			4


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

	class Us_HC_SR04 : protected PerifericoTemporizado, public Ultrasonido {
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
		public:
			uint8_t		__MATchannelTRIG;
			uint8_t		__CAPchannelECHO;

		private:
			uint16_t	__ticksUpdateCount;

			CTimer		*__CTimerFeatures;

			volatile void 	(**__sequenceCallbacks)(void);

			typedef enum pulse_e {
				N_PULSE	= 0,
				Y_PULSE	= 1
			} pulse_t;

		public:
			double		__distance_millimeters;


		// # Métodos #
		public:
						Us_HC_SR04( uint8_t portTrig, uint8_t pinTrig,
									 uint8_t portEcho, uint8_t pinEcho,
									 CTimer *inputCTimerObject = nullptr );
			uint32_t 	Measure_Time();
			void 		Time_microSec_to_Distance_millimeters();
			void 		Set_Callback_Sequence( volatile void (**inputCallback)(void) );
			void 		InicioDeSecuencia();
			void		HandlerDelPeriferico();
//						~Ultrasonido();
	};


#endif          /* DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_ */
