/*********************************************
 *
 * ### secuencia.cpp ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 10, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */

#include "Aplicacion/secuencia.h"


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
typedef enum estadoSeq_e {
	PARADA = 0,
	MARCHA = 1
} estadoSeq_t;


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
void Seq1();
void Seq2();
void Seq3();
void Seq4();
void Seq5();
void Seq6();
void Is_Starting();
void Is_Stopping();


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
uint8_t sequence;

Timer timerValue;

DigitalOutputs output0( GPIO::PORT1, 0, 0, GPIO::BAJO, 0 );		// TODO: revisar variable "modo"...
DigitalOutputs output1( GPIO::PORT1, 1, 0, GPIO::BAJO, 0 );
DigitalOutputs output2( GPIO::PORT1, 2, 0, GPIO::BAJO, 0 );


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */

/*********************************************
 * Seq1 (IRQ)
 *********************************************/
void Seq1() {
	output0.Set();
	output1.Clr();
	output2.Clr();
	timerValue.TimerStart(3, Timer::SEG, Seq2);
}


/*********************************************
 * Seq2 (IRQ)
 *********************************************/
void Seq2() {
	output0.Clr();
	output1.Set();
	output2.Set();
	timerValue.TimerStart(2, Timer::SEG, Seq3);
}


/*********************************************
 * Seq3 (IRQ)
 *********************************************/
void Seq3() {
	output1.Clr();
	timerValue.TimerStart(4, Timer::SEG, Seq4);
}


/*********************************************
 * Seq4 (IRQ)
 *********************************************/
void Seq4() {
	output0.Set();
	timerValue.TimerStart(6, Timer::DEC, Seq5);
}


/*********************************************
 * Seq5 (IRQ)
 *********************************************/
void Seq5() {
	output0.Clr();
	output2.Clr();
	timerValue.TimerStart(2, Timer::SEG, Seq6);
}


/*********************************************
 * Seq6 (IRQ)
 *********************************************/
void Seq6() {
	output2.Set();
	timerValue.TimerStart(4, Timer::DEC, Check_Seq);
}


/*********************************************
 * Is_Starting
 *********************************************/
void Is_Starting(){
	if ( startBut )
		sequence = MARCHA;
}


/*********************************************
 * Is_Stopping
 *********************************************/
void Is_Stopping() {
	if ( stopBut )
		sequence = PARADA;
}


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */

/*********************************************
 * Timers_Check
 *********************************************
 * Ejecuta los handlers de los timers que son
 * lo suficientemente pesados para ser ejecutados en
 * una interrupción. Se checkean desde el "main".
 */
void Timers_Check(){
	timerValue.TmrEvent();
}

/*********************************************
 * SetupTimers
 *********************************************/
void SetupTimers(){

	output0.Clr();
	output1.Clr();
	output2.Clr();

	Check_Seq();
	// TODO: revisar cómo empezar secuencia y pulsadores SIN interrupciones...
	// Hacer con un ciclo que checkee pulsadores.
}


/*********************************************
 * Check_Seq
 *********************************************/
void Check_Seq(){
	Is_Starting();
	Is_Stopping();

	if ( sequence == MARCHA )
		Seq1();
}


/*********************************************
 * Emergency_Stop (IRQ)
 *********************************************/
void Emergency_Stop() {
	timerValue.TimerStop();

	sequence = PARADA;

	output0.Clr();
	output1.Clr();
	output2.Clr();
}
