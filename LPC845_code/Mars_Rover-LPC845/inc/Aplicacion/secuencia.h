/*********************************************
 *
 * ### secuencia.h ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 10, 2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         APLICACION_SECUENCIA_H_
    #define     APLICACION_SECUENCIA_H_


    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Aplicacion/inicializar.h"
	#include "Modulos/03-Timer/Timer.h"
	#include "Modulos/07-InterrupcionesExternas/intext.h"
	#include "Modulos/04-EntradasDigitales/digital_inputs.h"
	#include "Modulos/05-SalidasDigitales/digital_outputs.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
    //


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
    void SetupTimers();
    void Emergency_Stop();
    void Timers_Check();
    void Check_Seq();


#endif          /* APLICACION_SECUENCIA_H_ */
