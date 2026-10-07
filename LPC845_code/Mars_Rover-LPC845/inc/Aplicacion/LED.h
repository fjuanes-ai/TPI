/*
 * LED.h
 *
 *  Created on: Apr 29, 2026
 *      Author: iyopolo
 */

#ifndef APLICACION_LED_H_
	#define APLICACION_LED_H_

	#include "Modulos/02-GPIO/GPIO.h"

	class LED: protected GPIO {
		public:
			LED( uint8_t port , uint8_t pin );
			void Encender();
			void Apagar();
			void CambiarEstado();
	};

#endif /* APLICACION_LED_H_ */
