#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "qt_includes.h"
#include "rover_includes.h"


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
        //void comandoPorTeclado();
        // void SLOT_Serial_Comando_Avanzar();
        // void SLOT_Serial_Comando_Retroceder();
        // void SLOT_Serial_Comando_Derecha();
        // void SLOT_Serial_Comando_Izquierda();
        // void SLOT_Serial_Comando_Frenar();

    private:
        Ui::MainWindow *ui;
        conexion *m_conexion;
        sensores  m_sensores;
        camara *m_camara;
        QPalette palette;
        QColor color;

        ControlRemoto *controlRemoto_Atajos;

        // ### GRÁFICOS ###
        int m_tiempo; /*eje x: tiempo*/
};

#endif // MAINWINDOW_H
