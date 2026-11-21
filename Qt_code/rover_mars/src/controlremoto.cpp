#include "controlremoto.h"


ControlRemoto::ControlRemoto( QObject *parent ) {
    teclaAvanzar = new QShortcut(QKeySequence("W"), parent  );
    teclaRetroceder = new QShortcut(QKeySequence("S"), parent );
    teclaDerecha = new QShortcut(QKeySequence("D"), parent );
    teclaIzquierda = new QShortcut(QKeySequence("A"), parent );
    teclaFrenar = new QShortcut(QKeySequence(" "), parent );


    QObject::connect( teclaAvanzar, SIGNAL(activated()),
             this, SLOT(SLOT_Avance()) );
    QObject::connect( teclaRetroceder, SIGNAL(activated()),
                     this, SLOT(SLOT_Retroceso()) );
    QObject::connect( teclaDerecha, SIGNAL(activated()),
                     this, SLOT(SLOT_Derecha()) );
    QObject::connect( teclaIzquierda, SIGNAL(activated()),
                     this, SLOT(SLOT_Izquierda()) );
    QObject::connect( teclaFrenar, SIGNAL(activated()),
                     this, SLOT(SLOT_Frenado()) );
}


void ControlRemoto::SLOT_Avance() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_AVANZAR ); }
void ControlRemoto::SLOT_Retroceso() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_RETROCEDER ); }
void ControlRemoto::SLOT_Derecha() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_DERECHA ); }
void ControlRemoto::SLOT_Izquierda() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_IZQUIERDA ); }
void ControlRemoto::SLOT_Frenado() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_FRENAR ); }
