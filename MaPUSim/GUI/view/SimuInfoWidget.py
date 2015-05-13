# -*- coding: utf-8 -*-  
from PyQt4.QtGui import *  
from PyQt4.QtCore import *  

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class SimuInfoWidget(QWidget):  
    def __init__(self,parent=None):  
        super(SimuInfoWidget,self).__init__(parent) 

        self.SoCLabel=QLabel("SoC") 
        self.ARMLabel=QLabel("ARM")
        self.APCLabel=QLabel("APC")
        self.SoCFreshButton=QPushButton(self.tr("Fresh"))   
        self.ARMFreshButton=QPushButton(self.tr("Fresh")) 
        self.APCFreshButton=QPushButton(self.tr("Fresh")) 
        self.SoCText=QTextEdit()
	self.SoCText.append(self.tr("Configuration:"))
	self.SoCText.append(self.tr("UART0        0X50C00000"))
	self.SoCText.append(self.tr("..."))
        self.ARMText=QTextEdit()
	self.ARMText.append(self.tr("Configuration:"))
	self.ARMText.append(self.tr("I-Cache         32KB"))
	self.ARMText.append(self.tr("D-Cache       64KB"))
	self.ARMText.append(self.tr("..."))
        self.APCText=QTextEdit()
	self.APCText.append(self.tr("Configuration:"))
	self.APCText.append(self.tr("Number of A PEs        4"))
	self.APCText.append(self.tr("APE0"))
	self.APCText.append(self.tr("IM            128KB"))
	self.APCText.append(self.tr("DM          1.5MB"))
	self.APCText.append(self.tr("..."))

        self.layout1=QHBoxLayout()  
        self.layout1.setMargin(15)          
        self.layout1.addWidget(self.SoCLabel) 
        self.layout1.addWidget(self.SoCFreshButton)       
        self.layout1.addWidget(self.ARMLabel)  
        self.layout1.addWidget(self.ARMFreshButton)  
        self.layout1.addWidget(self.APCLabel)  
        self.layout1.addWidget(self.APCFreshButton)  
        self.layout2=QHBoxLayout()  
        self.layout2.addWidget(self.SoCText)  
        self.layout2.addWidget(self.ARMText)  
        self.layout2.addWidget(self.APCText)           
        self.mainLayout=QVBoxLayout()    
        #self.mainLayout.setSpacing(20)  
        self.mainLayout.addLayout(self.layout1)  
        self.mainLayout.addLayout(self.layout2) 
        self.setLayout(self.mainLayout)        
        self.connect(self.SoCFreshButton,SIGNAL("clicked()"),self.slotSoCFresh)  
        self.connect(self.ARMFreshButton,SIGNAL("clicked()"),self.slotARMFresh)
        self.connect(self.APCFreshButton,SIGNAL("clicked()"),self.slotAPCFresh)

    def slotSoCFresh(self):
        self.SoCText.clear()

    def slotARMFresh(self):
        self.ARMText.clear()

    def slotAPCFresh(self):
        self.APCText.clear()
