/*
 * GPIO.h
 *
 *  Created on: Apr 15, 2026
 *      Author: iyopolo
 */


#ifndef DRIVERS_LPC845_H_
	#define	DRIVERS_LPC845_H_

	// Typedefs de enteros con y sin signo.
	typedef	unsigned int			uint32_t;
	typedef	short unsigned int		uint16_t;
	typedef	unsigned char			uint8_t;
	typedef	int						int32_t;
	typedef	short int				int16_t;
	typedef	char					int8_t;

	#define __I						volatile const  	// !< Modificador para solo lectura
	#define __O     				volatile 	     	// !< Modificador para solo escritura
	#define __IO					volatile           	// !< Modificador lectura / escritura


	typedef struct {
	  __IO uint8_t 	B[2][32];
		   uint8_t 	RESERVED_0[4032];
	  __IO uint32_t W[2][32];
		   uint8_t 	RESERVED_1[3840];
	  __IO uint32_t DIR[2];
		   uint8_t 	RESERVED_2[120];
	  __IO uint32_t MASK[2];
		   uint8_t 	RESERVED_3[120];
	  __IO uint32_t PIN[2];
		   uint8_t 	RESERVED_4[120];
	  __IO uint32_t MPIN[2];
		   uint8_t 	RESERVED_5[120];
	  __IO uint32_t SET[2];
		   uint8_t 	RESERVED_6[120];
	  __O  uint32_t CLR[2];
		   uint8_t 	RESERVED_7[120];
	  __O  uint32_t NOT[2];
		   uint8_t 	RESERVED_8[120];
	  __O  uint32_t DIRSET[2];
		   uint8_t 	RESERVED_9[120];
	  __O  uint32_t DIRCLR[2];
		   uint8_t 	RESERVED_10[120];
	  __O  uint32_t DIRNOT[2];
	} GPIO_t;


	#define GPIO_BASE_ADDR		( 0xA0000000UL )
	#define GPIO_REG 			( (GPIO_t *) GPIO_BASE_ADDR )				// Casteo a tipo entero 4B puntero. Dirección BASE. Se suman los OFFSETS.
//	#define GPIO_REG			( (GPIO_t *) 0xA0000000UL )

	#define SYSAHBCLKCTRL0 		(*( ( volatile uint32_t * ) 0x40048080 ) )	// Vamos a una dirección y accedemos a su contenido
	#define SYSAHBCLKCTRL1 		(*( ( volatile uint32_t * ) 0x40048084 ) )	// Vamos a una dirección y accedemos a su contenido


	#define MAXPORT				2		// Puerto máximo del microprocesador.
	#define MAXPIN				32		// Pines máximo del microprocesador.

#endif
