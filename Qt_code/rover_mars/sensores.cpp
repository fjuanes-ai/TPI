#include "sensores.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

sensores::sensores(): m_accel(0), m_ultra(0), m_motor(0)
{
}

bool sensores::parsear(const QByteArray &linea)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(linea, &err);

    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject obj = doc.object();

    if (!obj.contains("accel") || !obj.contains("ultra") || !obj.contains("motor"))
        return false;

    m_accel = obj.value("accel").toInt();
    m_ultra = obj.value("ultra").toInt();
    m_motor = obj.value("motor").toInt();

    return true;
}

int sensores::accel() const
{
    return m_accel;
}

int sensores::ultra() const
{
    return m_ultra;
}
int sensores::motor() const
{
    return m_motor;
}