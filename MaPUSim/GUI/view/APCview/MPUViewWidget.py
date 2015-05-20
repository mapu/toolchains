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

	self.DMButton=QPushButton(self.tr("DM"))
	self.DMButton.setFixedWidth(60)
	self.DMButton.setEnabled(False)

	self.blank0=QLabel()
	self.blank0.setFixedWidth(70)

	self.BIU0Button=QPushButton(self.tr("BIU0"))
	self.BIU0Button.setFixedWidth(60)
	#self.BIU0Button.setWindowFlags(Qt.WindowStaysOnTopHint)
	self.BIU1Button=QPushButton(self.tr("BIU1"))
	self.BIU1Button.setFixedWidth(60)
	self.BIU2Button=QPushButton(self.tr("BIU2"))
	self.BIU2Button.setFixedWidth(60)

	self.blank1=QLabel()
	self.blank1.setFixedWidth(70)

	self.SHU0Button=QPushButton(self.tr("SHU0"))
	self.SHU0Button.setFixedWidth(60)
	self.MRFButton=QPushButton(self.tr("M RF"))
	self.MRFButton.setFixedWidth(60)
	self.SHU1Button=QPushButton(self.tr("SHU1"))
	self.SHU1Button.setFixedWidth(60)

	self.byteComboBox=QComboBox()
	self.byteComboBox.setFixedWidth(80)

	self.byteComboBox.insertItem(0,"8 Bytes")
	self.byteComboBox.insertItem(1,"7 Bytes")
	self.byteComboBox.insertItem(2,"6 Bytes")
	self.byteComboBox.insertItem(3,"5 Bytes")
	self.byteComboBox.insertItem(4,"4 Bytes")
	self.byteComboBox.insertItem(5,"3 Bytes")
	self.byteComboBox.insertItem(6,"2 Bytes")
	self.byteComboBox.insertItem(7,"1 Bytes")

	self.laneComboBox=QComboBox()
	self.laneComboBox.setFixedWidth(80)
	self.laneComboBox.insertItem(0,"Lane0")
	self.laneComboBox.insertItem(1,"Lane1")
	self.laneComboBox.insertItem(2,"Lane2")
	self.laneComboBox.insertItem(3,"Lane3")	
	self.IALUButton=QPushButton(self.tr("IALU"))
	self.IALUButton.setFixedWidth(60)
	self.IMACButton=QPushButton(self.tr("IMAC"))
	self.IMACButton.setFixedWidth(60)
	self.FALUButton=QPushButton(self.tr("FALU"))
	self.FALUButton.setFixedWidth(60)
	self.FMACButton=QPushButton(self.tr("FMAC"))
	self.FMACButton.setFixedWidth(60)

	#layout left Widget
	self.gridLay=QGridLayout()
	self.gridLay.addWidget(self.DMButton,5,0)
	self.gridLay.addWidget(self.blank0,0,1)
	self.gridLay.addWidget(self.BIU0Button,3,2)
	self.gridLay.addWidget(self.BIU1Button,5,2)
	self.gridLay.addWidget(self.BIU2Button,7,2)
	self.gridLay.addWidget(self.blank1,0,3)
	self.gridLay.addWidget(self.SHU0Button,1,4)
	self.gridLay.addWidget(self.MRFButton,5,4)
	self.gridLay.addWidget(self.SHU1Button,9,4)
	self.gridLay.addWidget(self.byteComboBox,0,5)
	self.gridLay.addWidget(self.laneComboBox,0,6)
	self.gridLay.addWidget(self.IALUButton,2,6)
	self.gridLay.addWidget(self.IMACButton,4,6)
	self.gridLay.addWidget(self.FALUButton,6,6)
	self.gridLay.addWidget(self.FMACButton,8,6)
	self.gridLay.setAlignment(Qt.AlignCenter)
	self.leftWidget.setLayout(self.gridLay)
	
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

 	#define slot function
    def DMButtonSlot(self):
	self.DMButtonDialog=FloatDialog()
	self.DMButtonDialog.setWindowTitle(self.tr("DM Stages"))
	self.DMButtonDialog.show()

    def BIU0ButtonSlot(self):
	self.BIU0ButtonDialog=FloatDialog()
	self.BIU0ButtonDialog.setWindowTitle(self.tr("BIU0 Stages"))	
	self.BIU0ButtonDialog.show()

    def BIU1ButtonSlot(self):
	self.BIU1ButtonDialog=FloatDialog()
	self.BIU1ButtonDialog.setWindowTitle(self.tr("BIU1 Stages"))	
	self.BIU1ButtonDialog.show()

    def BIU2ButtonSlot(self):
	self.BIU2ButtonDialog=FloatDialog()
	self.BIU2ButtonDialog.setWindowTitle(self.tr("BIU2 Stages"))	
	self.BIU2ButtonDialog.show()

    def SHU0ButtonSlot(self):
	self.SHU0ButtonDialog=FloatDialog()
	self.SHU0ButtonDialog.setWindowTitle(self.tr("SHU0 Stages"))	
	self.SHU0ButtonDialog.show()

    def SHU1ButtonSlot(self):
	self.SHU1ButtonDialog=FloatDialog()
	self.SHU1ButtonDialog.setWindowTitle(self.tr("SHU1 Stages"))	
	self.SHU1ButtonDialog.show()

    def MRFButtonSlot(self):
	self.MRFButtonDialog=FloatDialog()
	self.MRFButtonDialog.setWindowTitle(self.tr("MRF Stages"))	
	self.MRFButtonDialog.show()

    def IALUButtonSlot(self):
	self.IALUButtonDialog=FloatDialog()
	self.IALUButtonDialog.setWindowTitle(self.tr("IALU Stages"))	
	self.IALUButtonDialog.show()

    def IMACButtonSlot(self):
	self.IMACButtonDialog=FloatDialog()
	self.IMACButtonDialog.setWindowTitle(self.tr("IMAC Stages"))	
	self.IMACButtonDialog.show()

    def FALUButtonSlot(self):
	self.FALUButtonDialog=FloatDialog()
	self.FALUButtonDialog.setWindowTitle(self.tr("FALU Stages"))	
	self.FALUButtonDialog.show()

    def FMACButtonSlot(self):
	self.FMACButtonDialog=FloatDialog()
	self.FMACButtonDialog.setWindowTitle(self.tr("FMAC Stages"))	
	self.FMACButtonDialog.show()


    def paintEvent(self,event):
	#define the line between DMButton and BIU0Button
	height=self.DMButton.geometry().y()-self.BIU0Button.geometry().y()-self.BIU0Button.height()
	width=self.BIU0Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.DM_BIU0Line=LineWidget(length,width,height,180-angle*180,True)
	self.gridLay.addWidget(self.DM_BIU0Line,3,1,2,1)
	self.BIU0_DMLine=LineWidget(length,width,height,180-angle*180,False)
	self.gridLay.addWidget(self.BIU0_DMLine,3,1,2,1)

	#define the line between DMButton and BIU1Button
	self.DM_BIU1Line=LineWidget(self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),7,0,True)
	self.gridLay.addWidget(self.DM_BIU1Line,5,1)
	self.BIU1_DMLine=LineWidget(self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),7,0,False)
	self.gridLay.addWidget(self.BIU1_DMLine,5,1)
	
	#define the line between DMButton and BIU2Button
	height=self.BIU2Button.geometry().y()-self.DMButton.geometry().y()-self.DMButton.height()
	width=self.BIU2Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.DM_BIU2Line=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.DM_BIU2Line,6,1,2,1)
	self.BIU2_DMLine=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.BIU2_DMLine,6,1,2,1)

	#define the line between BIU0Button and SHU0Button
	height=self.BIU0Button.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.height()
	width=self.SHU0Button.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU0_SHU0Line=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.BIU0_SHU0Line,1,3,2,1)
	self.SHU0_BIU0Line=LineWidget(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.SHU0_BIU0Line,1,3,2,1)

	#define the line between BIU0Button and MRFButton
	height=self.MRFButton.geometry().y()-self.BIU0Button.geometry().y()-self.BIU0Button.height()
	width=self.MRFButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU0_MRFLine=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.BIU0_MRFLine,4,3,2,1)
	self.MRF_BIU0Line=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.MRF_BIU0Line,4,3,2,1)

	#define the line between BIU1Button and MRFButton
	self.BIU1_MRFLine=LineWidget(self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),7,0,True)
	self.gridLay.addWidget(self.BIU1_MRFLine,5,3)
	self.MRF_BIU1Line=LineWidget(self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),7,0,False)
	self.gridLay.addWidget(self.MRF_BIU1Line,5,3)

	#define the line between BIU2Button and MRFButton 
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.MRF_BIU2Line=LineWidget(length,width,height,180-angle*180,True)
	self.gridLay.addWidget(self.MRF_BIU2Line,6,3,2,1)
	self.BIU2_MRFLine=LineWidget(length,width,height,180-angle*180,False)
	self.gridLay.addWidget(self.BIU2_MRFLine,6,3,2,1)	 

	#define the line between BIU2Button and SHU1Button
	height=self.SHU1Button.geometry().y()-self.BIU2Button.geometry().y()-self.BIU2Button.height()
	width=self.SHU1Button.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU2_SHU1Line=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.BIU2_SHU1Line,8,3,2,1)
	self.SHU1_BIU2Line=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.SHU1_BIU2Line,8,3,2,1)

	#define the line between SHU0Button and MRFButton 
	self.SHU0_MRFLine=LineWidget(self.MRFButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.geometry().height(),4,self.MRFButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.geometry().height(),90,True)
	self.SHU0_MRFLine.setMaximumWidth(8)
	self.SHU0_MRFLine.setMaximumHeight(self.MRFButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.geometry().height()+7)

	self.MRF_SHU0Line=LineWidget(self.MRFButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.geometry().height(),4,self.MRFButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.geometry().height(),90,False)
	self.MRF_SHU0Line.setMaximumWidth(8)
	self.MRF_SHU0Line.setMaximumHeight(self.MRFButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.geometry().height()+7)	

	SHU0_MRFLayout=QHBoxLayout()
	SHU0_MRFLayout.addWidget(self.SHU0_MRFLine)
	SHU0_MRFLayout.addWidget(self.MRF_SHU0Line)
	self.gridLay.addLayout(SHU0_MRFLayout,1,4,5,1)

	#define the line between MRFButton and SHU1Button
	self.MRF_SHU1Line=LineWidget(self.SHU1Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height(),4,self.SHU1Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height(),90,True)
	self.MRF_SHU1Line.setMaximumWidth(8)
	self.MRF_SHU1Line.setMaximumHeight(self.SHU1Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()+7)
	self.SHU1_MRFLine=LineWidget(self.SHU1Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height(),4,self.SHU1Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height(),90,False)
	self.SHU1_MRFLine.setMaximumWidth(8)
	self.SHU1_MRFLine.setMaximumHeight(self.SHU1Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()+7)
	SHU1_MRFLayout=QHBoxLayout()
	SHU1_MRFLayout.addWidget(self.MRF_SHU1Line)
	SHU1_MRFLayout.addWidget(self.SHU1_MRFLine)
	self.gridLay.addLayout(SHU1_MRFLayout,5,4,5,1)
	
	#define the line between SHU0Button and IALUButton(borrow other line parameters to do)
	height=self.SHU1Button.geometry().y()-self.BIU2Button.geometry().y()-self.BIU2Button.height()
	width=self.SHU1Button.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.SHU0_IALULine=LineWidget(length+10,width,height,angle*180,True)
	self.gridLay.addWidget(self.SHU0_IALULine,1,5,2,1)
	self.IALU_SHU0Line=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.IALU_SHU0Line,1,5,2,1)

	#define the line between SHU0Button and IMACButton(borrow other line parameters to do)
	height=self.SHU1Button.geometry().y()-self.BIU2Button.geometry().y()-self.BIU2Button.height()
	width=self.SHU1Button.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.SHU0_IMACLine=LineWidget(length+10,width,height,angle*180,True)
	self.gridLay.addWidget(self.SHU0_IMACLine,2,5,2,1)
	self.IMAC_SHU0Line=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.IMAC_SHU0Line,2,5,2,1)

	#define the line between MRFButton and IMACButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.IMAC_MRFLine=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.IMAC_MRFLine,4,5,2,1)
	self.MRF_IMACLine=LineWidget(length,self.IMACButton.geometry().x()-self.MRFButton.geometry().x()-self.MRFButton.width(),height,180-180*angle,False)
	self.gridLay.addWidget(self.MRF_IMACLine,4,5,2,1)

	#define the line between MRFButton and IALUButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.IALU_MRFLine=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.IALU_MRFLine,3,5,2,1)
	self.MRF_IALULine=LineWidget(length,self.IALUButton.geometry().x()-self.MRFButton.geometry().x()-self.MRFButton.width(),height,180-180*angle,False)
	self.gridLay.addWidget(self.MRF_IALULine,3,5,2,1)

	#define the line between MRFButton and FALUButton(borrow other line parameters to do)
	height=self.SHU1Button.geometry().y()-self.BIU2Button.geometry().y()-self.BIU2Button.height()
	width=self.SHU1Button.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.MRF_FALULine=LineWidget(length+10,width,height,angle*180,True)
	self.gridLay.addWidget(self.MRF_FALULine,5,5,2,1)
	self.FALU_MRFLine=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.FALU_MRFLine,5,5,2,1)

	#define the line between MRFButton and FMACButton(borrow other line parameters to do)
	height=self.SHU1Button.geometry().y()-self.BIU2Button.geometry().y()-self.BIU2Button.height()
	width=self.SHU1Button.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.MRF_FMACLine=LineWidget(length+10,width,height,angle*180,True)
	self.gridLay.addWidget(self.MRF_FMACLine,6,5,2,1)
	self.FMAC_MRFLine=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.FMAC_MRFLine,6,5,2,1)	

	#define the line between SHU1Button and FMACButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.FMAC_SHU1Line=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.FMAC_SHU1Line,8,5,2,1)
	self.SHU1_FMACLine=LineWidget(length,self.FMACButton.geometry().x()-self.SHU1Button.geometry().x()-self.SHU1Button.width(),height,180-180*angle,False)
	self.gridLay.addWidget(self.SHU1_FMACLine,8,5,2,1)

	#define the line between SHU1Button and FALUButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.FALU_SHU1Line=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.FALU_SHU1Line,7,5,2,1)
	self.SHU1_FALULine=LineWidget(length,self.FALUButton.geometry().x()-self.SHU1Button.geometry().x()-self.SHU1Button.width(),height,180-180*angle,False)
	self.gridLay.addWidget(self.SHU1_FALULine,7,5,2,1)
		
	#define the line between IALUButton and IMACButton
	self.IALU_IMACLine=LineWidget(self.IMACButton.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height(),4,self.IMACButton.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height(),90,True)
	self.IALU_IMACLine.setMaximumWidth(8)
	self.IALU_IMACLine.setMaximumHeight(self.IMACButton.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()+7)
	self.IMAC_IALULine=LineWidget(self.IMACButton.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height(),4,self.IMACButton.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height(),90,False)
	self.IMAC_IALULine.setMaximumWidth(8)
	self.IMAC_IALULine.setMaximumHeight(self.IMACButton.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()+7)
	IALU_IMACLayout=QHBoxLayout()
	IALU_IMACLayout.addWidget(self.IALU_IMACLine)
	IALU_IMACLayout.addWidget(self.IMAC_IALULine)
	self.gridLay.addLayout(IALU_IMACLayout,2,6,3,1)

	#define the line between IMACButton and FALUButton
	self.IMAC_FALULine=LineWidget(self.FALUButton.geometry().y()-self.IMACButton.geometry().y()-self.IMACButton.height(),4,self.FALUButton.geometry().y()-self.IMACButton.geometry().y()-self.IMACButton.height(),90,True)
	self.IMAC_FALULine.setMaximumWidth(8)
	self.IMAC_FALULine.setMaximumHeight(self.FALUButton.geometry().y()-self.IMACButton.geometry().y()-self.IMACButton.height()+7)
	self.FALU_IMACLine=LineWidget(self.FALUButton.geometry().y()-self.IMACButton.geometry().y()-self.IMACButton.height(),4,self.FALUButton.geometry().y()-self.IMACButton.geometry().y()-self.IMACButton.height(),90,False)
	self.FALU_IMACLine.setMaximumWidth(8)
	self.FALU_IMACLine.setMaximumHeight(self.FALUButton.geometry().y()-self.IMACButton.geometry().y()-self.IMACButton.height()+7)
	IMAC_FALULayout=QHBoxLayout()
	IMAC_FALULayout.addWidget(self.IMAC_FALULine)
	IMAC_FALULayout.addWidget(self.FALU_IMACLine)
	self.gridLay.addLayout(IMAC_FALULayout,4,6,3,1)

	#define the line between FALUButton and FMACButton
	self.FALU_FMACLine=LineWidget(self.FMACButton.geometry().y()-self.FALUButton.geometry().y()-self.FALUButton.height(),4,self.FMACButton.geometry().y()-self.FALUButton.geometry().y()-self.FALUButton.height(),90,True)
	self.FALU_FMACLine.setMaximumWidth(8)
	self.FALU_FMACLine.setMaximumHeight(self.FMACButton.geometry().y()-self.FALUButton.geometry().y()-self.FALUButton.height()+7)
	self.FMAC_FALULine=LineWidget(self.FMACButton.geometry().y()-self.FALUButton.geometry().y()-self.FALUButton.height(),4,self.FMACButton.geometry().y()-self.FALUButton.geometry().y()-self.FALUButton.height(),90,False)
	self.FMAC_FALULine.setMaximumWidth(8)
	self.FMAC_FALULine.setMaximumHeight(self.FMACButton.geometry().y()-self.FALUButton.geometry().y()-self.FALUButton.height()+7)
	FALU_FMACLayout=QHBoxLayout()
	FALU_FMACLayout.addWidget(self.FALU_FMACLine)
	FALU_FMACLayout.addWidget(self.FMAC_FALULine)
	self.gridLay.addLayout(FALU_FMACLayout,6,6,3,1)


