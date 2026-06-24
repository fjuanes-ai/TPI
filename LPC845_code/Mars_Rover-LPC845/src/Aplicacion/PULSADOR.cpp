/*
 * PULSADOR.cpp
 *
 *  Created on: Apr 29, 2026
 *      Author: iyopolo
 */

#include "Aplicacion/PULSADOR.h"

PULSADOR::PULSADOR( uint8_t port , uint8_t pin ) :
GPIO::GPIO( port, pin, ENTRADA, BAJO ) {

}

PULSADOR::operator bool() {
	return GPIO::GetPin();
//	# ALT #
//	return this->GetPin();
}

bool PULSADOR::operator ==( PULSADOR &o ) {
	if ( o.GetPin() == this->GetPin() )
		return true;
	else
		return false;
}
