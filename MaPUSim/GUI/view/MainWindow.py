# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import*  
import res.qrc_resources 
from SimuInfoWidget import*
from ARMview.ARMViewWidget import*
from APCview.APCViewWidget import*
from configview.ConfigViewWidget import*
import time
import os

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class MainWindow(QMainWindow):  
    def __init__(self,argv,parent=None):  
        super(MainWindow,self).__init__(parent)   
      	self.argv=argv
	self.path="config.xml"
        self.setWindowTitle('MaPU Simulator')                          
        self.createActions() 
        self.createMenus()  
        self.createToolBars()
	self.createStatusBar()
  
	#self.setMinimumSize(800,620)
	#self.setMaximumSize(1600,840)
	self.resize(800,600)
        self.tabWidget=QTabWidget()   
	self.setCentralWidget(self.tabWidget)  
        self.simuInfoWidget=SimuInfoWidget()
        self.armViewWidget=ARMViewWidget()
        self.apcViewWidget=APCViewWidget()
        self.configControlWidget=ConfigViewWidget()
        #self.tabWidget.addTab(self.simuInfoWidget,self.tr("Simulator Information"))        
        self.tabWidget.addTab(self.armViewWidget,self.tr("ARM Perspective"))
        self.tabWidget.addTab(self.apcViewWidget,self.tr("APC Perspective"))
        self.tabWidget.addTab(self.configControlWidget,self.tr("Configuration and Control")) 
	self.configControlWidget.APCSimulatorDoneSignal.connect(self.apcViewWidget.simulatorDoneSlot)    
	self.configControlWidget.APCSimulatorShowSignal.connect(self.apcViewWidget.statusWidget.simulatorShowText) 
	self.configControlWidget.ARMSimulatorShowSignal.connect(self.armViewWidget.statusWidget.simulatorShowText) 
	self.configControlWidget.ARMUart0StartProcess.connect(self.armViewWidget.UART0Widget.m5termProcessStart)

	#self.readXML()
	
    def createActions(self): 
        self.fileOpenAction=QAction(QIcon(":/open.png"),self.tr("&Open"),self)                                 
        self.fileNewAction=QAction(QIcon(":/new.png"),self.tr("&New"),self)                  
        self.fileSaveAction=QAction(QIcon(":/save.png"),self.tr("&Save"),self)                                     
        self.cutAction=QAction(QIcon(":/cut.png"),self.tr("&Cut"),self)                  
        self.copyAction=QAction(QIcon(":/copy.png"),self.tr("&Copy"),self)    
        self.pasteAction=QAction(QIcon(":/paste.png"),self.tr("&Paste"),self)          
        self.aboutAction=QAction(self.tr("&about"),self) 
	self.setAction=QAction(self.tr("&Simulator path setting..."),self)  
	self.connect(self.setAction,SIGNAL("triggered()"),self.setSimulatorPath)
       
    def createMenus(self):  
        fileMenu=self.menuBar().addMenu(self.tr("File"))    
        fileMenu.addAction(self.fileNewAction)
	fileMenu.addAction(self.fileOpenAction)
     	fileMenu.addAction(self.fileSaveAction)         
        editMenu=self.menuBar().addMenu(self.tr("Edit"))  
	editMenu.addAction(self.copyAction)
	editMenu.addAction(self.cutAction)
	editMenu.addAction(self.pasteAction)  
        setMenu=self.menuBar().addMenu(self.tr("Settings")) 
	setMenu.addAction(self.setAction)       
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
	self.statusLabel=QLabel("StatusBar")
        self.statusBar().addWidget(self.statusLabel)

    def setSimulatorPath(self):
	self.setDialog=QDialog()
	self.setDialog.setWindowTitle("Simulator path setting")
	self.setDialog.setFixedSize(400,200)
	self.pathEdit=QLineEdit(os.environ["MAPU_HOME"]+"/simulator")
	browserButton=QPushButton("Browser")
	self.okButton=QPushButton("OK")
	self.okButton.setFixedSize(100,30)
	self.connect(self.okButton,SIGNAL("clicked()"),self.okButtonSlot)
	self.connect(browserButton,SIGNAL("clicked()"),self.pathSlot)
	lay=QHBoxLayout()
	lay.addWidget(self.pathEdit)
	lay.addWidget(browserButton)
	setLay=QGridLayout()
	setLay.addLayout(lay,0,0,1,3)
	setLay.addWidget(self.okButton,1,1)
	self.setDialog.setLayout(setLay)
	self.setDialog.show()

    def pathSlot(self):
	path=QFileDialog.getExistingDirectory(self,self.tr("select file"),"/")
	self.pathEdit.setText(path)
  
    def okButtonSlot(self):
	if os.path.isdir(str(self.pathEdit.text())):
	    self.configControlWidget.simulatorPath=str(self.pathEdit.text())
	    self.armViewWidget.UART0Widget.embTerminal.simulatorPath=str(self.pathEdit.text())+self.armViewWidget.UART0Widget.embTerminal.simulatorPath	
	    self.setDialog.close()
	else:
	    QMessageBox.warning(self,"Warning","Simulator path is not exist!")
  
    def writeXML(self):
	xmlfile=QFile(self.path)
	if xmlfile.open(QIODevice.WriteOnly or QIODevice.Text):
	    xmlWrite=QXmlStreamWriter(xmlfile)
	    xmlWrite.setAutoFormatting(True)
	    xmlWrite.writeStartDocument()
	    xmlWrite.writeStartElement("UserSet")
            xmlWrite.writeTextElement("simulatorpath",self.configControlWidget.simulatorPath)
	    xmlWrite.writeStartElement("Fullsystem")	
            xmlWrite.writeTextElement("fulltrace",self.configControlWidget.fullTracefile.text())
            xmlWrite.writeTextElement("image",self.configControlWidget.fullEdit.text())
	    xmlWrite.writeEndElement()#Fullsystem
	    xmlWrite.writeStartElement("APCsystem")	
            xmlWrite.writeTextElement("apctrace",self.configControlWidget.traceFileEdit.text())
	    xmlWrite.writeStartElement("APE0")
            xmlWrite.writeTextElement("APE0SPU",self.configControlWidget.APE0SPUEdit.text())
            xmlWrite.writeTextElement("APE0MPU",self.configControlWidget.APE0MPUEdit.text())
	    xmlWrite.writeEndElement() #APE0
	    xmlWrite.writeEndElement() #APCsystem
	    xmlWrite.writeEndElement() #UserSet
	    xmlWrite.writeEndDocument()

    def readXML(self):
	xmlfile=QFile(self.path)
	xmlType=QXmlStreamReader.TokenType()
	if xmlfile.open(QIODevice.ReadOnly or QIODevice.Text):	
	    xmlRead=QXmlStreamReader(xmlfile)
	    while not xmlRead.atEnd():
		xmlType=xmlRead.readNext()
		if xmlType==QXmlStreamReader.StartElement:
		    if xmlRead.name()=="UserSet":
			while not xmlRead.atEnd():
			    xmlType=xmlRead.readNext()
			    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				print "simulator",xmlRead.text().toString()
			    if xmlType==QXmlStreamReader.EndElement:
                            	break
		    elif xmlRead.name()=="Fullsystem":
			while not xmlRead.atEnd():
			    xmlType=xmlRead.readNext()
		            if xmlRead.name()=="fulltrace":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        print "fulltrace",xmlRead.text().toString()
			            if xmlType==QXmlStreamReader.EndElement:
					if xmlRead.name()=="fulltrace":
                            	            break
		            elif xmlRead.name()=="image":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        print "image",xmlRead.text().toString()
			            if xmlType==QXmlStreamReader.EndElement:
					if xmlRead.name()=="image":
                            	            break
		            elif xmlRead.name()=="APCsystem":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        print "APCsystem",xmlRead.text().toString()
			            if xmlType==QXmlStreamReader.EndElement:
                            	        break
		            elif xmlRead.name()=="APE0":
			        while not xmlRead.atEnd():
		                    if xmlRead.name()=="APE0SPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				            print "APE0SPU",xmlRead.text().toString()
			                if xmlType==QXmlStreamReader.EndElement:
					     if xmlRead.name()=="APE0SPU":
                            	                 break
		                    elif xmlRead.name()=="APE0MPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                 print "APE0MPU",xmlRead.text().toString()
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE0MPU":
                            	                      break
	xmlfile.close()
    
    def closeEvent(self,event):
	#self.writeXML()
	self.configControlWidget.stopProcessExit(1)
	self.apcViewWidget.APE0Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE1Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE2Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE3Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE0Widget.SPUWidget.closeChildDialog() 
	self.apcViewWidget.APE1Widget.SPUWidget.closeChildDialog() 
	self.apcViewWidget.APE2Widget.SPUWidget.closeChildDialog() 
	self.apcViewWidget.APE3Widget.SPUWidget.closeChildDialog() 



