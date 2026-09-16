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
	#include "Drivers/Systick/systick.h"
	#include "Drivers/LPC845.h"
	#include "Drivers/tipos.h"
	#include "Drivers/C-Timer/c_timer.h"
	#include "Drivers/Ultrasonido-HC_SR04/HC_SR04.h"
	#include "Drivers/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"
	#include "Modulos/includeModulos.h"
	#include "Aplicacion/LED.h"
	#include "Aplicacion/PULSADOR.h"


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
    void Inicializar();
    void SecuenciaTRIG();

#endif          /* APLICACION_INICIALIZAR_H_ */
