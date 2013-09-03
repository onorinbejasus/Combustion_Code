#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLineEdit>
#include <QSlider>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void connectSignalsToSlots();
    void bypassParamLoad();
    
public slots:
    void Modes();
    void Zoom();
    void Change();
    void loadParams();
    void magZoomChanged(int value);
    void magRadiusChanged(int value);
    void arrowGlyphSliceClicked();
    void xSlicePosChanged(int value);
    void ySlicePosChanged(int value);
    void zSlicePosChanged(int value);
private:
    Ui::MainWindow *ui;
    float defaultMagZoom;
    float defaultMagRadius;
    void enableUIWidgets();
    float getSliderPct(QSlider* slider, int value);
    void loadParamsHelper(QString fileName);
};

#endif // MAINWINDOW_H
