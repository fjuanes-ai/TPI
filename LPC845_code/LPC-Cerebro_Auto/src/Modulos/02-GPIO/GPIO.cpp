/*
 * GPIO.cpp
 *
 *  Created on: Apr 15, 2026
 *      Author: iyopolo
 */

#include "Modulos/02-GPIO/GPIO.h"


// La clase, al tener ctes, SIEMPRE hay que inicializarlas. Los 2 constructores chocan en ese sentido y por eso da error.
//GPIO::GPIO() {
//	// TODO Auto-generated constructor stub
//
//}

// # LISTA INICIALIZADORA #
// Esa manera de poner los valroes entre paréntesis al lado de variables es para inicializar dichas variables constantes.
GPIO::GPIO( uint8_t port, uint8_t pin, uint8_t direccion, uint8_t actividad ) :
__port(port), __pin(pin), __direccion(direccion), __actividad(actividad)  {
//	this->port = port;			// Si hay nombres redundantes, se puede usar "this->" para indicar las variables privadas.

	SYSAHBCLKCTRL0 |= (1 << 6) | (1 << 20);			// No importa que siempre encendamos el CLOCK, con 1 vez alcanza y otras no afectan.
	// bit 6:	Habilita los puertos de GPIO_0 (puerto 0).
	// bit 20:	Habilita los puertos de GPIO_1 (puerto 1).

	switch ( __direccion ) {
		case SALIDA:
			GPIO_REG->DIR[__port] |= (1 << __pin);		// Por prolijidad, usamos las variables internas.
		break;

		case ENTRADA:
			GPIO_REG->DIR[__port] &= ~(1 << __pin);		// Lo negamos + AND para poner un 0 (entrada).
		break;
	}
}


void GPIO::SetPin(void) const {	// No recibe nada porque al ser parte del objeto, ya tenemos las características.

	GPIO_REG->B[__port][__pin] = ( __actividad == ALTO ) ? ON : OFF;
}


void GPIO::ClrPin(void) const {	// No recibe nada porque al ser parte del objeto, ya tenemos las características.

	GPIO_REG->B[__port][__pin] = ( __actividad == ALTO ) ? OFF : ON;
}


bool GPIO::GetPin(void) const {	// No recibe nada porque al ser parte del objeto, ya tenemos las características.

	return ( ( __actividad == ALTO )  ?  GPIO_REG->B[__port][__pin]  :  !(GPIO_REG->B[__port][__pin]) );
}


void GPIO::SetDir ( uint8_t direccion ) {

	__direccion = direccion;

	switch ( __direccion ) {
		case SALIDA:
			GPIO_REG->DIR[__port] |= (1 << __pin);
		break;

		case ENTRADA:
			GPIO_REG->DIR[__port] &= ~(1 << __pin);
		break;
	}
}


GPIO::~GPIO() {
	// TODO Auto-generated destructor stub
}

