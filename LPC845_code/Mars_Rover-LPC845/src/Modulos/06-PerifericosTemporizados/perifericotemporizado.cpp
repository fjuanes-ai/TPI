/*******************************************************************************************************************************//**
 *
 * @file		swhandler.cpp
 * @brief		funciones miembro de la clase swhandler
 * @date		27 may. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Modulos/06-PerifericosTemporizados/perifericotemporizado.h"
#include "Drivers/LPC845.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define N_PERIFERICOS_TEMPORIZADOS		50

/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/
PerifericoTemporizado *g_perifericosTemporizados[ N_PERIFERICOS_TEMPORIZADOS ];
uint8_t PerifericoTemporizado::m_countPerifericosTemporizados = 0;

list <PerifericoTemporizado*> g_ListaDePerifericos;

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
PerifericoTemporizado::PerifericoTemporizado(){
	g_ListaDePerifericos.push_back( this );
}

PerifericoTemporizado::~PerifericoTemporizado() {
}

void PerifericoTemporizado::InstalarPerifericoTemporizado (PerifericoTemporizado * periferico)
{
	g_perifericosTemporizados[m_countPerifericosTemporizados] = periferico;
	m_countPerifericosTemporizados ++;

}



/* #############################################
 * Scheduler (ASYNC)
 * #############################################
 * Función ASÍNCRONA que barre el vector de periféricos temporizados.
 * Ejecuta todos los handlers.
 */
void Scheduler ( void ) {
	uint8_t i;

	for( i=0; i < PerifericoTemporizado::m_countPerifericosTemporizados; i++ )
		g_perifericosTemporizados[i]->HandlerDelPeriferico();
}


/* #############################################
 * Systick_Callback (ASYNC)
 * #############################################
 * ALTERNATIVA para barrer la lista de periféricos.
 * Función ASÍNCRONA que barre el vector de periféricos temporizados.
 * Ejecuta todos los handlers.
 */
void Systick_Callback ( void ) {
	for ( PerifericoTemporizado *q : g_ListaDePerifericos )
		q->HandlerDelPeriferico();
}



