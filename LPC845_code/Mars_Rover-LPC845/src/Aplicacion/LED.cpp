/*
 * LED.cpp
 *
 *  Created on: Apr 29, 2026
 *      Author: iyopolo
 */

#include "Aplicacion/LED.h"


// # USO DE LISTAS INICIALIZADORAS (:) #
LED::LED( uint8_t port , uint8_t pin ): GPIO( port, pin, SALIDA, BAJO ) {
/* El constructor de LED recibe "port" & "pin".
 *
 * Después, con listas inicializadoras, se instancia el constructor de GPIO con las 4 variables necesarias,
 * siendo las 2 primeras variables los argumentos del constructor de LED , y los últimos 2 CONSTANTES.
 *
 */
}

void LED::Encender() {
	// # ALT #
	// this->SetPin();
	SetPin();		// No es necesario acceder como una estructura porque es propio del objeto a instanciar.
}

void LED::Apagar() {
	ClrPin();
}

void LED::CambiarEstado()
{
	static uint8_t cambio = 0;

	switch ( cambio ) {
		case false:
			SetPin();
			break;

		case true:
			ClrPin();
			break;
	}
	cambio ++;
	cambio %= 2;
}
