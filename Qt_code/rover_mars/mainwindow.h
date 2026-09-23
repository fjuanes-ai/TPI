#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "conexion.h"
#include "sensores.h"
#include "camara.h"

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

        void alPresionarVerGraficos();
        void alPresionarVolver();

        /*Parte de controles*/
        void alPresionarAdelante();
        void alSoltarAdelante();

        void alPresionarAtras();
        void alSoltarAtras();

        void alPresionarIzquierda();
        void alSoltarIzquierda();

        void alPresionarDerecha();
        void alSoltarDerecha();

    private:
        Ui::MainWindow *ui;
        conexion *m_conexion;
        sensores  m_sensores;
        camara *m_camara;

        /*PARA LOS GRAFICOS*/
        QChart *m_chartAccel;
        QLineSeries *m_serieAccel;
        QChartView *m_chartViewAccel;

        QChart *m_chartUltra;
        QLineSeries *m_serieUltra1;
        QLineSeries *m_serieUltra2;
        QLineSeries *m_serieUltra3;
        QChartView * m_chartViewUltra;
        int m_contadorMuestras= 0; /*eje x: numero de muestra*/
};

#endif // MAINWINDOW_H
