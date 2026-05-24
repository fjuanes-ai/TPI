/*
 * PULSADOR.h
 *
 *  Created on: Apr 29, 2026
 *      Author: iyopolo
 */

#ifndef APLICACION_PULSADOR_H_
	#define APLICACION_PULSADOR_H_

	#include "Modulos/02-GPIO/GPIO.h"

	class PULSADOR: public GPIO {
		private:
			enum Estado_Pulsador {
				SUELTO = 0,
				PRESIONADO = 1
			};

		public:
			PULSADOR( uint8_t port , uint8_t pin );

			// Sobrecarga de operador para leer el pin.
			explicit operator bool();
			bool operator ==( PULSADOR &o );
	};

#endif /* APLICACION_PULSADOR_H_ */
