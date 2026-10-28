#include "sensores.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

sensores::sensores(): m_accel(0), m_ultra(0), m_motor(0)
{

}

bool sensores::parsear(const QByteArray &linea)
{
    QString texto = QString::fromUtf8(linea).trimmed();
    QStringList partes = texto.split(',');
    bool ok1 = false, ok2 = false, ok3 = false, ok4 = false, ok5 = false, resultado = false;
    int accel = 0, ultra = 0, motor = 0;

    if(partes.size() == 5)
    {
        accel = partes.at(0).toInt(&ok1);
        ultra = partes.at(1).toInt(&ok2);
        motor = partes.at(4).toInt(&ok5);

        if(ok1 && ok2 && ok3 && ok4 && ok5)
        {
            m_accel = accel;
            m_ultra = ultra;
            m_motor = motor;
            resultado = true;
        }
    }

    return resultado;
}

int sensores::accel() const { return m_accel; }

int sensores::ultra() const{ return m_ultra; }

int sensores::motor() const { return m_motor; }