#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font(QStringLiteral("Arial"));   //设种字体
    font.setPixelSize(12);                  //字体大小，等同于PointSize的9点大小
    qApp->setFont(font);
    MainWindow w;

    // 设置主窗口对象的大小策略
    w.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    w.show();
    return a.exec();
}
