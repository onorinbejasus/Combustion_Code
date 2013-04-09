 #include <QtOpenGL/qgl.h>
 #include <QtGui/qwidget.h>
 #include "mainwindow.h"

 MainWindow::MainWindow()
 {
     splitter = new QSplitter;
     setCentralWidget(splitter);
	 splitter->setOrientation(Qt::Vertical);

	 /* create tensor widgets */
	 glWidget1 = new Tensor1(BIG_WINDOW_SIZE, BIG_WINDOW_SIZE, 0);	 
     glWidget2 = new Tensor1(BIG_WINDOW_SIZE, BIG_WINDOW_SIZE, 0);	 

	 /* create small multiple widgets */
	 smWidget1 = new Tensor1(SM_WINDOW_SIZE, SM_WINDOW_SIZE, 1);
	 smWidget2 = new Tensor1(SM_WINDOW_SIZE, SM_WINDOW_SIZE, 1);
	 smWidget3 = new Tensor1(SM_WINDOW_SIZE, SM_WINDOW_SIZE,1);
	 smWidget4 = new Tensor1(SM_WINDOW_SIZE, SM_WINDOW_SIZE,1);
	 smWidget5 = new Tensor1(SM_WINDOW_SIZE, SM_WINDOW_SIZE,1);

	/* create areas to hold widgets */
     glWidgetArea1 = new QScrollArea;
     glWidgetArea1->setWidget(glWidget1);
	 glWidgetArea1->setFixedSize(BIG_WINDOW_SIZE,BIG_WINDOW_SIZE);	 
     glWidgetArea1->setWidgetResizable(true);
     glWidgetArea1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);


     glWidgetArea2 = new QScrollArea;
     glWidgetArea2->setWidget(glWidget2);
	 glWidgetArea2->setFixedSize(BIG_WINDOW_SIZE,BIG_WINDOW_SIZE);
     glWidgetArea2->setWidgetResizable(true);
     glWidgetArea2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	 smWidgetArea1 = new QScrollArea;
     smWidgetArea1->setWidget(smWidget1);
	 smWidgetArea1->setFixedSize(SM_WINDOW_SIZE,SM_WINDOW_SIZE);    
	 smWidgetArea1->setWidgetResizable(true);
     smWidgetArea1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
 
	 smWidgetArea2 = new QScrollArea;
	 smWidgetArea2->setWidget(smWidget2);
	 smWidgetArea2->setFixedSize(SM_WINDOW_SIZE,SM_WINDOW_SIZE); 
     smWidgetArea2->setWidgetResizable(true);
     smWidgetArea2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
 
	 smWidgetArea3 = new QScrollArea;
     smWidgetArea3->setWidget(smWidget3);
	 smWidgetArea3->setFixedSize(SM_WINDOW_SIZE,SM_WINDOW_SIZE); 
	 smWidgetArea3->setWidgetResizable(true);
     smWidgetArea3->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	 smWidgetArea4 = new QScrollArea;
	 smWidgetArea4->setWidget(smWidget4);
	 smWidgetArea4->setFixedSize(SM_WINDOW_SIZE,SM_WINDOW_SIZE); 
	 smWidgetArea4->setWidgetResizable(true);
	 smWidgetArea4->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   
	smWidgetArea5 = new QScrollArea;
	smWidgetArea5->setWidget(smWidget5);
	smWidgetArea5->setFixedSize(SM_WINDOW_SIZE,SM_WINDOW_SIZE); 
	smWidgetArea5->setWidgetResizable(true);
	smWidgetArea5->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	 
	createActions();
    createMenus();

	/* create layouts */
    QGridLayout *topLayout = new QGridLayout;
	QGridLayout *smLayout = new QGridLayout;

	/* add GL windows to GUI */
	/* add widgets to layouts */

	topLayout->addWidget(glWidgetArea1, 0, 0);
    topLayout->addWidget(glWidgetArea2, 0, 1);

	smLayout->addWidget(smWidgetArea1, 0, 0);
	smLayout->addWidget(smWidgetArea2, 0, 1);
	smLayout->addWidget(smWidgetArea3, 0, 2);
	smLayout->addWidget(smWidgetArea4, 0, 3);
	smLayout->addWidget(smWidgetArea5, 0, 4);

	/* Widgets for layouts */

	QWidget *top = new QWidget;
	QWidget *bottom = new QWidget;

	top->setLayout(topLayout);
	bottom->setLayout(smLayout);

	 /* add layouts to main widget */

	splitter->addWidget(top);
	splitter->addWidget(bottom);

	QList <int> size;

	size.append(BIG_WINDOW_SIZE);
	size.append(SM_WINDOW_SIZE);

	splitter->setSizes(size);

    setWindowTitle(tr("Tensor Visualizer"));
 }


void MainWindow::open(){

	/* get the file name */
	 QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
	 char topDirectory[MAX_PATH] ;
	 strcpy( topDirectory, dir.toLatin1().data());

	 char paramFile[MAX_PATH] ;
	 strcpy(paramFile, topDirectory);
	 strcat(paramFile, "\\1\\parameters.csv");

	 char paramFilePath[MAX_PATH] ;
	 strcpy(paramFilePath, topDirectory) ;
	 strcat(paramFilePath, "\\1\\") ;
	 // if the file name isn's empty, pass it to be opened 
	 if(!dir.isEmpty() ) {
 		QFile *file1 = new QFile(); 
		if(file1->exists(QString(paramFile))){
			//read the parameter file
			params1.LoadParameters(paramFile, paramFilePath);	

			//read the tensor data
			tensorData1.SetDirPath(paramFilePath);
			tensorData1.Configure(&params1);
			tensorData1.readData();

			// connect the data and the widget
			glWidget1->tensorDataSet = &tensorData1 ;
			// update the projector of the widget
			glWidget1->ReconfigureProjector(&params1); 
			// update the actors. 
			glWidget1->UpdateRenderers(); 
			// redraw the 
			glWidget1->updateGL();
		 }
	 }

/*	 strcpy(paramFile, topDirectory);
	 strcat(paramFile, "\\2\\parameters.csv");

	 strcpy(paramFilePath, topDirectory) ;
	 strcat(paramFilePath, "\\2\\") ;
	 //if the file name isn's empty, pass it to be opened 
	 if(!dir.isEmpty() ) {
 		QFile *file2 = new QFile(); 
		if(file2->exists(QString(paramFile))){
			//read the parameter file
			params2.LoadParameters(paramFile, paramFilePath);	

			//read the tensor data
			tensorData2.SetDirPath(paramFilePath);
			tensorData2.Configure(&params2);
			tensorData2.readData();

			// connect the data and the widget
			smWidget2->tensorDataSet = &tensorData2 ;
			// update the projector of the widget
			smWidget2->ReconfigureProjector(&params2); 
			// update the actors. 
			smWidget2->UpdateRenderers(); 
			// redraw the 
			smWidget2->updateGL();
		 }
	 }

	 
	 strcpy(paramFile, topDirectory);
	 strcat(paramFile, "\\3\\parameters.csv");

	 strcpy(paramFilePath, topDirectory) ;
	 strcat(paramFilePath, "\\3\\") ;
	 //if the file name isn's empty, pass it to be opened 
	 if(!dir.isEmpty() ) {
 		QFile *file3 = new QFile(); 
		if(file3->exists(QString(paramFile))){
			//read the parameter file
			params3.LoadParameters(paramFile, paramFilePath);	

			//read the tensor data
			tensorData3.SetDirPath(paramFilePath);
			tensorData3.Configure(&params3);
			tensorData3.readData();

			// connect the data and the widget
			smWidget3->tensorDataSet = &tensorData3 ;
			// update the projector of the widget
			smWidget3->ReconfigureProjector(&params3); 
			// update the actors. 
			smWidget3->UpdateRenderers(); 
			// redraw the 
			smWidget3->updateGL();
		 }
	 } */
	 repaint(); 	 
 }

 void MainWindow::createActions()
 {
     exitAct = new QAction(tr("E&xit"), this);
     exitAct->setShortcuts(QKeySequence::Quit);
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

     openAct = new QAction(tr("&Open"), this);
     connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	 connect(smWidget1, SIGNAL(setRightWindow(Tensor1 *)), 
						SLOT(right_window(Tensor1 *)));
	 connect(smWidget1, SIGNAL(setLeftWindow(Tensor1 *)), 
						SLOT(left_window(Tensor1 *)));
	 
	 connect(smWidget2, SIGNAL(setRightWindow(Tensor1 *)), 
						SLOT(right_window(Tensor1 *)));
	 connect(smWidget2, SIGNAL(setLeftWindow(Tensor1 *)), 
						SLOT(left_window(Tensor1 *)));
	 
	 connect(smWidget3, SIGNAL(setRightWindow(Tensor1 *)), 
						SLOT(right_window(Tensor1 *)));
	 connect(smWidget3, SIGNAL(setLeftWindow(Tensor1 *)), 
						SLOT(left_window(Tensor1 *)));
	 
 }

 void MainWindow::createMenus()
 {
     fileMenu = menuBar()->addMenu(tr("&File"));
     fileMenu->addSeparator();
	 fileMenu->addAction(openAct);	
	 fileMenu->addAction(exitAct);
	
 }

/* This funciton is connected to the smMultiple windows.
When r or R is clicked, a signal is emitted, and will 
open the right window to the tensor chosen */

 void MainWindow::left_window(Tensor1 *srcTensor){
	//connect the data and the widget
	 glWidget1->tensorDataSet = srcTensor->tensorDataSet ;
	//update the actors. 
	glWidget1->UpdateRenderers(srcTensor); 
	//update the projector of the widget
	glWidget1->ReconfigureProjector(srcTensor->projector); 
	//redraw me 
	glWidget1->updateGL();	
 }

  void MainWindow::right_window(Tensor1 *srcTensor){
	//connect the data and the widget
	glWidget2->tensorDataSet = srcTensor->tensorDataSet ;
	//update the actors. 
	glWidget2->UpdateRenderers(srcTensor); 
	//update the projector of the widget
	glWidget2->ReconfigureProjector(srcTensor->projector); 
	//redraw me 
	glWidget2->updateGL();	
 }

