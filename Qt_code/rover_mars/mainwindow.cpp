#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), m_conexion(new conexion(this))
{
    ui->setupUi(this);
    m_camara = new camara(this);
    connect(m_camara, &camara::nuevoFrame, this, &MainWindow::onNuevoFrame);
    connect(ui->pushButtonConectar, &QPushButton::clicked, this, &MainWindow::onConectarClicked);
    connect(m_conexion, &conexion::lineaRecibida, this, &MainWindow::onLineaRecibida);
    connect(m_conexion, &conexion::conectado, this, &MainWindow::onConectado);
    connect(m_conexion, &conexion::desconectado, this, &MainWindow::onDesconectado);
    connect(m_conexion, &conexion::errorSocket, this, &MainWindow::onErrorSocket);

    ui->labelEstado->setText("Desconectado");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConectarClicked()
{
    if (m_conexion->estaConectado())
    {
        m_conexion->desconectar();
        m_camara->detener();
    }
    else
    {
        ui->labelEstado->setText("Conectando...");
        m_conexion->conectar("192.168.4.1", 3333);
        m_camara->iniciar();
    }
}

void MainWindow::onLineaRecibida(const QByteArray &linea)
{
    if (m_sensores.parsear(linea))
    {
        ui->labelAccel->setText(QString("Acelerometro: %1").arg(m_sensores.accel()));
        ui->labelUltra->setText(QString("Ultrasonico: %1").arg(m_sensores.ultra()));
        ui->labelMotor->setText(QString("Motor: %1").arg(m_sensores.motor()));
    }
}

void MainWindow::onConectado()
{
    ui->labelEstado->setText("Conectado");
    ui->pushButtonConectar->setText("Desconectar");
}

void MainWindow::onDesconectado()
{
    ui->labelEstado->setText("Desconectado");
    ui->pushButtonConectar->setText("Conectar");
}

void MainWindow::onErrorSocket(const QString &mensaje)
{
    ui->labelEstado->setText("Error: " + mensaje);
}

void MainWindow::onNuevoFrame(const QImage &imagen)
{
    ui->labelCamara->setPixmap(QPixmap::fromImage(imagen).scaled(
        ui->labelCamara->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}