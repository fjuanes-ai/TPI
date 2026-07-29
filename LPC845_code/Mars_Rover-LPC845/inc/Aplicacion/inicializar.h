/*********************************************
 *
 * ### inicializar.h ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 10, 2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         APLICACION_INICIALIZAR_H_
    #define     APLICACION_INICIALIZAR_H_


    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Drivers/LPC845.h"
	#include "Drivers/tipos.h"
	#include "Aplicacion/LED.h"
	#include "Aplicacion/PULSADOR.h"
	#include "Modulos/02-GPIO/GPIO.h"
	#include "Modulos/07-InterrupcionesExternas/intext.h"
	#include "Drivers/systick.h"
	#include "Modulos/05-SalidasDigitales/digital_outputs.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
    //


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
//	extern Intext pulsadorInterExt;
	extern DigitalOutputs startBut;		// TODO: revisar pinout de pulsadores.
	extern DigitalOutputs stopBut;
	extern Intext emergencyBut;


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
    void Inicializar();


#endif          /* APLICACION_INICIALIZAR_H_ */
