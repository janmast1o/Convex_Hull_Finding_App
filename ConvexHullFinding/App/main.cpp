#include <QApplication>
#include <QMetaType>
#include <QDebug>
#include <vector>
#include <utility>
#include <QObject>
#include "mainwindow.h"
#include "pointslog.h"


int main (int argc, char* argv[]) {
    QApplication app(argc, argv);
    qRegisterMetaType<std::vector<std::pair<double, double>>>();
    qRegisterMetaType<std::pair<double, double>>();
    MainWindow main_window;
    main_window.launch();
    return app.exec();
}