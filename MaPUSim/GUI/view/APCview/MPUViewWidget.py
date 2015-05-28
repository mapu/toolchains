# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from FloatDialog import*
from LineWidget import*
import sys
import math

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPUViewWidget(QWidget):
    def __init__(self,parent=None):
	super(MPUViewWidget,self).__init__(parent)

	#define left Widget
	self.leftWidget=QWidget()
	self.widget=QWidget()
	widgetLay=QGridLayout()
	widgetLay.addWidget(self.widget,0,0)
	self.leftWidget.setLayout(widgetLay)
	self.widget.setFixedSize(700,370)
	self.DMButton=QPushButton(self.tr("DM"))
	self.DMButton.setFixedSize(70,25)
	self.DMButton.setEnabled(False)
	self.BIU0Button=QPushButton(self.tr("BIU0"))
	self.BIU0Button.setFixedSize(70,25)
	self.BIU1Button=QPushButton(self.tr("BIU1"))
	self.BIU1Button.setFixedSize(70,25)
	self.BIU2Button=QPushButton(self.tr("BIU2"))
	self.BIU2Button.setFixedSize(70,25)
	self.SHU0Button=QPushButton(self.tr("SHU0"))
	self.SHU0Button.setFixedSize(70,25)
	self.MRFButton=QPushButton(self.tr("M RF"))
	self.MRFButton.setFixedSize(70,25)
	self.SHU1Button=QPushButton(self.tr("SHU1"))
	self.SHU1Button.setFixedSize(70,25)
	self.byteComboBox=QComboBox()
	self.byteComboBox.setFixedSize(80,20)
	self.byteComboBox.insertItem(0,"8 Bytes")
	self.byteComboBox.insertItem(1,"7 Bytes")
	self.byteComboBox.insertItem(2,"6 Bytes")
	self.byteComboBox.insertItem(3,"5 Bytes")
	self.byteComboBox.insertItem(4,"4 Bytes")
	self.byteComboBox.insertItem(5,"3 Bytes")
	self.byteComboBox.insertItem(6,"2 Bytes")
	self.byteComboBox.insertItem(7,"1 Bytes")

	self.laneComboBox=QComboBox()
	self.laneComboBox.setFixedSize(67,20)
	self.laneComboBox.insertItem(0,"Lane0")
	self.laneComboBox.insertItem(1,"Lane1")
	self.laneComboBox.insertItem(2,"Lane2")
	self.laneComboBox.insertItem(3,"Lane3")	
	self.IALUButton=QPushButton(self.tr("IALU"))
	self.IALUButton.setFixedSize(70,25)
	self.IMACButton=QPushButton(self.tr("IMAC"))
	self.IMACButton.setFixedSize(70,25)
	self.FALUButton=QPushButton(self.tr("FALU"))
	self.FALUButton.setFixedSize(70,25)
	self.FMACButton=QPushButton(self.tr("FMAC"))
	self.FMACButton.setFixedSize(70,25)

	self.blank0=QLabel()
	self.blank0.setFixedSize(70,25)
	self.blank1=QLabel()
	self.blank1.setFixedSize(70,25)
	self.blank2=QLabel()
	self.blank2.setFixedSize(70,25)
	self.blank3=QLabel()
	self.blank3.setFixedSize(70,25)
	self.blank4=QLabel()
	self.blank4.setFixedSize(70,25)
	self.blank5=QLabel()
	self.blank5.setFixedSize(70,25)
	self.blank6=QLabel()
	self.blank6.setFixedSize(70,25)

	#layout left Widget
	self.gridLay=QGridLayout()
	self.gridLay.addWidget(self.DMButton,9,0)
	self.gridLay.addWidget(self.blank0,0,1)
	self.gridLay.addWidget(self.blank1,3,2)
	self.gridLay.addWidget(self.BIU0Button,6,3)
	self.gridLay.addWidget(self.BIU1Button,9,3)
	self.gridLay.addWidget(self.BIU2Button,12,3)
	self.gridLay.addWidget(self.blank2,5,4)
	self.gridLay.addWidget(self.blank3,8,5)
	self.gridLay.addWidget(self.SHU0Button,2,6)

	self.gridLay.addWidget(self.SHU1Button,15,6)
	self.gridLay.addWidget(self.byteComboBox,0,7,1,2)
	self.gridLay.addWidget(self.blank4,11,7)
	self.gridLay.addWidget(self.blank5,14,8)
	self.gridLay.addWidget(self.laneComboBox,0,9,1,2)
	self.gridLay.addWidget(self.IALUButton,4,9)
	self.gridLay.addWidget(self.IMACButton,7,9)
	self.gridLay.addWidget(self.FALUButton,10,9)
	self.gridLay.addWidget(self.FMACButton,13,9)
	self.gridLay.addWidget(self.blank6,0,10)
	self.gridLay.setAlignment(Qt.AlignCenter)
	self.widget.setLayout(self.gridLay)

	#define rightTab
	self.rightTab=QTabWidget()
	self.regFileWidget=QTableWidget()
	self.specialRegWidget=QTableWidget()
	self.traceWidget=QTableWidget()
	self.rightTab.addTab(self.regFileWidget,self.tr("Register File"))
	self.rightTab.addTab(self.specialRegWidget,self.tr("Special Register"))
	self.rightTab.addTab(self.traceWidget,self.tr("Trace"))
	self.regFileWidget.horizontalHeader().setStretchLastSection(True)
	self.specialRegWidget.horizontalHeader().setStretchLastSection(True)
	self.traceWidget.horizontalHeader().setStretchLastSection(True)

	#define regFileWidget
	self.regFileWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.regFileWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.regFileWidget.horizontalHeader().setVisible(False)
	self.regFileWidget.verticalHeader().setVisible(False)
	self.regFileWidget.setShowGrid(False)
	self.regFileWidget.setColumnCount(2)
	self.regFileWidget.setRowCount(69)
	self.regFileWidget.verticalHeader().setDefaultSectionSize(25)
	#self.regFileWidget.setColumnWidth(0,122)
	#define regFileWidget M
	self.regFileWidget.setItem(0,0,QTableWidgetItem(self.tr("M")))
	self.regFileWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(1,0,QTableWidgetItem(self.tr("M0")))
        self.regFileWidget.setItem(2,0,QTableWidgetItem(self.tr("M1")))
        self.regFileWidget.setItem(3,0,QTableWidgetItem(self.tr("M2")))
        self.regFileWidget.setItem(4,0,QTableWidgetItem(self.tr("M3")))
	self.regFileWidget.setItem(5,0,QTableWidgetItem(self.tr("M4")))
        self.regFileWidget.setItem(6,0,QTableWidgetItem(self.tr("M5")))
        self.regFileWidget.setItem(7,0,QTableWidgetItem(self.tr("M6")))
        self.regFileWidget.setItem(8,0,QTableWidgetItem(self.tr("M7")))
	self.regFileWidget.setItem(9,0,QTableWidgetItem(self.tr("M8")))
        self.regFileWidget.setItem(10,0,QTableWidgetItem(self.tr("M9")))
        self.regFileWidget.setItem(11,0,QTableWidgetItem(self.tr("M10")))
        self.regFileWidget.setItem(12,0,QTableWidgetItem(self.tr("M11")))
        self.regFileWidget.setItem(13,0,QTableWidgetItem(self.tr("M12")))
        self.regFileWidget.setItem(14,0,QTableWidgetItem(self.tr("M13")))
	self.regFileWidget.setItem(15,0,QTableWidgetItem(self.tr("M14")))
        self.regFileWidget.setItem(16,0,QTableWidgetItem(self.tr("M15")))
        self.regFileWidget.setItem(17,0,QTableWidgetItem(self.tr("M16")))
        self.regFileWidget.setItem(18,0,QTableWidgetItem(self.tr("M17")))
	self.regFileWidget.setItem(19,0,QTableWidgetItem(self.tr("M18")))
        self.regFileWidget.setItem(20,0,QTableWidgetItem(self.tr("M19")))
        self.regFileWidget.setItem(21,0,QTableWidgetItem(self.tr("M20")))
        self.regFileWidget.setItem(22,0,QTableWidgetItem(self.tr("M21")))
        self.regFileWidget.setItem(23,0,QTableWidgetItem(self.tr("M22")))
        self.regFileWidget.setItem(24,0,QTableWidgetItem(self.tr("M23")))
	self.regFileWidget.setItem(25,0,QTableWidgetItem(self.tr("M24")))
        self.regFileWidget.setItem(26,0,QTableWidgetItem(self.tr("M25")))
        self.regFileWidget.setItem(27,0,QTableWidgetItem(self.tr("M26")))
        self.regFileWidget.setItem(28,0,QTableWidgetItem(self.tr("M27")))
	self.regFileWidget.setItem(29,0,QTableWidgetItem(self.tr("M28")))
        self.regFileWidget.setItem(30,0,QTableWidgetItem(self.tr("M29")))
	self.regFileWidget.setItem(31,0,QTableWidgetItem(self.tr("M30")))
	self.regFileWidget.setItem(32,0,QTableWidgetItem(self.tr("M31")))
	#define regFileWidget SHU0 T
	self.regFileWidget.setItem(33,0,QTableWidgetItem(self.tr("SHU0 T")))
	self.regFileWidget.setItem(33,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(33,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(33,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(34,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(35,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(36,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(37,0,QTableWidgetItem(self.tr("T3")))
	self.regFileWidget.setItem(38,0,QTableWidgetItem(self.tr("T4")))
        self.regFileWidget.setItem(39,0,QTableWidgetItem(self.tr("T5")))
        self.regFileWidget.setItem(40,0,QTableWidgetItem(self.tr("T6")))
        self.regFileWidget.setItem(41,0,QTableWidgetItem(self.tr("T7")))
	self.regFileWidget.setItem(42,0,QTableWidgetItem(self.tr("T8")))
        self.regFileWidget.setItem(43,0,QTableWidgetItem(self.tr("T9")))
        self.regFileWidget.setItem(44,0,QTableWidgetItem(self.tr("T10")))
        self.regFileWidget.setItem(45,0,QTableWidgetItem(self.tr("T11")))
        self.regFileWidget.setItem(46,0,QTableWidgetItem(self.tr("T12")))
        self.regFileWidget.setItem(47,0,QTableWidgetItem(self.tr("T13")))
	self.regFileWidget.setItem(48,0,QTableWidgetItem(self.tr("T14")))
        self.regFileWidget.setItem(49,0,QTableWidgetItem(self.tr("T15")))
	self.regFileWidget.setItem(50,0,QTableWidgetItem(self.tr("T16")))
	#define regFileWidget SHU1 T
	self.regFileWidget.setItem(51,0,QTableWidgetItem(self.tr("SHU1 T")))
	self.regFileWidget.setItem(51,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(51,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(51,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(52,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(53,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(54,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(55,0,QTableWidgetItem(self.tr("T3")))
	self.regFileWidget.setItem(56,0,QTableWidgetItem(self.tr("T4")))
        self.regFileWidget.setItem(57,0,QTableWidgetItem(self.tr("T5")))
        self.regFileWidget.setItem(58,0,QTableWidgetItem(self.tr("T6")))
        self.regFileWidget.setItem(59,0,QTableWidgetItem(self.tr("T7")))
	self.regFileWidget.setItem(60,0,QTableWidgetItem(self.tr("T8")))
        self.regFileWidget.setItem(61,0,QTableWidgetItem(self.tr("T9")))
        self.regFileWidget.setItem(62,0,QTableWidgetItem(self.tr("T10")))
        self.regFileWidget.setItem(63,0,QTableWidgetItem(self.tr("T11")))
        self.regFileWidget.setItem(64,0,QTableWidgetItem(self.tr("T12")))
        self.regFileWidget.setItem(65,0,QTableWidgetItem(self.tr("T13")))
	self.regFileWidget.setItem(66,0,QTableWidgetItem(self.tr("T14")))
        self.regFileWidget.setItem(67,0,QTableWidgetItem(self.tr("T15")))
	self.regFileWidget.setItem(68,0,QTableWidgetItem(self.tr("T16")))

	mainLayout=QHBoxLayout()
	mainLayout.addWidget(self.leftWidget)
	mainLayout.addWidget(self.rightTab)
	mainLayout.setStretchFactor(self.leftWidget,5)
	mainLayout.setStretchFactor(self.rightTab,2)
	self.setLayout(mainLayout)

	self.connect(self.DMButton,SIGNAL("clicked()"),self.DMButtonSlot)
	self.connect(self.BIU0Button,SIGNAL("clicked()"),self.BIU0ButtonSlot)
	self.connect(self.BIU1Button,SIGNAL("clicked()"),self.BIU1ButtonSlot)
	self.connect(self.BIU2Button,SIGNAL("clicked()"),self.BIU2ButtonSlot)
	self.connect(self.SHU0Button,SIGNAL("clicked()"),self.SHU0ButtonSlot)
	self.connect(self.SHU1Button,SIGNAL("clicked()"),self.SHU1ButtonSlot)
	self.connect(self.MRFButton,SIGNAL("clicked()"),self.MRFButtonSlot)
	self.connect(self.IALUButton,SIGNAL("clicked()"),self.IALUButtonSlot)
	self.connect(self.IMACButton,SIGNAL("clicked()"),self.IMACButtonSlot)
	self.connect(self.FALUButton,SIGNAL("clicked()"),self.FALUButtonSlot)
	self.connect(self.FMACButton,SIGNAL("clicked()"),self.FMACButtonSlot)
	
	#define float dialog
	self.DMButtonDialog=FloatDialog()
	self.DMButtonDialog.setWindowTitle(self.tr("DM Stages"))
	self.BIU0ButtonDialog=FloatDialog()
	self.BIU0ButtonDialog.setWindowTitle(self.tr("BIU0 Stages"))
	self.BIU1ButtonDialog=FloatDialog()
	self.BIU1ButtonDialog.setWindowTitle(self.tr("BIU1 Stages"))	
	self.BIU2ButtonDialog=FloatDialog()
	self.BIU2ButtonDialog.setWindowTitle(self.tr("BIU2 Stages"))	
	self.SHU0ButtonDialog=FloatDialog()
	self.SHU0ButtonDialog.setWindowTitle(self.tr("SHU0 Stages"))	
	self.SHU1ButtonDialog=FloatDialog()
	self.SHU1ButtonDialog.setWindowTitle(self.tr("SHU1 Stages"))
	self.MRFButtonDialog=FloatDialog()
	self.MRFButtonDialog.setWindowTitle(self.tr("MRF Stages"))	
	self.IALUButtonDialog=FloatDialog()
	self.IALUButtonDialog.setWindowTitle(self.tr("IALU Stages"))	
	self.IMACButtonDialog=FloatDialog()
	self.IMACButtonDialog.setWindowTitle(self.tr("IMAC Stages"))
	self.FALUButtonDialog=FloatDialog()
	self.FALUButtonDialog.setWindowTitle(self.tr("FALU Stages"))	
	self.FMACButtonDialog=FloatDialog()
	self.FMACButtonDialog.setWindowTitle(self.tr("FMAC Stages"))		
	#paint lines
	self.paintLines()

    #define slot function
    def DMButtonSlot(self):
	self.DMButtonDialog.show()

    def BIU0ButtonSlot(self):
	self.BIU0ButtonDialog.show()

    def BIU1ButtonSlot(self):
	self.BIU1ButtonDialog.show()

    def BIU2ButtonSlot(self):
	self.BIU2ButtonDialog.show()

    def SHU0ButtonSlot(self):
	self.SHU0ButtonDialog.show()

    def SHU1ButtonSlot(self):	
	self.SHU1ButtonDialog.show()

    def MRFButtonSlot(self):
	self.MRFButtonDialog.show()

    def IALUButtonSlot(self):
	self.IALUButtonDialog.show()

    def IMACButtonSlot(self):
	self.IMACButtonDialog.show()

    def FALUButtonSlot(self):
	self.FALUButtonDialog.show()

    def FMACButtonSlot(self):
	self.FMACButtonDialog.show()

    #define close all float dialogs
    def closeFloatDialogs(self):
	self.DMButtonDialog.close()
	self.BIU0ButtonDialog.close()
	self.BIU1ButtonDialog.close()
	self.BIU2ButtonDialog.close()
	self.SHU0ButtonDialog.close()
	self.SHU1ButtonDialog.close()
	self.MRFButtonDialog.close()
	self.IALUButtonDialog.close()
	self.IMACButtonDialog.close()
	self.FALUButtonDialog.close()	
	self.FMACButtonDialog.close()		

    def paintLines(self):
	#define the line between DMButton and BIU0Button,4rows,2columns
	height=98
	width=120
	self.DM_BIU0Line=LineWidget(3,height-25,width,5,0,10,height-25-5+10)
	self.gridLay.addWidget(self.DM_BIU0Line,6,1,4,2)
	self.BIU0_DMLine=LineWidget(3,height-20,width,10,1,10,height-20-10+10)
	self.gridLay.addWidget(self.BIU0_DMLine,6,1,4,2)
	#define the line between DMButton and BIU1Button,0rows,2columns
	height=25
	width=120
	self.DM_BIU1Line=LineWidget(3,height-15,width,height-15,0,0,0)
	self.gridLay.addWidget(self.DM_BIU1Line,9,1,1,2)
	self.BIU1_DMLine=LineWidget(3,height-10,width,height-10,1,0,0)
	self.gridLay.addWidget(self.BIU1_DMLine,9,1,1,2)
	
	#define the line between DMButton and BIU2Button,4rows,2columns
	height=98
	width=120
	self.DM_BIU2Line=LineWidget(3,25,width,height-20,0,10,height-20-25+10)
	self.gridLay.addWidget(self.DM_BIU2Line,9,1,4,2)
	self.BIU2_DMLine=LineWidget(3,30,width,height-15,1,10,height-15-30+10)
	self.gridLay.addWidget(self.BIU2_DMLine,9,1,4,2)

	#define the line between BIU0Button and SHU0Button,5rows,2columns
	height=125
	width=120
	self.BIU0_SHU0Line=LineWidget(3,height-30,width,5,0,10,height-30-5+10)
	self.gridLay.addWidget(self.BIU0_SHU0Line,2,4,5,2)
	self.SHU0_BIU0Line=LineWidget(3,height-25,width,10,1,10,height-25-10+10)
	self.gridLay.addWidget(self.SHU0_BIU0Line,2,4,5,2)

	#define the line between BIU0Button and MRFButton,4rows,2columns
	height=98
	width=120
	self.BIU0_MRFLine=LineWidget(3,20,width,height-25,0,10,height-25-20+10)
	self.gridLay.addWidget(self.BIU0_MRFLine,6,4,4,2)
	self.MRF_BIU0Line=LineWidget(3,25,width,height-20,1,10,height-20-25+10)
	self.gridLay.addWidget(self.MRF_BIU0Line,6,4,4,2)

	#define the line between BIU1Button and MRFButton,4rows,2columns
	height=25
	width=120
	self.BIU1_MRFLine=LineWidget(3,10,width,10,0,0,0)
	self.gridLay.addWidget(self.BIU1_MRFLine,9,4,1,2)
	self.MRF_BIU1Line=LineWidget(3,15,width,15,1,0,0)
	self.gridLay.addWidget(self.MRF_BIU1Line,9,4,1,2)

	#define the line between BIU2Button and MRFButton,4rows,2columns 
	height=98
	width=120
	self.MRF_BIU2Line=LineWidget(3,height-25,width,20,0,10,height-25-20+10)
	self.gridLay.addWidget(self.MRF_BIU2Line,9,4,4,2)
	self.BIU2_MRFLine=LineWidget(3,height-20,width,25,1,10,height-20-25+10)
	self.gridLay.addWidget(self.BIU2_MRFLine,9,4,4,2)	 

	#define the line between BIU2Button and SHU1Button,4rows,2columns 
	height=98
	width=120
	self.BIU2_SHU1Line=LineWidget(3,20,width,height-10,0,10,height-10-20+10)
	self.gridLay.addWidget(self.BIU2_SHU1Line,12,4,4,2)
	self.SHU1_BIU2Line=LineWidget(3,25,width,height-5,1,10,height-5-25+10)
	self.gridLay.addWidget(self.SHU1_BIU2Line,12,4,4,2)

	#define the line between SHU0Button and MRFButton,6rows,1columns  
	height=146
	width=70
	self.SHU0_MRFLine=LineWidget(30,0,30,height,2,0,0)
	self.gridLay.addWidget(self.SHU0_MRFLine,3,6,6,1)
	self.MRF_SHU0Line=LineWidget(35,0,35,height,3,0,0)
	self.gridLay.addWidget(self.MRF_SHU0Line,3,6,6,1)

	#define the line between MRFButton and SHU1Button,5rows,1columns  
	height=122
	width=70
	self.MRF_SHU1Line=LineWidget(30,0,30,height,2,0,0)
	self.gridLay.addWidget(self.MRF_SHU1Line,10,6,5,1)
	self.SHU1_MRFLine=LineWidget(35,0,35,height,3,0,0)
	self.gridLay.addWidget(self.SHU1_MRFLine,10,6,5,1)
	
	#define the line between SHU0Button and IALUButton,3rows,2columns 
	height=73
	width=120
	self.SHU0_IALULine=LineWidget(3,0,width,height-30,0,30,height-30+30)
	self.gridLay.addWidget(self.SHU0_IALULine,2,7,3,2)
	self.IALU_SHU0Line=LineWidget(3,5,width,height-25,1,30,height-25-5+30)
	self.gridLay.addWidget(self.IALU_SHU0Line,2,7,3,2)

	#define the line between MRFButton and IMACButton,3rows,2columns 
	height=73
	widht=120
	self.IMAC_MRFLine=LineWidget(3,height-15,width,10,0,30,height-15-10+30)
	self.gridLay.addWidget(self.IMAC_MRFLine,7,7,3,2)
	self.MRF_IMACLine=LineWidget(3,height-10,width,15,1,30,height-10-15+30)
	self.gridLay.addWidget(self.MRF_IMACLine,7,7,3,2)

	#define the line between MRFButton and FMACButton,5rows,2columns
	height=122
	width=120
	self.MRF_FMACLine=LineWidget(3,25,width,height-20,0,10,height-20-25+10)
	self.gridLay.addWidget(self.MRF_FMACLine,9,7,5,2)
	self.FMAC_MRFLine=LineWidget(3,30,width,height-15,1,10,height-15-30+10)
	self.gridLay.addWidget(self.FMAC_MRFLine,9,7,5,2)	

	#define the line between SHU1Button and FMACButton,3rows,2columns
	height=73
	width=120
	self.FMAC_SHU1Line=LineWidget(3,height-13,width,25,0,30,height-13-25+30)
	self.gridLay.addWidget(self.FMAC_SHU1Line,13,7,3,2)
	self.SHU1_FMACLine=LineWidget(3,height-8,width,30,1,30,height-8-30+30)
	self.gridLay.addWidget(self.SHU1_FMACLine,13,7,3,2)

	#define the line between IALUButton and IMACButton,2rows,1column
	height=48
	width=70
	self.IALU_IMACLine=LineWidget(30,0,30,height,2,0,0)
	self.gridLay.addWidget(self.IALU_IMACLine,5,9,2,1)
	self.IMAC_IALULine=LineWidget(35,0,35,height,3,0,0)
	self.gridLay.addWidget(self.IMAC_IALULine,5,9,2,1)

	#define the line between IMACButton and FALUButton,2rows,1column
	height=48
	width=70
	self.IMAC_FALULine=LineWidget(30,0,30,height,2,0,0)
	self.gridLay.addWidget(self.IMAC_FALULine,8,9,2,1)
	self.FALU_IMACLine=LineWidget(35,0,35,height,3,0,0)
	self.gridLay.addWidget(self.FALU_IMACLine,8,9,2,1)

	#define the line between FALUButton and FMACButton,2rows,1column
	height=48
	width=70
	self.FALU_FMACLine=LineWidget(30,0,30,height,2,0,0)
	self.gridLay.addWidget(self.FALU_FMACLine,11,9,2,1)
	self.FMAC_FALULine=LineWidget(35,0,35,height,3,0,0)
	self.gridLay.addWidget(self.FMAC_FALULine,11,9,2,1)

	#define the line between MRFButton and FALUButton,2rows,2columns
	height=48
	width=120
	self.MRF_FALULine=LineWidget(3,17,width,height-15,0,20,height-15-17+20)
	self.gridLay.addWidget(self.MRF_FALULine,9,7,2,2)
	self.FALU_MRFLine=LineWidget(3,22,width,height-10,1,20,height-10-22+20)
	self.gridLay.addWidget(self.FALU_MRFLine,9,7,2,2)

	#define the line between BIU0Button and SHU1Button,10rows,2columns
	height=244
	width=120
	self.BIU0_SHU1Line=LineWidget(3,10,width,height-20,4,30,height-5-10+30)
	self.gridLay.addWidget(self.BIU0_SHU1Line,6,4,10,2)
	self.SHU1_BIU0Line=LineWidget(3,15,width,height-15,5,30,height-15+30)
	self.gridLay.addWidget(self.SHU1_BIU0Line,6,4,10,2)

	#define the line between SHU0Button and IMACButton,6rows,2columns
	height=146
	width=120
	self.SHU0_IMACLine=LineWidget(3,10,width,height-20,4,10,height-20-10+10)
	self.gridLay.addWidget(self.SHU0_IMACLine,2,7,6,2)
	self.IMAC_SHU0Line=LineWidget(3,15,width,height-15,5,10,height-15-15+10)
	self.gridLay.addWidget(self.IMAC_SHU0Line,2,7,6,2)

	#define the line between MRFButton and IALUButton,6rows,2columns
	height=146
	width=120
	self.IALU_MRFLine=LineWidget(3,height-25,width,10,6,10,height-15-10+10)
	self.gridLay.addWidget(self.IALU_MRFLine,4,7,6,2)
	self.MRF_IALULine=LineWidget(3,height-20,width,15,7,10,height-10-15+10)
	self.gridLay.addWidget(self.MRF_IALULine,4,7,6,2)

	#define the line between SHU1Button and FALUButton,6rows,2columns
	height=146
	width=120
	self.FALU_SHU1Line=LineWidget(3,height-25,width,20,6,30,height-25-20+30)
	self.gridLay.addWidget(self.FALU_SHU1Line,10,7,6,2)
	self.SHU1_FALULine=LineWidget(3,height-20,width,25,7,30,height-20-25+30)
	self.gridLay.addWidget(self.SHU1_FALULine,10,7,6,2)

	#define the line between BIU1Button and SHU1Button,7rows,2columns
	height=171
	width=120
	self.BIU1_SHU1Line=LineWidget(3,20,width,height-20,4,10,height-20-20+10)
	self.gridLay.addWidget(self.BIU1_SHU1Line,9,4,7,2)
	self.SHU1_BIU1Line=LineWidget(3,25,width,height-15,5,10,height-15-25+10)
	self.gridLay.addWidget(self.SHU1_BIU1Line,9,4,7,2)

	#define the line between BIU1Button and SHU0Button,8rows,2columns
	height=196
	width=120
	self.SHU0_BIU1Line=LineWidget(3,height-25,width,25,6,30,height-25-25+30)
	self.gridLay.addWidget(self.SHU0_BIU1Line,2,4,8,2)
	self.BIU1_SHU0Line=LineWidget(3,height-20,width,30,7,30,height-20-30+30)
	self.gridLay.addWidget(self.BIU1_SHU0Line,2,4,8,2)

	#define the line between SHU1Button and IMACButton,9rows,2columns
	height=219
	width=120
	self.IMAC_SHU1Line=LineWidget(3,height-35,width,25,6,10,height-35-25+10)
	self.gridLay.addWidget(self.IMAC_SHU1Line,7,7,9,2)
	self.SHU1_IMACLine=LineWidget(3,height-30,width,30,7,10,height-30-30+10)
	self.gridLay.addWidget(self.SHU1_IMACLine,7,7,9,2)	

	#define the line between SHU0Button and FALUButton,9rows,2columns
	height=219
	width=120
	self.SHU0_FALULine=LineWidget(3,20,width,height-20,4,10,height-20-20+10)
	self.gridLay.addWidget(self.SHU0_FALULine,2,7,9,2)
	self.FALU_SHU0Line=LineWidget(3,25,width,height-15,5,10,height-15-25+10)
	self.gridLay.addWidget(self.FALU_SHU0Line,2,7,9,2)

	#define the line between BIU2Button and SHU0Button,11rows,2columns
	height=269
	width=120
	self.SHU0_BIU2Line=LineWidget(3,height-35,width,25,6,10,height-35-25+10)
	self.gridLay.addWidget(self.SHU0_BIU2Line,2,4,11,2)
	self.BIU2_SHU0Line=LineWidget(3,height-30,width,30,7,10,height-30-30+10)
	self.gridLay.addWidget(self.BIU2_SHU0Line,2,4,11,2)

	#define the line between SHU0Button and FMACButton,12rows,2columns
	height=292
	width=120
	self.SHU0_FMACLine=LineWidget(3,20,width,height-20,4,10,height-20-20+10)
	self.gridLay.addWidget(self.SHU0_FMACLine,2,7,12,2)
	self.FMAC_SHU0Line=LineWidget(3,25,width,height-15,5,10,height-15-25+10)
	self.gridLay.addWidget(self.FMAC_SHU0Line,2,7,12,2)

	#define the line between SHU1Button and IALUButton,12rows,2column
	height=292
	width=120
	self.IALU_SHU1Line=LineWidget(3,height-35,width,25,6,10,height-35-25+10)
	self.gridLay.addWidget(self.IALU_SHU1Line,4,7,12,2)
	self.SHU1_IALULine=LineWidget(3,height-30,width,30,7,10,height-30-30+10)
	self.gridLay.addWidget(self.SHU1_IALULine,4,7,12,2)

	#define the line between BIU0Button and IALUButton,3rows,5columns
	height=73
	width=310
	self.IALU_BIU0Line=LineWidget(3,height-20,width,15,0,100,height-20-15+100)
	self.gridLay.addWidget(self.IALU_BIU0Line,4,4,3,5)
	self.BIU0_IALULine=LineWidget(3,height-15,width,20,1,100,height-15-20+100)
	self.gridLay.addWidget(self.BIU0_IALULine,4,4,3,5)	

	#define the line between BIU1Button and IMACButton,3rows,5columns
	height=73
	width=310
	self.IMAC_BIU1Line=LineWidget(3,height-20,width,15,0,100,height-20-15+100)
	self.gridLay.addWidget(self.IMAC_BIU1Line,7,4,3,5)
	self.BIU1_IMACLine=LineWidget(3,height-15,width,20,1,100,height-15-20+100)
	self.gridLay.addWidget(self.BIU1_IMACLine,7,4,3,5)

	#define the line between BIU2Button and FALUButton,3rows,5columns
	height=73
	width=310
	self.FALU_BIU2Line=LineWidget(3,height-20,width,15,0,100,height-20-15+100)
	self.gridLay.addWidget(self.FALU_BIU2Line,10,4,3,5)
	self.BIU2_FALULine=LineWidget(3,height-15,width,20,1,100,height-15-20+100)
	self.gridLay.addWidget(self.BIU2_FALULine,10,4,3,5)

	#define the line between BIU0Button and IMACButton,2rows,5columns
	height=48
	width=310
	self.BIU0_IMACLine=LineWidget(3,20,width,height-20,0,100,height-20-20+100)
	self.gridLay.addWidget(self.BIU0_IMACLine,6,4,2,5)
	self.IMAC_BIU0Line=LineWidget(3,25,width,height-15,1,100,height-15-25+100)
	self.gridLay.addWidget(self.IMAC_BIU0Line,6,4,2,5)

	#define the line between BIU1Button and FALUButton,2rows,5columns
	height=48
	width=310
	self.BIU1_FALULine=LineWidget(3,20,width,height-10,0,100,height-10-20+100)
	self.gridLay.addWidget(self.BIU1_FALULine,9,4,2,5)
	self.FALU_BIU1Line=LineWidget(3,25,width,height-5,1,100,height-5-25+100)
	self.gridLay.addWidget(self.FALU_BIU1Line,9,4,2,5)

	#define the line between BIU2Button and FMACButton,2rows,5columns
	height=48
	width=310
	self.BIU2_FMACLine=LineWidget(3,20,width,height-10,0,100,height-10-20+100)
	self.gridLay.addWidget(self.BIU2_FMACLine,12,4,2,5)
	self.FMAC_BIU2Line=LineWidget(3,25,width,height-5,1,100,height-5-25+100)
	self.gridLay.addWidget(self.FMAC_BIU2Line,12,4,2,5)

	#define the line between BIU1Button and FMACButton,5rows,5columns
	height=122
	width=310
	self.BIU1_FMACLine=LineWidget(3,10,width,height-20,0,100,height-20-10+100)
	self.gridLay.addWidget(self.BIU1_FMACLine,9,4,5,5)
	self.FMAC_BIU1Line=LineWidget(3,15,width,height-15,1,100,height-15-15+100)
	self.gridLay.addWidget(self.FMAC_BIU1Line,9,4,5,5)

	#define the line between BIU1Button and IALUButton,6rows,5columns
	height=146
	width=310
	self.IALU_BIU1Line=LineWidget(3,height-20,width,15,0,100,height-20-15+100)
	self.gridLay.addWidget(self.IALU_BIU1Line,4,4,6,5)
	self.BIU1_IALULine=LineWidget(3,height-15,width,20,1,100,height-15-20+100)
	self.gridLay.addWidget(self.BIU1_IALULine,4,4,6,5)	

	#define the line between BIU0Button and FALUButton,5rows,5columns
	height=122
	width=310
	self.BIU0_FALULine=LineWidget(3,10,width,height-20,0,150,height-20-10+150)
	self.gridLay.addWidget(self.BIU0_FALULine,6,4,5,5)	
	self.FALU_BIU0Line=LineWidget(3,15,width,height-15,1,150,height-15-15+150)
	self.gridLay.addWidget(self.FALU_BIU0Line,6,4,5,5)	

	#define the line between BIU0Button and FMACButton,8rows,5columns
	height=196
	width=310
	self.BIU0_FMACLine=LineWidget(3,10,width,height-20,0,10,height-20-10+10)
	self.gridLay.addWidget(self.BIU0_FMACLine,6,4,8,5)
	self.FMAC_BIU0Line=LineWidget(3,15,width,height-15,1,10,height-15-15+10)
	self.gridLay.addWidget(self.FMAC_BIU0Line,6,4,8,5)


	#define the line between BIU2Button and IALUButton,9rows,5columns
	height=219
	width=310
	self.BIU2_IALULine=LineWidget(3,height-20,width,15,0,100,height-20-15+100)
	self.gridLay.addWidget(self.BIU2_IALULine,4,4,9,5)
	self.IALU_BIU2Line=LineWidget(3,height-15,width,20,1,100,height-15-20+100)
	self.gridLay.addWidget(self.IALU_BIU2Line,4,4,9,5)

	#define the line between BIU2Button and IMACButton,6rows,5columns
	height=146
	width=310
	self.BIU2_IMACLine=LineWidget(3,height-20,width,15,0,100,height-20-15+100)
	self.gridLay.addWidget(self.BIU2_IMACLine,7,4,6,5)
	self.IMAC_BIU2Line=LineWidget(3,height-15,width,20,1,100,height-15-20+100)
	self.gridLay.addWidget(self.IMAC_BIU2Line,7,4,6,5)

	#define the line between IALUButton and FALUButton,7rows,1column
	height=171
	width=70	
	self.IALU_FALULine=LineWidget(3,20,3,height-20,8,0,0)
	self.gridLay.addWidget(self.IALU_FALULine,4,10,7,1)
	self.FALU_IALULine=LineWidget(3,10,3,height-10,9,0,0)
	self.gridLay.addWidget(self.FALU_IALULine,4,10,7,1)
	
	#make MRFButton show on the lineswidget
	self.gridLay.addWidget(self.MRFButton,9,6)

	
