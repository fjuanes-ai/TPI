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
	#include "Drivers/tipos.h"
	#include "Modulos/includeModulos.h"
	#include "Drivers/Systick/systick.h"
	#include "Drivers/LPC845.h"
	#include "Drivers/C-Timer/c_timer.h"
	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04.h"
	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


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
