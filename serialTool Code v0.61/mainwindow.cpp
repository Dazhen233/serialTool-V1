#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    updateSerialPorts();

    //初始化timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateSerialPorts()));
    timer->start(1000);

    serial = new QSerialPort;

    setWindowTitle("MySerialTool");

    //设置波特率
    ui->baudrateCb->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->baudrateCb->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->baudrateCb->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudrateCb->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudrateCb->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudrateCb->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    //设置数据位
    ui->databitCb->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->databitCb->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->databitCb->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->databitCb->addItem(QStringLiteral("8"), QSerialPort::Data8);

    //设置停止位
    ui->stopbitCb->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    ui->stopbitCb->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
    ui->stopbitCb->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    //设置校验位
    ui->checkbitCb->addItem(tr("None"), QSerialPort::NoParity);
    ui->checkbitCb->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->checkbitCb->addItem(tr("Odd"), QSerialPort::OddParity);

    ui->sendBt->setEnabled(false);//禁用发送按钮
    ui->autoSend->setEnabled(false);//禁用自动发送按钮

    ui->widget->setVisible(false);//初始化示波器不可见
    ui->openChartLb->setStyleSheet("color:red;");

    ui->serialStatusLb->setStyleSheet("color:red;");
    ui->stateBar->setStyleSheet("color:red;");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

//打开已保存显示设置
void MainWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    QSettings settings("config.ini", QSettings::IniFormat);

    ui->serialCb->setCurrentText(settings.value("serialPort").toString());
    ui->baudrateCb->setCurrentText(settings.value("baudrate", 9600).toString());
    ui->databitCb->setCurrentIndex(settings.value("databits", 3).toInt());
    ui->stopbitCb->setCurrentIndex(settings.value("stopbits", 0).toInt());
    ui->checkbitCb->setCurrentIndex(settings.value("parity", 0).toInt());

    ui->chk_rev_hex->setChecked(settings.value("recvHex").toBool());
    ui->recvTS->setChecked(settings.value("recvTimeStamp").toBool());
    ui->chk_rev_line->setChecked(settings.value("autoChangeLine").toBool());
    ui->chk_send_hex->setChecked(settings.value("sendHex").toBool());
    ui->sendTS->setChecked(settings.value("sendTimeStamp").toBool());

    ui->txtSendMs->setValue(settings.value("SpinBoxValue", 1000).toInt());//保存间隔发送时间
    ui->sendEdit->setText(settings.value("text").toString());
}

//关闭保存设置
void MainWindow::closeEvent(QCloseEvent *e)
{
    QSettings settings("config.ini", QSettings::IniFormat);

    settings.setValue("serialPort", ui->serialCb->currentText());
    settings.setValue("baudrate", ui->baudrateCb->currentText());
    settings.setValue("databits", ui->databitCb->currentIndex());
    settings.setValue("stopbits", ui->stopbitCb->currentIndex());
    settings.setValue("parity", ui->checkbitCb->currentIndex());

    settings.setValue("recvHex", ui->chk_rev_hex->isChecked());
    settings.setValue("recvTimeStamp", ui->recvTS->isChecked());
    settings.setValue("autoChangeLine", ui->chk_rev_line->isChecked());
    settings.setValue("sendHex", ui->chk_send_hex->isChecked());
    settings.setValue("sendTimeStamp", ui->sendTS->isChecked());
    settings.setValue("SpinBoxValue", ui->txtSendMs->value());
    settings.setValue("text", ui->sendEdit->toPlainText());

    QWidget::closeEvent(e);
}

void MainWindow::updateSerialPorts()
{
    //获取当前可用串口列表
    QStringList currentPorts;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        //构造串口名称和描述信息的字符串
        QString portName = info.portName();
        QString portDescription = info.description();
        QString portInfo = portName;
        if (!portDescription.isEmpty()) {
            portInfo += "  " + portDescription;
        }

        //添加串口信息到列表中
        currentPorts << portInfo;
    }

    //检查并更新串口列表
    foreach(const QString &port, previousPorts)
    {
        if (!currentPorts.contains(port))
        {
            //之前的串口列表中存在，但当前不存在，说明串口被移除了，需要从下拉框中移除
            ui->serialCb->removeItem(ui->serialCb->findText(port));
        }
    }

    foreach(const QString &port, currentPorts)
    {
        if (!previousPorts.contains(port))
        {
            //当前的串口列表中存在，但之前不存在，说明是新添加的串口，需要添加到下拉框中
            ui->serialCb->addItem(port);
        }
    }

    //combobox计算下拉框长度，使串口信息显示完整
    int max_len = 0;
    QFontMetrics fontMetrics(ui->serialCb->font());
    for(int idx = 0; idx < this->ui->serialCb->count(); idx++)
    {
        QString portName = ui->serialCb->itemText(idx);
        int width = fontMetrics.horizontalAdvance(portName);
        if(max_len < width)
            max_len = width;
    }
    ui->serialCb->view()->setMinimumWidth(max_len*1.1);//1.1为计算值，不对需替换

    // 更新之前的串口列表为当前的串口列表
    previousPorts = currentPorts;
}

//打开串口和关闭串口设置
void MainWindow::on_OpenBt_clicked()
{
    if(ui->OpenBt->text()==tr("打开串口"))
    {
        //设置串口名字,使用空格分隔name & description
        QString currentText = ui->serialCb->currentText();
        QStringList parts = currentText.split(" ");
        qDebug()<<parts;
        QString portname = parts.at(0);

        qDebug() << portname;
        serial->setPortName(portname);

        //设置波特率
        serial->setBaudRate(ui->baudrateCb->currentText().toInt());

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
            ui->baudrateCb->setEnabled(false);
            ui->databitCb->setEnabled(false);
            ui->checkbitCb->setEnabled(false);
            ui->serialCb->setEnabled(false);
            ui->stopbitCb->setEnabled(false);
            ui->sendBt->setEnabled(true);//使能发送按钮
            ui->autoSend->setEnabled(true);//使能自动发送功能

            ui->serialStatusLb->setText(tr("串口已连接"));
            ui->serialStatusLb->setStyleSheet("color: #1cff55;");

            //ui左下角状态条
            QString si = "%1 OPENED, %2, %3, %4, %5";
            QString status = si.arg(serial->portName()).arg(serial->baudRate()).arg(serial->dataBits()).arg(serial->parity()).arg(serial->stopBits());
            ui->stateBar->setText(status);
            ui->stateBar->setStyleSheet("color: #1cff55;");

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
        ui->baudrateCb->setEnabled(true);
        ui->databitCb->setEnabled(true);
        ui->checkbitCb->setEnabled(true);
        ui->serialCb->setEnabled(true);
        ui->stopbitCb->setEnabled(true);
        ui->sendBt->setEnabled(false);//失能发送功能
        ui->autoSend->setEnabled(false);//失能自动发送功能

        ui->serialStatusLb->setText(tr("串口已关闭"));
        ui->serialStatusLb->setStyleSheet("color:red;");

        ui->stateBar->setText(tr("Closed"));
        ui->stateBar->setStyleSheet("color:red;");

        ui->OpenBt->setText(tr("打开串口"));

    }
}

//创建时间戳
QString getTimestamp()
{
    return QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

//以hex形式显示
QString hexStringWithSpaces(const QByteArray &data)
{
    QString hexString = data.toHex().toUpper(); //转换为大写的十六进制字符串

    //在每两个字符之间插入一个空格
    for (int i = 2; i < hexString.length(); i += 3)
    {
        hexString.insert(i, ' ');
    }

    return hexString;
}

//接收数据
void MainWindow::readData()
{
    QByteArray buf;

    buf = serial->readAll();

    if (!buf.isEmpty())
    {
        QString recvData;

        if (ui->chk_rev_hex->isChecked())
        {
            recvData = hexStringWithSpaces(buf); //使用封装的函数转换为带空格的十六进制字符串
        }
        else
        {
            recvData = QString::fromLatin1(buf); //使用 Latin1 编码解析接收的数据
        }

        QStringList dataList = recvData.split(" "); // 将接收到的数据分割成单独的值

        foreach(QString data, dataList)
        {
            bool ok;
            double value = data.toDouble(&ok);
            if (ok)
            {
                // 假设 series 是 MainWindow 类的成员变量。
                series->append(series->count(), value); // 将数据点添加到系列中

                // 检查系列中的数据点数量是否超过限制
                int maxDataPoints = 100; // 设置数据点数量限制
                while (series->count() > maxDataPoints)
                {
                    series->remove(0); // 移除最早的数据点
                }

                // 将 x 轴范围设置为使最新的数据点位于图表最右侧
                axisX->setRange(series->count() - maxDataPoints, series->count());
            }
        }

        QString textToShow;

        //检查接收是否需要时间戳
        if(ui->recvTS->isChecked())
        {
            textToShow = "[" + getTimestamp() + "]" + " 接收<- < " + recvData + " >";
        }
        else
        {
            textToShow = "接收<- < " + recvData + " >";
        }

        //检查发送是否需要自动换行
        if(ui->chk_rev_line->isChecked())
        {
            ui->recvEdit->append(textToShow);
            ui->recvEdit->ensureCursorVisible(); //确保光标可见
        }
        else
        {
            if(firstDisplay)
            {
                textToShow = "接收<-" + recvData;
                firstDisplay = false;
            }
            else
            {
                textToShow = recvData;
            }
            ui->recvEdit->insertPlainText(textToShow);
            ui->recvEdit->ensureCursorVisible(); //确保光标可见
        }
    }
    buf.clear();
}

//发送以及发送显示功能
void MainWindow::on_sendBt_clicked()
{
    QString data = ui->sendEdit->toPlainText().trimmed();
    QByteArray textToUart;
    QString recvData;
    QString textToSend;

    if(data.isEmpty())//检查是否为空
        return;

    //检查是否需要十六进制发送
    if(ui->chk_send_hex->isChecked())
    {
        textToUart = QByteArray::fromHex(data.toLatin1());
        serial->write(textToUart);
    }
    else
    {
        textToUart = data.toLatin1();
        serial->write(textToUart);
    }


    if(ui->chk_rev_hex->isChecked())
    {
        recvData = hexStringWithSpaces(textToUart);
    }
    else
    {
       recvData = QString::fromLatin1(textToUart);
    }

    //检查发送是否需要时间戳
    if(ui->sendTS->isChecked())
    {
        textToSend = "[" + getTimestamp() + "] 发送-> < " + recvData + " >";
    }
    else
    {
        textToSend = "发送-> < " + recvData + " >";
    }
    ui->recvEdit->append(textToSend);
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

//更新firstDisplay状态
void MainWindow::on_chk_rev_line_stateChanged(int state)
{
    Q_UNUSED(state);
    if (!ui->chk_rev_line->isChecked())
    {
        ui->recvEdit->insertPlainText("\n");//换行，避免和自动换行粘连
        firstDisplay = true;
    }
}

//自动发送功能
void MainWindow::on_autoSend_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    int interval = ui->txtSendMs->value(); //获取间隔值
    //获取按钮的bool值
    if(ui->autoSend->isChecked())
    {
        autoSendTimer = new QTimer(this);
        connect(autoSendTimer, SIGNAL(timeout()), this, SLOT(on_sendBt_clicked()));
        //使其不能为0
        if (interval == 0)
        {
            interval+=1;
            ui->txtSendMs->setValue(1);
        }
        autoSendTimer->start(interval);
        ui->txtSendMs->setEnabled(false);

    }
    else
    {
        autoSendTimer->stop();
        ui->txtSendMs->setEnabled(true);
    }
}

//查看字符是否Valid
void MainWindow::on_sendEdit_textChanged()
{
    QString data = ui->sendEdit->toPlainText().trimmed();
    //正则表达式来验证输入是否符合要求
    QRegExp regex("[0-9a-fA-F ]*");
    if (!regex.exactMatch(data) && !data.isEmpty())
    {
        QMessageBox::warning(this, "提醒", "发送内容只能包含0-9、a-f、A-F以及空格！");

        //如果输入不合法，删除最后一个字符
        ui->sendEdit->textCursor().deletePreviousChar();
    }
}

void MainWindow::graphInit()
{
    QPen pen(Qt::red, 3 , Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    chart = new QChart();
    series = new QSplineSeries();
    axisX = new QValueAxis();
    axisY = new QValueAxis();

    chart->legend()->hide();//隐藏图标
    ui->widget->setChart(chart);
    ui->widget->setRenderHint(QPainter::Antialiasing);

    //添加series
    chart->addSeries(series);//把线添加到chart中

    //设置xy轴
    axisX->setTickCount(11);//设置坐标轴格数
    axisX->setMin(0);//设置最小值，最大值
    axisX->setMax(100);
    //axisX->setFormat("hh:mm:ss");//设置x轴单位
    axisX->setTitleText("x1");//设置title

    axisY->setTickCount(5);
    axisY->setTitleText("y1");
    axisY->setMin(-100);
    axisY->setMax(100);
    axisY->setLinePenColor(QColor(Qt::darkBlue));//设置坐标轴颜色
    //axisY->setGridLineColor(QColor(Qt::darkBlue));
    axisY->setGridLineVisible(false);//设置Y轴网格不显示

    chart->addAxis(axisX, Qt::AlignBottom);//将x轴y轴添加到chart中
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);//将数据系列与坐标轴相关联
    series->attachAxis(axisY);

    //axisY->setLinePen(pen);
    //axisX->setLinePen(pen);

}

void MainWindow::on_openChart_clicked()
{
    if(ui->openChart->text()==tr("打开"))
    {
        ui->openChartLb->setStyleSheet("color:#1cff55;");
        ui->openChart->setText("关闭");
        ui->widget->setVisible(true);
        graphInit();
    }
    else
    {
        ui->openChartLb->setStyleSheet("color:red;");
        ui->openChart->setText("打开");
        ui->widget->setVisible(false);
    }
}

