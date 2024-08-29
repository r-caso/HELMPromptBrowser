#include "mainwindow.hpp"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    const auto screenSize = w.screen()->availableSize();
    w.resize({screenSize.width(), screenSize.height()});
    w.show();
    return a.exec();
}
