#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include "FatalError.hh"
#include "Streamline.hh"
#include "AllocArray.hh"
#include "VectorFieldRenderer.hh"
#include "Parameters.hh"

#include "Projector.hh"
#include "Parameters.hh"
#include "Vector.hh"
#include "point.hh"

#include "open_gl.hh"
#include "enums.hh"

#include "glwidget.h"

extern Streamline streamlines ;	 // from glwidget.cpp
extern Projector projector ; // from glwidget.cpp
extern VectorFieldRenderer vectorField ; // from glwidget.cpp
extern InteractionMode CurrIM; // from glwidget.cpp
extern bool magnifying; // from glwidget.cpp
extern bool shift; // from glwidget.cpp
extern void configureStreamlines(); // from glwidget.cpp

float value = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

/*    QButtonGroup interactionRadioButtons(ui->centralWidget);
    interactionRadioButtons.setExclusive(true);
    interactionRadioButtons.addButton(ui->selectRadioButton);
    interactionRadioButtons.addButton(ui->exploreRadioButton);

    QButtonGroup glyphRadioButtons(ui->centralWidget);
    glyphRadioButtons.setExclusive(true);
    glyphRadioButtons.addButton(ui->selectRadioButton);
    glyphRadioButtons.addButton(ui->exploreRadioButton);
*/}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSignalsToSlots()
{
    QObject::connect(ui->loadParamsButton, SIGNAL(clicked()), this, SLOT(loadParams()));
    QObject::connect(ui->xSliceCheckBox, SIGNAL(clicked()), this, SLOT(arrowGlyphSliceClicked()));
    QObject::connect(ui->ySliceCheckBox, SIGNAL(clicked()), this, SLOT(arrowGlyphSliceClicked()));
    QObject::connect(ui->zSliceCheckBox, SIGNAL(clicked()), this, SLOT(arrowGlyphSliceClicked()));
    QObject::connect(ui->magZoomSlider, SIGNAL(valueChanged(int)), this, SLOT(magZoomChanged(int)));
    QObject::connect(ui->magRadiusSlider, SIGNAL(valueChanged(int)), this, SLOT(magRadiusChanged(int)));
    QObject::connect(ui->xSlicePosSlider, SIGNAL(valueChanged(int)), this, SLOT(xSlicePosChanged(int)));
    QObject::connect(ui->ySlicePosSlider, SIGNAL(valueChanged(int)), this, SLOT(ySlicePosChanged(int)));
    QObject::connect(ui->zSlicePosSlider, SIGNAL(valueChanged(int)), this, SLOT(zSlicePosChanged(int)));
}

void MainWindow::Change()
{
    if(ui->selectRadioButton->isChecked())
        CurrIM = SelectMode;
    else if(ui->exploreRadioButton->isChecked())
        CurrIM = ExploreMode;
    if(ui->superquadRadioButton->isChecked() == true)
    {
        shift = false;
        ui->glyphSliceGroupBox->setEnabled(false);
        ui->magnificationGroupBox->setEnabled(true);
    }
    if(ui->arrowRadioButton->isChecked() == true)
    {
        shift = true;
        ui->glyphSliceGroupBox->setEnabled(true);
        ui->magnificationGroupBox->setEnabled(false);
        ui->magnificationGroupBox->setChecked(false);
        magnifying = false;
    }

    ui->displayWidget->paintGL();
}

void MainWindow::Zoom()
{
    projector.ChangeProjectorLocation((value - ui->volZoomSlider->value())*10);
    value = ui->volZoomSlider->value();
    ui->displayWidget->paintGL();
}

void MainWindow::Modes()
{
    if(ui->glyphCheckBox->checkState() == Qt::Checked) // glyphs
    {
        ui->glyphTypesGroupBox->setEnabled(true);
        ui->magnificationGroupBox->setEnabled(true);
        vectorField.bVisible = true;
    }
    else
    {
        ui->glyphTypesGroupBox->setEnabled(false);
        ui->magnificationGroupBox->setEnabled(false);
        ui->magnificationGroupBox->setChecked(false);
        magnifying = false;
        vectorField.bVisible = false;
    }

    if(ui->volRendCheckBox->checkState() == Qt::Checked) // volume
        ;

    if(ui->streamCheckBox->checkState() == Qt::Checked) // streamlines
        streamlines.bVisible = true;
    else
        streamlines.bVisible = false;

    if(ui->magnificationGroupBox->isChecked() == true) // magnify
        magnifying = true;
    else
        magnifying = false;

    ui->displayWidget->paintGL();
}

void MainWindow::arrowGlyphSliceClicked()
{
    vectorField.bSliceXVisible = (ui->xSliceCheckBox->checkState() == Qt::Checked);
    vectorField.bSliceYVisible = (ui->ySliceCheckBox->checkState() == Qt::Checked);
    vectorField.bSliceZVisible = (ui->zSliceCheckBox->checkState() == Qt::Checked);

    ui->displayWidget->paintGL();
}

void MainWindow::magZoomChanged(int value)
{
    ui->displayWidget->setMagZoom(defaultMagZoom + value/50.0);
}

void MainWindow::magRadiusChanged(int value)
{
    ui->displayWidget->setMagRadius(defaultMagRadius + value/500.0);
}

float MainWindow::getSliderPct(QSlider *slider, int value)
{
    int sliderMin = slider->minimum();
    int sliderMax = slider->maximum();
    return ((float)sliderMin + value)/sliderMax;
}

void MainWindow::xSlicePosChanged(int value)
{
    int val = (int)(this->getSliderPct(ui->xSlicePosSlider, value) * vectorField.iCols);
    vectorField.SetSliceXNumber(val);

    ui->displayWidget->paintGL();
}

void MainWindow::ySlicePosChanged(int value)
{
    int val = (int)(this->getSliderPct(ui->ySlicePosSlider, value) * vectorField.iRows);
    vectorField.SetSliceYNumber(val);

    ui->displayWidget->paintGL();
}

void MainWindow::zSlicePosChanged(int value)
{
    int val = (int)(this->getSliderPct(ui->zSlicePosSlider, value) * vectorField.iSlices);
    vectorField.SetSliceZNumber(val);

    ui->displayWidget->paintGL();
}

void MainWindow::enableUIWidgets()
{
    ui->interactionModeGroupBox->setEnabled(true);
    ui->volZoomGroupBox->setEnabled(true);
    ui->displayOptionsGroupBox->setEnabled(true);

    // ... and disable the load button
    ui->loadParamsGroupBox->setEnabled(false);
}

void MainWindow::loadParamsHelper(QString fileName)
{
    QStringList splitName = fileName.split("/", QString::SkipEmptyParts);
    ui->loadParamsLineEdit->setText(splitName.at(splitName.size() - 1));

    ui->displayWidget->initDisplay(fileName.toStdString());
    this->enableUIWidgets();

    defaultMagZoom = ui->displayWidget->getMagZoom();
    defaultMagRadius = ui->displayWidget->getMagRadius();
}

void MainWindow::loadParams()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Parameters File"),
                                                    "../../../../data", tr("CSV Files (*.csv);;All Files (*.*)"));
    loadParamsHelper(fileName);
}

void MainWindow::bypassParamLoad()
{
    QString fileName = "/Users/amaries/Desktop/Combustion_Code/data/AbedToTim/1/parameters.csv";
    loadParamsHelper(fileName);
}
