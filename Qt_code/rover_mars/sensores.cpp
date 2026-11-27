#include "sensores.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

sensores::sensores(): m_accel(0), m_ultra1(0), m_ultra2(0), m_ultra3(0), m_motor(0)
{

}

bool sensores::parsear(const QByteArray &linea)
{
    QString texto = QString::fromUtf8(linea).trimmed();
    QStringList partes = texto.split(',');
    bool ok1, ok2, ok3, ok4, ok5;
    int accel, ultra1, ultra2, ultra3, motor, resultado = false;

    if(partes.size() == 5)
    {
        accel = partes.at(0).toInt(&ok1);
        ultra1 = partes.at(1).toInt(&ok2);
        ultra2 = partes.at(2).toInt(&ok3);
        ultra3 = partes.at(3).toInt(&ok4);
        motor = partes.at(4).toInt(&ok5);

        if(ok1 && ok2 && ok3 && ok4 && ok5)
        {
            m_accel = accel;
            m_ultra1 = ultra1;
            m_ultra2 = ultra2;
            m_ultra3 = ultra3;
            m_motor = motor;
            resultado = true;
        }
    }

    return resultado;
}

int sensores::accel() const { return m_accel; }

int sensores::ultra1() const{ return m_ultra1; }

int sensores::ultra2() const { return m_ultra1; }

int sensores::ultra3() const { return m_ultra3; }

int sensores::motor() const { return m_motor; }