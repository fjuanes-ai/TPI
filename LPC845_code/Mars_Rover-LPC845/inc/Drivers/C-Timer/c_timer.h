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

	#define	CTIMER_TICKS_DEFAULT_FREQ	1000000		// MHz = 10^6 Hz
	#define	MAT_PERIOD_DEFAULT		10			// us  = 10^(-6) s
	#define	MAT_CHANNEL_LIMIT		4
	#define	CAP_CHANNEL_LIMIT		3


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

    		uint32_t			__ticksFrequency;

    	public:
    		// # Valores posibles para el CCR, por canal #
    		typedef enum CCRtriggers_e {
    			RISING_CCR = 0,
    			FALLING_CCR,
    			INTERRUPT_CCR
    		} CCRtriggers_t;

    		// # Valores posibles para el MCR, por canal #
    		typedef enum MCRtriggers_e {
    			INTERRUPT_MCR = 0,
    			RESET_MCR,
    			STOP_MCR
    		} MCRtriggers_t;

    		typedef enum CTCR_TimerCounter_Mode_e {
    			TIMER_MODE = 0x0,
				COUNTER_RISING_MODE,
				COUNTER_FALLING_MODE,
				COUNTER_DUAL_MODE
    		} CTCR_TimerCounter_Mode_t;

    		typedef enum CTCR_Edge_e {
    			CAP_RISING_EDGE = 0,
    			CAP_FALLING_EDGE
    		} CTCR_Edge_t;

    		typedef enum EMR_Mode_e {
    			EMR_NOTHING = 0,
				EMR_CLEAR,
				EMR_SET,
				EMR_TOGGLE
    		} EMR_Mode_t;


		// # Métodos #
    	private:
    		void 		SwitchMatrix_Config_MAT_CAP();

    	public:
						CTimer( uint8_t 	inputPort_MAT,
								uint8_t 	inputPin_MAT,
								uint8_t 	inputPort_CAP,
								uint8_t 	inputPin_CAP,
								uint32_t 	prescalerFrequency = CTIMER_TICKS_DEFAULT_FREQ );
			int8_t 		Set_MAT_CAP_Channels();
			void		Config_CountControlRegister( CTCR_TimerCounter_Mode_t 	inputMode,
													 bool 						clearTCwithCaptureEdge,
													 CTCR_Edge_t 				inputEdge );
			void 		Config_PrescalerFrequency( uint32_t prescalerFrequency );
			void 		Config_MatchOutput(  MCRtriggers_t 	inputMCRmode,
											 bool			bitValueMCR,
											 uint32_t 		microSecondsMATCH = MAT_PERIOD_DEFAULT );
			void		Config_CaptureInput( CCRtriggers_t 	inputCCRmode,
					  	  	  	  	  	 	 bool 			bitValueCCR );
			void		Config_ExternalMatchOutput( EMR_Mode_t inputMatchDemeanor );
			void 		Reset_Timer_Prescale();
			void 		EnableDisable_Timer_Prescale( bool inputEnableValue );
		__I uint32_t	GetCAPxValue() const;
			void 		SetMATxValue( uint32_t	inputMATvalue );
    };


#endif          /* DRIVERS_C_TIMER_C_TIMER_H_ */
