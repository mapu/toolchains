# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from FloatDialog import*
from MPULeftWidget import*
from StageDialog import*
import sys
import math
sys.path.append("../..")
from control.DataBase import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPUViewWidget(QMainWindow):
    def __init__(self,parent=None):
	super(MPUViewWidget,self).__init__(parent)

        centralWidget=QSplitter(Qt.Horizontal)
        self.setCentralWidget(centralWidget)  
        widget=QSplitter(Qt.Horizontal)
        widget.setMinimumSize(1400,600)

	self.stageButton=QPushButton("Instruction Pipeline Diagram")
	self.stageButton.setFixedSize(200,40)
	self.stageButton.setEnabled(False)
	self.connect(self.stageButton,SIGNAL("clicked()"),self.stageButtonSlot)
	self.buttonWidget=MPULeftWidget()

	#define left Widget
	self.leftWidget=QWidget()
	#layout left Widget    
        self.leftupLay=QHBoxLayout()
        blank=QLabel()
	blank.setFixedSize(500,30) 
	self.leftupLay.addWidget(blank)
	self.leftupLay.addWidget(self.stageButton) 
        self.leftLay=QVBoxLayout()
        self.leftLay.addLayout(self.leftupLay)
        self.leftLay.addWidget(self.buttonWidget)
	self.leftWidget.setLayout(self.leftLay)

	#define rightTab
	self.rightTab=QTabWidget()
	self.rightTab.setMinimumWidth(100)
	self.byteComboBox=QComboBox()
	self.byteComboBox.setEnabled(False)
	self.byteComboBox.insertItem(0,"1")
	self.byteComboBox.insertItem(1,"2")
	self.byteComboBox.insertItem(2,"4")
	self.byteComboBox.insertItem(3,"float")
	self.byteComboBox.insertItem(4,"double")
	self.connect(self.byteComboBox,SIGNAL("currentIndexChanged(int)"),self.currentIndexSlot)
	self.regFileWidget=QTableView()
	regWidget=QWidget()
	regLay=QVBoxLayout()
	regLay.addWidget(self.byteComboBox)
	regLay.addWidget(self.regFileWidget)
	regWidget.setLayout(regLay)																
	self.specialRegWidget=QTableWidget()
	self.traceWidget=QTableWidget()
	self.rightTab.addTab(regWidget,self.tr("Register File"))
	self.rightTab.addTab(self.specialRegWidget,self.tr("Special Register"))
	self.rightTab.addTab(self.traceWidget,self.tr("Trace"))
	self.specialRegWidget.horizontalHeader().setStretchLastSection(True)
	self.traceWidget.horizontalHeader().setStretchLastSection(True)

	#define regFileWidget
	self.regModel=QStandardItemModel(self.regFileWidget)
	self.regFileWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.regFileWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.regFileWidget.horizontalHeader().setVisible(False)
	self.regFileWidget.verticalHeader().setVisible(False)
	self.regFileWidget.setShowGrid(True)
	self.regFileWidget.verticalHeader().setDefaultSectionSize(25)
	self.regFileWidget.horizontalHeader().setStretchLastSection(True)
	self.regFileWidget.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
	self.regFileWidget.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
	self.regFileWidget.setModel(self.regModel)
	self.initData="0x00"
	self.size=64
	self.regFileTableInit()

	#define specialRegWidget
	self.specialRegWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.specialRegWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.specialRegWidget.horizontalHeader().setVisible(False)
	self.specialRegWidget.verticalHeader().setVisible(False)
	self.specialRegWidget.setShowGrid(False)
	self.specialRegWidget.setColumnCount(2)
	self.specialRegWidget.setRowCount(14)
	self.specialRegWidget.verticalHeader().setDefaultSectionSize(25)
	self.specialRegWidget.setColumnWidth(0,150)
	self.specialRegWidget.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
	self.specialRegWidget.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)

	self.specialRegWidget.setItem(0,0,QTableWidgetItem(self.tr("MPU condition")))
	self.specialRegWidget.setItem(0,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(1,0,QTableWidgetItem(self.tr("MPU loop enable")))
	self.specialRegWidget.setItem(1,1,QTableWidgetItem(self.tr("0")))
	self.specialRegWidget.setItem(2,0,QTableWidgetItem(self.tr("MPU loop0 start")))
	self.specialRegWidget.setItem(2,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(3,0,QTableWidgetItem(self.tr("MPU loop1 start")))
	self.specialRegWidget.setItem(3,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(4,0,QTableWidgetItem(self.tr("MPU loop2 start")))
	self.specialRegWidget.setItem(4,1,QTableWidgetItem(self.tr("0")))
	self.specialRegWidget.setItem(5,0,QTableWidgetItem(self.tr("MPU loop3 start")))
	self.specialRegWidget.setItem(5,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(6,0,QTableWidgetItem(self.tr("MPU loop0 end")))
	self.specialRegWidget.setItem(6,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(7,0,QTableWidgetItem(self.tr("MPU loop1 end")))
	self.specialRegWidget.setItem(7,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(8,0,QTableWidgetItem(self.tr("MPU loop2 end")))
	self.specialRegWidget.setItem(8,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(9,0,QTableWidgetItem(self.tr("MPU loop3 end")))
	self.specialRegWidget.setItem(9,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(10,0,QTableWidgetItem(self.tr("MPU loop0 number")))
	self.specialRegWidget.setItem(10,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(11,0,QTableWidgetItem(self.tr("MPU loop1 number")))
	self.specialRegWidget.setItem(11,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(12,0,QTableWidgetItem(self.tr("MPU loop2 number")))
	self.specialRegWidget.setItem(12,1,QTableWidgetItem(self.tr("0")))
        self.specialRegWidget.setItem(13,0,QTableWidgetItem(self.tr("MPU loop3 number")))
	self.specialRegWidget.setItem(13,1,QTableWidgetItem(self.tr("0")))

	splitter=QSplitter(Qt.Horizontal,widget)
	splitter.setChildrenCollapsible(False)
	splitter.addWidget(self.leftWidget)
	splitter.addWidget(self.rightTab)
	splitter.setStretchFactor(0,1)
	splitter.setStretchFactor(1,1)

	#scroll
        scroll=QScrollArea()
        scroll.setWidget(widget)
        scroll.setAutoFillBackground(True)
        scroll.setWidgetResizable(True)
        vbox = QVBoxLayout()
        vbox.addWidget(scroll)  
        centralWidget.setLayout(vbox)
	self.stageDialog=0
	
    def regFileTableInit(self):
	#define regFileWidget M
	self.regModel.setItem(0,0,QStandardItem(self.tr("M")))
	for i in range(0,self.size):
	    self.regModel.setItem(0,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(0,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(1,0,QStandardItem(self.tr("M0")))
        self.regModel.setItem(2,0,QStandardItem(self.tr("M1")))
        self.regModel.setItem(3,0,QStandardItem(self.tr("M2")))
        self.regModel.setItem(4,0,QStandardItem(self.tr("M3")))
	self.regModel.setItem(5,0,QStandardItem(self.tr("M4")))
        self.regModel.setItem(6,0,QStandardItem(self.tr("M5")))
        self.regModel.setItem(7,0,QStandardItem(self.tr("M6")))
        self.regModel.setItem(8,0,QStandardItem(self.tr("M7")))
	self.regModel.setItem(9,0,QStandardItem(self.tr("M8")))
        self.regModel.setItem(10,0,QStandardItem(self.tr("M9")))
        self.regModel.setItem(11,0,QStandardItem(self.tr("M10")))
        self.regModel.setItem(12,0,QStandardItem(self.tr("M11")))
        self.regModel.setItem(13,0,QStandardItem(self.tr("M12")))
        self.regModel.setItem(14,0,QStandardItem(self.tr("M13")))
	self.regModel.setItem(15,0,QStandardItem(self.tr("M14")))
        self.regModel.setItem(16,0,QStandardItem(self.tr("M15")))
        self.regModel.setItem(17,0,QStandardItem(self.tr("M16")))
        self.regModel.setItem(18,0,QStandardItem(self.tr("M17")))
	self.regModel.setItem(19,0,QStandardItem(self.tr("M18")))
        self.regModel.setItem(20,0,QStandardItem(self.tr("M19")))
        self.regModel.setItem(21,0,QStandardItem(self.tr("M20")))
        self.regModel.setItem(22,0,QStandardItem(self.tr("M21")))
        self.regModel.setItem(23,0,QStandardItem(self.tr("M22")))
        self.regModel.setItem(24,0,QStandardItem(self.tr("M23")))
	self.regModel.setItem(25,0,QStandardItem(self.tr("M24")))
        self.regModel.setItem(26,0,QStandardItem(self.tr("M25")))
        self.regModel.setItem(27,0,QStandardItem(self.tr("M26")))
        self.regModel.setItem(28,0,QStandardItem(self.tr("M27")))
	self.regModel.setItem(29,0,QStandardItem(self.tr("M28")))
        self.regModel.setItem(30,0,QStandardItem(self.tr("M29")))
	self.regModel.setItem(31,0,QStandardItem(self.tr("M30")))
	self.regModel.setItem(32,0,QStandardItem(self.tr("M31")))
        self.regModel.setItem(33,0,QStandardItem(self.tr("M32")))
        self.regModel.setItem(34,0,QStandardItem(self.tr("M33")))
	self.regModel.setItem(35,0,QStandardItem(self.tr("M34")))
        self.regModel.setItem(36,0,QStandardItem(self.tr("M35")))
        self.regModel.setItem(37,0,QStandardItem(self.tr("M36")))
        self.regModel.setItem(38,0,QStandardItem(self.tr("M37")))
	self.regModel.setItem(39,0,QStandardItem(self.tr("M38")))
	self.regModel.setItem(40,0,QStandardItem(self.tr("M39")))
        self.regModel.setItem(41,0,QStandardItem(self.tr("M40")))
        self.regModel.setItem(42,0,QStandardItem(self.tr("M41")))
        self.regModel.setItem(43,0,QStandardItem(self.tr("M42")))
        self.regModel.setItem(44,0,QStandardItem(self.tr("M43")))
	self.regModel.setItem(45,0,QStandardItem(self.tr("M44")))
        self.regModel.setItem(46,0,QStandardItem(self.tr("M45")))
        self.regModel.setItem(47,0,QStandardItem(self.tr("M46")))
        self.regModel.setItem(48,0,QStandardItem(self.tr("M47")))
	self.regModel.setItem(49,0,QStandardItem(self.tr("M48")))
	self.regModel.setItem(50,0,QStandardItem(self.tr("M49")))
        self.regModel.setItem(51,0,QStandardItem(self.tr("M50")))
        self.regModel.setItem(52,0,QStandardItem(self.tr("M51")))
        self.regModel.setItem(53,0,QStandardItem(self.tr("M52")))
        self.regModel.setItem(54,0,QStandardItem(self.tr("M53")))
	self.regModel.setItem(55,0,QStandardItem(self.tr("M54")))
        self.regModel.setItem(56,0,QStandardItem(self.tr("M55")))
        self.regModel.setItem(57,0,QStandardItem(self.tr("M56")))
        self.regModel.setItem(58,0,QStandardItem(self.tr("M57")))
	self.regModel.setItem(59,0,QStandardItem(self.tr("M58")))
	self.regModel.setItem(60,0,QStandardItem(self.tr("M59")))
        self.regModel.setItem(61,0,QStandardItem(self.tr("M60")))
        self.regModel.setItem(62,0,QStandardItem(self.tr("M61")))
        self.regModel.setItem(63,0,QStandardItem(self.tr("M62")))
        self.regModel.setItem(64,0,QStandardItem(self.tr("M63")))
	self.regModel.setItem(65,0,QStandardItem(self.tr("M64")))
        self.regModel.setItem(66,0,QStandardItem(self.tr("M65")))
        self.regModel.setItem(67,0,QStandardItem(self.tr("M66")))
        self.regModel.setItem(68,0,QStandardItem(self.tr("M67")))
	self.regModel.setItem(69,0,QStandardItem(self.tr("M68")))
	self.regModel.setItem(70,0,QStandardItem(self.tr("M69")))
        self.regModel.setItem(71,0,QStandardItem(self.tr("M70")))
        self.regModel.setItem(72,0,QStandardItem(self.tr("M71")))
        self.regModel.setItem(73,0,QStandardItem(self.tr("M72")))
        self.regModel.setItem(74,0,QStandardItem(self.tr("M73")))
	self.regModel.setItem(75,0,QStandardItem(self.tr("M74")))
        self.regModel.setItem(76,0,QStandardItem(self.tr("M75")))
        self.regModel.setItem(77,0,QStandardItem(self.tr("M76")))
        self.regModel.setItem(78,0,QStandardItem(self.tr("M77")))
	self.regModel.setItem(79,0,QStandardItem(self.tr("M78")))
	self.regModel.setItem(80,0,QStandardItem(self.tr("M79")))
        self.regModel.setItem(81,0,QStandardItem(self.tr("M80")))
        self.regModel.setItem(82,0,QStandardItem(self.tr("M81")))
        self.regModel.setItem(83,0,QStandardItem(self.tr("M82")))
        self.regModel.setItem(84,0,QStandardItem(self.tr("M83")))
	self.regModel.setItem(85,0,QStandardItem(self.tr("M84")))
        self.regModel.setItem(86,0,QStandardItem(self.tr("M85")))
        self.regModel.setItem(87,0,QStandardItem(self.tr("M86")))
        self.regModel.setItem(88,0,QStandardItem(self.tr("M87")))
	self.regModel.setItem(89,0,QStandardItem(self.tr("M88")))
	self.regModel.setItem(90,0,QStandardItem(self.tr("M89")))
        self.regModel.setItem(91,0,QStandardItem(self.tr("M90")))
        self.regModel.setItem(92,0,QStandardItem(self.tr("M91")))
        self.regModel.setItem(93,0,QStandardItem(self.tr("M92")))
        self.regModel.setItem(94,0,QStandardItem(self.tr("M93")))
	self.regModel.setItem(95,0,QStandardItem(self.tr("M94")))
        self.regModel.setItem(96,0,QStandardItem(self.tr("M95")))
        self.regModel.setItem(97,0,QStandardItem(self.tr("M96")))
        self.regModel.setItem(98,0,QStandardItem(self.tr("M97")))
	self.regModel.setItem(99,0,QStandardItem(self.tr("M98")))
	self.regModel.setItem(100,0,QStandardItem(self.tr("M99")))
        self.regModel.setItem(101,0,QStandardItem(self.tr("M100")))
        self.regModel.setItem(102,0,QStandardItem(self.tr("M101")))
        self.regModel.setItem(103,0,QStandardItem(self.tr("M102")))
        self.regModel.setItem(104,0,QStandardItem(self.tr("M103")))
	self.regModel.setItem(105,0,QStandardItem(self.tr("M104")))
        self.regModel.setItem(106,0,QStandardItem(self.tr("M105")))
        self.regModel.setItem(107,0,QStandardItem(self.tr("M106")))
        self.regModel.setItem(108,0,QStandardItem(self.tr("M107")))
	self.regModel.setItem(109,0,QStandardItem(self.tr("M108")))
	self.regModel.setItem(110,0,QStandardItem(self.tr("M109")))
        self.regModel.setItem(111,0,QStandardItem(self.tr("M110")))
        self.regModel.setItem(112,0,QStandardItem(self.tr("M111")))
        self.regModel.setItem(113,0,QStandardItem(self.tr("M112")))
        self.regModel.setItem(114,0,QStandardItem(self.tr("M113")))
	self.regModel.setItem(115,0,QStandardItem(self.tr("M114")))
        self.regModel.setItem(116,0,QStandardItem(self.tr("M115")))
        self.regModel.setItem(117,0,QStandardItem(self.tr("M116")))
        self.regModel.setItem(118,0,QStandardItem(self.tr("M117")))
	self.regModel.setItem(119,0,QStandardItem(self.tr("M118")))
	self.regModel.setItem(120,0,QStandardItem(self.tr("M119")))
        self.regModel.setItem(121,0,QStandardItem(self.tr("M120")))
        self.regModel.setItem(122,0,QStandardItem(self.tr("M121")))
        self.regModel.setItem(123,0,QStandardItem(self.tr("M122")))
        self.regModel.setItem(124,0,QStandardItem(self.tr("M123")))
	self.regModel.setItem(125,0,QStandardItem(self.tr("M124")))
        self.regModel.setItem(126,0,QStandardItem(self.tr("M125")))
        self.regModel.setItem(127,0,QStandardItem(self.tr("M126")))
        self.regModel.setItem(128,0,QStandardItem(self.tr("M127")))
	for i in range(1,129):
	    for j in range(1,self.size+1):
		self.regModel.setItem(i,j,QStandardItem(self.initData))
	#define regFileWidget SHU0 T
	self.regModel.setItem(129,0,QStandardItem(self.tr("SHU0 T")))
	for i in range(0,self.size):
	    self.regModel.setItem(129,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(129,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(130,0,QStandardItem(self.tr("T0")))
        self.regModel.setItem(131,0,QStandardItem(self.tr("T1")))
        self.regModel.setItem(132,0,QStandardItem(self.tr("T2")))
        self.regModel.setItem(133,0,QStandardItem(self.tr("T3")))
	self.regModel.setItem(134,0,QStandardItem(self.tr("T4")))
        self.regModel.setItem(135,0,QStandardItem(self.tr("T5")))
        self.regModel.setItem(136,0,QStandardItem(self.tr("T6")))
        self.regModel.setItem(137,0,QStandardItem(self.tr("T7")))
	self.regModel.setItem(138,0,QStandardItem(self.tr("T8")))
        self.regModel.setItem(139,0,QStandardItem(self.tr("T9")))
        self.regModel.setItem(140,0,QStandardItem(self.tr("T10")))
        self.regModel.setItem(141,0,QStandardItem(self.tr("T11")))
        self.regModel.setItem(142,0,QStandardItem(self.tr("T12")))
        self.regModel.setItem(143,0,QStandardItem(self.tr("T13")))
	self.regModel.setItem(144,0,QStandardItem(self.tr("T14")))
        self.regModel.setItem(145,0,QStandardItem(self.tr("T15")))
	for i in range(130,146):
	    for j in range(1,self.size+1):
		self.regModel.setItem(i,j,QStandardItem(self.initData))
	#define regFileWidget SHU1 T
	self.regModel.setItem(146,0,QStandardItem(self.tr("SHU1 T")))
	for i in range(0,self.size):
	    self.regModel.setItem(146,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(146,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(147,0,QStandardItem(self.tr("T0")))
        self.regModel.setItem(148,0,QStandardItem(self.tr("T1")))
        self.regModel.setItem(149,0,QStandardItem(self.tr("T2")))
        self.regModel.setItem(150,0,QStandardItem(self.tr("T3")))
	self.regModel.setItem(151,0,QStandardItem(self.tr("T4")))
        self.regModel.setItem(152,0,QStandardItem(self.tr("T5")))
        self.regModel.setItem(153,0,QStandardItem(self.tr("T6")))
        self.regModel.setItem(154,0,QStandardItem(self.tr("T7")))
	self.regModel.setItem(155,0,QStandardItem(self.tr("T8")))
        self.regModel.setItem(156,0,QStandardItem(self.tr("T9")))
        self.regModel.setItem(157,0,QStandardItem(self.tr("T10")))
        self.regModel.setItem(158,0,QStandardItem(self.tr("T11")))
        self.regModel.setItem(159,0,QStandardItem(self.tr("T12")))
        self.regModel.setItem(160,0,QStandardItem(self.tr("T13")))
	self.regModel.setItem(161,0,QStandardItem(self.tr("T14")))
        self.regModel.setItem(162,0,QStandardItem(self.tr("T15")))
	for i in range(147,163):
	    for j in range(1,self.size+1):
		self.regModel.setItem(i,j,QStandardItem(self.initData))
	#define regFileWidget IALU T
	self.regModel.setItem(163,0,QStandardItem(self.tr("IALU T")))
	for i in range(0,self.size):
	    self.regModel.setItem(163,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(163,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(164,0,QStandardItem(self.tr("T0")))
        self.regModel.setItem(165,0,QStandardItem(self.tr("T1")))
        self.regModel.setItem(166,0,QStandardItem(self.tr("T2")))
        self.regModel.setItem(167,0,QStandardItem(self.tr("T3")))
	for i in range(164,168):
	    for j in range(1,self.size+1):
		self.regModel.setItem(i,j,QStandardItem(self.initData))
	#define regFileWidget IMAC T
	self.regModel.setItem(168,0,QStandardItem(self.tr("IMAC T")))
	for i in range(0,self.size):
	    self.regModel.setItem(168,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(168,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(169,0,QStandardItem(self.tr("T0")))
        self.regModel.setItem(170,0,QStandardItem(self.tr("T1")))
        self.regModel.setItem(171,0,QStandardItem(self.tr("T2")))
        self.regModel.setItem(172,0,QStandardItem(self.tr("T3")))
	for i in range(169,173):
	    for j in range(1,self.size+1):
		self.regModel.setItem(i,j,QStandardItem(self.initData))
	#define regFileWidget FALU T
	self.regModel.setItem(173,0,QStandardItem(self.tr("FALU T")))
	for i in range(0,self.size):
	    self.regModel.setItem(173,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(173,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(174,0,QStandardItem(self.tr("T0")))
        self.regModel.setItem(175,0,QStandardItem(self.tr("T1")))
        self.regModel.setItem(176,0,QStandardItem(self.tr("T2")))
        self.regModel.setItem(177,0,QStandardItem(self.tr("T3")))
	for i in range(174,178):
	    for j in range(1,self.size+1):
		self.regModel.setItem(i,j,QStandardItem(self.initData))
	#define regFileWidget FMAC T
	self.regModel.setItem(178,0,QStandardItem(self.tr("FMAC T")))
	for i in range(0,self.size):
	    self.regModel.setItem(178,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(178,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(179,0,QStandardItem(self.tr("T0")))
        self.regModel.setItem(180,0,QStandardItem(self.tr("T1")))
        self.regModel.setItem(181,0,QStandardItem(self.tr("T2")))
        self.regModel.setItem(182,0,QStandardItem(self.tr("T3")))
	for i in range(179,183):
	    for j in range(1,self.size+1):
		self.regModel.setItem(i,j,QStandardItem(self.initData))
	#define regFileWidget IMRL
        self.regModel.setItem(183,0,QStandardItem(self.tr("IMRL")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(183,j,QStandardItem(self.initData))
	#define regFileWidget IMRH
        self.regModel.setItem(184,0,QStandardItem(self.tr("IMRH")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(184,j,QStandardItem(self.initData))
	#define regFileWidget FMR
        self.regModel.setItem(185,0,QStandardItem(self.tr("FMR")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(185,j,QStandardItem(self.initData))
	#define regFileWidget BIU0W
        self.regModel.setItem(186,0,QStandardItem(self.tr("BIU0W")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(186,j,QStandardItem(self.initData))
	#define regFileWidget BIU1W
        self.regModel.setItem(187,0,QStandardItem(self.tr("BIU1W")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(187,j,QStandardItem(self.initData))
	#define regFileWidget BIU2W
        self.regModel.setItem(188,0,QStandardItem(self.tr("BIU2W")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(188,j,QStandardItem(self.initData))
	#define regFileWidget DIVQU
        self.regModel.setItem(189,0,QStandardItem(self.tr("DIVQU")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(189,j,QStandardItem(self.initData))
	#define regFileWidget DIVRE
        self.regModel.setItem(190,0,QStandardItem(self.tr("DIVRE")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(190,j,QStandardItem(self.initData))
	#define regFileWidget DIV-CNT
        self.regModel.setItem(191,0,QStandardItem(self.tr("DIV-CNT")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(191,j,QStandardItem(self.initData))
	#define regFileWidget SVR
        self.regModel.setItem(192,0,QStandardItem(self.tr("SVR")))
	for i in range(0,self.size):
	    self.regModel.setItem(192,i+1,QStandardItem(self.tr("")))
	for i in range(0,self.size+1):
	    self.regModel.item(192,i).setBackground(QBrush(QColor(192,192,192)))
        self.regModel.setItem(193,0,QStandardItem(self.tr("SVR0")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(193,j,QStandardItem(self.initData))
        self.regModel.setItem(194,0,QStandardItem(self.tr("SVR1")))
	for j in range(1,self.size+1):
	    self.regModel.setItem(194,j,QStandardItem(self.initData))	
	self.regFileWidget.resizeColumnsToContents()

    def stageButtonSlot(self):
	self.stageDialog.setWindowTitle("MPU Instruction Pipeline Diagram")
	self.stageDialog.openFlag=1
	self.stageDialog.show()

    def currentIndexSlot(self,index):
	if index==0:
	    self.initData="0x00"
	elif index==1:
	    self.initData="0x0000"
	elif index==2:
	    self.initData="0x00000000"
	elif index==3:
	    self.initData="0.00"
	elif index==4:
	    self.initData="0.0000"
	self.regFileTableInit()
	self.indexCall()

    def indexCallback(self,indexCall):
	self.indexCall=indexCall

    def updateMPUFloatDialog(self,r):
	#update float dialog
	#clear stage list
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
	#BIU0
	for i in range(0,40,2):
	    if r[40+i]!="nop":
		self.buttonWidget.BIU0ButtonDialog.stages.item(i/2,1).setData(0,r[40+i])
		self.buttonWidget.BIU0ButtonDialog.stages.item(i/2,1).setToolTip(r[40+i+1])
	#BIU1
	for i in range(0,40,2):
	    if r[80+i]!="nop":
		self.buttonWidget.BIU1ButtonDialog.stages.item(i/2,1).setData(0,r[80+i])
		self.buttonWidget.BIU1ButtonDialog.stages.item(i/2,1).setToolTip(r[80+i+1])
	#BIU2
	for i in range(0,40,2):
	    if r[120+i]!="nop":
		self.buttonWidget.BIU2ButtonDialog.stages.item(i/2,1).setData(0,r[120+i])
		self.buttonWidget.BIU2ButtonDialog.stages.item(i/2,1).setToolTip(r[120+i+1])
	#SHU0
	for i in range(0,40,2):
	    if r[160+i]!="nop":
		self.buttonWidget.SHU0ButtonDialog.stages.item(i/2,1).setData(0,r[160+i])
		self.buttonWidget.SHU0ButtonDialog.stages.item(i/2,1).setToolTip(r[160+i+1])
	#MRF
	for i in range(0,40,2):
	    if r[200+i]!="nop":
		self.buttonWidget.MRFButtonDialog.stages.item(i/2,1).setData(0,r[200+i])
		self.buttonWidget.MRFButtonDialog.stages.item(i/2,1).setToolTip(r[200+i+1])
	#SHU1
	for i in range(0,40,2):
	    if r[240+i]!="nop":
		self.buttonWidget.SHU1ButtonDialog.stages.item(i/2,1).setData(0,r[240+i])
		self.buttonWidget.SHU1ButtonDialog.stages.item(i/2,1).setToolTip(r[240+i+1])
	#IALU
	for i in range(0,40,2):
	    if r[280+i]!="nop":
		self.buttonWidget.IALUButtonDialog.stages.item(i/2,1).setData(0,r[280+i])
		self.buttonWidget.IALUButtonDialog.stages.item(i/2,1).setToolTip(r[280+i+1])
	#IMAC
	for i in range(0,40,2):
	    if r[320+i]!="nop":
		self.buttonWidget.IMACButtonDialog.stages.item(i/2,1).setData(0,r[320+i])
		self.buttonWidget.IMACButtonDialog.stages.item(i/2,1).setToolTip(r[320+i+1])
	#FALU
	for i in range(0,40,2):
	    if r[360+i]!="nop":
		self.buttonWidget.FALUButtonDialog.stages.item(i/2,1).setData(0,r[360+i])
		self.buttonWidget.FALUButtonDialog.stages.item(i/2,1).setToolTip(r[360+i+1])
	#FMAC
	for i in range(0,40,2):
	    if r[400+i]!="nop":
		self.buttonWidget.FMACButtonDialog.stages.item(i/2,1).setData(0,r[400+i])
		self.buttonWidget.FMACButtonDialog.stages.item(i/2,1).setToolTip(r[400+i+1])

    def updateMPUWidget(self,r):
	#update mpu reg
	self.regValue=r
	index=self.byteComboBox.currentIndex()
	num=64
	if index<=2:
	    if index==0:      #1
		length=4
	    elif index==1:    #2
		length=6
	    elif index==2:    #4
		length=10
	    for i in range(0,187):
	        if i>=0 and i<=127:  #m0-127
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+1,j+1,QStandardItem(s))
		            self.regModel.item(i+1,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+1,j+1).setText(self.initData)
		            self.regModel.item(i+1,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+1,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=128 and i<=143: #shu0
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+2,j+1,QStandardItem(s))
		            self.regModel.item(i+2,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+2,j+1).setText(self.initData)
		            self.regModel.item(i+2,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+2,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=144 and i<=159: #shu1
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+3,j+1,QStandardItem(s))
		            self.regModel.item(i+3,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+3,j+1).setText(self.initData)
		            self.regModel.item(i+3,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+3,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=160 and i<=163: #ialu
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+4,j+1,QStandardItem(s))
		            self.regModel.item(i+4,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+4,j+1).setText(self.initData)
		            self.regModel.item(i+4,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+4,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=164 and i<=167:  #imac
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+5,j+1,QStandardItem(s))
		            self.regModel.item(i+5,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+5,j+1).setText(self.initData)
		            self.regModel.item(i+5,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+5,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=168 and i<=171: #falu
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+6,j+1,QStandardItem(s))
		            self.regModel.item(i+6,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+6,j+1).setText(self.initData)
		            self.regModel.item(i+6,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+6,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=172 and i<=175: #fmac
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+7,j+1,QStandardItem(s))
		            self.regModel.item(i+7,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+7,j+1).setText(self.initData)
		            self.regModel.item(i+7,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+7,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+7,j+1,QStandardItem(s))
		            self.regModel.item(i+7,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+7,j+1).setText(self.initData)
		            self.regModel.item(i+7,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+7,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=185 and i<=186: #svr
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    self.regModel.setItem(i+8,j+1,QStandardItem(s))
		            self.regModel.item(i+8,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+8,j+1).setText(self.initData)
		            self.regModel.item(i+8,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+8,j+1).setForeground(QBrush(QColor(0,0,0)))
	elif index==3:    #float
	    length=10
	    for i in range(0,187):
	        if i>=0 and i<=127:  #m0-127
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(round(float(f),7))
			    self.regModel.setItem(i+1,j+1,QStandardItem(s))
		            self.regModel.item(i+1,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+1,j+1).setText(self.initData)
		            self.regModel.item(i+1,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+1,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=128 and i<=143: #shu0
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+2,j+1,QStandardItem(s))
		            self.regModel.item(i+2,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+2,j+1).setText(self.initData)
		            self.regModel.item(i+2,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+2,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=144 and i<=159: #shu1
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+3,j+1,QStandardItem(s))
		            self.regModel.item(i+3,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+3,j+1).setText(self.initData)
		            self.regModel.item(i+3,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+3,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=160 and i<=163: #ialu
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+4,j+1,QStandardItem(s))
		            self.regModel.item(i+4,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+4,j+1).setText(self.initData)
		            self.regModel.item(i+4,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+4,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=164 and i<=167:  #imac
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+5,j+1,QStandardItem(s))
		            self.regModel.item(i+5,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+5,j+1).setText(self.initData)
		            self.regModel.item(i+5,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+5,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=168 and i<=171: #falu
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+6,j+1,QStandardItem(s))
		            self.regModel.item(i+6,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+6,j+1).setText(self.initData)
		            self.regModel.item(i+6,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+6,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=172 and i<=175: #fmac
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+7,j+1,QStandardItem(s))
		            self.regModel.item(i+7,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+7,j+1).setText(self.initData)
		            self.regModel.item(i+7,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+7,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+7,j+1,QStandardItem(s))
		            self.regModel.item(i+7,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+7,j+1).setText(self.initData)
		            self.regModel.item(i+7,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+7,j+1).setForeground(QBrush(QColor(0,0,0)))	
	        elif i>=185 and i<=186: #svr
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+8,j+1,QStandardItem(s))
		            self.regModel.item(i+8,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+8,j+1).setText(self.initData)
		            self.regModel.item(i+8,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+8,j+1).setForeground(QBrush(QColor(0,0,0)))
	elif index==4:    #double
	    length=18
	    for i in range(0,187):
	        if i>=0 and i<=127:  #m0-127
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+1,j+1,QStandardItem(s))
		            self.regModel.item(i+1,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+1,j+1).setText(self.initData)
		            self.regModel.item(i+1,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+1,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=128 and i<=143: #shu0
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+2,j+1,QStandardItem(s))
		            self.regModel.item(i+2,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+2,j+1).setText(self.initData)
		            self.regModel.item(i+2,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+2,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=144 and i<=159: #shu1
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+3,j+1,QStandardItem(s))
		            self.regModel.item(i+3,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+3,j+1).setText(self.initData)
		            self.regModel.item(i+3,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+3,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=160 and i<=163: #ialu
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+4,j+1,QStandardItem(s))
		            self.regModel.item(i+4,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+4,j+1).setText(self.initData)
		            self.regModel.item(i+4,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+4,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=164 and i<=167:  #imac
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+5,j+1,QStandardItem(s))
		            self.regModel.item(i+5,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+5,j+1).setText(self.initData)
		            self.regModel.item(i+5,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+5,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=168 and i<=171: #falu
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+6,j+1,QStandardItem(s))
		            self.regModel.item(i+6,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+6,j+1).setText(self.initData)
		            self.regModel.item(i+6,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+6,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=172 and i<=175: #fmac
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+7,j+1,QStandardItem(s))
		            self.regModel.item(i+7,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+7,j+1).setText(self.initData)
		            self.regModel.item(i+7,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+7,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+7,j+1,QStandardItem(s))
		            self.regModel.item(i+7,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+7,j+1).setText(self.initData)
		            self.regModel.item(i+7,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+7,j+1).setForeground(QBrush(QColor(0,0,0)))
	        elif i>=185 and i<=186: #svr
	    	    if r[1+i]!="nop":
			stringList=r[i+1].split(" ")
			num=len(stringList)-1
			for j in range(0,num):
			    s=stringList[j]
			    bit=len(s)
			    if bit<length:
				s=s[:2]+'0'*(length-bit)+s[2:]
			    f= eval(s)
			    s=str(float(f))
			    self.regModel.setItem(i+8,j+1,QStandardItem(s))
		            self.regModel.item(i+8,j+1).setToolTip(s)
		    else:
		        for j in range(0,num):
	                    self.regModel.item(i+8,j+1).setText(self.initData)
		            self.regModel.item(i+8,j+1).setToolTip("")
		    for j in range(0,num):
	                self.regModel.item(i+8,j+1).setForeground(QBrush(QColor(0,0,0)))
	self.regFileWidget.resizeColumnsToContents()
	for i in range(0,14):
	    if r[386+i]!="nop":
		self.specialRegWidget.item(i,1).setData(0,r[386+i])
		self.specialRegWidget.item(i,1).setToolTip(r[386+i])
	    else:
		self.specialRegWidget.item(i,1).setData(0,"0")
		self.specialRegWidget.item(i,1).setToolTip("")

    def updateMPURegWFlag(self,r):
	i=r[7]
	if i>=0 and i<=127:
	    for j in range(1,self.size+1):
	        self.regModel.item(i+1,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=128 and i<=143: #shu0
	    for j in range(1,self.size+1):
	        self.regModel.item(i+2,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=144 and i<=159: #shu1
	    for j in range(1,self.size+1):
	        self.regModel.item(i+3,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=160 and i<=163: #ialu
	    for j in range(1,self.size+1):
	        self.regModel.item(i+4,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=164 and i<=167:  #imac
	    for j in range(1,self.size+1):
	        self.regModel.item(i+5,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=168 and i<=171: #falu
	    for j in range(1,self.size+1):
	        self.regModel.item(i+6,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=172 and i<=175: #fmac
	    for j in range(1,self.size+1):
	        self.regModel.item(i+7,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    for j in range(1,self.size+1):
	        self.regModel.item(i+7,j).setForeground(QBrush(QColor(255,0,0)))
	elif i>=185 and i<=186:
	    for j in range(1,self.size+1):
	        self.regModel.item(i+8,j).setForeground(QBrush(QColor(255,0,0)))

    def updateMPURegRFlag(self,i):
	if i>=0 and i<=127:	
	    if self.regModel.item(i+1,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+1,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=128 and i<=143: #shu0
	    if self.regModel.item(i+2,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+2,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=144 and i<=159: #shu1
	    if self.regModel.item(i+3,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+3,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=160 and i<=163: #ialu
	    if self.regModel.item(i+4,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+4,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=164 and i<=167:  #imac
	    if self.regModel.item(i+5,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+5,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=168 and i<=171: #falu
	    if self.regModel.item(i+6,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+6,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=172 and i<=175: #fmac
	    if self.regModel.item(i+7,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+7,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=176 and i<=184: #IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU,DIVER,DIV-CNT
	    if self.regModel.item(i+7,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+7,j).setForeground(QBrush(QColor(255,153,18)))
	elif i>=185 and i<=186:
	    if self.regModel.item(i+8,1).foreground()==QBrush(QColor(255,0,0)):
	        for j in range(1,self.size+1):
	    	    self.regModel.item(i+8,j).setForeground(QBrush(QColor(255,153,18)))

    def closeChildDialog(self):
	if self.stageDialog!=0:
	    if self.stageDialog.openFlag==1:
	        self.stageDialog.close()
	self.buttonWidget.closeFloatDialogs()	

    def setButtonEnabled(self,enable):
	self.stageButton.setEnabled(True)
	self.buttonWidget.BIU0Button.setEnabled(True)
	self.buttonWidget.BIU1Button.setEnabled(True)
	self.buttonWidget.BIU2Button.setEnabled(True)
	self.buttonWidget.SHU0Button.setEnabled(True)
	self.buttonWidget.MRFButton.setEnabled(True)
	self.buttonWidget.SHU1Button.setEnabled(True)
	self.buttonWidget.IALUButton.setEnabled(True)
	self.buttonWidget.IMACButton.setEnabled(True)
	self.buttonWidget.FALUButton.setEnabled(True)
	self.buttonWidget.FMACButton.setEnabled(True)



