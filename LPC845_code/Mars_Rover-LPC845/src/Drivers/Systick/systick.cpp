/* Código de TIMER (SysTick)
 * Iván Yolo.
 * R2022.
 * 13/05/2026
 */




/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Drivers/systick.h"


/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/


/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/
#define MAX_TICKS	0xFFFFFFUL


/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/
//typedef void (* Callback_Func)(void);


/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/


/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/
//list <PerifericoTemporizado*> g_ListaDePerifericos;
Callback_Func CallbackGlobal = nullptr;


/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS
 **********************************************************************************************************************************/


/***********************************************************************************************************************************
 *** PROTOTIPOS DE FUNCIONES PRIVADAS
 **********************************************************************************************************************************/



/* #############################################
 * SysTick_Config
 * #############################################
 * Configura manualmente las interrupciones del SysTick con
 * campos de bits.
 */
uint32_t SysTick_Config( uint32_t ms ) {

	uint32_t frecuenciaSystick, ticks;

	frecuenciaSystick = 1000 / ms;				 	// frecuenciaSystick = 1 / ( ms * 0.001)

	ticks = FREQ_CLOCK / frecuenciaSystick;  		// ticks = Tsystic / Tclock ;

	if (ticks > MAX_TICKS)
		return 1 ;

	SYST_RVR  =  ticks - 1UL;
//	SYST_RVR  =  0UL;
	SYST_CVR  =  0UL;			// Metemos el valor actual en 0 para que entre instantáneamente al IRQ.
//	SYS_CSR_ENABLE  =  1UL;
//	SYS_CSR_CLKSOURCE  =  1UL;
//	SYS_CSR_TICKINT  =  1UL;
	SYS_CSR = 0x7UL;			// Metemos una máscara para habilitar "ENABLE", "CLKSOURCE" y "TICKINT" al mismo tiempo.
								// Manualmente NO funciona (por alguna razón...).
	//o…. SYS_CSR  = 7;


	// # Función de interrupción CALLBACK #
	InstalarCallback( Scheduler );

	return 0;
}


/* #############################################
 * InstalarCallback
 * #############################################
 * Instala la función callback a elegir mediante un
 * puntero global
 */
void InstalarCallback( Callback_Func callback_input ) {
	if ( callback_input )
		CallbackGlobal = callback_input;
}


/* #############################################
 * SysTick_Handler (ASYNC)
 * #############################################
 * Función ASÍNCRONA que es ejecutada cada vez que se
 * reinicia la cuenta del SysTick.
 */
void SysTick_Handler() {
//	Le metemos un callback para no tocar más la función SysTick_Handler.
	if ( CallbackGlobal != nullptr )
		CallbackGlobal();
}



