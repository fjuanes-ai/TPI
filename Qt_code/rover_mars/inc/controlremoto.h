#ifndef CONTROLREMOTO_H
#define CONTROLREMOTO_H


// IWYU pragma: begin_exports
#include "qt_includes.h"
#include "conexion.h"
#include <QKeySequence>
#include <QShortcut>
// IWYU pragma: end_exports


class ControlRemoto : public QObject {
    Q_OBJECT

    // ## VARIABLES MIEMBRO ##
private:
    QShortcut *teclaAvanzar;
    QShortcut *teclaRetroceder;
    QShortcut *teclaDerecha;
    QShortcut *teclaIzquierda;
    QShortcut *teclaFrenar;


    // ## MÉTODOS ##
public slots:
    void SLOT_Avance();
    void SLOT_Retroceso();
    void SLOT_Derecha();
    void SLOT_Izquierda();
    void SLOT_Frenado();

signals:
    void SIGNAL_Comando_Teclado( conexion::comandos_t );
    // void SIGNAL_ComandoAvanzar( conexion::comandos_t );
    // void SIGNAL_ComandoRetroceder( conexion::comandos_t );
    // void SIGNAL_ComandoDerecha( conexion::comandos_t );
    // void SIGNAL_ComandoIzquierda( conexion::comandos_t );
    // void SIGNAL_ComandoFrenado( conexion::comandos_t );


public:
    ControlRemoto( QObject *parent );    // Constructor
};

#endif // CONTROLREMOTO_H
