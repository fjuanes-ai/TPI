#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "qt_includes.h"
#include "rover_includes.h"


#define TAM_EJE             180
#define UPDATE_TIME_MSEC    100

QT_BEGIN_NAMESPACE namespace Ui
{
    class MainWindow;
}


QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

    private slots:
        void alHacerClickConectar();
        void alRecibirDatos(const QByteArray &linea);
        void alConectar();
        void alDesconectar();
        void alFallarLaConexion(const QString &mensaje);
        void alRecibirImagen(const QImage &imagen);
        void plotData();

    private:
        void SetupCamera();
        void SetupConection();
        void SetupRemoteControl();
        void SetupGraphics();


        Ui::MainWindow  *ui;

        QTimer          timer_plot;
        QElapsedTimer   timer;


        // ### Periféricos ###
        conexion        *m_conexion;
        sensores        m_sensores;
        camara          *m_camara;

        ControlRemoto   *controlRemoto_Atajos;

        // ### GRÁFICOS ###
        QCPGraph        *graficoAcelerometro;
        QCPGraph        *graficoMotor;
        QCPGraph        *graficoUltrasonido;
        // # Vector de datos #
        QSharedPointer<QCPAxisTickerTime> timeTicker;
};

#endif // MAINWINDOW_H
