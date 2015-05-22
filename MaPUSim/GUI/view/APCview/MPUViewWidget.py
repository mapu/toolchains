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
	self.DMButton.setFixedWidth(70)
	self.DMButton.setEnabled(False)
	self.BIU0Button=QPushButton(self.tr("BIU0"))
	self.BIU0Button.setFixedWidth(70)
	self.BIU1Button=QPushButton(self.tr("BIU1"))
	self.BIU1Button.setFixedWidth(70)
	self.BIU2Button=QPushButton(self.tr("BIU2"))
	self.BIU2Button.setFixedWidth(70)
	self.SHU0Button=QPushButton(self.tr("SHU0"))
	self.SHU0Button.setFixedWidth(70)
	self.MRFButton=QPushButton(self.tr("M RF"))
	self.MRFButton.setFixedWidth(70)
	self.SHU1Button=QPushButton(self.tr("SHU1"))
	self.SHU1Button.setFixedWidth(70)
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
	self.IALUButton.setFixedWidth(70)
	self.IMACButton=QPushButton(self.tr("IMAC"))
	self.IMACButton.setFixedWidth(70)
	self.FALUButton=QPushButton(self.tr("FALU"))
	self.FALUButton.setFixedWidth(70)
	self.FMACButton=QPushButton(self.tr("FMAC"))
	self.FMACButton.setFixedWidth(70)

	self.blank0=QLabel()
	self.blank0.setFixedWidth(70)
	self.blank1=QLabel()
	self.blank1.setFixedWidth(70)
	self.blank2=QLabel()
	self.blank2.setFixedWidth(70)
	self.blank3=QLabel()
	self.blank3.setFixedWidth(70)
	self.blank4=QLabel()
	self.blank4.setFixedWidth(70)
	self.blank5=QLabel()
	self.blank5.setFixedWidth(70)
	self.blank6=QLabel()
	self.blank6.setFixedWidth(70)

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
	self.gridLay.addWidget(self.MRFButton,9,6)
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
	self.paintLines()

    def paintLines(self):
	#define the line between DMButton and BIU0Button
	height=self.DMButton.geometry().y()-self.BIU0Button.geometry().y()-self.BIU0Button.height()
	width=self.BIU0Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.DM_BIU0Line=LineWidget(length,width,height,180-angle*180,True)
	self.gridLay.addWidget(self.DM_BIU0Line,6,1,3,2)
	self.BIU0_DMLine=LineWidget(length,width,height,180-angle*180,False)
	self.gridLay.addWidget(self.BIU0_DMLine,6,1,3,2)
	self.DM_BIU0Edit=QLineEdit()
	self.DM_BIU0Edit.setFixedWidth(70)
	self.gridLay.addWidget(self.DM_BIU0Edit,6,1,1,2)
	self.BIU0_DMEdit=QLineEdit()
	self.BIU0_DMEdit.setFixedWidth(70)
	self.gridLay.addWidget(self.BIU0_DMEdit,8,2,1,2)

	#define the line between DMButton and BIU1Button
	self.DM_BIU1Line=LineWidget(self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),7,0,True)
	self.gridLay.addWidget(self.DM_BIU1Line,9,1,1,2)
	self.BIU1_DMLine=LineWidget(self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),self.BIU1Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width(),7,0,False)
	self.gridLay.addWidget(self.BIU1_DMLine,9,1,1,2)
	
	#define the line between DMButton and BIU2Button
	height=self.BIU2Button.geometry().y()-self.DMButton.geometry().y()-self.DMButton.height()
	width=self.BIU2Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.DM_BIU2Line=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.DM_BIU2Line,10,1,3,2)
	self.BIU2_DMLine=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.BIU2_DMLine,10,1,3,2)

	#define the line between BIU0Button and SHU0Button(borrow other line parameters to do)
	height=self.DMButton.geometry().y()-self.BIU0Button.geometry().y()-self.BIU0Button.height()
	width=self.BIU0Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.BIU0_SHU0Line=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.BIU0_SHU0Line,3,4,3,2)
	self.SHU0_BIU0Line=LineWidget(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.SHU0_BIU0Line,3,4,3,2)

	#define the line between BIU0Button and MRFButton
	height=self.MRFButton.geometry().y()-self.BIU0Button.geometry().y()-self.BIU0Button.height()
	width=self.MRFButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU0_MRFLine=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.BIU0_MRFLine,7,4,3,2)
	self.MRF_BIU0Line=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.MRF_BIU0Line,7,4,3,2)

	#define the line between BIU1Button and MRFButton
	self.BIU1_MRFLine=LineWidget(self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),7,0,True)
	self.gridLay.addWidget(self.BIU1_MRFLine,9,4,1,2)
	self.MRF_BIU1Line=LineWidget(self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),self.MRFButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width(),7,0,False)
	self.gridLay.addWidget(self.MRF_BIU1Line,9,4,1,2)

	#define the line between BIU2Button and MRFButton 
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.MRF_BIU2Line=LineWidget(length,width,height,180-angle*180,True)
	self.gridLay.addWidget(self.MRF_BIU2Line,10,4,3,2)
	self.BIU2_MRFLine=LineWidget(length,width,height,180-angle*180,False)
	self.gridLay.addWidget(self.BIU2_MRFLine,10,4,3,2)	 

	#define the line between BIU2Button and SHU1Button
	height=self.BIU2Button.geometry().y()-self.DMButton.geometry().y()-self.DMButton.height()
	width=self.BIU2Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU2_SHU1Line=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.BIU2_SHU1Line,13,4,3,2)
	self.SHU1_BIU2Line=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.SHU1_BIU2Line,13,4,3,2)

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
	self.gridLay.addLayout(SHU0_MRFLayout,2,6,8,1)

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
	self.gridLay.addLayout(SHU1_MRFLayout,9,6,7,1)
	
	#define the line between SHU0Button and IALUButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.DMButton.geometry().y()-self.DMButton.height()
	width=self.BIU2Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.SHU0_IALULine=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.SHU0_IALULine,2,7,3,2)
	self.IALU_SHU0Line=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.IALU_SHU0Line,2,7,3,2)

	#define the line between MRFButton and IMACButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.IMAC_MRFLine=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.IMAC_MRFLine,7,7,3,2)
	self.MRF_IMACLine=LineWidget(length,self.IMACButton.geometry().x()-self.MRFButton.geometry().x()-self.MRFButton.width(),height,180-180*angle,False)
	self.gridLay.addWidget(self.MRF_IMACLine,7,7,3,2)

	#define the line between MRFButton and FMACButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.DMButton.geometry().y()-self.DMButton.height()
	width=self.BIU2Button.geometry().x()-self.DMButton.geometry().x()-self.DMButton.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.MRF_FMACLine=LineWidget(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.MRF_FMACLine,10,7,3,2)
	self.FMAC_MRFLine=LineWidget(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.FMAC_MRFLine,10,7,3,2)	

	#define the line between SHU1Button and FMACButton(borrow other line parameters to do)
	height=self.BIU2Button.geometry().y()-self.MRFButton.geometry().y()-self.MRFButton.height()
	width=self.MRFButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.FMAC_SHU1Line=LineWidget(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.FMAC_SHU1Line,13,7,3,2)
	self.SHU1_FMACLine=LineWidget(length,self.FMACButton.geometry().x()-self.SHU1Button.geometry().x()-self.SHU1Button.width(),height,180-180*angle,False)
	self.gridLay.addWidget(self.SHU1_FMACLine,13,7,3,2)

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
	self.gridLay.addLayout(IALU_IMACLayout,4,9,4,1)

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
	self.gridLay.addLayout(IMAC_FALULayout,7,9,4,1)

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
	self.gridLay.addLayout(FALU_FMACLayout,10,9,4,1)

	#define the line between MRFButton and FALUButton
	height=self.FALUButton.geometry().y()-self.MRFButton.geometry().y()+self.FALUButton.height()/3
	width=self.FALUButton.geometry().x()-self.MRFButton.geometry().x()-self.MRFButton.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.MRF_FALULine=LineWidget02(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.MRF_FALULine,9,7,2,2)
	self.FALU_MRFLine=LineWidget02(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.FALU_MRFLine,9,7,3,2)

	#define the line between BIU0Button and SHU1Button (qi qiao)
	height=self.SHU1Button.geometry().y()-self.BIU0Button.geometry().y()-self.BIU0Button.height()
	width=self.SHU1Button.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	lenght=height/math.sin(angle)	
	self.BIU0_SHU1Line=LineWidget82(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.BIU0_SHU1Line,6,4,9,2)
	self.SHU1_BIU0Line=LineWidget82(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.SHU1_BIU0Line,6,4,9,2)

	#define the line between SHU0Button and IMACButton(qi qiao)
	height=self.IMACButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.height()
	width=self.IMACButton.geometry().x()-self.SHU0Button.geometry().x()-self.SHU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.SHU0_IMACLine=LineWidget42(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.SHU0_IMACLine,3,7,5,2)
	self.IMAC_SHU0Line=LineWidget42(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.IMAC_SHU0Line,3,7,5,2)

	#define the line between MRFButton and IALUButton(qi qiao)
	height=self.MRFButton.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()
	width=self.IALUButton.geometry().x()-self.MRFButton.geometry().x()-self.MRFButton.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.IALU_MRFLine=LineWidget42(length,width,height,180*angle,True)
	self.gridLay.addWidget(self.IALU_MRFLine,4,7,5,2)
	self.MRF_IALULine=LineWidget42(length,width,height,180*angle,False)
	self.gridLay.addWidget(self.MRF_IALULine,4,7,5,2)

	#define the line between SHU1Button and FALUButton(qi qiao)
	height=self.SHU1Button.geometry().y()-self.FALUButton.geometry().y()-self.FALUButton.height()
	width=self.FALUButton.geometry().x()-self.SHU1Button.geometry().x()-self.SHU1Button.width()
	angle=math.atan(float(height)/float(width)) #h/w
	length=height/math.sin(angle)  #h/sin(a)
	self.FALU_SHU1Line=LineWidget42(length,width,height,180*angle,True)
	self.gridLay.addWidget(self.FALU_SHU1Line,10,7,5,2)
	self.SHU1_FALULine=LineWidget42(length,width,height,180*angle,False)
	self.gridLay.addWidget(self.SHU1_FALULine,10,7,5,2)

	#define the line between BIU1Button and SHU1Button(borrow other line parameters to do)
	height=self.IMACButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.height()
	width=self.IMACButton.geometry().x()-self.SHU0Button.geometry().x()-self.SHU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU1_SHU1Line=LineWidget42(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.BIU1_SHU1Line,10,4,5,2)
	self.SHU1_BIU1Line=LineWidget42(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.SHU1_BIU1Line,10,4,5,2)	

	#define the line between BIU1Button and SHU0Button(qi qiao)
	height=self.BIU1Button.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.height()
	width=self.SHU0Button.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width()
	angle=math.atan(float(height)/float(width))
	lenght=height/math.sin(angle)
	self.SHU0_BIU1Line=LineWidget62(lenght,width,height,angle*180,True)
	self.gridLay.addWidget(self.SHU0_BIU1Line,3,4,7,2)
	self.BIU1_SHU0Line=LineWidget62(lenght,width,height,angle*180,False)
	self.gridLay.addWidget(self.BIU1_SHU0Line,3,4,7,2)

	#define the line between SHU1Button and IMACButton(qi qiao)
	height=self.SHU1Button.geometry().y()-self.IMACButton.geometry().y()-self.IMACButton.height()
	width=self.IMACButton.geometry().x()-self.SHU1Button.geometry().x()-self.SHU1Button.width()
	angle=math.atan(float(height)/float(width))
	lenght=height/math.sin(angle)
	self.IMAC_SHU1Line=LineWidget72(lenght,width,height,angle*180,True)
	self.gridLay.addWidget(self.IMAC_SHU1Line,7,7,8,2)
	self.SHU1_IMACLine=LineWidget72(lenght,width,height,angle*180,False)
	self.gridLay.addWidget(self.SHU1_IMACLine,7,7,8,2)	

	#define the line between SHU0Button and FALUButton(qi qiao)
	height=self.FALUButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.height()
	width=self.FALUButton.geometry().x()-self.SHU0Button.geometry().x()-self.SHU0Button.width()
	angle=math.atan(float(height)/float(width))
	lenght=height/math.sin(angle)
	self.SHU0_FALULine=LineWidget72(lenght,width,height,180-angle*180,True)
	self.gridLay.addWidget(self.SHU0_FALULine,3,7,8,2)
	self.FALU_SHU0Line=LineWidget72(lenght,width,height,180-angle*180,False)
	self.gridLay.addWidget(self.FALU_SHU0Line,3,7,8,2)

	#define the line between BIU2Button and SHU0Button(qi qiao)
	height=self.BIU2Button.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.height()
	width=self.SHU0Button.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.SHU0_BIU2Line=LineWidget92(length,width,height,180*angle,True)
	self.gridLay.addWidget(self.SHU0_BIU2Line,2,4,10,2)
	self.BIU2_SHU0Line=LineWidget92(length,width,height,180*angle,False)
	self.gridLay.addWidget(self.BIU2_SHU0Line,2,4,10,2)

	#define the line between SHU0Button and FMACButton(qi qiao)
	height=self.FMACButton.geometry().y()-self.SHU0Button.geometry().y()-self.SHU0Button.height()
	width=self.FMACButton.geometry().x()-self.SHU0Button.geometry().x()-self.SHU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.SHU0_FMACLine=LineWidget102(length,width,height,70,True)
	self.gridLay.addWidget(self.SHU0_FMACLine,3,7,11,2)
	self.FMAC_SHU0Line=LineWidget102(lenght,width,height,70,False)
	self.gridLay.addWidget(self.FMAC_SHU0Line,3,7,11,2)

	#define the line between SHU1Button and IALUButton(qi qiao)
	height=self.SHU1Button.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()
	width=self.IALUButton.geometry().x()-self.SHU1Button.geometry().x()-self.SHU1Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.IALU_SHU1Line=LineWidget102(length,width,height,110,True)
	self.gridLay.addWidget(self.IALU_SHU1Line,4,7,11,2)
	self.SHU1_IALULine=LineWidget102(length,width,height,110,False)
	self.gridLay.addWidget(self.SHU1_IALULine,5,7,11,2)

	#define the line between BIU0Button and IALUButton
	height=self.BIU0Button.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()
	width=self.IALUButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.IALU_BIU0Line=LineWidget15(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.IALU_BIU0Line,4,4,2,5)
	self.BIU0_IALULine=LineWidget15(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.BIU0_IALULine,4,4,2,5)	

	#define the line between BIU1Button and IMACButton
	height=self.BIU0Button.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()
	width=self.IALUButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.IMAC_BIU1Line=LineWidget15(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.IMAC_BIU1Line,7,4,2,5)
	self.BIU1_IMACLine=LineWidget15(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.BIU1_IMACLine,7,4,2,5)

	#define the line between BIU2Button and FALUButton
	height=self.BIU0Button.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()
	width=self.IALUButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.FALU_BIU2Line=LineWidget15(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.FALU_BIU2Line,10,4,2,5)
	self.BIU2_FALULine=LineWidget15(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.BIU2_FALULine,10,4,2,5)

	#define the line between BIU0Button and IMACButton
	height=self.IMACButton.geometry().y()-self.BIU0Button.geometry().y()
	width=self.IMACButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU0_IMACLine=LineWidget05(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.BIU0_IMACLine,7,4,1,5)
	self.IMAC_BIU0Line=LineWidget05(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.IMAC_BIU0Line,7,4,1,5)

	#define the line between BIU1Button and FALUButton
	height=self.IMACButton.geometry().y()-self.BIU0Button.geometry().y()
	width=self.IMACButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU1_FALULine=LineWidget05(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.BIU1_FALULine,10,4,1,5)
	self.FALU_BIU1Line=LineWidget05(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.FALU_BIU1Line,10,4,1,5)

	#define the line between BIU2Button and FMACButton
	height=self.IMACButton.geometry().y()-self.BIU0Button.geometry().y()
	width=self.IMACButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU2_FMACLine=LineWidget05(length,width,height,angle*180,True)
	self.gridLay.addWidget(self.BIU2_FMACLine,13,4,1,5)
	self.FMAC_BIU2Line=LineWidget05(length,width,height,angle*180,False)
	self.gridLay.addWidget(self.FMAC_BIU2Line,13,4,1,5)

	#define the line between BIU1Button and FMACButton
	height=self.FMACButton.geometry().y()-self.BIU1Button.geometry().y()-self.BIU1Button.height()
	width=self.FMACButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.BIU1_FMACLine=LineWidget35(length,width,height,180*angle,True)
	self.gridLay.addWidget(self.BIU1_FMACLine,10,4,4,5)
	self.FMAC_BIU1Line=LineWidget35(length,width,height,180*angle,False)
	self.gridLay.addWidget(self.FMAC_BIU1Line,10,4,4,5)

	#define the line between BIU1Button and IALUButton
	height=self.BIU1Button.geometry().y()-self.IALUButton.geometry().y()-self.IALUButton.height()
	width=self.IALUButton.geometry().x()-self.BIU1Button.geometry().x()-self.BIU1Button.width()
	angle=math.atan(float(height)/float(width))
	length=height/math.sin(angle)
	self.IALU_BIU1Line=LineWidget45(length,width,height,180-180*angle,True)
	self.gridLay.addWidget(self.IALU_BIU1Line,4,4,5,5)
	self.BIU1_IALULine=LineWidget45(length,width,height,180-180*angle,False)
	self.gridLay.addWidget(self.BIU1_IALULine,4,4,5,5)	

	#define the line between IALUButton and FALUButton
	height=self.FALUButton.geometry().y()-self.IALUButton.geometry().y()+self.FALUButton.height()
	width=self.FALUButton.width()
	self.IALU_FALULine=LineWidgetB71(0,width,height,0,True)
	self.gridLay.addWidget(self.IALU_FALULine,4,10,7,1)
	self.FALU_IALULine=LineWidgetB71(0,width,height,0,False)
	self.gridLay.addWidget(self.FALU_IALULine,4,10,7,1)

	#define the line between BIU0Button and FALUButton
	#i
	height=self.FALUButton.geometry().y()-self.BIU0Button.geometry().y()+self.FALUButton.height()
	width=self.FALUButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	self.BIU0_FALULinei=LineWidgetBi55(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU0_FALULinei,6,4,1,5)
	self.FALU_BIU0Linei=LineWidgetBi55(0,width,height,0,False)
	self.gridLay.addWidget(self.FALU_BIU0Linei,6,4,1,5)
	self.BIU0_FALULinej=LineWidgetBj55(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU0_FALULinej,6,8,5,1)
	self.FALU_BIU0Linej=LineWidgetBj55(0,width,height,0,False)
	self.gridLay.addWidget(self.FALU_BIU0Linej,6,8,5,1)	

	#define the line between BIU0Button and FMACButton
	height=self.FMACButton.geometry().y()-self.BIU0Button.geometry().y()+self.FMACButton.height()
	width=self.FMACButton.geometry().x()-self.BIU0Button.geometry().x()-self.BIU0Button.width()
	self.BIU0_FMACLinei=LineWidgetBi85(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU0_FMACLinei,6,4,1,5)
	self.FMAC_BIU0Linei=LineWidgetBi85(0,width,height,0,False)
	self.gridLay.addWidget(self.FMAC_BIU0Linei,6,4,1,5)
	self.BIU0_FMACLinej=LineWidgetBj85(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU0_FMACLinej,6,8,8,1)
	self.FMAC_BIU0Linej=LineWidgetBj85(0,width,height,0,False)
	self.gridLay.addWidget(self.FMAC_BIU0Linej,6,8,8,1)

	#define the line between BIU2Button and IALUButton
	height=self.BIU2Button.geometry().y()-self.IALUButton.geometry().y()+self.BIU2Button.height()
	width=self.IALUButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	self.BIU2_IALULinei=LineWidgetBi95(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU2_IALULinei,4,4,9,1)
	self.IALU_BIU2Linei=LineWidgetBi95(0,width,height,0,False)
	self.gridLay.addWidget(self.IALU_BIU2Linei,4,4,9,1)
	self.BIU2_IALULinej=LineWidgetBj95(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU2_IALULinej,4,4,1,5)
	self.IALU_BIU2Linej=LineWidgetBj95(0,width,height,0,False)
	self.gridLay.addWidget(self.IALU_BIU2Linej,4,4,1,5)

	#define the line between BIU2Button and IMACButton
	height=self.BIU2Button.geometry().y()-self.IMACButton.geometry().y()+self.BIU2Button.height()
	width=self.IMACButton.geometry().x()-self.BIU2Button.geometry().x()-self.BIU2Button.width()
	self.BIU2_IMACLinei=LineWidgetBi65(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU2_IMACLinei,7,4,6,1)
	self.IMAC_BIU2Linei=LineWidgetBi65(0,width,height,0,False)
	self.gridLay.addWidget(self.IMAC_BIU2Linei,7,4,6,1)
	self.BIU2_IMACLinej=LineWidgetBj65(0,width,height,0,True)
	self.gridLay.addWidget(self.BIU2_IMACLinej,7,4,1,5)
	self.IMAC_BIU2Linej=LineWidgetBj65(0,width,height,0,False)
	self.gridLay.addWidget(self.IMAC_BIU2Linej,7,4,1,5)	


    def linesShow(self):
	#between DMButton and BIU0Button
	self.DM_BIU0Line.setVisible(False)
	self.BIU0_DMLine.setVisible(False)
	self.DM_BIU0Edit.setVisible(False)
	self.BIU0_DMEdit.setVisible(False)

	#between DMButton and BIU1Button
	self.DM_BIU1Line.setVisible(False)
	self.BIU1_DMLine.setVisible(False)
	
	#between DMButton and BIU2Button
	self.DM_BIU2Line.setVisible(False)
	self.BIU2_DMLine.setVisible(False)


	#between BIU0Button and SHU0Button
	self.BIU0_SHU0Line.setVisible(False)
	self.SHU0_BIU0Line.setVisible(False)

	#between BIU0Button and MRFButton
	self.BIU0_MRFLine.setVisible(False)
	self.MRF_BIU0Line.setVisible(False)

	#between BIU1Button and MRFButton
	self.BIU1_MRFLine.setVisible(False)
	self.MRF_BIU1Line.setVisible(False)

	#between BIU2Button and MRFButton 
	self.MRF_BIU2Line.setVisible(False)
	self.BIU2_MRFLine.setVisible(False) 

	#between BIU2Button and SHU1Button
	self.BIU2_SHU1Line.setVisible(False)
	self.SHU1_BIU2Line.setVisible(False)

	#between SHU0Button and MRFButton 
	self.SHU0_MRFLine.setVisible(False)
	self.MRF_SHU0Line.setVisible(False)

	#between MRFButton and SHU1Button
	self.MRF_SHU1Line.setVisible(False)
	self.SHU1_MRFLine.setVisible(False)
	
	#SHU0Button and IALUButton
	self.SHU0_IALULine.setVisible(False)
	self.IALU_SHU0Line.setVisible(False)

	#between MRFButton and IMACButton
	self.IMAC_MRFLine.setVisible(False)
	self.MRF_IMACLine.setVisible(False)

	#between MRFButton and FMACButton
	self.MRF_FMACLine.setVisible(False)
	self.FMAC_MRFLine.setVisible(False)	

	#between SHU1Button and FMACButton
	self.FMAC_SHU1Line.setVisible(False)
	self.SHU1_FMACLine.setVisible(False)

	#between IALUButton and IMACButton
	self.IALU_IMACLine.setVisible(False)
	self.IMAC_IALULine.setVisible(False)

	#between IMACButton and FALUButton
	self.IMAC_FALULine.setVisible(False)
	self.FALU_IMACLine.setVisible(False)

	#between FALUButton and FMACButton
	self.FALU_FMACLine.setVisible(False)
	self.FMAC_FALULine.setVisible(False)

	#between MRFButton and FALUButton
	self.MRF_FALULine.setVisible(False)
	self.FALU_MRFLine.setVisible(False)

	#between BIU0Button and SHU1Button
	self.BIU0_SHU1Line.setVisible(False)
	self.SHU1_BIU0Line.setVisible(False)

	#between SHU0Button and IMACButton
	self.SHU0_IMACLine.setVisible(False)
	self.IMAC_SHU0Line.setVisible(False)

	#between MRFButton and IALUButton
	self.IALU_MRFLine.setVisible(False)
	self.MRF_IALULine.setVisible(False)

	#between SHU1Button and FALUButton
	self.FALU_SHU1Line.setVisible(False)
	self.SHU1_FALULine.setVisible(False)

	#between BIU1Button and SHU1Button
	self.BIU1_SHU1Line.setVisible(False)
	self.SHU1_BIU1Line.setVisible(False)	

	#between BIU1Button and SHU0Button
	self.SHU0_BIU1Line.setVisible(False)
	self.BIU1_SHU0Line.setVisible(False)

	#between SHU1Button and IMACButton
	self.IMAC_SHU1Line.setVisible(False)
	self.SHU1_IMACLine.setVisible(False)

	#between SHU0Button and FALUButton
	self.SHU0_FALULine.setVisible(False)
	self.FALU_SHU0Line.setVisible(False)

	#between BIU2Button and SHU0Button
	self.SHU0_BIU2Line.setVisible(False)
	self.BIU2_SHU0Line.setVisible(False)

	#between SHU0Button and FMACButton
	self.SHU0_FMACLine.setVisible(False)
	self.FMAC_SHU0Line.setVisible(False)

	#between SHU1Button and IALUButton
	self.IALU_SHU1Line.setVisible(False)
	self.SHU1_IALULine.setVisible(False)

	#between BIU0Button and IALUButton
	self.IALU_BIU0Line.setVisible(False)
	self.BIU0_IALULine.setVisible(False)

	#between BIU1Button and IMACButton
	self.IMAC_BIU1Line.setVisible(False)
	self.BIU1_IMACLine.setVisible(False)

	#between BIU2Button and FALUButton
	self.FALU_BIU2Line.setVisible(False)
	self.BIU2_FALULine.setVisible(False)

	#between BIU0Button and IMACButton
	self.BIU0_IMACLine.setVisible(False)
	self.IMAC_BIU0Line.setVisible(False)

	#between BIU1Button and FALUButton
	self.BIU1_FALULine.setVisible(False)
	self.FALU_BIU1Line.setVisible(False)

	#between BIU2Button and FMACButton
	self.BIU2_FMACLine.setVisible(False)
	self.FMAC_BIU2Line.setVisible(False)

	#between BIU1Button and FMACButton
	self.BIU1_FMACLine.setVisible(False)
	self.FMAC_BIU1Line.setVisible(False)

	#between BIU1Button and IALUButton
	self.IALU_BIU1Line.setVisible(False)
	self.BIU1_IALULine.setVisible(False)	

	#between IALUButton and FALUButton
	self.IALU_FALULine.setVisible(False)
	self.FALU_IALULine.setVisible(False)

	#between BIU0Button and FALUButton
	self.BIU0_FALULinei.setVisible(False)
	self.FALU_BIU0Linei.setVisible(False)
	self.BIU0_FALULinej.setVisible(False)
	self.FALU_BIU0Linej.setVisible(False)	

	#between BIU0Button and FMACButton
	self.BIU0_FMACLinei.setVisible(False)
	self.FMAC_BIU0Linei.setVisible(False)
	self.BIU0_FMACLinej.setVisible(False)
	self.FMAC_BIU0Linej.setVisible(False)

	#between BIU2Button and IALUButton
	self.BIU2_IALULinei.setVisible(False)
	self.IALU_BIU2Linei.setVisible(False)
	self.BIU2_IALULinej.setVisible(False)
	self.IALU_BIU2Linej.setVisible(False)

	#between BIU2Button and IMACButton
	self.BIU2_IMACLinei.setVisible(False)
	self.IMAC_BIU2Linei.setVisible(False)
	self.BIU2_IMACLinej.setVisible(False)
	self.IMAC_BIU2Linej.setVisible(False)
	
