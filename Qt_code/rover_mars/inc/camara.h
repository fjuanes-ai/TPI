#ifndef CAMARA_H
#define CAMARA_H


#include "qt_includes.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>

class camara : public QObject
{
    Q_OBJECT
public:
    explicit camara(QObject *parent = nullptr);

    void iniciar(const QString &url = "http://192.168.4.1:81/stream");
    void detener();

signals:
    void nuevoFrame(const QImage &imagen);
    void errorStream(const QString &mensaje);

private slots:
    void onReadyRead();
    void onFinished();

private:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QByteArray m_buffer;

    void procesarBuffer();
};

#endif // CAMARA_H
