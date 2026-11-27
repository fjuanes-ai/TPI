#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// IWYU pragma: begin_exports
#include "qt_includes.h"
#include "rover_includes.h"
// IWYU pragma: end_exports


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

    public slots:
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
        QElapsedTimer   timer_elapsed_program;


        // ### Periféricos ###
        conexion        *m_conexion;
        sensores        m_sensores;
        camara          *m_camara;

        ControlRemoto   *controlRemoto_Atajos;

        // ### GRÁFICOS ###
        QCPGraph        **graficosAcelerometro;
        typedef enum axis_e {
            X_AXIS = 0,
            Y_AXIS,
            Z_AXIS,
            N_AXIS = 3
        } axis_t;
        QCPGraph        *graficoUltrasonido;

        // # Vector de datos #
        QSharedPointer<QCPAxisTickerTime> timeTicker;
};

#endif // MAINWINDOW_H
