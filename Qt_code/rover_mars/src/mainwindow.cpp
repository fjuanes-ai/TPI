#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QDebug>

MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow ),
        m_conexion( new conexion(this) ),
        controlRemoto_Atajos( new ControlRemoto(parent) ),
        timeTicker( new QCPAxisTickerTime ) {

    ui->setupUi(this);

    timer.start();

    SetupCamera();
    SetupConection();
    SetupRemoteControl();
    SetupGraphics();

    ui->label_estado->setText("Desconectado");
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::SetupCamera() {
    m_camara = new camara(this);

    connect( m_camara, SIGNAL( nuevoFrame( const QImage &imagen ) ),
            this, SLOT( alRecibirImagen( const QImage &imagen ) ) );
}


void MainWindow::SetupConection() {
    connect( ui->pushButtonConectar, SIGNAL( clicked() ),
            this, SLOT( alHacerClickConectar() ) );

    connect( m_conexion, SIGNAL( datoRecibido() ),
            this, SLOT( alRecibirDatos() ) );

    connect( m_conexion, SIGNAL( conectado() ),
            this, SLOT( alConectar() ) );

    connect( m_conexion, SIGNAL( desconectado() ),
            this, SLOT( alDesconectar() ) );

    connect( m_conexion, SIGNAL( errorSocket() ),
            this, SLOT( alFallarLaConexion() ) );
}


void MainWindow::SetupRemoteControl() {
    connect( controlRemoto_Atajos, SIGNAL(SIGNAL_Comando_Teclado( conexion::comandos_t )),
            m_conexion, SLOT(enviarComando( conexion::comandos_t )) );
}


void MainWindow::SetupGraphics() {
    graficoAcelerometro  = ui->widgetGraficoAccel->addGraph();
    graficoMotor         = ui->widgetGraficoMotor->addGraph();
    graficoUltrasonido   = ui->widgetGraficoUltra->addGraph();


    if ( (!graficoAcelerometro) || (!graficoMotor) || (!graficoUltrasonido) ) {
        return;
    }

    // # Eje de abscisas en segundos #
    //QSharedPointer<QCPAxisTickerTime> timeTicker( new QCPAxisTickerTime );
    timeTicker->setTimeFormat( "%s" );


    ui->widgetGraficoAccel->xAxis->setTicker( timeTicker );
    ui->widgetGraficoMotor->xAxis->setTicker( timeTicker );
    ui->widgetGraficoUltra->xAxis->setTicker( timeTicker );

    ui->widgetGraficoAccel->xAxis->setLabel( "t [segundos]" );
    ui->widgetGraficoMotor->xAxis->setLabel( "t [segundos]" );
    ui->widgetGraficoUltra->xAxis->setLabel( "t [segundos]" );

    ui->widgetGraficoAccel->xAxis->setRange( 0, TAM_EJE, Qt::AlignLeft );
    ui->widgetGraficoMotor->xAxis->setRange( 0, TAM_EJE, Qt::AlignLeft );
    ui->widgetGraficoUltra->xAxis->setRange( 0, TAM_EJE, Qt::AlignLeft );


    connect( &timer_plot, SIGNAL( timeout() ),
             this, SLOT( plotData() ) );
    // Repite constantemente el timer.
    timer_plot.start( UPDATE_TIME_MSEC );
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


void MainWindow::alRecibirDatos( const QByteArray &linea ) {

    if ( m_sensores.parsear(linea) ) {
        plotData();
    }
}


void MainWindow::plotData() {
    double key = timer.elapsed() / 1000.0;
    static double lastPointKey = 0;

    if( key - lastPointKey > 0.1 ) {
        ui->widgetGraficoAccel->graph(0)->addData(key, (double)m_sensores.accel());
        ui->widgetGraficoMotor->graph(0)->addData(key, (double)m_sensores.motor());
        ui->widgetGraficoUltra->graph(0)->addData(key, (double)m_sensores.ultra());
        lastPointKey = key;
    }

    /* make key axis range scroll right with the data at a constant range of 8. */
    ui->widgetGraficoAccel->graph( 0 )->rescaleValueAxis();
    ui->widgetGraficoAccel->xAxis->setRange( key, TAM_EJE, Qt::AlignRight );
    ui->widgetGraficoAccel->replot();

    ui->widgetGraficoMotor->graph( 0 )->rescaleValueAxis();
    ui->widgetGraficoMotor->xAxis->setRange( key, TAM_EJE, Qt::AlignRight );
    ui->widgetGraficoMotor->replot();

    ui->widgetGraficoUltra->graph( 0 )->rescaleValueAxis();
    ui->widgetGraficoUltra->xAxis->setRange( key, TAM_EJE, Qt::AlignRight );
    ui->widgetGraficoUltra->replot();
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




