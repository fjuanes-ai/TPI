#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
        void onConectarClicked();
        void onLineaRecibida(const QByteArray &linea);
        void onConectado();
        void onDesconectado();
        void onErrorSocket(const QString &mensaje);
        void onNuevoFrame(const QImage &imagen);

    private:
        Ui::MainWindow *ui;
        conexion *m_conexion;
        sensores  m_sensores;
        camara *m_camara;
};

#endif // MAINWINDOW_H
