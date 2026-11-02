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



#include "Drivers/LPC845.h"
#include "pwm.h"

int main()
{
	SctPwm ledv(12000000U);
	SctPwm leda(12000000U);
	ledv.init();

	ledv.Config(SctCounter::L, PA_SCT_OUT1, 1, 0, 1000U);
	ledv.setPeriodMs(1000);      // 1 s
	ledv.setDutyCycle(80.0f);    // 250 ms encendido
	ledv.start();

	leda.Config(SctCounter::H, PA_SCT_OUT2, 1, 1, 1000U);
	leda.setPeriodMs(500);
	leda.setDutyCycle(50.0f);
	leda.start();


    while (1)
    {

    }
}
