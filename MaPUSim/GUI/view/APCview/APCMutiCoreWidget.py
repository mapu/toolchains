# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import math

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class APCMutiCoreWidget(QWidget):
    def __init__(self,parent=None):
	super(APCMutiCoreWidget,self).__init__(parent)

	#define left Widget
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
        self.leftLayout.setSpacing(40)
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
	self.inDMAWidget=QTableWidget()
	self.statusWidget=QTableWidget()
	self.rightTab.addTab(self.outDMAWidget,self.tr("External DMA"))
	self.rightTab.addTab(self.inDMAWidget,self.tr("Internal DMA"))
	self.rightTab.addTab(self.statusWidget,self.tr("Status"))
	self.outDMAWidget.horizontalHeader().setStretchLastSection(True)
	self.inDMAWidget.horizontalHeader().setStretchLastSection(True)
	self.statusWidget.horizontalHeader().setStretchLastSection(True)
	  
	#define outDMAWidget
	self.outDMAWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.outDMAWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.outDMAWidget.horizontalHeader().setVisible(False)
	self.outDMAWidget.verticalHeader().setVisible(False)
	self.outDMAWidget.setShowGrid(False)
	self.outDMAWidget.setColumnCount(2)
	self.outDMAWidget.setRowCount(60)
	self.outDMAWidget.verticalHeader().setDefaultSectionSize(25)
	self.outDMAWidget.setColumnWidth(0,122)
	#define outDMAWidget channel 0
	self.outDMAWidget.setItem(0,0,QTableWidgetItem(self.tr("channel 0")))
	self.outDMAWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.outDMAWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.outDMAWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.outDMAWidget.setItem(1,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.outDMAWidget.setItem(2,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.outDMAWidget.setItem(3,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.outDMAWidget.setItem(4,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.outDMAWidget.setItem(5,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.outDMAWidget.setItem(6,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.outDMAWidget.setItem(7,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.outDMAWidget.setItem(8,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.outDMAWidget.setItem(9,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.outDMAWidget.setItem(10,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.outDMAWidget.setItem(11,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.outDMAWidget.setItem(12,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.outDMAWidget.setItem(13,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.outDMAWidget.setItem(14,0,QTableWidgetItem(self.tr("DMACmd")))
	#define outDMAWidget channel 1
	self.outDMAWidget.setItem(15,0,QTableWidgetItem(self.tr("channel 1")))
	self.outDMAWidget.setItem(15,1,QTableWidgetItem(self.tr("")))
	self.outDMAWidget.item(15,0).setBackgroundColor(QColor(192,192,192))
	self.outDMAWidget.item(15,1).setBackgroundColor(QColor(192,192,192))
        self.outDMAWidget.setItem(16,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.outDMAWidget.setItem(17,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.outDMAWidget.setItem(18,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.outDMAWidget.setItem(19,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.outDMAWidget.setItem(20,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.outDMAWidget.setItem(21,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.outDMAWidget.setItem(22,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.outDMAWidget.setItem(23,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.outDMAWidget.setItem(24,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.outDMAWidget.setItem(25,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.outDMAWidget.setItem(26,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.outDMAWidget.setItem(27,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.outDMAWidget.setItem(28,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.outDMAWidget.setItem(29,0,QTableWidgetItem(self.tr("DMACmd")))
	#define outDMAWidget channel 2
	self.outDMAWidget.setItem(30,0,QTableWidgetItem(self.tr("channel 2")))
	self.outDMAWidget.setItem(30,1,QTableWidgetItem(self.tr("")))
	self.outDMAWidget.item(30,0).setBackgroundColor(QColor(192,192,192))
	self.outDMAWidget.item(30,1).setBackgroundColor(QColor(192,192,192))
        self.outDMAWidget.setItem(31,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.outDMAWidget.setItem(32,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.outDMAWidget.setItem(33,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.outDMAWidget.setItem(34,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.outDMAWidget.setItem(35,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.outDMAWidget.setItem(36,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.outDMAWidget.setItem(37,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.outDMAWidget.setItem(38,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.outDMAWidget.setItem(39,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.outDMAWidget.setItem(40,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.outDMAWidget.setItem(41,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.outDMAWidget.setItem(42,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.outDMAWidget.setItem(43,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.outDMAWidget.setItem(44,0,QTableWidgetItem(self.tr("DMACmd")))
	#define outDMAWidget channel 3
	self.outDMAWidget.setItem(45,0,QTableWidgetItem(self.tr("channel 3")))
	self.outDMAWidget.setItem(45,1,QTableWidgetItem(self.tr("")))
	self.outDMAWidget.item(45,0).setBackgroundColor(QColor(192,192,192))
	self.outDMAWidget.item(45,1).setBackgroundColor(QColor(192,192,192))
        self.outDMAWidget.setItem(46,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.outDMAWidget.setItem(47,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.outDMAWidget.setItem(48,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.outDMAWidget.setItem(49,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.outDMAWidget.setItem(50,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.outDMAWidget.setItem(51,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.outDMAWidget.setItem(52,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.outDMAWidget.setItem(53,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.outDMAWidget.setItem(54,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.outDMAWidget.setItem(55,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.outDMAWidget.setItem(56,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.outDMAWidget.setItem(57,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.outDMAWidget.setItem(58,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.outDMAWidget.setItem(59,0,QTableWidgetItem(self.tr("DMACmd")))

	#define inDMAWidget
	self.inDMAWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.inDMAWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.inDMAWidget.horizontalHeader().setVisible(False)
	self.inDMAWidget.verticalHeader().setVisible(False)
	self.inDMAWidget.setShowGrid(False)
	self.inDMAWidget.setColumnCount(2)
	self.inDMAWidget.setRowCount(120)
	self.inDMAWidget.verticalHeader().setDefaultSectionSize(25)
	self.inDMAWidget.setColumnWidth(0,122)
	#define inDMAWidget channel 0
	self.inDMAWidget.setItem(0,0,QTableWidgetItem(self.tr("channel 0")))
	self.inDMAWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(1,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(2,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(3,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(4,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(5,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(6,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(7,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(8,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(9,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(10,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(11,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(12,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(13,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(14,0,QTableWidgetItem(self.tr("DMACmd")))
	#define inDMAWidget channel 1
	self.inDMAWidget.setItem(15,0,QTableWidgetItem(self.tr("channel 1")))
	self.inDMAWidget.setItem(15,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(15,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(15,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(16,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(17,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(18,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(19,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(20,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(21,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(22,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(23,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(24,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(25,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(26,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(27,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(28,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(29,0,QTableWidgetItem(self.tr("DMACmd")))
	#define inDMAWidget channel 2
	self.inDMAWidget.setItem(30,0,QTableWidgetItem(self.tr("channel 2")))
	self.inDMAWidget.setItem(30,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(30,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(30,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(31,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(32,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(33,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(34,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(35,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(36,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(37,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(38,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(39,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(40,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(41,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(42,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(43,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(44,0,QTableWidgetItem(self.tr("DMACmd")))
	#define inDMAWidget channel 3
	self.inDMAWidget.setItem(45,0,QTableWidgetItem(self.tr("channel 3")))
	self.inDMAWidget.setItem(45,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(45,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(45,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(46,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(47,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(48,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(49,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(50,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(51,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(52,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(53,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(54,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(55,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(56,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(57,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(58,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(59,0,QTableWidgetItem(self.tr("DMACmd")))
	#define inDMAWidget channel 4
	self.inDMAWidget.setItem(60,0,QTableWidgetItem(self.tr("channel 4")))
	self.inDMAWidget.setItem(60,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(60,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(60,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(61,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(62,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(63,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(64,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(65,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(66,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(67,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(68,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(69,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(70,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(71,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(72,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(73,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(74,0,QTableWidgetItem(self.tr("DMACmd")))
	#define inDMAWidget channel 5
	self.inDMAWidget.setItem(75,0,QTableWidgetItem(self.tr("channel 5")))
	self.inDMAWidget.setItem(75,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(75,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(75,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(76,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(77,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(78,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(79,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(80,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(81,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(82,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(83,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(84,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(85,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(86,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(87,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(88,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(89,0,QTableWidgetItem(self.tr("DMACmd")))
	#define inDMAWidget channel 6
	self.inDMAWidget.setItem(90,0,QTableWidgetItem(self.tr("channel 6")))
	self.inDMAWidget.setItem(90,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(90,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(90,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(91,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(92,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(93,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(94,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(95,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(96,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(97,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(98,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(99,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(100,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(101,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(102,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(103,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(104,0,QTableWidgetItem(self.tr("DMACmd")))
	#define inDMAWidget channel 7
	self.inDMAWidget.setItem(105,0,QTableWidgetItem(self.tr("channel 7")))
	self.inDMAWidget.setItem(105,1,QTableWidgetItem(self.tr("")))
	self.inDMAWidget.item(105,0).setBackgroundColor(QColor(192,192,192))
	self.inDMAWidget.item(105,1).setBackgroundColor(QColor(192,192,192))
        self.inDMAWidget.setItem(106,0,QTableWidgetItem(self.tr("DMALSAddress")))
        self.inDMAWidget.setItem(107,0,QTableWidgetItem(self.tr("DMALSXNum")))
        self.inDMAWidget.setItem(108,0,QTableWidgetItem(self.tr("DMALSYStep")))
        self.inDMAWidget.setItem(109,0,QTableWidgetItem(self.tr("DMALSYNum")))
	self.inDMAWidget.setItem(110,0,QTableWidgetItem(self.tr("DMALSZStep")))
        self.inDMAWidget.setItem(111,0,QTableWidgetItem(self.tr("DMALSYAllNum")))
        self.inDMAWidget.setItem(112,0,QTableWidgetItem(self.tr("DMAGDAddress")))
        self.inDMAWidget.setItem(113,0,QTableWidgetItem(self.tr("DMAGDXNum")))
        self.inDMAWidget.setItem(114,0,QTableWidgetItem(self.tr("DMAGDYStep")))
	self.inDMAWidget.setItem(115,0,QTableWidgetItem(self.tr("DMAGDYSNum")))
	self.inDMAWidget.setItem(116,0,QTableWidgetItem(self.tr("DMAGDZStep")))
        self.inDMAWidget.setItem(117,0,QTableWidgetItem(self.tr("DMAGDYAllNum")))
	self.inDMAWidget.setItem(118,0,QTableWidgetItem(self.tr("DMAGroupNum")))
	self.inDMAWidget.setItem(119,0,QTableWidgetItem(self.tr("DMACmd")))

	#define statusWidget
	self.statusWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.statusWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.statusWidget.horizontalHeader().setVisible(False)
	self.statusWidget.verticalHeader().setVisible(False)
	self.statusWidget.setShowGrid(False)
	self.statusWidget.setColumnCount(2)
	self.statusWidget.setRowCount(7)
	self.statusWidget.verticalHeader().setDefaultSectionSize(25)
	self.statusWidget.setColumnWidth(0,138)
	#define inDMAWidget channel 0
	self.statusWidget.setItem(0,0,QTableWidgetItem(self.tr("status")))
	self.statusWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.statusWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.statusWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.statusWidget.setItem(1,0,QTableWidgetItem(self.tr("APE No.")))
        self.statusWidget.setItem(2,0,QTableWidgetItem(self.tr("SPU Status")))
        self.statusWidget.setItem(3,0,QTableWidgetItem(self.tr("MPU Status")))
        self.statusWidget.setItem(4,0,QTableWidgetItem(self.tr("External DMA Status")))
	self.statusWidget.setItem(5,0,QTableWidgetItem(self.tr("Internal DMA Status")))
        self.statusWidget.setItem(6,0,QTableWidgetItem(self.tr("Event Status")))

	mainLayout=QHBoxLayout()
	mainLayout.addWidget(self.leftWidget)
	mainLayout.addWidget(self.rightTab)
	mainLayout.setStretchFactor(self.leftWidget,5)
	mainLayout.setStretchFactor(self.rightTab,2)
	self.setLayout(mainLayout)


