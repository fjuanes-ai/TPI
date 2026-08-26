#ifndef __DRIVERS_SYSTICK_H
	#define __DRIVERS_SYSTICK_H


	// # COMPATIBILIDAD CON C & C++ #
	#if defined (__cplusplus)
		extern "C" {
			// Prototipos de funciones en C a incorporar
			void SysTick_Handler();
		}
	#endif


	/* #############################################
	 * INCLUDES
	 * #############################################
	 */
	#include "Modulos/02-GPIO/GPIO.h"
	#include "Drivers/LPC845.h"
	#include "Modulos/06-PerifericosTemporizados/perifericotemporizado.h"

	/* #############################################
	 * TIPOS DE DATOS GLOBALES
	 * #############################################
	 */
	typedef void (* Callback_Func)(void);


	/* #############################################
	 * VARIABLES GLOBALES
	 * #############################################
	 */
//	extern list <PerifericoTemporizado*> g_ListaDePerifericos;


	/* #############################################
	 * FUNCIONES
	 * #############################################
	 */
	uint32_t SysTick_Config( uint32_t ms );
	void InstalarCallback( Callback_Func callback_input );

#endif
