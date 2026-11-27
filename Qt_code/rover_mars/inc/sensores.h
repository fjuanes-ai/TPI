#ifndef SENSORES_H
#define SENSORES_H

// #include <QByteArray>
#include "qt_includes.h"

#define US_MIN_DISTANCE 20
#define US_MAX_DISTANCE 2000


class sensores
{
    public:
        typedef enum partes_json_e {
            PARTES = 4
        } partes_json_t;


        sensores();

        bool parsear(const QByteArray &linea);   // intenta interpretar una línea json; devuelve true si tuvo éxito

        int acelerometro_x() const;
        int acelerometro_y() const;
        int acelerometro_z() const;
        int ultrasonido() const;

    private:
        int m_acelerometro_x;
        int m_acelerometro_y;
        int m_acelerometro_z;
        int m_ultrasonido;
};

#endif // SENSORES_H
