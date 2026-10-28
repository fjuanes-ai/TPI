#ifndef CONEXION_H
#define CONEXION_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class conexion : public QObject
{
    Q_OBJECT
    public:
        typedef enum puerto_esp_webserver_e {
            ESP_PORT = 3333
        } puerto_esp_webserver_t;

        #define ESP_IP "192.168.4.1"


        explicit conexion(QObject *parent = nullptr);

        void conectar(const QString &host = ESP_IP, quint16 puerto = conexion::puerto_esp_webserver_t::ESP_PORT);
        void desconectar();
        bool estaConectado() const;
        void enviarComando(char comando); /*para CONTROLES*/

    signals:
        void lineaRecibida(const QByteArray &linea);   /*una línea completa (json) lista para parsear*/
        void conectado();
        void desconectado();
        void errorSocket(const QString &mensaje);

    private slots:
        void onReadyRead();
        void onConnected();
        void onDisconnected();
        void onErrorOccurred(QAbstractSocket::SocketError socketError);

    private:
        QTcpSocket *m_socket;
        QByteArray  m_buffer;
};

#endif // CONEXION_H
