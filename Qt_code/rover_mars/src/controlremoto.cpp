#include "controlremoto.h"


ControlRemoto::ControlRemoto( QObject *parent ) {
//    qDebug() << "# ControlRemoto (clase) #";

    teclaAvanzar = new QShortcut(QKeySequence("W"), parent  );
    teclaRetroceder = new QShortcut(QKeySequence("S"), parent );
    teclaDerecha = new QShortcut(QKeySequence("D"), parent );
    teclaIzquierda = new QShortcut(QKeySequence("A"), parent );
    teclaFrenar = new QShortcut(QKeySequence(" "), parent );


//    qDebug() << "Connect activated -> SLOT_Avance works:" <<
        QObject::connect( teclaAvanzar, &QShortcut::activated,
                      this, &ControlRemoto::SLOT_Avance );

//    qDebug() << "Connect activated -> SLOT_Retroceso works:" <<
        QObject::connect( teclaRetroceder, &QShortcut::activated,
                         this, &ControlRemoto::SLOT_Retroceso );

//    qDebug() << "Connect activated -> SLOT_Derecha works:" <<
        QObject::connect( teclaDerecha, &QShortcut::activated,
                         this, &ControlRemoto::SLOT_Derecha );

//    qDebug() << "Connect activated -> SLOT_Izquierda works:" <<
        QObject::connect( teclaIzquierda, &QShortcut::activated,
                         this, &ControlRemoto::SLOT_Izquierda );

//    qDebug() << "Connect activated -> SLOT_Frenado works:" <<
        QObject::connect( teclaFrenar, &QShortcut::activated,
                         this, &ControlRemoto::SLOT_Frenado );
}


void ControlRemoto::SLOT_Avance() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_AVANZAR ); }
void ControlRemoto::SLOT_Retroceso() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_RETROCEDER ); }
void ControlRemoto::SLOT_Derecha() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_DERECHA ); }
void ControlRemoto::SLOT_Izquierda() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_IZQUIERDA ); }
void ControlRemoto::SLOT_Frenado() { emit SIGNAL_Comando_Teclado( conexion::comandos_t::COMANDO_FRENAR ); }
