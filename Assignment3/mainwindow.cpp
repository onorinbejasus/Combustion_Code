#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include "FatalError.hh"
#include "Streamline.hh"
#include "AllocArray.hh"
#include "VectorFieldRenderer.hh"
#include "Parameters.hh"
#include "VolumeRenderer.hh"

#include "VolumeRenderer.hh"
#include "Parameters.hh"
#include "Vector.hh"
#include "point.hh"

#include "open_gl.hh"
#include "enums.hh"

#include "glwidget.h"

extern Streamline				streamlines ;				//streamlines object.
extern VolumeRenderer		    volRenderer ;               //volume renderer
extern VectorFieldRenderer		vectorField ;				//vector field renderer
extern InteractionMode CurrIM;
extern bool magnifying;

float value = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Change(){

    if(ui->radioButton->isChecked()){

        CurrIM = SelectMode;
    }else{
        CurrIM =ExploreMode;
    }
}

void MainWindow::Zoom(){

    volRenderer.ChangeProjectorLocation((value - ui->horizontalSlider->value())*10) ;
    volRenderer.SetProjectionResolution(true, true) ;
    value = ui->horizontalSlider->value();
    ui->widget_2->paintGL();
}

void MainWindow::Modes(){

    if(ui->checkBox->checkState() == Qt::Checked){ // glyphs

        vectorField.bVisible = true;

    }
    if(ui->checkBox_2->checkState() == Qt::Checked){ // volume

    }
   if(ui->checkBox_3->checkState() == Qt::Checked){

        streamlines.bVisible  = true;
    }
   if(ui->checkBox_4->checkState() == Qt::Checked){
       magnifying = true;
   }
    if(ui->checkBox->checkState() != Qt::Checked){

        vectorField.bVisible = false;
    }
    if(ui->checkBox_3->checkState() != Qt::Checked){

        streamlines.bVisible  = false;
    }

    if(ui->checkBox_4->checkState() != Qt::Checked){

        magnifying = false;

    }

    ui->widget_2->paintGL();
}

