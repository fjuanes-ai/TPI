#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_conexion(new conexion(this)),
    color(Qt::red)
{
    ui->setupUi(this);

    palette = ui->centralwidget->palette();
    palette.setColor(QPalette::Active, QPalette::Window, color);

    /*TEMA CAMARA Y FUNCIONES DEL .UI*/
    m_camara = new camara(this);
    connect(m_camara, &camara::nuevoFrame, this, &MainWindow::alRecibirImagen);
    connect(ui->pushButtonConectar, &QPushButton::clicked, this, &MainWindow::alHacerClickConectar);
    connect(m_conexion, &conexion::lineaRecibida, this, &MainWindow::alRecibirDatos);
    connect(m_conexion, &conexion::conectado, this, &MainWindow::alConectar);
    connect(m_conexion, &conexion::desconectado, this, &MainWindow::alDesconectar);
    connect(m_conexion, &conexion::errorSocket, this, &MainWindow::alFallarLaConexion);


    /*PARTE DE CONTROLES*/
    connect(ui->lineEdit_comandos, SIGNAL(textChanged(QString)), this, SLOT(comandoPorTeclado()));

    m_tiempo = 0;

    /*GRAFICO DE ACELERÓMETRO*/
    m_serieAccel = new QLineSeries();
    m_chartAccel = new QChart();
    m_chartAccel->addSeries(m_serieAccel);
    m_chartAccel->createDefaultAxes();
    m_chartAccel->setTitle("Aceleometro");
    m_chartAccel->legend()->hide();

    auto ejesXAccelInit = m_chartAccel->axes(Qt::Horizontal);
    auto ejesYAccelInit = m_chartAccel->axes(Qt::Vertical);
    ejesXAccelInit.first()->setTitleText("Tiempo");
    ejesYAccelInit.first()->setTitleText("Valor ADC");

    m_chartViewAccel = new QChartView(m_chartAccel);
    m_chartViewAccel->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layoutAccel = new QVBoxLayout(ui->widgetGraficoAccel);
    layoutAccel->addWidget(m_chartViewAccel);
    ui->widgetGraficoAccel->setLayout(layoutAccel);

    /*GRAFICO DE MOTOR*/
    m_serieMotor = new QLineSeries();
    m_chartMotor = new QChart();
    m_chartMotor->addSeries(m_serieMotor);
    m_chartMotor->createDefaultAxes();
    m_chartMotor->setTitle("Motor");
    m_chartMotor->legend()->hide();

    auto ejesXMotorInit = m_chartMotor->axes(Qt::Horizontal);
    auto ejesYMotorInit = m_chartMotor->axes(Qt::Vertical);
    ejesXMotorInit.first()->setTitleText("Tiempo");
    ejesYMotorInit.first()->setTitleText("Velocidad [m/s]");

    m_chartViewMotor = new QChartView(m_chartMotor);
    m_chartViewMotor->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layoutMotor = new QVBoxLayout(ui->widgetGraficoMotor);
    layoutMotor->addWidget(m_chartViewMotor);
    ui->widgetGraficoMotor->setLayout(layoutMotor);

    /*GRAFICO DEL SENSOR ULTRASÓNICO*/
    m_serieUltra = new QLineSeries();
    m_serieUltra->setName("Ultra");

    m_chartUltra = new QChart();
    m_chartUltra->addSeries(m_serieUltra);
    m_chartUltra->createDefaultAxes();
    m_chartUltra->setTitle("Ultrasónicos");

    auto ejesXUltraInit = m_chartUltra->axes(Qt::Horizontal);
    auto ejesYUltraInit = m_chartUltra->axes(Qt::Vertical);
    ejesXUltraInit.first()->setTitleText("Tiempo");
    ejesYUltraInit.first()->setTitleText("Distancia (mm)");

    m_chartViewUltra = new QChartView(m_chartUltra);
    m_chartViewUltra->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layoutUltra = new QVBoxLayout(ui->widgetGraficoUltra);
    layoutUltra->addWidget(m_chartViewUltra);
    ui->widgetGraficoUltra->setLayout(layoutUltra);

    ui->label_estado->setText("Desconectado");
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
        ui->label_estado->setText("Conectando...");
        m_conexion->conectar(ESP_IP, conexion::puerto_esp_webserver_t::ESP_PORT);
        m_camara->iniciar();
    }
}

void MainWindow::alRecibirDatos(const QByteArray &linea)
{
    const int maxPuntos = 50;

    if (m_sensores.parsear(linea))
    {
        /*LOS VALORES OBTENIDOS POR INTERFAZ*/
        //ui->labelAccel->setText(QString("Acelerometro: %1").arg(m_sensores.accel()));
        ui->label_ultrasonido_txt->setText(QString("Ultrasonico:  %1").arg(m_sensores.ultra()));
        ui->label_motor_txt->setText(QString("Motor: %1").arg(m_sensores.motor()));

        /*PARA CADA GRAFICO*/
        m_serieAccel->append(m_tiempo, m_sensores.accel());
        m_serieUltra->append(m_tiempo, m_sensores.ultra());

        m_tiempo++;

        if (m_serieAccel->count() > maxPuntos)
        {
            m_serieAccel->removePoints(0, m_serieAccel->count() - maxPuntos);
            m_serieUltra->removePoints(0, m_serieUltra->count() - maxPuntos);
        }


        auto ejesXAccel = m_chartAccel->axes(Qt::Horizontal);
        auto ejesYAccel = m_chartAccel->axes(Qt::Vertical);

        if (!ejesXAccel.isEmpty())
            ejesXAccel.first()->setRange(m_tiempo - maxPuntos, m_tiempo);

        if (!ejesYAccel.isEmpty())
            ejesYAccel.first()->setRange(0, 1023);   // Rango típico de un ADC de 10 bits.

        auto ejesXUltra = m_chartUltra->axes(Qt::Horizontal);
        auto ejesYUltra = m_chartUltra->axes(Qt::Vertical);

        if (!ejesXUltra.isEmpty())
            ejesXUltra.first()->setRange(m_tiempo - maxPuntos, m_tiempo);

        if (!ejesYUltra.isEmpty())
            ejesYUltra.first()->setRange(US_MIN_DISTANCE, US_MAX_DISTANCE);   // Rango efectivo del HC-SR04 en mm.

        auto ejesXMot = m_chartMotor->axes(Qt::Horizontal);
        auto ejesYMot = m_chartMotor->axes(Qt::Vertical);

        if (!ejesXMot.isEmpty())
            ejesXMot.first()->setRange(m_tiempo - maxPuntos, m_tiempo);

        if (!ejesYMot.isEmpty())
            ejesYMot.first()->setRange(0, 1023);   // Rango típico de un ADC de 10 bits.
    }
}

void MainWindow::alConectar()
{
    ui->label_estado->setText("Conectado");
    ui->pushButtonConectar->setText("Desconectar");
}

void MainWindow::alDesconectar()
{
    ui->label_estado->setText("Desconectado");
    ui->pushButtonConectar->setText("Conectar");
}

void MainWindow::alFallarLaConexion(const QString &mensaje)
{
    ui->label_estado->setText("Error: " + mensaje);
}

void MainWindow::alRecibirImagen(const QImage &imagen)
{
    ui->label_camara->setPixmap(QPixmap::fromImage(imagen).scaled(
        ui->label_camara->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

/*CONSTRUCTOR CONTROLES*/
/* # Mapa de Controles #
 * Son iguales con las flechas.
 * 'W': Adelante.
 * 'A': Izquierda.
 * 'S': Atrás.
 * 'D': Derecha.
 * ' ': Frenado (espacio).
 */
void MainWindow::comandoPorTeclado() {
    QString temp_string = ui->lineEdit_comandos->text();
    uint8_t cursorIndex = ui->lineEdit_comandos->cursorPosition();
    QChar temp_qchar = temp_string.at(cursorIndex - 1);
    char temp_char = temp_qchar.unicode();
    switch ( temp_char ) {
        case 'w':
            m_conexion->enviarComando('w');
            break;

        case 'a':
            m_conexion->enviarComando('a');
            break;

        case 's':
            m_conexion->enviarComando('s');
            break;

        case 'd':
            m_conexion->enviarComando('d');
            break;

        case ' ':
            m_conexion->enviarComando(' '); // Frenado.
            break;
    }
    ui->lineEdit_comandos->clear();
}
