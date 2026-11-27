// IWYU pragma: begin_exports
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QDebug>
// IWYU pragma: end_exports



MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow ),
        timeTicker( new QCPAxisTickerTime ) {

    ui->setupUi(this);

//    qDebug() << "### SIGNALS & SLOTS ###";
//    qDebug() << "# Main Window #";

    timer_elapsed_program.start();

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


//    qDebug() << "Connect nuevoFrame -> alRecibirImagen works:" <<
        connect( m_camara, &camara::nuevoFrame,
                 this, &MainWindow::alRecibirImagen );
}


void MainWindow::SetupConection() {
    m_conexion = new conexion(this);


//    qDebug() << "Connect Clicked -> alHacerClickConectar works:" <<
        connect( ui->pushButtonConectar, &QPushButton::clicked,
                 this, &MainWindow::alHacerClickConectar );

//    qDebug() << "Connect datoRecibido -> alRecibirDatos works:" <<
        connect( m_conexion, &conexion::lineaRecibida,
                 this, &MainWindow::alRecibirDatos );

//    qDebug() << "Connect conectado -> alConectar works:" <<
        connect( m_conexion, &conexion::conectado,
                 this, &MainWindow::alConectar );

//    qDebug() << "Connect desconectado -> alDesconectar works:" <<
        connect( m_conexion, &conexion::desconectado,
                 this, &MainWindow::alDesconectar );

//    qDebug() << "Connect errorSocket -> alFallarLaConexion works:" <<
        connect( m_conexion, &conexion::errorSocket,
                 this, &MainWindow::alFallarLaConexion );
}


void MainWindow::SetupRemoteControl() {
    controlRemoto_Atajos = new ControlRemoto( ui->tab_cam );

//    qDebug() << "Connect Comando Teclado -> Enviar Comando works:" <<
        connect( controlRemoto_Atajos, &ControlRemoto::SIGNAL_Comando_Teclado,
                 m_conexion, &conexion::enviarComando );
}


void MainWindow::SetupGraphics() {
    graficosAcelerometro = new QCPGraph*[N_AXIS];

    graficosAcelerometro[X_AXIS] = ui->widgetGrafico_Acelerometro_X->addGraph();
    graficosAcelerometro[Y_AXIS] = ui->widgetGrafico_Acelerometro_Y->addGraph();
    graficosAcelerometro[Z_AXIS] = ui->widgetGrafico_Acelerometro_Z->addGraph();

    graficoUltrasonido = ui->widgetGrafico_Ultrasonido->addGraph();

    if ( !graficoUltrasonido ) {
        return;
    }

    for ( uint8_t index = 0; index < N_AXIS; ++index ) {
        if ( !(graficosAcelerometro[index]) ) {
            return;
        }
    }

    // # Eje de abscisas en segundos #
    //QSharedPointer<QCPAxisTickerTime> timeTicker( new QCPAxisTickerTime );
    timeTicker->setTimeFormat( "%s" );

    ui->widgetGrafico_Acelerometro_X->xAxis->setTicker( timeTicker );
    ui->widgetGrafico_Acelerometro_X->xAxis->setLabel( "t [segundos]" );
    ui->widgetGrafico_Acelerometro_X->xAxis->setRange( 0, TAM_EJE, Qt::AlignLeft );
    ui->widgetGrafico_Acelerometro_X->setInteraction( QCP::iRangeDrag, true );

    ui->widgetGrafico_Acelerometro_Y->xAxis->setTicker( timeTicker );
    ui->widgetGrafico_Acelerometro_Y->xAxis->setLabel( "t [segundos]" );
    ui->widgetGrafico_Acelerometro_Y->xAxis->setRange( 0, TAM_EJE, Qt::AlignLeft );
    ui->widgetGrafico_Acelerometro_Y->setInteraction( QCP::iRangeDrag, true );

    ui->widgetGrafico_Acelerometro_Z->xAxis->setTicker( timeTicker );
    ui->widgetGrafico_Acelerometro_Z->xAxis->setLabel( "t [segundos]" );
    ui->widgetGrafico_Acelerometro_Z->xAxis->setRange( 0, TAM_EJE, Qt::AlignLeft );
    ui->widgetGrafico_Acelerometro_Z->setInteraction( QCP::iRangeDrag, true );

    ui->widgetGrafico_Ultrasonido->xAxis->setTicker( timeTicker );
    ui->widgetGrafico_Ultrasonido->xAxis->setLabel( "t [segundos]" );
    ui->widgetGrafico_Ultrasonido->xAxis->setRange( 0, TAM_EJE, Qt::AlignLeft );
    ui->widgetGrafico_Ultrasonido->setInteraction( QCP::iRangeDrag, true );


//    qDebug() << "Connect timeout -> plotData works:" <<
        connect( &timer_plot, &QTimer::timeout,
                 this, &MainWindow::plotData );
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
        ui->label_estado->setText( "Conectando..." );
        m_conexion->conectar( ESP_IP, conexion::puerto_esp_webserver_t::ESP_PORT );
        m_camara->iniciar();
    }
}


void MainWindow::alRecibirDatos( const QByteArray &linea ) {

    if ( m_sensores.parsear(linea) ) {
        plotData();
    }
}


void MainWindow::plotData() {
    double key = timer_elapsed_program.elapsed() / 1000.0;
    static double lastPointKey = 0;

    if( key - lastPointKey > 0.1 ) {
        graficosAcelerometro[X_AXIS]->addData( key, (double)m_sensores.acelerometro_x() );
        graficosAcelerometro[Y_AXIS]->addData( key, (double)m_sensores.acelerometro_y() );
        graficosAcelerometro[Z_AXIS]->addData( key, (double)m_sensores.acelerometro_z() );
        graficoUltrasonido->addData( key, (double)m_sensores.ultrasonido() );
        lastPointKey = key;
    }


    /* make key axis range scroll right with the data at a constant range of 8. */
    ui->widgetGrafico_Acelerometro_X->graph( 0 )->rescaleValueAxis();
    ui->widgetGrafico_Acelerometro_X->replot();

    ui->widgetGrafico_Acelerometro_Y->graph( 0 )->rescaleValueAxis();
    ui->widgetGrafico_Acelerometro_Y->replot();

    ui->widgetGrafico_Acelerometro_Z->graph( 0 )->rescaleValueAxis();
    ui->widgetGrafico_Acelerometro_Z->replot();

    ui->widgetGrafico_Ultrasonido->graph( 0 )->rescaleValueAxis();
    ui->widgetGrafico_Ultrasonido->replot();

    // Reacomodamiento de ejes.
    if ( key >= TAM_EJE ) {
        ui->widgetGrafico_Acelerometro_X->xAxis->setRange( key, TAM_EJE, Qt::AlignRight );
        ui->widgetGrafico_Acelerometro_Y->xAxis->setRange( key, TAM_EJE, Qt::AlignRight );
        ui->widgetGrafico_Acelerometro_Z->xAxis->setRange( key, TAM_EJE, Qt::AlignRight );
         ui->widgetGrafico_Ultrasonido->xAxis->setRange( key, TAM_EJE, Qt::AlignRight );
    }
}


void MainWindow::alConectar()
{
    ui->label_estado->setText("Conectado");
    ui->pushButtonConectar->setText("Desconectar");

    // Repite constantemente el timer.
    timer_plot.start( UPDATE_TIME_MSEC );
}

void MainWindow::alDesconectar()
{
    ui->label_estado->setText("Desconectado");
    ui->pushButtonConectar->setText("Conectar");

    timer_plot.stop();
}

void MainWindow::alFallarLaConexion(const QString &mensaje)
{
    ui->label_estado->setText(">>> Error: " + mensaje + " <<<");
    ui->pushButtonConectar->setText("Conectar");

    m_camara->detener();
}

void MainWindow::alRecibirImagen(const QImage &imagen)
{
    ui->label_camara->setPixmap(QPixmap::fromImage(imagen).scaled(
        ui->label_camara->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}




