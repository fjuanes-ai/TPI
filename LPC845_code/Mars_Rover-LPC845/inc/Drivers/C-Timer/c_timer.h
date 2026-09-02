/*********************************************
 *
 * ### c_timer.h ###
 * 
 * @brief           Descripción del módulo...
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


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
    //


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
    void	CTimer_Config( uint8_t inputPort_MAT,
						   uint8_t inputPin_MAT,
						   uint8_t inputPort_CAP,
						   uint8_t inputPin_CAP );

    void 	CTimer_Config_MatchOutput( uint8_t inputPort_MAT,
			   	   	   	   	   	  	   uint8_t inputPin_MAT );

    void 	CTimer_Config_CaptureInput( uint8_t inputPort_CAP,
			   	   	   	   	   	   	    uint8_t inputPin_CAP );

    void 	SwitchMatrix_Config_MAT_CAP( uint8_t inputPort_MAT,
									     uint8_t inputPin_MAT,
										 uint8_t inputPort_CAP,
										 uint8_t inputPin_CAP );


    /* ###########################################
     * ### DEFINICIONES DE CLASES ###
     * ########################################### */
    //


#endif          /* DRIVERS_C_TIMER_C_TIMER_H_ */
