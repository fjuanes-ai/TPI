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

//	#include "Aplicacion/inicializar.h"


    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Drivers/tipos.h"
	#include "Modulos/includeModulos.h"
	#include "Drivers/Systick/systick.h"
	#include "Drivers/LPC845.h"
	#include "Drivers/C-Timer/c_timer.h"
	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04.h"
//	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
    //


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
	extern CTimer ctimerObject;
	extern Us_HC_SR04 sensor_hc_sr04;
	extern Uart serialCOMS_LPC_ESP;


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
    void Inicializar();

#endif          /* APLICACION_INICIALIZAR_H_ */
