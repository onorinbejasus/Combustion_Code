 #ifndef MAINWINDOW_H
 #define MAINWINDOW_H

#include <QTGui/QMainWindow.h>
#include "Headers.h"
#include "Window1.h"
#include "Parameters.h"
#include "TensorData.h"
#include "Projector.h"

 class QAction;
 class QLabel;
 class QMenu;
 class QScrollArea;
 class QSlider;
 class QPixmap;
 class QMessageBox;

 class Tensor1; /* tensor class */

#define BIG_WINDOW_SIZE		400
#define SM_WINDOW_SIZE		200

 class MainWindow : public QMainWindow
 {
     Q_OBJECT

 public:
     MainWindow();
	 Parameters params1 ;							// parameter file containing all information about the dataset
	 Parameters params2 ;
	 Parameters params3 ;

	 TensorData tensorData1 ;
	 TensorData tensorData2 ;
	 TensorData tensorData3 ;

 public slots:
	 void open();
	 

 private slots:     
	 void left_window(Tensor1 *tensor);
	 void right_window(Tensor1 *tensor);

 private:
     void createActions();
     void createMenus();

     QSplitter *splitter;
     
	 QScrollArea *glWidgetArea1;
     QScrollArea *glWidgetArea2;
    
	 /* main windows */

	 QScrollArea *smWidgetArea1, *smWidgetArea2, *smWidgetArea3, *smWidgetArea4, *smWidgetArea5;

	 /* File Menus */
     QMenu *fileMenu;;
	 
	 QAction *exitAct;
     QAction *openAct;
  
 };

 #endif