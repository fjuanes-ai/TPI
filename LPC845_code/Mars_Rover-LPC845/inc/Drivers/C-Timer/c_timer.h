/*********************************************
 *
 * ### c_timer.h ###
 * 
 * @brief           Drivers del periférico CTimer del LPC845.
 * @date            Aug 25, 2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         DRIVERS_C_TIMER_C_TIMER_H_
    #define     DRIVERS_C_TIMER_C_TIMER_H_

//	#include "Drivers/C-Timer/c_timer.h"

    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Drivers/LPC845.h"
	#include "Drivers/tipos.h"
	#include "Modulos/includeModulos.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */

	#define	PRESCALER_DEFAULT_FREQ	1000000		// MHz = 10^6 Hz
	#define	MAT_PERIOD_DEFAULT		10			// us  = 10^(-6) s
	#define	MAT_CHANNEL_LIMIT		4
	#define	CAP_CHANNEL_LIMIT		4


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
    //


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
//    void	CTimer_Config( uint8_t 	inputPort_MAT,
//						   uint8_t 	inputPin_MAT,
//						   uint8_t 	inputPort_CAP,
//						   uint8_t 	inputPin_CAP,
//						   uint32_t prescalerFrequency = PRESCALER_DEFAULT );
//
//    void 	CTimer_Config_MatchOutput( uint8_t 		inputMATchannel,
//    								   MCRvalues_t 	inputMCRmode,
//									   uint8_t 		bitValueCCR,
//									   uint32_t 	microSecondsMATCH );
//
//    void 	CTimer_Config_CaptureInput( uint8_t 	inputCAPchannel,
//    									CCRvalues_t inputCCRmode,
//										uint8_t 	bitValueMCR );
//
//    void 	SwitchMatrix_Config_MAT_CAP( uint8_t inputPort_MAT,
//									     uint8_t inputPin_MAT,
//										 uint8_t inputPort_CAP,
//										 uint8_t inputPin_CAP );


    /* ###########################################
     * ### DEFINICIONES DE CLASES ###
     * ########################################### */
    class CTimer {
    	// # Variables #
    	private:
    		uint8_t				__MATport;
    		uint8_t				__MATpin;
    		uint8_t 			__MATchannel;
    		static int8_t		__MATchannelsAvailable;		// Valor neg (< 0) = sin espacio para canales MAT.
    		uint32_t			__MATperiod;

    		uint8_t				__CAPport;
    		uint8_t				__CAPpin;
    		uint8_t 			__CAPchannel;
    		static int8_t		__CAPchannelsAvailable;

    		uint32_t			__prescalerFrequency;

    	public:
    		// # Valores posibles para el CCR, por canal #
    		typedef enum CCRvalues_e {
    			RISING_CCR = 0,
    			FALLING_CCR,
    			INTERRUPT_CCR
    		} CCRvalues_t;

    		// # Valores posibles para el MCR, por canal #
    		typedef enum MCRvalues_e {
    			INTERRUPT_MCR = 0,
    			RESET_MCR,
    			STOP_MCR
    		} MCRvalues_t;


		// # Métodos #
    	private:
    		void 		SwitchMatrix_Config_MAT_CAP();

    	public:
						CTimer( uint8_t 	inputPort_MAT,
								uint8_t 	inputPin_MAT,
								uint8_t 	inputPort_CAP,
								uint8_t 	inputPin_CAP,
								uint32_t 	prescalerFrequency = PRESCALER_DEFAULT_FREQ );
			void 		Config_MatchOutput(  MCRvalues_t 	inputMCRmode,
											 uint8_t		bitValueMCR,
											 uint32_t 		microSecondsMATCH = MAT_PERIOD_DEFAULT );
			void		Config_CaptureInput( CCRvalues_t 	inputCCRmode,
					  	  	  	  	  	 	 uint8_t 		bitValueCCR );
			uint32_t	GetCAPxValue();
			void 		SetMATxValue( uint32_t	inputMATvalue );
    };


#endif          /* DRIVERS_C_TIMER_C_TIMER_H_ */
