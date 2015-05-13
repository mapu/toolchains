# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPUViewWidget(QWidget):
    def __init__(self,parent=None):
	super(MPUViewWidget,self).__init__(parent)

	self.BIU0LineBox=QGroupBox(self.tr("BIU0 Line"))
	self.BIU0LineBox.setMaximumHeight(90)
	self.BIU0LineBox.setMaximumWidth(100)
	self.BIU0Line=QListWidget()
	self.BIU0Line.setMaximumWidth(100)
	self.BIU0Line.addItem(self.tr("sn:41,sln:3"))
	self.BIU0Line.addItem(self.tr("sn:55,sln:4"))
	self.BIU0Line.addItem(self.tr("sn:69,sln:5"))
	BIU0LineLay=QVBoxLayout()
	BIU0LineLay.addWidget(self.BIU0Line)
	self.BIU0LineBox.setLayout(BIU0LineLay)

	self.DMButton=QPushButton(self.tr("DM"))
	self.DMButton.setFixedWidth(60)
	self.DMButton.setEnabled(False)

	self.BIU0Button=QPushButton(self.tr("BIU0"))
	self.BIU0Button.setFixedWidth(60)
	self.BIU1Button=QPushButton(self.tr("BIU1"))
	self.BIU1Button.setFixedWidth(60)
	self.BIU2Button=QPushButton(self.tr("BIU2"))
	self.BIU2Button.setFixedWidth(60)

	self.text=QLineEdit(self.tr("0xFFFF"))
	self.text.setFixedWidth(70)
	

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

	gridLay=QGridLayout()
	gridLay.addWidget(self.BIU0LineBox,0,0,3,2)
	gridLay.addWidget(self.DMButton,5,0)
	gridLay.addWidget(self.BIU0Button,3,2)
	gridLay.addWidget(self.BIU1Button,5,2)
	gridLay.addWidget(self.BIU2Button,7,2)
	gridLay.addWidget(self.text,3,3)
	gridLay.addWidget(self.SHU0Button,1,4)
	gridLay.addWidget(self.MRFButton,5,4)
	gridLay.addWidget(self.SHU1Button,9,4)
	gridLay.addWidget(self.byteComboBox,0,5)
	gridLay.addWidget(self.laneComboBox,0,6)
	gridLay.addWidget(self.IALUButton,2,6)
	gridLay.addWidget(self.IMACButton,4,6)
	gridLay.addWidget(self.FALUButton,6,6)
	gridLay.addWidget(self.FMACButton,8,6)
	gridLay.setAlignment(Qt.AlignCenter)

	self.setLayout(gridLay)









