#include "start_menue.h"
#include"map.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Map::init();
//    MainWindow w;
    Start_menue w;
    w.show();
    return a.exec();
}
