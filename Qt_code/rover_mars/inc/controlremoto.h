#ifndef CONTROLREMOTO_H
#define CONTROLREMOTO_H


#include "qt_includes.h"
#include <QKeySequence>
#include <QShortcut>

class ControlRemoto : public QObject {
    Q_OBJECT

    // ## VARIABLES MIEMBRO ##
private:
    QShortcut *teclaAvanzar;
    QShortcut *teclaRetroceder;
    QShortcut *teclaDerecha;
    QShortcut *teclaIzquierda;


    // ## MÉTODOS ##
private slots:
    void SLOT_Avance();
    void SLOT_Retroceso();
    void SLOT_Derecha();
    void SLOT_Izquierda();

signals:
    void SIGNAL_ComandoAvanzar();
    void SIGNAL_ComandoRetroceder();
    void SIGNAL_ComandoDerecha();
    void SIGNAL_ComandoIzquierda();


public:
    ControlRemoto();    // Constructor
};

#endif // CONTROLREMOTO_H
