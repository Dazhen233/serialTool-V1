#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QWidget>
#include <QSizePolicy>
#include <QtSerialPort/QSerialPortInfo>
#include <Qdebug>
#include <QList>
#include <QMessageBox>
#include <QDateTime>
#include <QSettings>
#include <QTimer>
#include <QFont>
#include<QComboBox>
#include<QAbstractItemView>

#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include "QDateTime"

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

    void graphInit();

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

    void on_openChart_clicked();

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

    QTimer *chartTimer;//计时器

    QChart *chart;//画布

    QSplineSeries *series;//线

    QValueAxis *axisX;//轴
    QValueAxis *axisY;

};
#endif // MAINWINDOW_H
