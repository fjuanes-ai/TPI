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

//Intext pulsadorInterExt;

int main( void ) {

//	GPIO temp_GPIO_ECHO_CAP0( GPIO::PORT0, 17, GPIO::direccion_e::ENTRADA, GPIO::actividad_e::ALTO);
//	uint32_t temp_counter = 0;

	Inicializar();

    while ( 1 ) {
//    	if ( temp_GPIO_ECHO_CAP0.GetPin() ) {
//    		++temp_counter;
//    	}
    	test_UART.Transmit( "Messi   " );
    }



    return 0;
}
