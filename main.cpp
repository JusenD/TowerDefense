#include "mainwindow.h"
#include"map.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Map::init();
    MainWindow w;
    w.show();
    return a.exec();
}
