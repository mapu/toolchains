# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import*  
import res.qrc_resources 
from SimuInfoWidget import*
from ARMview.ARMViewWidget import*
from APCview.APCViewWidget import*
from configview.ConfigViewWidget import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class MainWindow(QMainWindow):  
    threadSignal=pyqtSignal()
    def __init__(self,parent=None):  
        super(MainWindow,self).__init__(parent)   
      
        self.setWindowTitle('MaPU Simulator')                          
        self.createActions() 
        self.createMenus()  
        self.createToolBars()
	self.createStatusBar()
  
	self.setMinimumSize(1400,820)

        self.tabWidget=QTabWidget()   
	self.setCentralWidget(self.tabWidget)  
        self.simuInfoWidget=SimuInfoWidget()
        self.armViewWidget=ARMViewWidget()
        self.apcViewWidget=APCViewWidget()
        self.configControlWidget=ConfigViewWidget()
        self.tabWidget.addTab(self.simuInfoWidget,self.tr("Simulator Information"))        
        self.tabWidget.addTab(self.armViewWidget,self.tr("ARM Perspective"))
        self.tabWidget.addTab(self.apcViewWidget,self.tr("APC Perspective"))
        self.tabWidget.addTab(self.configControlWidget,self.tr("Configuration and Control")) 

	self.configControlWidget.simulatorDoneSignal.connect(self.apcViewWidget.simulatorDoneSlot)    
	self.configControlWidget.simulatorShowSignal.connect(self.apcViewWidget.statusWidget.simulatorShowText) 
	 
	
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
        
    def closeEvent(self,event):
	self.apcViewWidget.APE0Widget.MPUWidget.buttonWidget.closeFloatDialogs() 
	self.apcViewWidget.APE1Widget.MPUWidget.buttonWidget.closeFloatDialogs() 
	self.apcViewWidget.APE2Widget.MPUWidget.buttonWidget.closeFloatDialogs() 
	self.apcViewWidget.APE3Widget.MPUWidget.buttonWidget.closeFloatDialogs() 
        
