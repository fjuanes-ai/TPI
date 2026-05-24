/* ### MAIN CON CÓDIGO DE PRUEBA (ejercicio de cátedra) ###
 * ## UTN.FRBA ##
 * # PROYECTO: Mars Rover con mapeo topográfico #
 *
 * Grupo:			N° 6.
 *
 * Integrantes:		Juanes 	Francisco.
 * 					Román 	Mateo.
 * 					Yopolo	Iván.
 *
 *
 *
 */



#include "Aplicacion/LED.h"
#include "Aplicacion/PULSADOR.h"



int main( void ) {

	LED led_G( GPIO::PORT1, 0 );
	LED led_B( GPIO::PORT1, 1 );
	LED led_R( GPIO::PORT1, 2 );
	PULSADOR puls( GPIO::PORT0, 4 );


	led_B.Apagar();
	led_G.Apagar();
	led_R.Apagar();
	for ( uint32_t i = 0; i < 50000; i++ );


	led_B.Encender();
	for ( uint32_t i = 0; i < 75000; i++ );
	led_B.Apagar();

	led_G.Encender();
	for ( uint32_t i = 0; i < 75000; i++ );
	led_G.Apagar();

	led_R.Encender();
	for ( uint32_t i = 0; i < 75000; i++ );
	led_R.Apagar();


    while ( 1 ) {
    	if ( puls ) {

    		led_B.CambiarEstado();
    		for ( uint32_t i = 0; i < 50000; i++ );

    		led_G.CambiarEstado();
    		for ( uint32_t i = 0; i < 50000; i++ );

    		led_R.CambiarEstado();
    		for ( uint32_t i = 0; i < 50000; i++ );
    	} else {

    		led_B.Apagar();
    		led_G.Apagar();
    		led_R.Apagar();
    	}

    }


    return 0;
}
