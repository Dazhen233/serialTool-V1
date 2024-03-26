#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <Qdebug>
#include <QList>
#include <QMessageBox>
#include <QDateTime>
#include <QSettings>
#include <QTimer>
#include <QFont>

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

    void on_chk_rev_line_stateChanged(int arg1);

    void updateSerialPorts();


    void on_autoSend_stateChanged(int arg1);

    void on_sendEdit_textChanged();

private:

    //void updateSerialPorts();

    Ui::MainWindow *ui;

    QSerialPort *serial;

    QString hexDataToSend;

    bool firstDisplay;

    void showEvent(QShowEvent *e);

    void closeEvent(QCloseEvent *e);

    QTimer *timer;

    QTimer *autoSendTimer;

    QStringList previousPorts; // 存储之前的串口列表
};
#endif // MAINWINDOW_H
