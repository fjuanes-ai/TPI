#ifndef SENSORES_H
#define SENSORES_H

#include <QByteArray>

class sensores
{
    public:
        sensores();

        bool parsear(const QByteArray &linea);   // intenta interpretar una línea json; devuelve true si tuvo éxito

        int accel() const;
        int ultra1() const;
        int ultra2() const;
        int ultra3() const;
        int motor() const;

    private:
        int m_accel;
        int m_ultra1;
        int m_ultra2;
        int m_ultra3;
        int m_motor;
};

#endif // SENSORES_H
