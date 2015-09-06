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
import sys

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
	self.apcViewWidget.updateWidgetSignal.connect(self.updateWidgetShowStaus,Qt.DirectConnection)
	self.simulatorPath=""
	self.readXML()
	self.tabWidget.setCurrentIndex(2)
	image=0
	trace=0
	if len(self.argv)==3:
	    if self.argv[1][:7]=="--image":
		image=self.argv[1][8:len(self.argv[1])]
		if os.path.exists(image)<=0:
		    return
		else:
		    self.configControlWidget.fullEdit.setText(image)		
	    if self.argv[2][:7]=="--trace":
		trace=self.argv[2][8:len(self.argv[2])]
		self.configControlWidget.fullTracefile.setText(trace)
	if image!=0 and trace!=0:
	    #start full system and skip ARM Perspective
	    self.configControlWidget.simulatorPath=os.environ["MAPU_HOME"]+"/simulator"
	    self.configControlWidget.fullGroup.setChecked(True)
	    self.configControlWidget.APCGroup.setChecked(False)
	    self.configControlWidget.startProcess()
	
    def createActions(self): 
        self.fileOpenAction=QAction(QIcon(":/open.png"),self.tr("&Open"),self)   
	self.connect(self.fileOpenAction,SIGNAL("triggered()"),self.fileOpenSlot)                            
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
	self.statusLabel=QLabel()
	self.statusText="StatusBar"
	self.statusLabel.setText(self.statusText)	
        self.statusBar().addWidget(self.statusLabel)

    def updateWidgetShowStaus(self,string):
	self.statusText=string
	self.statusLabel.setText(self.statusText)

    def fileOpenSlot(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.apcViewWidget.mainOpen=1
	self.apcViewWidget.simulatorDoneSlot(4,path)

    def setSimulatorWidget(self):
	self.setDialog=QDialog()
	self.setDialog.setWindowTitle("Simulator path setting")
	self.setDialog.setFixedSize(400,200)
	if self.simulatorPath!="":
	    self.pathEdit=QLineEdit(self.simulatorPath)
	else:
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

    def setSimulatorPath(self):
	self.setSimulatorWidget()
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
	    if self.configControlWidget.fullGroup.isChecked()==True:
		xmlWrite.writeTextElement("fullcheck","1")	
	    else:
		xmlWrite.writeTextElement("fullcheck","0")
            xmlWrite.writeTextElement("fulltrace",self.configControlWidget.fullTracefile.text())
            xmlWrite.writeTextElement("image",self.configControlWidget.fullEdit.text())
	    xmlWrite.writeEndElement()#Fullsystem
	    xmlWrite.writeStartElement("APCsystem")
	    if self.configControlWidget.APCGroup.isChecked()==True:
		xmlWrite.writeTextElement("apccheck","1")	
	    else:
		xmlWrite.writeTextElement("apccheck","0")	
            xmlWrite.writeTextElement("apctrace",self.configControlWidget.traceFileEdit.text())
	    xmlWrite.writeStartElement("APE0")
            xmlWrite.writeTextElement("APE0SPU",self.configControlWidget.APE0SPUEdit.text())
            xmlWrite.writeTextElement("APE0MPU",self.configControlWidget.APE0MPUEdit.text())
	    xmlWrite.writeEndElement() #APE0
	    xmlWrite.writeStartElement("APE1")
	    if self.configControlWidget.APE1Check.isChecked()==True:
		xmlWrite.writeTextElement("APE1check","1")
	    else:
		xmlWrite.writeTextElement("APE1check","0")
            xmlWrite.writeTextElement("APE1SPU",self.configControlWidget.APE1SPUEdit.text())
            xmlWrite.writeTextElement("APE1MPU",self.configControlWidget.APE1MPUEdit.text())
	    xmlWrite.writeEndElement() #APE1
	    xmlWrite.writeStartElement("APE2")
	    if self.configControlWidget.APE2Check.isChecked()==True:
		xmlWrite.writeTextElement("APE2check","1")
	    else:
		xmlWrite.writeTextElement("APE2check","0")
            xmlWrite.writeTextElement("APE2SPU",self.configControlWidget.APE2SPUEdit.text())
            xmlWrite.writeTextElement("APE2MPU",self.configControlWidget.APE2MPUEdit.text())
	    xmlWrite.writeEndElement() #APE2
	    xmlWrite.writeStartElement("APE3")
	    if self.configControlWidget.APE3Check.isChecked()==True:
		xmlWrite.writeTextElement("APE3check","1")
	    else:
		xmlWrite.writeTextElement("APE3check","0")
            xmlWrite.writeTextElement("APE3SPU",self.configControlWidget.APE3SPUEdit.text())
            xmlWrite.writeTextElement("APE3MPU",self.configControlWidget.APE3MPUEdit.text())
	    xmlWrite.writeEndElement() #APE3
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
		    if xmlRead.name()=="simulatorpath":
			while not xmlRead.atEnd():
			    xmlType=xmlRead.readNext()
			    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				self.simulatorPath=xmlRead.text().toString()
			    if xmlType==QXmlStreamReader.EndElement:
                                if xmlRead.name()=="simulatorpath":
                                    break
		    elif xmlRead.name()=="Fullsystem":
			while not xmlRead.atEnd():
			    xmlType=xmlRead.readNext()
			    if xmlRead.name()=="fullcheck":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        if xmlRead.text().toString()=="1":
					    self.configControlWidget.fullGroup.setChecked(True)
					else:
					    self.configControlWidget.fullGroup.setChecked(False)  
			            if xmlType==QXmlStreamReader.EndElement:
					if xmlRead.name()=="fullcheck":
                            	            break
		            elif xmlRead.name()=="fulltrace":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        self.configControlWidget.fullTracefile.setText(xmlRead.text().toString())
			            if xmlType==QXmlStreamReader.EndElement:
					if xmlRead.name()=="fulltrace":
                            	            break
		            elif xmlRead.name()=="image":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        self.configControlWidget.fullEdit.setText(xmlRead.text().toString())
			            if xmlType==QXmlStreamReader.EndElement:
					if xmlRead.name()=="image":
                            	            break
			    if xmlType==QXmlStreamReader.EndElement:
                                if xmlRead.name()=="Fullsystem":
                                    break
		    elif xmlRead.name()=="APCsystem":
			while not xmlRead.atEnd():
			    xmlType=xmlRead.readNext()
			    if xmlRead.name()=="apccheck":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        if xmlRead.text().toString()=="1":
					    self.configControlWidget.APCGroup.setChecked(True)  
					else:
					    self.configControlWidget.APCGroup.setChecked(False)  
			            if xmlType==QXmlStreamReader.EndElement:
					if xmlRead.name()=="apccheck":
                            	            break
		            elif xmlRead.name()=="apctrace":
			        while not xmlRead.atEnd():
			            xmlType=xmlRead.readNext()
			            if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				        self.configControlWidget.traceFileEdit.setText(xmlRead.text().toString())
			            if xmlType==QXmlStreamReader.EndElement:
					 if xmlRead.name()=="apctrace":
                            	             break
		            elif xmlRead.name()=="APE0":
			        while not xmlRead.atEnd():
				    xmlType=xmlRead.readNext()
		                    if xmlRead.name()=="APE0SPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                self.configControlWidget.APE0SPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE0SPU":
                            	                     break
		                    elif xmlRead.name()=="APE0MPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                 self.configControlWidget.APE0MPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE0MPU":
                            	                      break
			            if xmlType==QXmlStreamReader.EndElement:
                                        if xmlRead.name()=="APE0":
                                            break
		            elif xmlRead.name()=="APE1":
			        while not xmlRead.atEnd():
				    xmlType=xmlRead.readNext()
		                    if xmlRead.name()=="APE1check":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                if xmlRead.text().toString()=="1":
						    self.configControlWidget.APE1Check.setCheckState(Qt.Checked)
						else:
						    self.configControlWidget.APE1Check.setCheckState(Qt.Unchecked)							   
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE1check":
                            	                     break
		                    elif xmlRead.name()=="APE1SPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                self.configControlWidget.APE1SPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE1SPU":
                            	                     break
		                    elif xmlRead.name()=="APE1MPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                 self.configControlWidget.APE1MPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE1MPU":
                            	                      break
			            if xmlType==QXmlStreamReader.EndElement:
                                        if xmlRead.name()=="APE1":
                                            break
		            elif xmlRead.name()=="APE2":
			        while not xmlRead.atEnd():
				    xmlType=xmlRead.readNext()
		                    if xmlRead.name()=="APE2check":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                if xmlRead.text().toString()=="1":
						    self.configControlWidget.APE2Check.setCheckState(Qt.Checked)
						else:
						    self.configControlWidget.APE2Check.setCheckState(Qt.Unchecked)
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE2check":
                            	                     break
		                    elif xmlRead.name()=="APE2SPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                self.configControlWidget.APE2SPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE2SPU":
                            	                     break
		                    elif xmlRead.name()=="APE2MPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                 self.configControlWidget.APE2MPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE2MPU":
                            	                      break
			            if xmlType==QXmlStreamReader.EndElement:
                                        if xmlRead.name()=="APE2":
                                            break
		            elif xmlRead.name()=="APE3":
			        while not xmlRead.atEnd():
				    xmlType=xmlRead.readNext()
		                    if xmlRead.name()=="APE3check":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                if xmlRead.text().toString()=="1":
						    self.configControlWidget.APE3Check.setCheckState(Qt.Checked)
						else:
						    self.configControlWidget.APE3Check.setCheckState(Qt.Unchecked)
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE3check":
                            	                     break
		                    elif xmlRead.name()=="APE3SPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                self.configControlWidget.APE3SPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE3SPU":
                            	                     break
		                    elif xmlRead.name()=="APE3MPU":
			                while not xmlRead.atEnd():
			                    xmlType=xmlRead.readNext()
			                    if xmlType == QXmlStreamReader.Characters and not xmlRead.isWhitespace():
				                 self.configControlWidget.APE3MPUEdit.setText(xmlRead.text().toString())
			                    if xmlType==QXmlStreamReader.EndElement:
					         if xmlRead.name()=="APE3MPU":
                            	                      break
			            if xmlType==QXmlStreamReader.EndElement:
                                        if xmlRead.name()=="APE3":
                                            break
			    if xmlType==QXmlStreamReader.EndElement:
                                if xmlRead.name()=="APCsystem":
                                    break
	xmlfile.close()
	self.configControlWidget.fullCheckedSlot()
	self.configControlWidget.APCCheckedSlot()
 
    def closeEvent(self,event):
	self.writeXML()
	self.configControlWidget.stopProcessExit(1)
	self.apcViewWidget.APE0Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE1Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE2Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE3Widget.MPUWidget.closeChildDialog() 
	self.apcViewWidget.APE0Widget.SPUWidget.closeChildDialog() 
	self.apcViewWidget.APE1Widget.SPUWidget.closeChildDialog() 
	self.apcViewWidget.APE2Widget.SPUWidget.closeChildDialog() 
	self.apcViewWidget.APE3Widget.SPUWidget.closeChildDialog() 



