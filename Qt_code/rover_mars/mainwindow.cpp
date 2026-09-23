#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), m_conexion(new conexion(this))
{
    ui->setupUi(this);

    /*TEMA CAMARA Y FUNCIONES DEL .UI*/
    m_camara = new camara(this);
    connect(m_camara, &camara::nuevoFrame, this, &MainWindow::alRecibirImagen);
    connect(ui->pushButtonConectar, &QPushButton::clicked, this, &MainWindow::alHacerClickConectar);
    connect(m_conexion, &conexion::lineaRecibida, this, &MainWindow::alRecibirDatos);
    connect(m_conexion, &conexion::conectado, this, &MainWindow::alConectar);
    connect(m_conexion, &conexion::desconectado, this, &MainWindow::alDesconectar);
    connect(m_conexion, &conexion::errorSocket, this, &MainWindow::alFallarLaConexion);

    /*PARTE DE GRAFICOS*/
    connect(ui->pushButtonVerGraficos, &QPushButton::clicked, this, &MainWindow::alPresionarVerGraficos);
    connect(ui->pushButtonVolver, &QPushButton::clicked, this, &MainWindow::alPresionarVolver);

    /*PARTE DE CONTROLES*/
    connect(ui->pushButtonAdelante, &QPushButton::pressed, this, &MainWindow::alPresionarAdelante);
    connect(ui->pushButtonAdelante, &QPushButton::released, this, &MainWindow::alSoltarAdelante);

    connect(ui->pushButtonAtras, &QPushButton::pressed, this, &MainWindow::alPresionarAtras);
    connect(ui->pushButtonAtras, &QPushButton::released, this, &MainWindow::alSoltarAtras);

    connect(ui->pushButtonIzquierda, &QPushButton::pressed, this, &MainWindow::alPresionarIzquierda);
    connect(ui->pushButtonIzquierda, &QPushButton::released, this, &MainWindow::alSoltarIzquierda);

    connect(ui->pushButtonDerecha, &QPushButton::pressed, this, &MainWindow::alPresionarDerecha);
    connect(ui->pushButtonDerecha, &QPushButton::released, this, &MainWindow::alSoltarDerecha);

    /*GRAFICO DE ACELEROMETRO*/
    m_serieAccel = new QLineSeries();
    m_chartAccel = new QChart();
    m_chartAccel->addSeries(m_serieAccel);
    m_chartAccel->createDefaultAxes();
    m_chartAccel->setTitle("Aceleometro");
    m_chartAccel->legend()->hide();

    auto ejesXAccelInit = m_chartAccel->axes(Qt::Horizontal);
    auto ejesYAccelInit = m_chartAccel->axes(Qt::Vertical);
    ejesXAccelInit.first()->setTitleText("Muestra");
    ejesYAccelInit.first()->setTitleText("Valor ADC");

    m_chartViewAccel = new QChartView(m_chartAccel);
    m_chartViewAccel->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layoutAccel = new QVBoxLayout(ui->widgetGraficoAccel);
    layoutAccel->addWidget(m_chartViewAccel);
    ui->widgetGraficoAccel->setLayout(layoutAccel);

    /*GRAFICO DE LOS ULTRASONICOS*/
    m_serieUltra1 = new QLineSeries();
    m_serieUltra1->setName("Ultra 1");
    m_serieUltra2 = new QLineSeries();
    m_serieUltra2->setName("Ultra 2");
    m_serieUltra3 = new QLineSeries();
    m_serieUltra3->setName("Ultra 3");

    m_chartUltra = new QChart();
    m_chartUltra->addSeries(m_serieUltra1);
    m_chartUltra->addSeries(m_serieUltra2);
    m_chartUltra->addSeries(m_serieUltra3);
    m_chartUltra->createDefaultAxes();
    m_chartUltra->setTitle("Ultrasónicos");

    auto ejesXUltraInit = m_chartUltra->axes(Qt::Horizontal);
    auto ejesYUltraInit = m_chartUltra->axes(Qt::Vertical);
    ejesXUltraInit.first()->setTitleText("Muestra");
    ejesYUltraInit.first()->setTitleText("Distancia (cm)");

    m_chartViewUltra = new QChartView(m_chartUltra);
    m_chartViewUltra->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layoutUltra = new QVBoxLayout(ui->widgetGraficoUltra);
    layoutUltra->addWidget(m_chartViewUltra);
    ui->widgetGraficoUltra->setLayout(layoutUltra);

    ui->labelEstado->setText("Desconectado");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::alHacerClickConectar()
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

void MainWindow::alRecibirDatos(const QByteArray &linea)
{
    const int maxPuntos = 50;

    if (m_sensores.parsear(linea))
    {
        /*LOS VALORES OBTENIDOS POR INTERFAZ*/
        ui->labelAccel->setText(QString("Acelerometro: %1").arg(m_sensores.accel()));
        ui->labelUltra->setText(QString("Ultrasonico1: %1").arg(m_sensores.ultra1()));
        ui->labelUltra_2->setText(QString("Ultrasonico2: %1").arg(m_sensores.ultra2()));
        ui->labelUltra_3->setText(QString("Ultrasonico3: %1").arg(m_sensores.ultra3()));
        ui->labelMotor->setText(QString("Motor: %1").arg(m_sensores.motor()));

        /*PARA CADA GRAFICO*/
        m_serieAccel->append(m_contadorMuestras, m_sensores.accel());
        m_serieUltra1->append(m_contadorMuestras, m_sensores.ultra1());
        m_serieUltra2->append(m_contadorMuestras, m_sensores.ultra2());
        m_serieUltra3->append(m_contadorMuestras, m_sensores.ultra3());

        m_contadorMuestras++;

        if (m_serieAccel->count() > maxPuntos)
        {
            m_serieAccel->removePoints(0, m_serieAccel->count() - maxPuntos);
            m_serieUltra1->removePoints(0, m_serieUltra1->count() - maxPuntos);
            m_serieUltra2->removePoints(0, m_serieUltra2->count() - maxPuntos);
            m_serieUltra3->removePoints(0, m_serieUltra3->count() - maxPuntos);
        }


        auto ejesXAccel = m_chartAccel->axes(Qt::Horizontal);
        auto ejesYAccel = m_chartAccel->axes(Qt::Vertical);
        if (!ejesXAccel.isEmpty()) ejesXAccel.first()->setRange(m_contadorMuestras - maxPuntos, m_contadorMuestras);
        if (!ejesYAccel.isEmpty()) ejesYAccel.first()->setRange(0, 1023);   // rango típico de un ADC de 10 bits

        auto ejesXUltra = m_chartUltra->axes(Qt::Horizontal);
        auto ejesYUltra = m_chartUltra->axes(Qt::Vertical);
        if (!ejesXUltra.isEmpty()) ejesXUltra.first()->setRange(m_contadorMuestras - maxPuntos, m_contadorMuestras);
        if (!ejesYUltra.isEmpty()) ejesYUltra.first()->setRange(0, 400);   // rango típico de un ultrasónico en cm

    }
}

void MainWindow::alConectar()
{
    ui->labelEstado->setText("Conectado");
    ui->pushButtonConectar->setText("Desconectar");
}

void MainWindow::alDesconectar()
{
    ui->labelEstado->setText("Desconectado");
    ui->pushButtonConectar->setText("Conectar");
}

void MainWindow::alFallarLaConexion(const QString &mensaje)
{
    ui->labelEstado->setText("Error: " + mensaje);
}

void MainWindow::alRecibirImagen(const QImage &imagen)
{
    ui->labelCamara->setPixmap(QPixmap::fromImage(imagen).scaled(
        ui->labelCamara->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

/*CONSTRUCTOR GRAFICOS*/

void MainWindow::alPresionarVerGraficos()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::alPresionarVolver()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/*CONSTRUCTOR CONTROLES*/
void MainWindow::alPresionarAdelante() { m_conexion->enviarComando('A'); }
void MainWindow::alSoltarAdelante()    { m_conexion->enviarComando('S'); }

void MainWindow::alPresionarAtras() { m_conexion->enviarComando('B'); }
void MainWindow::alSoltarAtras()    { m_conexion->enviarComando('S'); }

void MainWindow::alPresionarIzquierda() { m_conexion->enviarComando('I'); }
void MainWindow::alSoltarIzquierda()    { m_conexion->enviarComando('S'); }

void MainWindow::alPresionarDerecha() { m_conexion->enviarComando('D'); }
void MainWindow::alSoltarDerecha()    { m_conexion->enviarComando('S'); }
