/*********************************************
 *
 * ### HC_SR04-IRQ.cpp ###
 * 
 * @brief           Descripción del módulo...
 * @date            Jun 24, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */
#include "Sensores/Ultrasonido-HC_SR04/HC_SR04-IRQ.h"


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
//#define __HC_SR04_MDE_STEPS		4


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
#if defined (__cplusplus)
	extern "C" {
		static volatile void MdE_Ultrasonido_MidiendoTiempoECHO();
		static volatile void MdE_Ultrasonido_DelayReinicio();
		static volatile void MdE_Ultrasonido_PulsoTRIGalto();
		static volatile void MdE_Ultrasonido_SetupDelayReinicio();
		static volatile void MdE_Ultrasonido_EsperandoECHO();
	}
#endif


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
volatile void (*ultrasonido_secuencia[__HC_SR04_MDE_STEPS])(void) = {
		MdE_Ultrasonido_PulsoTRIGalto,
		MdE_Ultrasonido_EsperandoECHO,
		MdE_Ultrasonido_MidiendoTiempoECHO,
		MdE_Ultrasonido_SetupDelayReinicio,
		MdE_Ultrasonido_DelayReinicio
};

static uint8_t	ultrasonido_indice_mde = 0;



// ====================================================================================
// ====================================================================================



/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */


/* #############################################
 * MdE_Ultrasonido_PulsoTRIGalto (IRQ)
 * #############################################
 * Paso de la máquina de estados.
 *
 * Entra por interrupción de HW (el MATx llegó a su valor y EMR pasó a ALTO).
 *
 * Entra con:
 * 	|--> EMRx en ALTO.
 * 	|--> EMRx Habilitado.
 * 	|--> MATx = 10 (us).
 * 	|--> TC = 0 (reseteado).
 *
 * Sale con:
 * 	|--> EMRx en BAJO (si TC = MRx).
 */
volatile void MdE_Ultrasonido_PulsoTRIGalto() {
	// Si TC = MRx  =>  pasa a bajo.
	ctimerObject.Config_ExternalMatchRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::EMR_Demeanor_t::EMR_CLEAR );

	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::STOP_MCR, false );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::RESET_MCR, false );
	// Al interrumpir en el siguiente paso de la secuencia, deshabilita MAT y habilita CAP.
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, true );

	ctimerObject.Config_MatchShadow( sensor_hc_sr04.__MATchannelTRIG, false );
	ctimerObject.SetMSRxValue( sensor_hc_sr04.__MATchannelTRIG, 20e3 );

//	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::RISING_CCR, true );

	++ultrasonido_indice_mde;
	ultrasonido_indice_mde %= __HC_SR04_MDE_STEPS;

	// Mete como callback de interrupción asociado al MAT el siguiente.
	ctimerObject.Set_Callback( CTimer::registerSelection_MAT_CAP_t::MAT_REGISTER, sensor_hc_sr04.__MATchannelTRIG, ultrasonido_secuencia[ultrasonido_indice_mde] );
}


/* #############################################
 * MdE_Ultrasonido_EsperandoECHO (ASYNC)
 * #############################################
 * Paso de la máquina de estados.
 *
 * Entra por interrupción de HW (el EMRx paso a BAJO).
 *
 * Entra con:
 * 	|--> EMRx en BAJO.
 * 	|--> EMRx Habilitado.
 * 	|--> B.
 *
 * Sale con:
 * 	|--> EMRx Deshabilitado.
 * 	|--> CAPx Habilitado (flanco ascendente).
 * 	|--> B.
 * 	|--> B.
 * 	|--> B.
 */
volatile void MdE_Ultrasonido_EsperandoECHO() {
	// # Deshabilitación del MATx/EMRx #
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::STOP_MCR, false );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::RESET_MCR, false );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, false );
	ctimerObject.Config_ExternalMatchRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::EMR_Demeanor_t::EMR_NOTHING );

	// # Habilitación del CAPx #
	// # Interrupción por flanco ascendente #
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::RISING_CCR, true );
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::FALLING_CCR, false );
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::INTERRUPT_CCR, true );

	// Resetea TC a flanco ascendente del CAP (ECHO).
//	ctimerObject.Config_CountControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CTCR_TimerCounter_Mode_t::TIMER_MODE,
//												   true, CTimer::CTCR_Edge_t::CAP_RISING_EDGE );

	++ultrasonido_indice_mde;
	ultrasonido_indice_mde %= __HC_SR04_MDE_STEPS;

	// Mete como callback de interrupción asociado al MAT el siguiente.
	ctimerObject.Set_Callback( CTimer::registerSelection_MAT_CAP_t::CAP_REGISTER, sensor_hc_sr04.__CAPchannelECHO, ultrasonido_secuencia[ultrasonido_indice_mde] );
}


/* #############################################
 * MdE_Ultrasonido_MidiendoTiempoECHO (ASYNC)
 * #############################################
 * Paso de la máquina de estados.
 *
 * Entra por interrupción de HW (el CAP paso a ALTO).
 *
 * Entra con:
 * 	|--> TC = 0 (reseteado por HW, flanco ascendente en CAPx).
 * 	|--> CAPx en Alto.
 * 	|--> B.
 *
 * Sale con:
 * 	|--> B.
 * 	|--> B.
 */
volatile void MdE_Ultrasonido_MidiendoTiempoECHO() {
	// # Interrupción por flanco descendente #
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::RISING_CCR, false );
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::FALLING_CCR, true );
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::INTERRUPT_CCR, true );

	++ultrasonido_indice_mde;
	ultrasonido_indice_mde %= __HC_SR04_MDE_STEPS;

	ctimerObject.Set_Callback( CTimer::registerSelection_MAT_CAP_t::CAP_REGISTER, sensor_hc_sr04.__CAPchannelECHO, ultrasonido_secuencia[ultrasonido_indice_mde] );
}


/* #############################################
 * MdE_Ultrasonido_SetupDelayReinicio (ASYNC)
 * #############################################
 * Paso de la máquina de estados.
 *
 * Entra por interrupción de HW (el CAP paso a BAJO).
 *
 * Entra con:
 * 	|--> A.
 * 	|--> B.
 *
 * Sale con:
 * 	|--> B.
 * 	|--> B.
 */
volatile void MdE_Ultrasonido_SetupDelayReinicio() {
	// Continuación...
//	nullptr = 2;

	// # Deshabilitación del CAPx #
	// # Interrupción por flanco ascendente #
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::RISING_CCR, false );
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::FALLING_CCR, false );
	ctimerObject.Config_CaptureControlRegister( sensor_hc_sr04.__CAPchannelECHO, CTimer::CCRtriggers_t::INTERRUPT_CCR, false );


	// # Habilitación del MATx #
	// Baja el TRIG por si las moscas.
	ctimerObject.Config_ExternalMatchRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::EMR_Demeanor_t::EMR_CLEAR );

	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::STOP_MCR, false );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::RESET_MCR, true );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, true );
	ctimerObject.SetMATxValue( sensor_hc_sr04.__MATchannelTRIG, 1 );

	ctimerObject.Config_MatchShadow( sensor_hc_sr04.__MATchannelTRIG, true );
	ctimerObject.SetMSRxValue( sensor_hc_sr04.__MATchannelTRIG, 20e3 );


	++ultrasonido_indice_mde;
	ultrasonido_indice_mde %= __HC_SR04_MDE_STEPS;

	ctimerObject.Set_Callback( CTimer::registerSelection_MAT_CAP_t::MAT_REGISTER, sensor_hc_sr04.__MATchannelTRIG, ultrasonido_secuencia[ultrasonido_indice_mde] );
}



/* #############################################
 * MdE_Ultrasonido_DelayReinicio (ASYNC)
 * #############################################
 * Paso de la máquina de estados.
 *
 * Entra por interrupción de HW (el CAP paso a BAJO).
 *
 * Entra con:
 * 	|--> A.
 * 	|--> B.
 *
 * Sale con:
 * 	|--> B.
 * 	|--> B.
 */
volatile void MdE_Ultrasonido_DelayReinicio() {
	// Continuación...
//	nullptr = 2;

	ctimerObject.Config_ExternalMatchRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::EMR_Demeanor_t::EMR_SET );

	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::STOP_MCR, false );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::RESET_MCR, true );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, true );
	ctimerObject.SetMATxValue( sensor_hc_sr04.__MATchannelTRIG, 20e3 );

	ctimerObject.Config_MatchShadow( sensor_hc_sr04.__MATchannelTRIG, true );
	ctimerObject.SetMSRxValue( sensor_hc_sr04.__MATchannelTRIG, 10 );


	++ultrasonido_indice_mde;
	ultrasonido_indice_mde %= __HC_SR04_MDE_STEPS;

	ctimerObject.Set_Callback( CTimer::registerSelection_MAT_CAP_t::MAT_REGISTER, sensor_hc_sr04.__MATchannelTRIG, ultrasonido_secuencia[ultrasonido_indice_mde] );
}


// ====================================================================================
// ====================================================================================



/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ###########################################
 * Cada 10 us (si está habilitado el pin de TRIG)
 * mide tiempo en ECHO para luego ser convertido a distancia.
 *
 * Problablemente en desuso debido a ejecución de interrupciones por HARDWARE,
 * controlado por CTimer.
 */
volatile void HC_SR04_IRQ ( void ) {
//	ultrasonido_secuencia[ultrasonido_indice_mde]();
}



/* #############################################
 * InicioDeSecuencia
 * #############################################
 * \brief:			Inicia la secuencia por máquina de estados.
 */
volatile void HC_SR04_InicioDeSecuencia() {
	// Frena el timer.
	ctimerObject.Enable_Timer_Prescale( false );


	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::STOP_MCR, false );
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::RESET_MCR, true );
	// Interrumpe a los 20 ms. Recarga con 10 us al resetear TC.
	ctimerObject.Config_MatchControlRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::MCRtriggers_t::INTERRUPT_MCR, true );
	// (Delay entre ECHO -> 0 y TRIG -> 1).
	ctimerObject.SetMATxValue( sensor_hc_sr04.__MATchannelTRIG, 20e3 );

	ctimerObject.Config_MatchShadow( sensor_hc_sr04.__MATchannelTRIG, true );
	ctimerObject.SetMSRxValue( sensor_hc_sr04.__MATchannelTRIG, 10 );


	// Si TC = MAT => se prende la salida.
	ctimerObject.Config_ExternalMatchRegister( sensor_hc_sr04.__MATchannelTRIG, CTimer::EMR_Demeanor_t::EMR_SET );

	ctimerObject.Set_Callback( CTimer::registerSelection_MAT_CAP_t::MAT_REGISTER, sensor_hc_sr04.__MATchannelTRIG, ultrasonido_secuencia[0] );

	// Arranca el timer.
	ctimerObject.Enable_Timer_Prescale( true );

	ctimerObject.Reset_Timer_Prescale();
}

