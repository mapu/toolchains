# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from FloatDialog import*
from LineWidget import*
from MPULeftWidget import*
from StageDialog import*
import sys
import math
sys.path.append("../..")
from control.DataBase import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPUViewWidget(QWidget):
    def __init__(self,parent=None):
	super(MPUViewWidget,self).__init__(parent)

	self.byteComboBox=QComboBox()
	self.byteComboBox.setFixedSize(100,40)
	self.byteComboBox.insertItem(0,"8 Bytes")
	self.byteComboBox.insertItem(1,"7 Bytes")
	self.byteComboBox.insertItem(2,"6 Bytes")
	self.byteComboBox.insertItem(3,"5 Bytes")
	self.byteComboBox.insertItem(4,"4 Bytes")
	self.byteComboBox.insertItem(5,"3 Bytes")
	self.byteComboBox.insertItem(6,"2 Bytes")
	self.byteComboBox.insertItem(7,"1 Bytes")

	self.laneComboBox=QComboBox()
	self.laneComboBox.setFixedSize(100,40)
	self.laneComboBox.insertItem(0,"Lane0")
	self.laneComboBox.insertItem(1,"Lane1")
	self.laneComboBox.insertItem(2,"Lane2")
	self.laneComboBox.insertItem(3,"Lane3")	  

	self.stageButton=QPushButton("MPU Stage")
	self.stageButton.setFixedSize(100,40)
	self.connect(self.stageButton,SIGNAL("clicked()"),self.stageButtonSlot)
	self.buttonWidget=MPULeftWidget()

	#define left Widget
	self.leftWidget=QWidget()
	#layout left Widget    
        self.leftupLay=QHBoxLayout()
        self.leftupLay.addStretch()
        self.leftupLay.setSpacing(50)
        self.leftupLay.setMargin(10)  
        self.leftupLay.addWidget(self.byteComboBox)
        self.leftupLay.addWidget(self.laneComboBox) 
	self.leftupLay.addWidget(self.stageButton) 
        self.leftLay=QVBoxLayout()
        self.leftLay.addLayout(self.leftupLay)
        self.leftLay.addWidget(self.buttonWidget)
	self.leftWidget.setLayout(self.leftLay)

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
	self.regFileWidget.setRowCount(195)
	self.regFileWidget.verticalHeader().setDefaultSectionSize(25)
	self.regFileWidget.resizeColumnToContents(1)
	self.regFileWidget.setColumnWidth(1,2500)
	#define regFileWidget M
	self.regFileWidget.setItem(0,0,QTableWidgetItem(self.tr("M")))
	self.regFileWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(1,0,QTableWidgetItem(self.tr("M0")))
        self.regFileWidget.setItem(1,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(2,0,QTableWidgetItem(self.tr("M1")))
        self.regFileWidget.setItem(2,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(3,0,QTableWidgetItem(self.tr("M2")))
        self.regFileWidget.setItem(3,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(4,0,QTableWidgetItem(self.tr("M3")))
        self.regFileWidget.setItem(4,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(5,0,QTableWidgetItem(self.tr("M4")))
        self.regFileWidget.setItem(5,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(6,0,QTableWidgetItem(self.tr("M5")))
        self.regFileWidget.setItem(6,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(7,0,QTableWidgetItem(self.tr("M6")))
        self.regFileWidget.setItem(7,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(8,0,QTableWidgetItem(self.tr("M7")))
        self.regFileWidget.setItem(8,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(9,0,QTableWidgetItem(self.tr("M8")))
        self.regFileWidget.setItem(9,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(10,0,QTableWidgetItem(self.tr("M9")))
        self.regFileWidget.setItem(10,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(11,0,QTableWidgetItem(self.tr("M10")))
        self.regFileWidget.setItem(11,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(12,0,QTableWidgetItem(self.tr("M11")))
        self.regFileWidget.setItem(12,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(13,0,QTableWidgetItem(self.tr("M12")))
        self.regFileWidget.setItem(13,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(14,0,QTableWidgetItem(self.tr("M13")))
        self.regFileWidget.setItem(14,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(15,0,QTableWidgetItem(self.tr("M14")))
        self.regFileWidget.setItem(15,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(16,0,QTableWidgetItem(self.tr("M15")))
        self.regFileWidget.setItem(16,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(17,0,QTableWidgetItem(self.tr("M16")))
        self.regFileWidget.setItem(17,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(18,0,QTableWidgetItem(self.tr("M17")))
        self.regFileWidget.setItem(18,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(19,0,QTableWidgetItem(self.tr("M18")))
        self.regFileWidget.setItem(19,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(20,0,QTableWidgetItem(self.tr("M19")))
        self.regFileWidget.setItem(20,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(21,0,QTableWidgetItem(self.tr("M20")))
        self.regFileWidget.setItem(21,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(22,0,QTableWidgetItem(self.tr("M21")))
        self.regFileWidget.setItem(22,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(23,0,QTableWidgetItem(self.tr("M22")))
        self.regFileWidget.setItem(23,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(24,0,QTableWidgetItem(self.tr("M23")))
        self.regFileWidget.setItem(24,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(25,0,QTableWidgetItem(self.tr("M24")))
        self.regFileWidget.setItem(25,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(26,0,QTableWidgetItem(self.tr("M25")))
        self.regFileWidget.setItem(26,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(27,0,QTableWidgetItem(self.tr("M26")))
        self.regFileWidget.setItem(27,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(28,0,QTableWidgetItem(self.tr("M27")))
        self.regFileWidget.setItem(28,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(29,0,QTableWidgetItem(self.tr("M28")))
        self.regFileWidget.setItem(29,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(30,0,QTableWidgetItem(self.tr("M29")))
        self.regFileWidget.setItem(30,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(31,0,QTableWidgetItem(self.tr("M30")))
        self.regFileWidget.setItem(31,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(32,0,QTableWidgetItem(self.tr("M31")))
        self.regFileWidget.setItem(32,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(33,0,QTableWidgetItem(self.tr("M32")))
        self.regFileWidget.setItem(33,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(34,0,QTableWidgetItem(self.tr("M33")))
        self.regFileWidget.setItem(34,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(35,0,QTableWidgetItem(self.tr("M34")))
        self.regFileWidget.setItem(35,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(36,0,QTableWidgetItem(self.tr("M35")))
        self.regFileWidget.setItem(36,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(37,0,QTableWidgetItem(self.tr("M36")))
        self.regFileWidget.setItem(37,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(38,0,QTableWidgetItem(self.tr("M37")))
        self.regFileWidget.setItem(38,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(39,0,QTableWidgetItem(self.tr("M38")))
        self.regFileWidget.setItem(39,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(40,0,QTableWidgetItem(self.tr("M39")))
	self.regFileWidget.setItem(40,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(41,0,QTableWidgetItem(self.tr("M40")))
        self.regFileWidget.setItem(41,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(42,0,QTableWidgetItem(self.tr("M41")))
        self.regFileWidget.setItem(42,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(43,0,QTableWidgetItem(self.tr("M42")))
        self.regFileWidget.setItem(43,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(44,0,QTableWidgetItem(self.tr("M43")))
        self.regFileWidget.setItem(44,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(45,0,QTableWidgetItem(self.tr("M44")))
        self.regFileWidget.setItem(45,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(46,0,QTableWidgetItem(self.tr("M45")))
        self.regFileWidget.setItem(46,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(47,0,QTableWidgetItem(self.tr("M46")))
        self.regFileWidget.setItem(47,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(48,0,QTableWidgetItem(self.tr("M47")))
        self.regFileWidget.setItem(48,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(49,0,QTableWidgetItem(self.tr("M48")))
        self.regFileWidget.setItem(49,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(50,0,QTableWidgetItem(self.tr("M49")))
	self.regFileWidget.setItem(50,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(51,0,QTableWidgetItem(self.tr("M50")))
        self.regFileWidget.setItem(51,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(52,0,QTableWidgetItem(self.tr("M51")))
        self.regFileWidget.setItem(52,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(53,0,QTableWidgetItem(self.tr("M52")))
        self.regFileWidget.setItem(53,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(54,0,QTableWidgetItem(self.tr("M53")))
        self.regFileWidget.setItem(54,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(55,0,QTableWidgetItem(self.tr("M54")))
        self.regFileWidget.setItem(55,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(56,0,QTableWidgetItem(self.tr("M55")))
        self.regFileWidget.setItem(56,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(57,0,QTableWidgetItem(self.tr("M56")))
        self.regFileWidget.setItem(57,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(58,0,QTableWidgetItem(self.tr("M57")))
        self.regFileWidget.setItem(58,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(59,0,QTableWidgetItem(self.tr("M58")))
        self.regFileWidget.setItem(59,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(60,0,QTableWidgetItem(self.tr("M59")))
	self.regFileWidget.setItem(60,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(61,0,QTableWidgetItem(self.tr("M60")))
        self.regFileWidget.setItem(61,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(62,0,QTableWidgetItem(self.tr("M61")))
        self.regFileWidget.setItem(62,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(63,0,QTableWidgetItem(self.tr("M62")))
        self.regFileWidget.setItem(63,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(64,0,QTableWidgetItem(self.tr("M63")))
        self.regFileWidget.setItem(64,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(65,0,QTableWidgetItem(self.tr("M64")))
        self.regFileWidget.setItem(65,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(66,0,QTableWidgetItem(self.tr("M65")))
        self.regFileWidget.setItem(66,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(67,0,QTableWidgetItem(self.tr("M66")))
        self.regFileWidget.setItem(67,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(68,0,QTableWidgetItem(self.tr("M67")))
        self.regFileWidget.setItem(68,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(69,0,QTableWidgetItem(self.tr("M68")))
        self.regFileWidget.setItem(69,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(70,0,QTableWidgetItem(self.tr("M69")))
	self.regFileWidget.setItem(70,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(71,0,QTableWidgetItem(self.tr("M70")))
        self.regFileWidget.setItem(71,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(72,0,QTableWidgetItem(self.tr("M71")))
        self.regFileWidget.setItem(72,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(73,0,QTableWidgetItem(self.tr("M72")))
        self.regFileWidget.setItem(73,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(74,0,QTableWidgetItem(self.tr("M73")))
        self.regFileWidget.setItem(74,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(75,0,QTableWidgetItem(self.tr("M74")))
        self.regFileWidget.setItem(75,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(76,0,QTableWidgetItem(self.tr("M75")))
        self.regFileWidget.setItem(76,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(77,0,QTableWidgetItem(self.tr("M76")))
        self.regFileWidget.setItem(77,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(78,0,QTableWidgetItem(self.tr("M77")))
        self.regFileWidget.setItem(78,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(79,0,QTableWidgetItem(self.tr("M78")))
        self.regFileWidget.setItem(79,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(80,0,QTableWidgetItem(self.tr("M79")))
	self.regFileWidget.setItem(80,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(81,0,QTableWidgetItem(self.tr("M80")))
        self.regFileWidget.setItem(81,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(82,0,QTableWidgetItem(self.tr("M81")))
        self.regFileWidget.setItem(82,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(83,0,QTableWidgetItem(self.tr("M82")))
        self.regFileWidget.setItem(83,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(84,0,QTableWidgetItem(self.tr("M83")))
        self.regFileWidget.setItem(84,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(85,0,QTableWidgetItem(self.tr("M84")))
        self.regFileWidget.setItem(85,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(86,0,QTableWidgetItem(self.tr("M85")))
        self.regFileWidget.setItem(86,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(87,0,QTableWidgetItem(self.tr("M86")))
        self.regFileWidget.setItem(87,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(88,0,QTableWidgetItem(self.tr("M87")))
        self.regFileWidget.setItem(88,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(89,0,QTableWidgetItem(self.tr("M88")))
        self.regFileWidget.setItem(89,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(90,0,QTableWidgetItem(self.tr("M89")))
	self.regFileWidget.setItem(90,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(91,0,QTableWidgetItem(self.tr("M90")))
        self.regFileWidget.setItem(91,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(92,0,QTableWidgetItem(self.tr("M91")))
        self.regFileWidget.setItem(92,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(93,0,QTableWidgetItem(self.tr("M92")))
        self.regFileWidget.setItem(93,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(94,0,QTableWidgetItem(self.tr("M93")))
        self.regFileWidget.setItem(94,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(95,0,QTableWidgetItem(self.tr("M94")))
        self.regFileWidget.setItem(95,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(96,0,QTableWidgetItem(self.tr("M95")))
        self.regFileWidget.setItem(96,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(97,0,QTableWidgetItem(self.tr("M96")))
        self.regFileWidget.setItem(97,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(98,0,QTableWidgetItem(self.tr("M97")))
        self.regFileWidget.setItem(98,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(99,0,QTableWidgetItem(self.tr("M98")))
        self.regFileWidget.setItem(99,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(100,0,QTableWidgetItem(self.tr("M99")))
	self.regFileWidget.setItem(100,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(101,0,QTableWidgetItem(self.tr("M100")))
        self.regFileWidget.setItem(101,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(102,0,QTableWidgetItem(self.tr("M101")))
        self.regFileWidget.setItem(102,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(103,0,QTableWidgetItem(self.tr("M102")))
        self.regFileWidget.setItem(103,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(104,0,QTableWidgetItem(self.tr("M103")))
        self.regFileWidget.setItem(104,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(105,0,QTableWidgetItem(self.tr("M104")))
        self.regFileWidget.setItem(105,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(106,0,QTableWidgetItem(self.tr("M105")))
        self.regFileWidget.setItem(106,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(107,0,QTableWidgetItem(self.tr("M106")))
        self.regFileWidget.setItem(107,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(108,0,QTableWidgetItem(self.tr("M107")))
        self.regFileWidget.setItem(108,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(109,0,QTableWidgetItem(self.tr("M108")))
        self.regFileWidget.setItem(109,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(110,0,QTableWidgetItem(self.tr("M109")))
	self.regFileWidget.setItem(110,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(111,0,QTableWidgetItem(self.tr("M110")))
        self.regFileWidget.setItem(111,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(112,0,QTableWidgetItem(self.tr("M111")))
        self.regFileWidget.setItem(112,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(113,0,QTableWidgetItem(self.tr("M112")))
        self.regFileWidget.setItem(113,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(114,0,QTableWidgetItem(self.tr("M113")))
        self.regFileWidget.setItem(114,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(115,0,QTableWidgetItem(self.tr("M114")))
        self.regFileWidget.setItem(115,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(116,0,QTableWidgetItem(self.tr("M115")))
        self.regFileWidget.setItem(116,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(117,0,QTableWidgetItem(self.tr("M116")))
        self.regFileWidget.setItem(117,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(118,0,QTableWidgetItem(self.tr("M117")))
        self.regFileWidget.setItem(118,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(119,0,QTableWidgetItem(self.tr("M118")))
        self.regFileWidget.setItem(119,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(120,0,QTableWidgetItem(self.tr("M119")))
	self.regFileWidget.setItem(120,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(121,0,QTableWidgetItem(self.tr("M120")))
        self.regFileWidget.setItem(121,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(122,0,QTableWidgetItem(self.tr("M121")))
        self.regFileWidget.setItem(122,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(123,0,QTableWidgetItem(self.tr("M122")))
        self.regFileWidget.setItem(123,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(124,0,QTableWidgetItem(self.tr("M123")))
        self.regFileWidget.setItem(124,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(125,0,QTableWidgetItem(self.tr("M124")))
        self.regFileWidget.setItem(125,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(126,0,QTableWidgetItem(self.tr("M125")))
        self.regFileWidget.setItem(126,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(127,0,QTableWidgetItem(self.tr("M126")))
        self.regFileWidget.setItem(127,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(128,0,QTableWidgetItem(self.tr("M127")))
        self.regFileWidget.setItem(128,1,QTableWidgetItem(self.tr("0")))
	
	#define regFileWidget SHU0 T
	self.regFileWidget.setItem(129,0,QTableWidgetItem(self.tr("SHU0 T")))
	self.regFileWidget.setItem(129,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(129,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(129,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(130,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(130,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(131,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(131,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(132,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(132,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(133,0,QTableWidgetItem(self.tr("T3")))
        self.regFileWidget.setItem(133,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(134,0,QTableWidgetItem(self.tr("T4")))
        self.regFileWidget.setItem(134,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(135,0,QTableWidgetItem(self.tr("T5")))
        self.regFileWidget.setItem(135,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(136,0,QTableWidgetItem(self.tr("T6")))
        self.regFileWidget.setItem(136,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(137,0,QTableWidgetItem(self.tr("T7")))
        self.regFileWidget.setItem(137,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(138,0,QTableWidgetItem(self.tr("T8")))
        self.regFileWidget.setItem(138,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(139,0,QTableWidgetItem(self.tr("T9")))
        self.regFileWidget.setItem(139,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(140,0,QTableWidgetItem(self.tr("T10")))
        self.regFileWidget.setItem(140,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(141,0,QTableWidgetItem(self.tr("T11")))
        self.regFileWidget.setItem(141,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(142,0,QTableWidgetItem(self.tr("T12")))
        self.regFileWidget.setItem(142,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(143,0,QTableWidgetItem(self.tr("T13")))
        self.regFileWidget.setItem(143,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(144,0,QTableWidgetItem(self.tr("T14")))
        self.regFileWidget.setItem(144,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(145,0,QTableWidgetItem(self.tr("T15")))
        self.regFileWidget.setItem(145,1,QTableWidgetItem(self.tr("0")))
	#define regFileWidget SHU1 T
	self.regFileWidget.setItem(146,0,QTableWidgetItem(self.tr("SHU1 T")))
	self.regFileWidget.setItem(146,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(146,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(146,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(147,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(147,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(148,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(148,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(149,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(149,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(150,0,QTableWidgetItem(self.tr("T3")))
        self.regFileWidget.setItem(150,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(151,0,QTableWidgetItem(self.tr("T4")))
        self.regFileWidget.setItem(151,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(152,0,QTableWidgetItem(self.tr("T5")))
        self.regFileWidget.setItem(152,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(153,0,QTableWidgetItem(self.tr("T6")))
        self.regFileWidget.setItem(153,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(154,0,QTableWidgetItem(self.tr("T7")))
        self.regFileWidget.setItem(154,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(155,0,QTableWidgetItem(self.tr("T8")))
        self.regFileWidget.setItem(155,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(156,0,QTableWidgetItem(self.tr("T9")))
        self.regFileWidget.setItem(156,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(157,0,QTableWidgetItem(self.tr("T10")))
        self.regFileWidget.setItem(157,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(158,0,QTableWidgetItem(self.tr("T11")))
        self.regFileWidget.setItem(158,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(159,0,QTableWidgetItem(self.tr("T12")))
        self.regFileWidget.setItem(159,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(160,0,QTableWidgetItem(self.tr("T13")))
        self.regFileWidget.setItem(160,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(161,0,QTableWidgetItem(self.tr("T14")))
        self.regFileWidget.setItem(161,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(162,0,QTableWidgetItem(self.tr("T15")))
        self.regFileWidget.setItem(162,1,QTableWidgetItem(self.tr("0")))
	#define regFileWidget IALU T
	self.regFileWidget.setItem(163,0,QTableWidgetItem(self.tr("IALU T")))
	self.regFileWidget.setItem(163,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(163,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(163,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(164,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(164,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(165,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(165,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(166,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(166,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(167,0,QTableWidgetItem(self.tr("T3")))
        self.regFileWidget.setItem(167,1,QTableWidgetItem(self.tr("0")))
	#define regFileWidget IMAC T
	self.regFileWidget.setItem(168,0,QTableWidgetItem(self.tr("IMAC T")))
	self.regFileWidget.setItem(168,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(168,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(168,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(169,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(169,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(170,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(170,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(171,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(171,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(172,0,QTableWidgetItem(self.tr("T3")))
        self.regFileWidget.setItem(172,1,QTableWidgetItem(self.tr("0")))
	#define regFileWidget FALU T
	self.regFileWidget.setItem(173,0,QTableWidgetItem(self.tr("FALU T")))
	self.regFileWidget.setItem(173,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(173,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(173,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(174,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(174,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(175,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(175,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(176,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(176,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(177,0,QTableWidgetItem(self.tr("T3")))
        self.regFileWidget.setItem(177,1,QTableWidgetItem(self.tr("0")))
	#define regFileWidget FMAC T
	self.regFileWidget.setItem(178,0,QTableWidgetItem(self.tr("FMAC T")))
	self.regFileWidget.setItem(178,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(178,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(178,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(179,0,QTableWidgetItem(self.tr("T0")))
        self.regFileWidget.setItem(179,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(180,0,QTableWidgetItem(self.tr("T1")))
        self.regFileWidget.setItem(180,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(181,0,QTableWidgetItem(self.tr("T2")))
        self.regFileWidget.setItem(181,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(182,0,QTableWidgetItem(self.tr("T3")))
        self.regFileWidget.setItem(182,1,QTableWidgetItem(self.tr("0")))
	#define regFileWidget IMRL
        self.regFileWidget.setItem(183,0,QTableWidgetItem(self.tr("IMRL")))
        self.regFileWidget.setItem(183,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(183,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(183,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget IMRH
        self.regFileWidget.setItem(184,0,QTableWidgetItem(self.tr("IMRH")))
        self.regFileWidget.setItem(184,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(184,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(184,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget FMR
        self.regFileWidget.setItem(185,0,QTableWidgetItem(self.tr("FMR")))
        self.regFileWidget.setItem(185,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(185,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(185,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget BIU0W
        self.regFileWidget.setItem(186,0,QTableWidgetItem(self.tr("BIU0W")))
        self.regFileWidget.setItem(186,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(186,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(186,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget BIU1W
        self.regFileWidget.setItem(187,0,QTableWidgetItem(self.tr("BIU1W")))
        self.regFileWidget.setItem(187,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(187,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(187,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget BIU2W
        self.regFileWidget.setItem(188,0,QTableWidgetItem(self.tr("BIU2W")))
        self.regFileWidget.setItem(188,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(188,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(188,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget DIVQU
        self.regFileWidget.setItem(189,0,QTableWidgetItem(self.tr("DIVQU")))
        self.regFileWidget.setItem(189,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(189,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(189,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget DIVRE
        self.regFileWidget.setItem(190,0,QTableWidgetItem(self.tr("DIVRE")))
        self.regFileWidget.setItem(190,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(190,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(190,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget DIV-CNT
        self.regFileWidget.setItem(191,0,QTableWidgetItem(self.tr("DIV-CNT")))
        self.regFileWidget.setItem(191,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.item(191,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(191,1).setBackgroundColor(QColor(192,192,192))
	#define regFileWidget SVR
        self.regFileWidget.setItem(192,0,QTableWidgetItem(self.tr("SVR")))
        self.regFileWidget.setItem(192,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(192,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(192,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(193,0,QTableWidgetItem(self.tr("SVR0")))
        self.regFileWidget.setItem(193,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(194,0,QTableWidgetItem(self.tr("SVR1")))
        self.regFileWidget.setItem(194,1,QTableWidgetItem(self.tr("0")))	

	mainLayout=QHBoxLayout()
	mainLayout.addWidget(self.leftWidget)
	mainLayout.addWidget(self.rightTab)
	mainLayout.setStretchFactor(self.leftWidget,5)
	mainLayout.setStretchFactor(self.rightTab,2)
	self.setLayout(mainLayout)

	self.stageDialog=StageDialog()
	
    def stageButtonSlot(self):
	self.stageDialog.setWindowTitle("MPU Stage Dialog")
	self.stageDialog.show()

    def updateMPUWidget(self,r):
	#update lines
	#DM BIU0
	if r[1]!="nop":
	    print "DM BIU0"
	#BIU0 DM
	if r[2]!="nop":
	    print "BIU0 DM"
	#DM BIU1
	if r[3]!="nop":
	    print "DM BIU1"
	#BIU1 DM
	if r[4]!="nop":
	    print "BIU1 DM"
	#DM BIU2
	if r[5]!="nop":
	    print "DM BIU2"
	#BIU2 DM
	if r[6]!="nop":
	    print "BIU2 DM"
	#BIU0 SHU0
	if r[7]!="nop":
	    print "BIU0 SHU0"
	#SHU0 BIU0
	if r[8]!="nop":
	    print "SHU0 BIU0"
	#BIU1 SHU0
	if r[9]!="nop":
	    print "BIU1 SHU0"
	#SHU0 BIU1
	if r[10]!="nop":
	    print "SHU0 BIU1"
	#BIU2 SHU0
	if r[11]!="nop":
	    print "BIU2 SHU0"
	#SHU0 BIU2
	if r[12]!="nop":
	    print "SHU0 BIU2"
	#BIU0 SHU1
	if r[13]!="nop":
	    print "BIU0 SHU1"
	#SHU1 BIU0
	if r[14]!="nop":
	    print "SHU1 BIU0"
	#BIU1 SHU1
	if r[15]!="nop":
	    print "BIU1 SHU1"
	#SHU1 BIU1
	if r[16]!="nop":
	    print "SHU1 BIU1"
	#BIU2 SHU1
	if r[17]!="nop":
	    print "BIU2 SHU1"
	#SHU1 BIU2
	if r[18]!="nop":
	    print "SHU1 BIU2"
	#BIU0 MRF
	if r[19]!="nop":
	    print "BIU0 MRF"
	#MRF BIU0
	if r[20]!="nop":
	    print "MRF BIU0"
	#BIU1 MRF
	if r[21]!="nop":
	    print "BIU1 MRF"
	#MRF BIU1
	if r[22]!="nop":
	    print "MRF BIU1"
	#BIU2 MRF
	if r[23]!="nop":
	    print "BIU2 MRF"
	#MRF BIU2
	if r[24]!="nop":
	    print "MRF BIU2"
	#SHU0 IALU
	if r[25]!="nop":
	   print "SHU0 IALU"
	#IALU SHU0
	if r[26]!="nop":
	    print "IALU SHU0"
	#SHU0 IMAC
	if r[27]!="nop":
	    print "SHU0 IMAC"
	#IMAC SHU0
	if r[28]!="nop":
	    print "IMAC SHU0"
	#SHU0 FALU
	if r[29]!="nop":
	    print "SHU0 FALU"
	#FALU SHU0
	if r[30]!="nop":
	    print "FALU SHU0"
	#SHU0 FMAC
	if r[31]!="nop":
	    print "SHU0 FMAC"
	#FMAC SHU0
	if r[32]!="nop":
	    print "FMAC SHU0"
	#MRF IALU
	if r[33]!="nop":
	    print "MRF IALU"
	#IALU MRF
	if r[34]!="nop":
	    print "IALU MRF"
	#MRF IMAC
	if r[35]!="nop":
	    print "MRF IMAC"
	#IMAC MRF
	if r[36]!="nop":
	    print "IMAC MRF"
	#MRF FALU
	if r[37]!="nop":
	    print "MRF FALU"
	#FALU MRF
	if r[38]!="nop":
	    print "FALU MRF"
	#MRF FMAC
	if r[39]!="nop":
	    print "MRF FMAC"
	#FMAC MRF
	if r[40]!="nop":
	    print "FMAC MRF"
	#SHU1 IALU
	if r[41]!="nop":
	    print "SHU1 IALU"
	#IALU SHU1
	if r[42]!="nop":
	    print "IALU SHU1"
	#SHU1 IMAC
	if r[43]!="nop":
	    print "SHU1 IMAC"
	#IMAC SHU1
	if r[44]!="nop":
	    print "IMAC SHU1"
	#SHU1 FALU
	if r[45]!="nop":
	    print "SHU1 FALU"
	#FALU SHU1
	if r[46]!="nop":
	    print "FALU SHU1"
	#SHU1 FMAC
	if r[47]!="nop":
	    print "SHU1 FMAC"
	#FMAC SHU1
	if r[48]!="nop":
	    print "FMAC SHU1"
	#IALU FALU
	if r[49]!="nop":
	    print "IALU FALU"
	#FALU IALU
	if r[50]!="nop":
	    print "FALU IALU"
	#SHU0 MRF
	if r[51]!="nop":
	    print "SHU0 MRF"
	#MRF SHU0
	if r[52]!="nop":
	    print "MRF SHU0"
	#MRF SHU1
	if r[53]!="nop":
	    print "MRF SHU1"
	#SHU1 MRF
	if r[54]!="nop":
	    print "SHU1 MRF"
	#IALU IMAC
	if r[55]!="nop":
	    print "IALU IMAC"
	#IMAC IALU
	if r[56]!="nop":
	    print "IMAC IALU"
	#IMAC FALU
	if r[57]!="nop":
	    print "IMAC FALU"
	#FALU IMAC
	if r[58]!="nop":
	    print "FALU IMAC"
	#FALU FMAC
	if r[59]!="nop":
	    print "FALU FMAC"
	#FMAC FALU
	if r[60]!="nop":
	    print "FMAC FALU"

	#update mpu reg
	for i in range(0,187):
	    if i>=0 and i<=127:  #m0-127
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+1,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+1,1).setToolTip(r[61+i])
		else:
	            self.regFileWidget.item(i+1,1).setData(0,"0")
		    self.regFileWidget.item(i+1,1).setToolTip("")
	        self.regFileWidget.item(i+1,1).setTextColor(QColor(0,0,0))
	    elif i>=128 and i<=143: #shu0
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+2,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+2,1).setToolTip(r[61+i])
		else:
		    self.regFileWidget.item(i+2,1).setData(0,"0")
		    self.regFileWidget.item(i+2,1).setToolTip("")
	        self.regFileWidget.item(i+2,1).setTextColor(QColor(0,0,0))
	    elif i>=144 and i<=159: #shu1
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+3,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+3,1).setToolTip(r[61+i])
		else:
		    self.regFileWidget.item(i+3,1).setData(0,"0")
		    self.regFileWidget.item(i+3,1).setToolTip("")
	        self.regFileWidget.item(i+3,1).setTextColor(QColor(0,0,0))
	    elif i>=160 and i<=163: #ialu
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+4,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+4,1).setToolTip(r[61+i])
		else:
		    self.regFileWidget.item(i+4,1).setData(0,"0")
		    self.regFileWidget.item(i+4,1).setToolTip("")
	        self.regFileWidget.item(i+4,1).setTextColor(QColor(0,0,0))
	    elif i>=164 and i<=167:  #imac
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+5,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+5,1).setToolTip(r[61+i])
		else:
		    self.regFileWidget.item(i+5,1).setData(0,"0")
		    self.regFileWidget.item(i+5,1).setToolTip("")
	        self.regFileWidget.item(i+5,1).setTextColor(QColor(0,0,0))
	    elif i>=168 and i<=171: #falu
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+6,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+6,1).setToolTip(r[61+i])
		else:
		    self.regFileWidget.item(i+6,1).setData(0,"0")
		    self.regFileWidget.item(i+6,1).setToolTip("")
	        self.regFileWidget.item(i+6,1).setTextColor(QColor(0,0,0))
	    elif i>=172 and i<=175: #fmac
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+7,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+7,1).setToolTip(r[61+i])
		else:
		    self.regFileWidget.item(i+7,1).setData(0,"0")
		    self.regFileWidget.item(i+7,1).setToolTip("")
	        self.regFileWidget.item(i+7,1).setTextColor(QColor(0,0,0))
	    elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+7,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+7,1).setToolTip(r[61+i])	
		else:
		    self.regFileWidget.item(i+7,1).setData(0,"0")
		    self.regFileWidget.item(i+7,1).setToolTip("")
	        self.regFileWidget.item(i+7,1).setTextColor(QColor(0,0,0))	
	    elif i>=185 and i<=186: #svr
	    	if r[61+i]!="nop":
		    self.regFileWidget.item(i+8,1).setData(0,r[61+i])
		    self.regFileWidget.item(i+8,1).setToolTip(r[61+i])
		else:
		    self.regFileWidget.item(i+8,1).setData(0,"0")
		    self.regFileWidget.item(i+8,1).setToolTip("")
	        self.regFileWidget.item(i+8,1).setTextColor(QColor(0,0,0))

	#update float dialog
	#clear stage list
	self.buttonWidget.DMButtonDialog.stages.clear()
	self.buttonWidget.BIU0ButtonDialog.stages.clear()
	self.buttonWidget.BIU1ButtonDialog.stages.clear()
	self.buttonWidget.BIU2ButtonDialog.stages.clear()
	self.buttonWidget.SHU0ButtonDialog.stages.clear()
	self.buttonWidget.MRFButtonDialog.stages.clear()
	self.buttonWidget.SHU1ButtonDialog.stages.clear()
	self.buttonWidget.IALUButtonDialog.stages.clear()
	self.buttonWidget.IMACButtonDialog.stages.clear()
	self.buttonWidget.FALUButtonDialog.stages.clear()
	self.buttonWidget.FMACButtonDialog.stages.clear()
	#init float dialog
	self.buttonWidget.DMButtonDialog.initDialog()
	self.buttonWidget.BIU0ButtonDialog.initDialog()
	self.buttonWidget.BIU1ButtonDialog.initDialog()
	self.buttonWidget.BIU2ButtonDialog.initDialog()
	self.buttonWidget.SHU0ButtonDialog.initDialog()
	self.buttonWidget.MRFButtonDialog.initDialog()
	self.buttonWidget.SHU1ButtonDialog.initDialog()
	self.buttonWidget.IALUButtonDialog.initDialog()
	self.buttonWidget.IMACButtonDialog.initDialog()
	self.buttonWidget.FALUButtonDialog.initDialog()
	self.buttonWidget.FMACButtonDialog.initDialog()
	#DM
	for i in range(0,40,2):
	    if r[312+i]!="nop":
		self.buttonWidget.DMButtonDialog.stages.item(i/2,1).setData(0,r[312+i])
		self.buttonWidget.DMButtonDialog.stages.item(i/2,1).setToolTip(r[312+i+1])
	#BIU0
	for i in range(0,40,2):
	    if r[352+i]!="nop":
		self.buttonWidget.BIU0ButtonDialog.stages.item(i/2,1).setData(0,r[352+i])
		self.buttonWidget.BIU0ButtonDialog.stages.item(i/2,1).setToolTip(r[352+i+1])
	#BIU1
	for i in range(0,40,2):
	    if r[392+i]!="nop":
		self.buttonWidget.BIU1ButtonDialog.stages.item(i/2,1).setData(0,r[392+i])
		self.buttonWidget.BIU1ButtonDialog.stages.item(i/2,1).setToolTip(r[392+i+1])
	#BIU2
	for i in range(0,40,2):
	    if r[432+i]!="nop":
		self.buttonWidget.BIU2ButtonDialog.stages.item(i/2,1).setData(0,r[432+i])
		self.buttonWidget.BIU2ButtonDialog.stages.item(i/2,1).setToolTip(r[432+i+1])
	#SHU0
	for i in range(0,40,2):
	    if r[472+i]!="nop":
		self.buttonWidget.SHU0ButtonDialog.stages.item(i/2,1).setData(0,r[472+i])
		self.buttonWidget.SHU0ButtonDialog.stages.item(i/2,1).setToolTip(r[472+i+1])
	#MRF
	for i in range(0,40,2):
	    if r[512+i]!="nop":
		self.buttonWidget.MRFButtonDialog.stages.item(i/2,1).setData(0,r[512+i])
		self.buttonWidget.MRFButtonDialog.stages.item(i/2,1).setToolTip(r[512+i+1])
	#SHU1
	for i in range(0,40,2):
	    if r[552+i]!="nop":
		self.buttonWidget.SHU1ButtonDialog.stages.item(i/2,1).setData(0,r[552+i])
		self.buttonWidget.SHU1ButtonDialog.stages.item(i/2,1).setToolTip(r[552+i+1])
	#IALU
	for i in range(0,40,2):
	    if r[592+i]!="nop":
		self.buttonWidget.IALUButtonDialog.stages.item(i/2,1).setData(0,r[592+i])
		self.buttonWidget.IALUButtonDialog.stages.item(i/2,1).setToolTip(r[592+i+1])
	#IMAC
	for i in range(0,40,2):
	    if r[632+i]!="nop":
		self.buttonWidget.IMACButtonDialog.stages.item(i/2,1).setData(0,r[632+i])
		self.buttonWidget.IMACButtonDialog.stages.item(i/2,1).setToolTip(r[632+i+1])
	#FALU
	for i in range(0,40,2):
	    if r[672+i]!="nop":
		self.buttonWidget.FALUButtonDialog.stages.item(i/2,1).setData(0,r[672+i])
		self.buttonWidget.FALUButtonDialog.stages.item(i/2,1).setToolTip(r[672+i+1])
	#FMAC
	for i in range(0,40,2):
	    if r[712+i]!="nop":
		self.buttonWidget.FMACButtonDialog.stages.item(i/2,1).setData(0,r[712+i])
		self.buttonWidget.FMACButtonDialog.stages.item(i/2,1).setToolTip(r[712+i+1])

    def updateMPURegWFlag(self,r):
	i=r[7]
	if i>=0 and i<=127:
	    self.regFileWidget.item(i+1,1).setTextColor(QColor(255,0,0))
	elif i>=128 and i<=143: #shu0
	    self.regFileWidget.item(i+2,1).setTextColor(QColor(255,0,0))
	elif i>=144 and i<=159: #shu1
	    self.regFileWidget.item(i+3,1).setTextColor(QColor(255,0,0))
	elif i>=160 and i<=163: #ialu
	    self.regFileWidget.item(i+4,1).setTextColor(QColor(255,0,0))
	elif i>=164 and i<=167:  #imac
	    self.regFileWidget.item(i+5,1).setTextColor(QColor(255,0,0))
	elif i>=168 and i<=171: #falu
	    self.regFileWidget.item(i+6,1).setTextColor(QColor(255,0,0))
	elif i>=172 and i<=175: #fmac
	    self.regFileWidget.item(i+7,1).setTextColor(QColor(255,0,0))
	elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    self.regFileWidget.item(i+7,1).setTextColor(QColor(255,0,0))	
	elif i>=185 and i<=186:
	    self.regFileWidget.item(i+8,1).setTextColor(QColor(255,0,0))

    def updateMPURegRFlag(self,i):
	if i>=0 and i<=127:	
	    if self.regFileWidget.item(i+1,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+1,1).setTextColor(QColor(255,153,18))
	elif i>=128 and i<=143: #shu0
	    if self.regFileWidget.item(i+2,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+2,1).setTextColor(QColor(255,153,18))
	elif i>=144 and i<=159: #shu1
	    if self.regFileWidget.item(i+3,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+3,1).setTextColor(QColor(255,153,18))
	elif i>=160 and i<=163: #ialu
	    if self.regFileWidget.item(i+4,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+4,1).setTextColor(QColor(255,153,18))
	elif i>=164 and i<=167:  #imac
	    if self.regFileWidget.item(i+5,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+5,1).setTextColor(QColor(255,153,18))
	elif i>=168 and i<=171: #falu
	    if self.regFileWidget.item(i+6,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+6,1).setTextColor(QColor(255,153,18))
	elif i>=172 and i<=175: #fmac
	    if self.regFileWidget.item(i+7,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+7,1).setTextColor(QColor(255,153,18))
	elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    if self.regFileWidget.item(i+7,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+7,1).setTextColor(QColor(255,153,18))	
	elif i>=185 and i<=186:
	    if self.regFileWidget.item(i+8,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+8,1).setTextColor(QColor(255,153,18))



