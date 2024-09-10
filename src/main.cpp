#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow   w;

    if (!w.mMinimizeStartUp) w.show();

    return a.exec();
}
