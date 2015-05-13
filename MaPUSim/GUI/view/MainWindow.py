# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import*  
import res.qrc_resources 
from SimuInfoWidget import*
from ARMViewWidget import*
from APCview.APCViewWidget import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class MainWindow(QMainWindow):  
    def __init__(self,parent=None):  
        super(MainWindow,self).__init__(parent)   
      
        self.setWindowTitle('MaPU Simulator')                          
        self.createActions() 
        self.createMenus()  
        self.createToolBars()
	self.createStatusBar()
  
	self.resize(750,500)

        self.tabWidget=QTabWidget()   
	self.setCentralWidget(self.tabWidget)  
        self.simuInfoWidget=SimuInfoWidget(self)
        self.armViewWidget=ARMViewWidget(self)
        self.apcViewWidget=APCViewWidget(self)
        self.setControlWidget=QWidget()
        self.tabWidget.addTab(self.simuInfoWidget,self.tr("Simulator Information"))        
        self.tabWidget.addTab(self.armViewWidget,self.tr("ARM View"))
        self.tabWidget.addTab(self.apcViewWidget,self.tr("APC View"))
        self.tabWidget.addTab(self.setControlWidget,self.tr("configuration and Control"))       
     
    def createActions(self): 
        self.fileOpenAction=QAction(QIcon(":/open.png"),self.tr("Open"),self)                                 
        self.fileNewAction=QAction(QIcon(":/new.png"),self.tr("New"),self)                  
        self.fileSaveAction=QAction(QIcon(":/save.png"),self.tr("Save"),self)                                     
        self.cutAction=QAction(QIcon(":/cut.png"),self.tr("Cut"),self)                  
        self.copyAction=QAction(QIcon(":/copy.png"),self.tr("Copy"),self)    
        self.pasteAction=QAction(QIcon(":/paste.png"),self.tr("Paste"),self)          
        self.aboutAction=QAction(self.tr("&about"),self)   
       
    def createMenus(self):  
        fileMenu=self.menuBar().addMenu(self.tr("File"))    
        fileMenu.addAction(self.fileNewAction)
	fileMenu.addAction(self.fileOpenAction)
     	fileMenu.addAction(self.fileSaveAction)         
        editMenu=self.menuBar().addMenu(self.tr("Edit"))  
	editMenu.addAction(self.copyAction)
	editMenu.addAction(self.cutAction)
	editMenu.addAction(self.pasteAction)       
        aboutMenu=self.menuBar().addMenu(self.tr("Help")) 
	aboutMenu.addAction(self.aboutAction)  
        
    def createToolBars(self):  
        fileToolBar=self.addToolBar("File")    
        fileToolBar.addAction(self.fileNewAction)  
        fileToolBar.addAction(self.fileOpenAction)  
        fileToolBar.addAction(self.fileSaveAction) 
        editTool=self.addToolBar("Edit")  
        editTool.addAction(self.copyAction)  
        editTool.addAction(self.cutAction)  
        editTool.addAction(self.pasteAction)  
       
    def createStatusBar(self):    
        self.statusBar().showMessage(self.tr("Statusbar"))    
        
    
        

  
