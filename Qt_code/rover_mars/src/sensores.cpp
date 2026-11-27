#include "sensores.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

sensores::sensores() :
        m_acelerometro_x(0), m_acelerometro_y(0), m_acelerometro_z(0), m_ultrasonido(0)
{

}

/* Formato JSON de entrada:
 * {"accel_x":<valor_a_x>,"accel_y":<valor_a_y>,"accel_z":<valor_a_z>,"ultra":<valor_u>}\n
 */

bool sensores::parsear(const QByteArray &linea)
{
    bool ok1 = false, ok2 = false, ok3 = false, ok4 = false,
        resultado = false;
    int temp_acelerometro_x = 0,
        temp_acelerometro_y = 0,
        temp_acelerometro_z = 0,
        temp_ultrasonido = 0;

    QString texto = QString::fromUtf8(linea).trimmed();
    QStringList partes = texto.split(',');

    if( partes.size() == sensores::partes_json_t::PARTES ) {
        temp_acelerometro_x = partes.at(0).toInt(&ok1);
        temp_acelerometro_y = partes.at(1).toInt(&ok2);
        temp_acelerometro_z = partes.at(2).toInt(&ok3);
        temp_ultrasonido    = partes.at(3).toInt(&ok4);

        if(ok1 && ok2 && ok3 && ok4) {
            m_acelerometro_x = temp_acelerometro_x;
            m_acelerometro_y = temp_acelerometro_y;
            m_acelerometro_z = temp_acelerometro_z;
            m_ultrasonido = temp_ultrasonido;
            resultado = true;
        }
    }

    return resultado;
}

int sensores::acelerometro_x() const { return m_acelerometro_x; }
int sensores::acelerometro_y() const { return m_acelerometro_y; }
int sensores::acelerometro_z() const { return m_acelerometro_z; }

int sensores::ultrasonido() const { return m_ultrasonido; }
