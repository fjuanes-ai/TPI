/*********************************************
 *
 * ### HC_SR04-IRQ.h ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 24, 2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         DRIVERS_Ultrasonido_HC_SR04_HC_SR04_IRQ_H_
    #define     DRIVERS_Ultrasonido_HC_SR04_HC_SR04_IRQ_H_

//	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Sensores/Ultrasonido-HC_SR04/HC_SR04.h"
//	#include "Aplicacion/inicializar.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
	#define __HC_SR04_MDE_STEPS		5


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
    extern void (*ultrasonido_secuencia[])(void);
	extern CTimer ctimerObject;
	extern class Ultrasonido sensor_hc_sr04;


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
    void HC_SR04_IRQ( void );


    /* ###########################################
     * ### DEFINICIONES DE CLASES ###
     * ########################################### */
    //


#endif          /* DRIVERS_Ultrasonido_HC_SR04_HC_SR04_IRQ_H_ */
