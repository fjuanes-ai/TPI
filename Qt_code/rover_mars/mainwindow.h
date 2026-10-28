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

        /*Parte de controles*/
        void comandoPorTeclado();

    private:
        Ui::MainWindow *ui;
        conexion *m_conexion;
        sensores  m_sensores;
        camara *m_camara;
        QPalette palette;
        QColor color;

        /*PARA LOS GRAFICOS*/
        QChart *m_chartAccel;
        QLineSeries *m_serieAccel;
        QChartView *m_chartViewAccel;

        QChart *m_chartUltra;
        QLineSeries *m_serieUltra;
        QChartView * m_chartViewUltra;

        QChart *m_chartMotor;
        QLineSeries *m_serieMotor;
        QChartView * m_chartViewMotor;
        int m_tiempo; /*eje x: tiempo*/
};

#endif // MAINWINDOW_H
