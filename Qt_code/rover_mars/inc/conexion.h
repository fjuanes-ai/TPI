#ifndef CONEXION_H
#define CONEXION_H


// IWYU pragma: begin_exports
#include "qt_includes.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
// IWYU pragma: end_exports


class conexion : public QObject {
    Q_OBJECT
    public:
        typedef enum trama_e {
            INICIO_TRAMA = '[',
            FIN_TRAMA = ']'
        } trama_t;


        /* # Mapa de Controles #
         * Son iguales con las flechas.
         * 'W': Adelante.
         * 'A': Izquierda.
         * 'S': Atrás.
         * 'D': Derecha.
         * ' ': Frenado (espacio).
         */
        typedef enum comandos_e {
            COMANDO_AVANZAR = 'W',
            COMANDO_RETROCEDER = 'S',
            COMANDO_DERECHA = 'D',
            COMANDO_IZQUIERDA = 'I',
            COMANDO_FRENAR = ' '
        } comandos_t;


        typedef enum puerto_esp_webserver_e {
            ESP_PORT = 3333
        } puerto_esp_webserver_t;

        #define ESP_IP "192.168.4.1"


        explicit conexion( QObject *parent = nullptr );

        void conectar(const QString &host = ESP_IP,
                      uint32_t puerto = conexion::puerto_esp_webserver_t::ESP_PORT);
        void desconectar();
        bool estaConectado() const;
        void static ArmarTrama( char *arrayBytes, const uint32_t nBytes,
                                const trama_t inicioDeTrama, const trama_t finDeTrama );

    signals:
        void lineaRecibida(const QByteArray &linea);   /*una línea completa (json) lista para parsear*/
        //void datoRecibido();
        void conectado();
        void desconectado();
        void errorSocket(const QString &mensaje);


    public slots:
        void timedOut();
        void onReadyRead();
        void onConnected();
        void onDisconnected();
        void onErrorOccurred( QAbstractSocket::SocketError socketError );
        void enviarComando( comandos_t comando ); /*para CONTROLES*/

    private:
        QTcpSocket *m_socket;
        QByteArray  m_buffer;
};

#endif // CONEXION_H
