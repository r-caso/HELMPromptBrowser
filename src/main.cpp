#include "mainwindow.hpp"

#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QSysInfo>

int main(int argc, char *argv[])
{
    QApplication const app(argc, argv);
    if (QSysInfo::productType() != "macos") {
        QApplication::setStyle("fusion");
    }
    MainWindow window;
    const auto screenSize = window.screen()->availableSize();
    window.resize({screenSize.width(), screenSize.height()});
    window.show();
    return app.exec();
}
