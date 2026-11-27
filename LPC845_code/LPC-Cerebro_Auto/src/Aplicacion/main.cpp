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
    // Crear el objeto PWM
    SctPwm led(12000000U);

    // Inicializar el SCTimer una sola vez
    led.init();

    // Configurar el contador L y el pin PIO1_1
    // Los valores de pinMovible y asignación deben
    // corresponder al canal SCT_OUT elegido.
    led.Config(
        SctCounter::L,
        /* pinMovible */ 0,
        /* port */ 1,
        /* pin */ 1,
        /* frecuencia */ 1U
    );

    // Ciclo de trabajo del 50 %
    led.setDutyCycle(50.0f);

    // Arrancar el PWM
    led.start();

    while (1)
    {
    }
}
