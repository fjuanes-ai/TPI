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

	#define	CTIMER_DEFAULT_FREQ		1000000		// MHz = 10^6 Hz
	#define	MAT_PERIOD_DEFAULT		10			// us  = 10^(-6) s


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
    //


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */

	#if defined (__cplusplus)
		extern "C" {
			void CTIMER0_IRQHandler();
		}
	#endif


    /* ###########################################
     * ### DEFINICIONES DE CLASES ###
     * ########################################### */

    class CTimer {
    	// ## Variables ##
    	private:
			typedef struct MAT_data_s {
				uint8_t				port;
				uint8_t				pin;
				uint32_t			period;
				__IO uint32_t		* const EMRx;
				__IO uint32_t		* const MCRx;
				__IO uint32_t 		* const MRx;
				void				(*__callback)();
			} MAT_data_t;

			typedef struct CAP_data_s {
				uint8_t				port;
				uint8_t				pin;
				__IO uint32_t 		* const CCRmode;
				__IO uint32_t 		* const CTCRedge;
				__I  uint32_t 		* const CRx;
				void				(*__callback)();
			} CAP_data_t;

    	public:
    		static MAT_data_t	__MAT[__CTimer_MAX_MR];
    		static CAP_data_t	__CAP[__CTimer_MAX_CR];

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

    	private:
    		static bool			__isSetup;
    		static int8_t		__availableMATchannels;		// Valor neg (< 0) = sin espacio para canales MAT.
    		static int8_t		__availableCAPchannels;

    		uint32_t			__ticksFrequency;


		// ## Métodos ##
//    	private:

    	public:
						CTimer( uint32_t prescalerFrequency = CTIMER_DEFAULT_FREQ );
			int8_t 		SwitchMatrix_Config_MAT( uint8_t inputMATport, uint8_t inputMATpin, uint8_t channel );
			int8_t 		SwitchMatrix_Config_CAP( uint8_t inputCAPport, uint8_t inputCAPpin, uint8_t channel );
			int8_t 		Set_MAT_Channel();
			int8_t 		Set_CAP_Channel();
			void		Config_CountControlRegister( uint8_t					inputCAPchannel,
					  	  	  	  	  	  	  	  	 CTCR_TimerCounter_Mode_t 	inputMode,
													 bool 						clearTCwithCaptureEdge,
													 CTCR_Edge_t 				inputEdge );
			void 		Config_PrescalerFrequency( uint32_t prescalerFrequency );
			void 		Config_MatchOutput(  uint8_t		inputMATchannel,
	  	  	  	  	  	 	 	 	 	 	 MCRtriggers_t 	inputMCRmode,
											 bool			bitValueMCR,
											 uint32_t 		microSecondsMATCH = MAT_PERIOD_DEFAULT );
			int8_t		Config_CaptureInput( uint8_t		inputCAPchannel,
	  	 	 	 	 	  	  	  	  	  	 CCRtriggers_t 	inputCCRmode,
					  	  	  	  	  	 	 bool 			bitValueCCR );
			void		Config_ExternalMatchOutput( uint8_t inputMATchannel, EMR_Mode_t inputMatchDemeanor );
			void 		Reset_Timer_Prescale();
			void 		Enable_Timer_Prescale( bool inputEnableValue );
		__I uint32_t	GetCAPxValue( uint8_t channel ) const;
			void 		SetMATxValue( uint8_t channel, uint32_t	inputMATvalue );
    };


#endif          /* DRIVERS_C_TIMER_C_TIMER_H_ */
