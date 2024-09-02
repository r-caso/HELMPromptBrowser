#include "mainwindow.hpp"

#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QSysInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (QSysInfo::productType() != "macos") {
        QApplication::setStyle("fusion");
    }
    MainWindow w;
    const auto screenSize = w.screen()->availableSize();
    w.resize({screenSize.width(), screenSize.height()});
    w.show();
    return a.exec();
}
