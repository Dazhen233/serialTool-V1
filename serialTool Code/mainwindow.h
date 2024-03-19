#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <Qdebug>
#include <QList>
#include <QMessageBox>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_OpenBt_clicked();

    void on_sendBt_clicked();

    void readData();

    void on_ClearBt_clicked();

    void on_btnClearSend_clicked();

private:

    void InitSerialPortName();

    Ui::MainWindow *ui;

    QSerialPort *serial;

    QString getTimestamp;
};
#endif // MAINWINDOW_H
