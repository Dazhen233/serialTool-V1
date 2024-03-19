#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    serial = new QSerialPort;

    setWindowTitle("MySerialTool");
    InitSerialPortName();

    //设置波特率
    ui->baundrateCb->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->baundrateCb->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->baundrateCb->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baundrateCb->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baundrateCb->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baundrateCb->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baundrateCb->setCurrentIndex(2);

    //设置数据位
    ui->databitCb->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->databitCb->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->databitCb->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->databitCb->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->databitCb->setCurrentIndex(3);

    //设置停止位
    ui->stopbitCb->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    ui->stopbitCb->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
    ui->stopbitCb->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    ui->stopbitCb->setCurrentIndex(0);

    //设置校验位
    ui->checkbitCb->addItem(tr("None"), QSerialPort::NoParity);
    ui->checkbitCb->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->checkbitCb->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->checkbitCb->setCurrentIndex(0);

    //禁用发送按钮
    ui->sendBt->setEnabled(false);

    ui->serialStatusLb->setStyleSheet("color:red;");
    ui->stateBar->setStyleSheet("color:red;");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitSerialPortName()
{
    // 清空下拉框
    ui->serialCb->clear();

    //通过QSerialPortInfo查找可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QString showName = info.portName()+":"+info.description();
        qDebug() << showName.length();
        ui->serialCb->addItem(showName);
    }
}

//打开串口和关闭串口设置
void MainWindow::on_OpenBt_clicked()
{
    if(ui->OpenBt->text()==tr("打开串口"))
    {
        //设置串口名字
        QString portname = (ui->serialCb->currentText()).split(":").at(0);
        qDebug() << portname;
        serial->setPortName(portname);

        //设置波特率
        serial->setBaudRate(ui->baundrateCb->currentText().toInt());

        //设置数据位
        QString dataBitsText = ui->databitCb->currentText();
        QSerialPort::DataBits dataBits;

        if (dataBitsText == "5")
            dataBits = QSerialPort::Data5;
        else if (dataBitsText == "6")
            dataBits = QSerialPort::Data6;
        else if (dataBitsText == "7")
            dataBits = QSerialPort::Data7;
        else
            dataBits = QSerialPort::Data8;

        serial->setDataBits(dataBits);

        //设置停止位
        QString stopbits = ui->stopbitCb->currentText();
        QSerialPort::StopBits stopBits;

        if(stopbits == "1")
            stopBits = QSerialPort::OneStop;
        else if(stopbits == "1.5")
            stopBits = QSerialPort::OneAndHalfStop;
        else
            stopBits = QSerialPort::TwoStop;

        serial->setStopBits(stopBits);

        //设置校验位
        QString paritytext = ui->checkbitCb->currentText();
        QSerialPort::Parity parity;

        if (paritytext == "None")
            parity = QSerialPort::NoParity;
        else if (paritytext == "Even")
            parity = QSerialPort::EvenParity;
        else
            parity = QSerialPort::OddParity;

        serial->setParity(parity);


        if (serial->open(QIODevice::ReadWrite))
        {
            ui->baundrateCb->setEnabled(false);
            ui->databitCb->setEnabled(false);
            ui->checkbitCb->setEnabled(false);
            ui->serialCb->setEnabled(false);
            ui->stopbitCb->setEnabled(false);

            //使能发送按钮
            ui->sendBt->setEnabled(true);

            ui->serialStatusLb->setText(tr("Connected"));
            ui->serialStatusLb->setStyleSheet("color: #00FF00;");

            //ui左下角状态条
            QString si = "%1 OPENED, %2, %3, %4, %5";
            QString status = si.arg(serial->portName()).arg(serial->baudRate()).arg(serial->dataBits()).arg(serial->parity()).arg(serial->stopBits());
            ui->stateBar->setText(status);
            ui->stateBar->setStyleSheet("color: #00FF00;");

            ui->OpenBt->setText(tr("关闭串口"));
            connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
        }
        else
        {
            QMessageBox::information(this, tr("错误提示"), tr("无法打开串口，请重试！"));
        }
    }
    else
    {
        //关闭串口
        serial->close();
        //重新使能属性
        ui->baundrateCb->setEnabled(true);
        ui->databitCb->setEnabled(true);
        ui->checkbitCb->setEnabled(true);
        ui->serialCb->setEnabled(true);
        ui->stopbitCb->setEnabled(true);
        //失能发送功能
        ui->sendBt->setEnabled(false);

        ui->serialStatusLb->setText(tr("Closed"));
        ui->serialStatusLb->setStyleSheet("color:red;");

        ui->stateBar->setText(tr("Closed"));
        ui->stateBar->setStyleSheet("color:red;");

        ui->OpenBt->setText(tr("打开串口"));

    }
}

void MainWindow::readData()
{
    QByteArray buf;
    QByteArray receivedData;

    qDebug() << "readData: " << endl;

    buf = serial->readAll();
    if (!buf.isEmpty())
    {
        QString str = ui->recvEdit->toPlainText();
        str += tr(buf);
        ui->recvEdit->clear();
        ui->recvEdit->append("接收：" + str + "\n");
    }
    buf.clear();
}

//发送功能
void MainWindow::on_sendBt_clicked()
{

    //发送显示在接收区
    QString data = ui->sendEdit->toPlainText();

    if(!data.isEmpty())
    {
        getTimestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");//获取时间戳
        QString textToSend = "[" + getTimestamp + "]" + "发送-> <" + data + ">";
        ui->recvEdit->append(textToSend);

        //发送数据至串口
        QByteArray dataToUart = data.toLatin1();
        serial->write(dataToUart);
    }
    else
        QMessageBox::warning(nullptr, "Empty Input", "Input is empty!");
}

//清空接收功能
void MainWindow::on_ClearBt_clicked()
{
    ui->recvEdit->clear();
}

//清空发送功能
void MainWindow::on_btnClearSend_clicked()
{
    ui->sendEdit->clear();
}

