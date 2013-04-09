#include <QtGui/QApplication>
#include "mainwindow.hh"

float mag = 1.0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();

}
