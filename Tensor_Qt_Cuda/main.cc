#include <QtGui/QApplication>
#include "mainwindow.hh"

extern "C" void runCudaPart();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
runCudaPart();

    w.connectSignalsToSlots();

    w.show();

    {
        // To be removed in the final version
        w.bypassParamLoad();
    }
    return a.exec();
}
