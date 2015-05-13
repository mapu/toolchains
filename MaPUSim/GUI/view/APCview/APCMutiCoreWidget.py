# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import math

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class APCMutiCoreWidget(QWidget):
    def __init__(self,parent=None):
	super(APCMutiCoreWidget,self).__init__(parent)

	self.leftWidget=QWidget()
        self.APE0Button=QPushButton("APE0")          
        self.APE1Button=QPushButton("APE1")        
        self.APE2Button=QPushButton("APE2")
        self.APE3Button=QPushButton("APE3")
	self.ARMButton=QPushButton("ARM")
	self.ARMButton.setEnabled(False)
	self.DDR0Button=QPushButton("DDR0")
	self.DDR0Button.setEnabled(False)
	self.DDR1Button=QPushButton("DDR1")
	self.DDR1Button.setEnabled(False)
	self.CDDRButton=QPushButton("CDDR")
	self.CDDRButton.setEnabled(False)

        self.leftLayout=QGridLayout() 
        self.leftLayout.setSpacing(30)
	self.leftLayout.addWidget(self.DDR0Button,2,0)
        self.leftLayout.addWidget(self.APE0Button,1,1)          
        self.leftLayout.addWidget(self.APE1Button,3,1) 
	self.leftLayout.addWidget(self.ARMButton,0,2)
	self.leftLayout.addWidget(self.CDDRButton,4,2)
        self.leftLayout.addWidget(self.APE3Button,1,3) 
	self.leftLayout.addWidget(self.APE2Button,3,3)
	self.leftLayout.addWidget(self.DDR1Button,2,4)

        self.leftLayout.setAlignment(Qt.AlignCenter)
        self.leftWidget.setLayout(self.leftLayout)
         
	#define right tab
	self.rightTab=QTabWidget()
	self.outDMAWidget=QTableWidget()
	self.inDMAWidget=QWidget()
	self.statusWidget=QWidget()
	self.rightTab.addTab(self.outDMAWidget,self.tr("Out DMA"))
	self.rightTab.addTab(self.inDMAWidget,self.tr("In DMA"))
	self.rightTab.addTab(self.statusWidget,self.tr("Status"))
	  
	#define tunTable in outDMAWidget
	self.outDMAWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.outDMAWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.outDMAWidget.horizontalHeader().setVisible(False)
	self.outDMAWidget.verticalHeader().setVisible(False)
	self.outDMAWidget.setShowGrid(False)
	self.outDMAWidget.setColumnCount(2)
	self.outDMAWidget.setRowCount(10)
	self.outDMAWidget.verticalHeader().setDefaultSectionSize(25)
	self.outDMAWidget.setColumnWidth(1,122)
	self.outDMAWidget.setItem(0,0,QTableWidgetItem(self.tr("Tunnel 0")))
	self.outDMAWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.outDMAWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.outDMAWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.outDMAWidget.setItem(1,0,QTableWidgetItem(self.tr("Dest Addr")))
        self.outDMAWidget.setItem(2,0,QTableWidgetItem(self.tr("Src Addr")))
        self.outDMAWidget.setItem(3,0,QTableWidgetItem(self.tr("X Size")))
        self.outDMAWidget.setItem(4,0,QTableWidgetItem(self.tr("Y Size")))
	self.outDMAWidget.setItem(5,0,QTableWidgetItem(self.tr("Tunnel 1")))
	self.outDMAWidget.setItem(5,1,QTableWidgetItem(self.tr("")))
	self.outDMAWidget.item(5,0).setBackgroundColor(QColor(192,192,192))
	self.outDMAWidget.item(5,1).setBackgroundColor(QColor(192,192,192))
        self.outDMAWidget.setItem(6,0,QTableWidgetItem(self.tr("Dest Addr")))
        self.outDMAWidget.setItem(7,0,QTableWidgetItem(self.tr("Src Addr")))
        self.outDMAWidget.setItem(8,0,QTableWidgetItem(self.tr("X Size")))
        self.outDMAWidget.setItem(9,0,QTableWidgetItem(self.tr("Y Size")))

	mainLayout=QHBoxLayout()
	mainLayout.addWidget(self.leftWidget)
	mainLayout.addWidget(self.rightTab)
	mainLayout.setStretchFactor(self.leftWidget,2)
	mainLayout.setStretchFactor(self.rightTab,1)
	self.setLayout(mainLayout)


