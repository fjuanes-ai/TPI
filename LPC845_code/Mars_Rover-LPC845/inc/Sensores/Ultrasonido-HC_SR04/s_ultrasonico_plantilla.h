/*
 * s_ultrasonico_plantilla.h
 *
 *  Created on: Sep 29, 2026
 *      Author: iyopolo
 */



#ifndef         SENSORES_ULTRASONIDO_HC_SR04_S_ULTRASONICO_PLANTILLA_H_     
    #define     SENSORES_ULTRASONIDO_HC_SR04_S_ULTRASONICO_PLANTILLA_H_

    // #include    "Sensores/Ultrasonido-HC_SR04/s_ultrasonico_plantilla.h"

    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
    //


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
    //


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
    //


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
	//


    /* ###########################################
     * ### DEFINICIONES DE CLASES ###
     * ########################################### */
	class Ultrasonido {
		// # Variables #
		private:
			uint16_t	__ticksUpdateCount;

			volatile void 	(**__sequenceCallbacks)(void);

		public:
			double		__distance_millimeters;


		// # Métodos #
		public:
//			virtual				Ultrasonido( uint8_t portTrig, uint8_t pinTrig,
//									 	 	 uint8_t portEcho, uint8_t pinEcho ) = 0;
			virtual uint32_t 	Measure_Time() = 0;
			virtual void 		Time_microSec_to_Distance_millimeters() = 0;
			virtual void 		Set_Callback_Sequence( volatile void (**inputCallback)(void) ) = 0;
			virtual void 		InicioDeSecuencia() = 0;
//						~Ultrasonido();
	};


#endif          /* SENSORES_ULTRASONIDO_HC_SR04_S_ULTRASONICO_PLANTILLA_H_ */
