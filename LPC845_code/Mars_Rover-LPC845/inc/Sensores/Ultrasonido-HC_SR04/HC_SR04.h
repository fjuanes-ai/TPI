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
//    void Debug_HC_SR04();
    volatile void Callback_Save_Time();


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

		// ### Variables ###
			// TODO:
		// Estos son públicos solamente para que las funciones callback funcionen.
		// En caso de que funcionen sin callbacks, PONERLAS PRIVADAS.
//		private:
		public:
			// # MAT = TRIG #
			uint8_t			__MATchannelTRIG;
			const uint8_t	__MATchannelPWM;
			// # CAP = ECHO #
			uint8_t			__CAPchannelECHO_risingEdge;
			uint8_t			__CAPchannelECHO_fallingEdge;

		private:
			uint32_t	__measuredTime_microSec;
			uint16_t	__ticksUpdateCount;
			uint32_t	__PWM_period;
			uint32_t	__PWM_dutyCicle;

			CTimer		*__CTimerFeatures;

			volatile void 	(**__sequenceCallbacks)(void);

			typedef enum pulse_e {
				N_PULSE	= 0,
				Y_PULSE	= 1
			} pulse_t;

		public:
			double		__distance_millimeters;


		// ### Métodos ###
		public:
						Us_HC_SR04( uint8_t portTrig, uint8_t pinTrig,
									 uint8_t portEcho, uint8_t pinEcho,
									 CTimer *inputCTimerObject = nullptr );
			void 		Measure_Time();
			void		Set_PWM_dutyCycle( uint32_t dutyCycle );
			void		Set_PWM_period( uint32_t period );
			void 		Save_Distance_millimeters_from_Time_microSec();
			void 		InicioDeSecuencia();
			void		HandlerDelPeriferico();
//						~Ultrasonido();

		private:
			void		Config_TRIG( uint8_t portTrig, uint8_t pinTrig );
			void		Config_ECHO( uint8_t portEcho, uint8_t pinEcho );
			void  		Debug_HC_SR04();
	};


#endif          /* DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_ */
