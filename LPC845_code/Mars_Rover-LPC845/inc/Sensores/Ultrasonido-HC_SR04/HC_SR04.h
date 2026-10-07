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


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
//	#define		__MY_FLOAT_POS_INFINITY		0x7F800000
//	#define		__MY_FLOAT_NEG_INFINITY		0xFF800000
//	#define		__MY_DOUBLE_POS_INFINITY	0x7FF0000000000000
//	#define		__MY_DOUBLE_NEG_INFINITY	0xFFF0000000000000


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
//    extern Us_HC_SR04 sensor_hc_sr04;


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
//	#if defined (__cplusplus)
//		extern "C" {
//	    	volatile void Callback_CAP_Save_Time();
//	    	volatile void Callback_MAT_Enable_CAP_Readings();
//		}
//	#endif


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
							DISTANCE_MIN		 	=	20,
							DISTANCE_MAX		 	=	4000,
							DISTANCE_NO_OBSTACLE  	=  	0,
							DISTANCE_OUT_OF_RANGE 	=  	-1
			} HC_SR04_DistanceValues_millimeters_t;

		private:
			typedef enum HC_SR04_PulseState_e {
							PULSE_IDLE_READY 	 	= 	0,
							PULSE_OVER_WAITING_DELAY,
							PULSE_DELAY_OVER
			} HC_SR04_PulseState_t;


		// ### Variables ###
		public:
//			double			__distance_millimeters;
			uint16_t		__distance_millimeters;

		private:
			uint16_t		__ticksUpdateCount;
			HC_SR04_PulseState_t	__trigPulseState;

			// # MAT = TRIG #
			uint8_t			__MATchannelTRIG;
			const uint8_t	__MATchannelPWM;
//			uint8_t			__MATchannelDelay;

//			const uint32_t	__MAT_delayValue;
			uint32_t		__PWM_totalPeriod;
			uint32_t		__PWM_onPeriod;

			// # CAP = ECHO #
			uint8_t			__CAPchannelECHO_risingEdge;
			uint8_t			__CAPchannelECHO_fallingEdge;
			uint32_t		__initialCAPvalue;
			uint32_t		__finalCAPvalue;

			uint16_t		__measuredTime_microSec;

			CTimer		   *__CTimerFeatures;

//			volatile void 	(**__sequenceCallbacks)(void);


		// ### Métodos ###
		public:
						Us_HC_SR04( uint8_t portTrig, uint8_t pinTrig,
									uint8_t portEcho, uint8_t pinEcho,
									CTimer *inputCTimerObject = nullptr );
			void		PulseSent_TRIG();
			void 		Enable_ECHO_CAP_Readings();
			void 		Measure_Time();
			void 		Save_Distance_millimeters_from_Time_microSec();
			void		CAP_Save_Rising_Edge_Value();
			void		CAP_Save_Falling_Edge_Value();
			void		HandlerDelPeriferico();
//			void		Set_PWM_onPeriod( uint32_t onPeriod );
//			void		Set_PWM_totalPeriod( uint32_t totalPeriod );
//						~Ultrasonido();

		private:
			void		Config_TRIG( uint8_t portTrig, uint8_t pinTrig );
			void		Config_ECHO( uint8_t portEcho, uint8_t pinEcho );
			void  		Debug_HC_SR04();
	};


#endif          /* DRIVERS_Ultrasonido_HC_SR04_HC_SR04_H_ */
