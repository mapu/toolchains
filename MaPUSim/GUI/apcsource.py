# -*- coding: utf-8 -*-   
from PyQt4.QtCore import *  
from PyQt4.QtGui import *  
import sys  
  
QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  
  
class apcdialog(QDialog):  
    def __init__(self,parent=None):  
        super(apcdialog,self).__init__(parent)  

        self.Ltabapc=QDialog()  #左侧标签
        self.Lape0=QDialog()
        self.Lape1=QDialog()
        self.Lape2=QDialog()
        self.Lape3=QDialog()
        self.Ltab=QTabWidget()
        self. Ltab.addTab(self.Ltabapc,"APC")
        self.Ltab.addTab(self.Lape0,"APE0")         
        self.Ltab.addTab(self.Lape1,"APE1")
        self.Ltab.addTab(self.Lape2,"APE2")
        self.Ltab.addTab(self.Lape3,"APE3")  
        
        self.Ltabapc.butt0=QPushButton("APE0")   # APC标签
        self.Ltabapc.butt1=QPushButton("APE1")        
        self.Ltabapc.butt2=QPushButton("APE2")
        self.Ltabapc.butt3=QPushButton("APE3")
        apclayout=QGridLayout()
        apclayout.addWidget(self.Ltabapc.butt0,0,0)          
        apclayout.addWidget(self.Ltabapc.butt3,0,1) 
        apclayout.addWidget(self.Ltabapc.butt1,1,0) 
        apclayout.addWidget(self.Ltabapc.butt2,1,1) 
        self.Ltabapc.setLayout(apclayout)
        self.connect(self.Ltabapc.butt0,SIGNAL("clicked()"),self.slottab0)
        self.connect(self.Ltabapc.butt1,SIGNAL("clicked()"),self.slottab1)
        self.connect(self.Ltabapc.butt2,SIGNAL("clicked()"),self.slottab2)
        self.connect(self.Ltabapc.butt3,SIGNAL("clicked()"),self.slottab3)
                              
        wDMA=QDialog()   # 右侧标签
        nDMA=QDialog()
        station=QDialog()
        Rtab=QTabWidget() 
        Rtab.addTab(wDMA,self.tr("外DMA"))
        Rtab.addTab(nDMA,self.tr("内DMA"))
        Rtab.addTab(station,self.tr("状态"))

        wDMA.aisle0=QLabel(self.tr("通道0"))  # 外DMA标签细节
        wDMA.aimadd0=QLabel(self.tr("目标地址"))  
        wDMA.souadd0=QLabel(self.tr("源地址"))
        wDMA.x0=QLabel(self.tr("X长度"))
        wDMA.y0=QLabel(self.tr("Y长度"))
        wDMA.aisle1=QLabel(self.tr("通道1"))
        wDMA.aimadd1=QLabel(self.tr("目标地址"))
        wDMA.souadd1=QLabel(self.tr("源地址"))
        wDMA.x1=QLabel(self.tr("X长度"))
        wDMA.y1=QLabel(self.tr("Y长度"))
        wDMA.layout=QVBoxLayout()
        wDMA.layout.addWidget(wDMA.aisle0)  
        wDMA.layout.addWidget(wDMA.aimadd0)      
        wDMA.layout.addWidget(wDMA.souadd0)
        wDMA.layout.addWidget(wDMA.x0)
        wDMA.layout.addWidget(wDMA.y0)
        wDMA.layout.addWidget(wDMA.aisle1)
        wDMA.layout.addWidget(wDMA.aimadd1)
        wDMA.layout.addWidget(wDMA.souadd1)
        wDMA.layout.addWidget(wDMA.x1)
        wDMA.layout.addWidget(wDMA.y1)
        wDMA.setLayout(wDMA.layout)
  
        hlayout=QHBoxLayout()  #左右侧标签布局，比例设为2:1
        hlayout.addWidget(self.Ltab)  
        hlayout.addWidget(Rtab) 
        hlayout.setStretchFactor(self.Ltab,2)  
        hlayout.setStretchFactor(Rtab,1)  
        self.setLayout(hlayout)  
    def slottab0(self):
        self.Ltab.setCurrentWidget(self.Lape0)  
    def slottab1(self):
        self.Ltab.setCurrentWidget(self.Lape1) 
    def slottab2(self):
        self.Ltab.setCurrentWidget(self.Lape2) 
    def slottab3(self):
        self.Ltab.setCurrentWidget(self.Lape3)
    

