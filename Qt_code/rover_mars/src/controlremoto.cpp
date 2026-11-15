#include "controlremoto.h"


ControlRemoto::ControlRemoto() {
    teclaAvanzar = new QShortcut(QKeySequence("W"), parent );

    QObject::connect( teclaAvanzar, SIGNAL(activated()),
             this, SLOT(SLOT_Avance()) );
}


void ControlRemoto::SLOT_Avance() { emit SIGNAL_ComandoAvanzar(); }
void ControlRemoto::SLOT_Retroceso() { emit SIGNAL_ComandoRetroceder(); }
void ControlRemoto::SLOT_Derecha() { emit SIGNAL_ComandoDerecha(); }
void ControlRemoto::SLOT_Izquierda() { emit SIGNAL_ComandoIzquierda(); }
