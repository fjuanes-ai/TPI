#include "conexion.h"

#define TRAMA_TAMANIO 3



conexion::conexion(QObject *parent): QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::readyRead, this, &conexion::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &conexion::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &conexion::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &conexion::onErrorOccurred);
}

void conexion::conectar(const QString &host, uint32_t puerto)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        return;

    m_buffer.clear();
    m_socket->connectToHost( host, puerto );
}

void conexion::desconectar()
{
    m_socket->disconnectFromHost();
}

bool conexion::estaConectado() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void conexion::onReadyRead()
{
    m_buffer += m_socket->readAll();
    int idx;

    while ((idx = m_buffer.indexOf('\n')) != -1)
    {
        QByteArray linea = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);

        if (!linea.isEmpty())
            emit lineaRecibida(linea);
    }
}

void conexion::onConnected()
{
    emit conectado();
}

void conexion::onDisconnected()
{
    emit desconectado();
}

void conexion::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit errorSocket(m_socket->errorString());
}


/* # Mapa de Controles #
 * Son iguales con las flechas.
 * 'W': Adelante.
 * 'A': Izquierda.
 * 'S': Atrás.
 * 'D': Derecha.
 * ' ': Frenado (espacio).
 */
void conexion::enviarComando( comandos_t comando ) {
    char trama[TRAMA_TAMANIO];


    if ( m_socket->state() == QAbstractSocket::ConnectedState ) {

        ArmarTrama( trama, TRAMA_TAMANIO, trama_t::INICIO_TRAMA, trama_t::FIN_TRAMA );

        trama[1] = (char) comando;

        m_socket->write( trama, TRAMA_TAMANIO );
    }
}


void conexion::ArmarTrama( char *arrayBytes, uint32_t nBytes, trama_t inicioDeTrama, trama_t finDeTrama ) {

    if ( (!arrayBytes) || (nBytes == 0) ) {
        return;
    }

    arrayBytes[0] = inicioDeTrama;
    arrayBytes[nBytes - 1] = finDeTrama;
}


void datoRecibido(  ) {

}
