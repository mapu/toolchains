# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import* 
import sys
sys.path.append("../..")
from control.DataBase import* 
from APCMutiCoreWidget import*
from APCSigCoreWidget import*
from DataBaseDialog import*
from StatusWidget import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class APCViewWidget(QWidget):  
    def __init__(self,parent=None):
	super(APCViewWidget,self).__init__(parent)

        self.APCWidget=APCMutiCoreWidget(self)  #APC mutiple core view
        self.APE0Widget=APCSigCoreWidget()
        self.APE1Widget=APCSigCoreWidget()
        self.APE2Widget=APCSigCoreWidget()
        self.APE3Widget=APCSigCoreWidget() 
	self.statusWidget=StatusWidget()
     
	#define left tab
        self.leftTab=QTabWidget()
        self.leftTab.addTab(self.APCWidget,self.tr("APC"))
        self.leftTab.addTab(self.APE0Widget,self.tr("APE0"))         
        self.leftTab.addTab(self.APE1Widget,self.tr("APE1"))
        self.leftTab.addTab(self.APE2Widget,self.tr("APE2"))
        self.leftTab.addTab(self.APE3Widget,self.tr("APE3"))  
	self.leftTab.addTab(self.statusWidget,self.tr("Status"))

        self.connect(self.APCWidget.APE0Button,SIGNAL("clicked()"),self.slotAPE0Widget)
        self.connect(self.APCWidget.APE1Button,SIGNAL("clicked()"),self.slotAPE1Widget)
        self.connect(self.APCWidget.APE2Button,SIGNAL("clicked()"),self.slotAPE2Widget)
        self.connect(self.APCWidget.APE3Button,SIGNAL("clicked()"),self.slotAPE3Widget) 

	#lay out all widget  
        self.slider = QSlider(Qt.Horizontal)
        self.slider.setTickPosition(QSlider.TicksBothSides)
	self.slider.setMaximumHeight(30)
	self.slider.setEnabled(False)    
	self.spinBox=QSpinBox()
	self.spinBox.setMaximumHeight(40)
	self.spinBox.setEnabled(False)  
	bottomLay=QHBoxLayout()
	bottomLay.addWidget(self.slider)
	bottomLay.addWidget(self.spinBox)
	bottomLay.setStretchFactor(self.slider,7)
	bottomLay.setStretchFactor(self.spinBox,1)    
        self.mainLayout=QVBoxLayout()
        self.mainLayout.addWidget(self.leftTab)
        self.mainLayout.addLayout(bottomLay)
        self.setLayout(self.mainLayout)  

 	#define slot function
    def slotAPE0Widget(self):
        self.leftTab.setCurrentWidget(self.APE0Widget)  

    def slotAPE1Widget(self):
        self.leftTab.setCurrentWidget(self.APE1Widget) 

    def slotAPE2Widget(self):
        self.leftTab.setCurrentWidget(self.APE2Widget) 

    def slotAPE3Widget(self):
        self.leftTab.setCurrentWidget(self.APE3Widget)

    def simulatorDoneSlot(self):
	#simulator exit normal,create data base
	#show dialog and show data base is building
	self.dataBaseDialog=DataBaseDialog()
	self.dataBaseDialog.show()
	self.dataBase=self.dataBaseDialog.createDataBase()
	#set the range
	self.slider.setEnabled(True)
	self.spinBox.setEnabled(True)
	#read file content, get min and max time
	f=open(self.dataBase.filePath,"r")
        lines=f.readlines()
	line=lines[0]
	pos=line.index("[")
	self.minTime=int(line[:pos])/1000
	count=len(lines)
	line=lines[count-1]
	pos=line.index("[")
	self.maxTime=int(line[:pos])/1000
	f.close()
	#update MPU stage dialog
	self.APE0Widget.MPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"m")  
	self.APE1Widget.MPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"m")  
	self.APE2Widget.MPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"m")  
	self.APE3Widget.MPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"m") 
	#update SPU stage dialog
	self.APE0Widget.SPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"s")  
	self.APE1Widget.SPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"s")  
	self.APE2Widget.SPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"s")  
	self.APE3Widget.SPUWidget.stageDialog.updateDialog(self.dataBase,self.minTime,self.maxTime,"s") 
	#set slider min and max value
	self.slider.setRange(self.minTime,self.maxTime)
	self.spinBox.setRange(self.minTime,self.maxTime)
	#add slot for slider,if silder's value change, then response
	self.connect(self.slider,SIGNAL("valueChanged(int)"),self.currentValueSlot)
	self.connect(self.slider,SIGNAL("valueChanged(int)"),self.spinBox.setValue)
	self.connect(self.spinBox,SIGNAL("valueChanged(int)"),self.slider.setValue)
	self.dataBaseDialog.close()

    def currentValueSlot(self,time):
	curTime=self.slider.value()

	fetchall_sql="SELECT * FROM "+self.dataBase.timeTableName+" WHERE time = "+str(curTime)
	r=self.dataBase.fetchall(self.dataBase.timeFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		self.APE0Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE1Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE2Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE3Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE0Widget.SPUWidget.updateSPUWidget(r[e])
		self.APE1Widget.SPUWidget.updateSPUWidget(r[e])
		self.APE2Widget.SPUWidget.updateSPUWidget(r[e])
		self.APE3Widget.SPUWidget.updateSPUWidget(r[e])
	
    
