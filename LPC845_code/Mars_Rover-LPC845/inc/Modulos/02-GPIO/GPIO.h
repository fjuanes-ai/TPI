/*
 * GPIO.h
 *
 *  Created on: Apr 15, 2026
 *      Author: iyopolo
 */


#ifndef MODULOS_02_GPIO_GPIO_H_
	#define MODULOS_02_GPIO_GPIO_H_

	#include "Drivers/LPC845.h"
	#include "Modulos/01-In_Out/in.h"
	#include "Modulos/01-In_Out/out.h"


	class GPIO : public IN, OUT {
		private:
			const uint8_t	__port;
			const uint8_t	__pin;
			uint8_t			__direccion;
			const uint8_t	__actividad;

		public:
			enum actividad_e {
				BAJO = 0,
				ALTO = 1
			};

			enum estado_e {
				OFF = 0,
				ON = 1
			};

			enum direccion_e {
				ENTRADA = 0,
				SALIDA = 1
			};

			enum puertos_e {
				PORT0 = 0,
				PORT1 = 1
			};


//			GPIO();				// CONSTRUCTOR (prototipo).
			GPIO( uint8_t port, uint8_t pin, uint8_t direccion, uint8_t actividad = ALTO );		// CONSTRUCTOR (prototipo).																					// SOLO en su prototipo se ponen los parámetros defaults.

			void SetPin () const;	// Son ctes ya que no cambian objetos dentro de la clase.
			void ClrPin () const;
			bool GetPin () const;
			void SetDir ( uint8_t direccion );

			virtual ~GPIO();	// DESTRUCTOR (prototipo).

	};

#endif /* MODULOS_02_GPIO_GPIO_H_ */
