#ifndef SENSORES_H
#define SENSORES_H

#include <QByteArray>

class sensores
{
    public:
        sensores();

        bool parsear(const QByteArray &linea);   // intenta interpretar una línea json; devuelve true si tuvo éxito

        int accel() const;
        int ultra() const;
        int motor() const;

    private:
        int m_accel;
        int m_ultra;
        int m_motor;
};

#endif // SENSORES_H
