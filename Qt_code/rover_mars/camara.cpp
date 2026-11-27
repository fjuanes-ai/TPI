#include "camara.h"

camara::camara(QObject *parent): QObject(parent), m_manager(new QNetworkAccessManager(this)), m_reply(nullptr)
{
}

void camara::iniciar(const QString &url)
{
    if (m_reply)
        return; // ya está corriendo

    QNetworkRequest request{QUrl(url)};
    m_reply = m_manager->get(request);

    connect(m_reply, &QNetworkReply::readyRead, this, &camara::onReadyRead);
    connect(m_reply, &QNetworkReply::finished, this, &camara::onFinished);
}

void camara::detener()
{
    if (m_reply)
    {
        m_reply->abort();
        m_reply = nullptr;
    }
    m_buffer.clear();
}

void camara::onReadyRead()
{
    m_buffer += m_reply->readAll();
    procesarBuffer();
}

void camara::procesarBuffer()
{
    // Buscamos el inicio y fin de cada frame JPEG dentro del stream multipart
    // Un JPEG siempre empieza con FF D8 y termina con FF D9
    int inicio = m_buffer.indexOf("\xFF\xD8");
    int fin    = m_buffer.indexOf("\xFF\xD9", inicio + 2);

    while (inicio != -1 && fin != -1)
    {
        QByteArray jpegData = m_buffer.mid(inicio, fin - inicio + 2);
        QImage imagen;
        if (imagen.loadFromData(jpegData, "JPEG"))
            emit nuevoFrame(imagen);

        m_buffer.remove(0, fin + 2);
        inicio = m_buffer.indexOf("\xFF\xD8");
        fin    = m_buffer.indexOf("\xFF\xD9", inicio + 2);
    }

    // Evitar que el buffer crezca sin límite si algo sale mal
    if (m_buffer.size() > 200000)
        m_buffer.clear();
}

void camara::onFinished()
{
    if (m_reply->error() != QNetworkReply::NoError)
        emit errorStream(m_reply->errorString());

    m_reply->deleteLater();
    m_reply = nullptr;
}