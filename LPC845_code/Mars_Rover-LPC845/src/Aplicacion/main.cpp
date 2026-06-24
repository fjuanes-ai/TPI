/*********************************************
 *
 * ### TPO: Mars Rover - Grupo 6 ###
 * ## Informática II - R2002 - CL2026 ##
 *
 * @brief           Prueba de interrupciones externas. + Práctica de
 * 					timers en secuencia.
 *
 * @author          Francisco Juanes
 * 					Mateo Román
 * 					Iván Yopolo
 *
 *********************************************/
//
#include "Aplicacion/inicializar.h"
#include "Aplicacion/secuencia.h"

//Intext pulsadorInterExt;

int main( void ) {

	Inicializar();

    while ( 1 ) {
    	Timers_Check();
    	Check_Seq();
    }



    return 0;
}
