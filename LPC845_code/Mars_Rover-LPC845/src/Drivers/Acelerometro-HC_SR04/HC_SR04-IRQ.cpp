/*********************************************
 *
 * ### HC_SR04-IRQ.cpp ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 24, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */

#include "Drivers/Acelerometro-HC_SR04/HC_SR04-IRQ.h"


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
#define		PORT_TRIG	GPIO::PORT1
#define		PORT_ECHO	GPIO::PORT1
#define		PIN_TRIG	1
#define		PIN_ECHO	2


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
// Instanciamos un objeto tipo Acelerómetro con pines predefinidos...
Acelerometro sensAcelerometro( PORT_TRIG, PIN_TRIG, PORT_ECHO, PIN_ECHO );


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//


/**********************************************/


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */
void HC_SR04_IRQ ( void ) {

}

