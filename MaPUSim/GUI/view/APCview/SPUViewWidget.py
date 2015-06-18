# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from StageDialog import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class SPUViewWidget(QWidget):
    def __init__(self,parent=None):
	super(SPUViewWidget,self).__init__(parent)

	self.leftWidget=QWidget()
	self.stageButton=QPushButton("SPU Stage")
	self.stageButton.setFixedSize(100,40)
	self.connect(self.stageButton,SIGNAL("clicked()"),self.stageButtonSlot)
	leftLay=QVBoxLayout()
	leftLay.addWidget(self.stageButton)
	self.leftWidget.setLayout(leftLay)

	self.rightTab=QTabWidget()

	#define rightTab
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
	self.regFileWidget.setRowCount(66)
	self.regFileWidget.verticalHeader().setDefaultSectionSize(25)
	self.regFileWidget.setColumnWidth(0,122)
	#define regFileWidget R 
	self.regFileWidget.setItem(0,0,QTableWidgetItem(self.tr("R")))
	self.regFileWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(1,0,QTableWidgetItem(self.tr("R0")))
	self.regFileWidget.setItem(1,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(2,0,QTableWidgetItem(self.tr("R1")))
	self.regFileWidget.setItem(2,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(3,0,QTableWidgetItem(self.tr("R2")))
	self.regFileWidget.setItem(3,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(4,0,QTableWidgetItem(self.tr("R3")))
	self.regFileWidget.setItem(4,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(5,0,QTableWidgetItem(self.tr("R4")))
	self.regFileWidget.setItem(5,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(6,0,QTableWidgetItem(self.tr("R5")))
	self.regFileWidget.setItem(6,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(7,0,QTableWidgetItem(self.tr("R6")))
	self.regFileWidget.setItem(7,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(8,0,QTableWidgetItem(self.tr("R7")))
	self.regFileWidget.setItem(8,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(9,0,QTableWidgetItem(self.tr("R8")))
	self.regFileWidget.setItem(9,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(10,0,QTableWidgetItem(self.tr("R9")))
	self.regFileWidget.setItem(10,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(11,0,QTableWidgetItem(self.tr("R10")))
	self.regFileWidget.setItem(11,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(12,0,QTableWidgetItem(self.tr("R11")))
	self.regFileWidget.setItem(12,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(13,0,QTableWidgetItem(self.tr("R12")))
	self.regFileWidget.setItem(13,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(14,0,QTableWidgetItem(self.tr("R13")))
	self.regFileWidget.setItem(14,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(15,0,QTableWidgetItem(self.tr("R14")))
	self.regFileWidget.setItem(15,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(16,0,QTableWidgetItem(self.tr("R15")))
	self.regFileWidget.setItem(16,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(17,0,QTableWidgetItem(self.tr("R16")))
	self.regFileWidget.setItem(17,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(18,0,QTableWidgetItem(self.tr("R17")))
	self.regFileWidget.setItem(18,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(19,0,QTableWidgetItem(self.tr("R18")))
	self.regFileWidget.setItem(19,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(20,0,QTableWidgetItem(self.tr("R19")))
	self.regFileWidget.setItem(20,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(21,0,QTableWidgetItem(self.tr("R20")))
	self.regFileWidget.setItem(21,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(22,0,QTableWidgetItem(self.tr("R21")))
	self.regFileWidget.setItem(22,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(23,0,QTableWidgetItem(self.tr("R22")))
	self.regFileWidget.setItem(23,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(24,0,QTableWidgetItem(self.tr("R23")))
	self.regFileWidget.setItem(24,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(25,0,QTableWidgetItem(self.tr("R24")))
	self.regFileWidget.setItem(25,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(26,0,QTableWidgetItem(self.tr("R25")))
	self.regFileWidget.setItem(26,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(27,0,QTableWidgetItem(self.tr("R26")))
	self.regFileWidget.setItem(27,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(28,0,QTableWidgetItem(self.tr("R27")))
	self.regFileWidget.setItem(28,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(29,0,QTableWidgetItem(self.tr("R28")))
	self.regFileWidget.setItem(29,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(30,0,QTableWidgetItem(self.tr("R29")))
	self.regFileWidget.setItem(30,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(31,0,QTableWidgetItem(self.tr("R30")))
	self.regFileWidget.setItem(31,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(32,0,QTableWidgetItem(self.tr("R31")))
	self.regFileWidget.setItem(32,1,QTableWidgetItem(self.tr("0")))
	#define regFileWidget J
	self.regFileWidget.setItem(33,0,QTableWidgetItem(self.tr("J")))
	self.regFileWidget.setItem(33,1,QTableWidgetItem(self.tr("")))
	self.regFileWidget.item(33,0).setBackgroundColor(QColor(192,192,192))
	self.regFileWidget.item(33,1).setBackgroundColor(QColor(192,192,192))
        self.regFileWidget.setItem(34,0,QTableWidgetItem(self.tr("J0")))
	self.regFileWidget.setItem(34,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(35,0,QTableWidgetItem(self.tr("J1")))
	self.regFileWidget.setItem(35,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(36,0,QTableWidgetItem(self.tr("J2")))
	self.regFileWidget.setItem(36,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(37,0,QTableWidgetItem(self.tr("J3")))
	self.regFileWidget.setItem(37,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(38,0,QTableWidgetItem(self.tr("J4")))
	self.regFileWidget.setItem(38,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(39,0,QTableWidgetItem(self.tr("J5")))
	self.regFileWidget.setItem(39,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(40,0,QTableWidgetItem(self.tr("J6")))
	self.regFileWidget.setItem(40,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(41,0,QTableWidgetItem(self.tr("J7")))
	self.regFileWidget.setItem(41,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(42,0,QTableWidgetItem(self.tr("J8")))
	self.regFileWidget.setItem(42,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(43,0,QTableWidgetItem(self.tr("J9")))
	self.regFileWidget.setItem(43,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(44,0,QTableWidgetItem(self.tr("J10")))
	self.regFileWidget.setItem(44,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(45,0,QTableWidgetItem(self.tr("J11")))
	self.regFileWidget.setItem(45,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(46,0,QTableWidgetItem(self.tr("J12")))
	self.regFileWidget.setItem(46,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(47,0,QTableWidgetItem(self.tr("J13")))
	self.regFileWidget.setItem(47,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(48,0,QTableWidgetItem(self.tr("J14")))
	self.regFileWidget.setItem(48,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(49,0,QTableWidgetItem(self.tr("J15")))
	self.regFileWidget.setItem(49,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(50,0,QTableWidgetItem(self.tr("J16")))
	self.regFileWidget.setItem(50,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(51,0,QTableWidgetItem(self.tr("J17")))
	self.regFileWidget.setItem(51,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(52,0,QTableWidgetItem(self.tr("J18")))
	self.regFileWidget.setItem(52,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(53,0,QTableWidgetItem(self.tr("J19")))
	self.regFileWidget.setItem(53,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(54,0,QTableWidgetItem(self.tr("J20")))
	self.regFileWidget.setItem(54,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(55,0,QTableWidgetItem(self.tr("J21")))
	self.regFileWidget.setItem(55,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(56,0,QTableWidgetItem(self.tr("J22")))
	self.regFileWidget.setItem(56,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(57,0,QTableWidgetItem(self.tr("J23")))
	self.regFileWidget.setItem(57,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(58,0,QTableWidgetItem(self.tr("J24")))
	self.regFileWidget.setItem(58,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(59,0,QTableWidgetItem(self.tr("J25")))
	self.regFileWidget.setItem(59,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(60,0,QTableWidgetItem(self.tr("J26")))
	self.regFileWidget.setItem(60,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(61,0,QTableWidgetItem(self.tr("J27")))
	self.regFileWidget.setItem(61,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(62,0,QTableWidgetItem(self.tr("J28")))
	self.regFileWidget.setItem(62,1,QTableWidgetItem(self.tr("0")))
        self.regFileWidget.setItem(63,0,QTableWidgetItem(self.tr("J29")))
	self.regFileWidget.setItem(63,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(64,0,QTableWidgetItem(self.tr("J30")))
	self.regFileWidget.setItem(64,1,QTableWidgetItem(self.tr("0")))
	self.regFileWidget.setItem(65,0,QTableWidgetItem(self.tr("J31")))
	self.regFileWidget.setItem(65,1,QTableWidgetItem(self.tr("0")))

	#define specialRegWidget
	self.specialRegWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.specialRegWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.specialRegWidget.horizontalHeader().setVisible(False)
	self.specialRegWidget.verticalHeader().setVisible(False)
	self.specialRegWidget.setShowGrid(False)
	self.specialRegWidget.setColumnCount(2)
	self.specialRegWidget.setRowCount(3)
	self.specialRegWidget.verticalHeader().setDefaultSectionSize(25)
	self.specialRegWidget.setColumnWidth(1,2000)
	#define specialRegWidget SVR 
	self.specialRegWidget.setItem(0,0,QTableWidgetItem(self.tr("SVR")))
	self.specialRegWidget.setItem(0,1,QTableWidgetItem(self.tr("")))
	self.specialRegWidget.item(0,0).setBackgroundColor(QColor(192,192,192))
	self.specialRegWidget.item(0,1).setBackgroundColor(QColor(192,192,192))
        self.specialRegWidget.setItem(1,0,QTableWidgetItem(self.tr("SVR0")))
        self.specialRegWidget.setItem(2,0,QTableWidgetItem(self.tr("SVR1")))

	mainLayout=QHBoxLayout()
	mainLayout.addWidget(self.leftWidget)
	mainLayout.addWidget(self.rightTab)
	mainLayout.setStretchFactor(self.leftWidget,5)
	mainLayout.setStretchFactor(self.rightTab,2)
	self.setLayout(mainLayout)

	self.stageDialog=StageDialog()

    def stageButtonSlot(self):
	self.stageDialog.setWindowTitle("SPU Stage Dialog")
	self.stageDialog.show()

    def updateSPUWidget(self,r):
	for i in range (0,64):
	    if i>=0 and i<=31:
	        if r[308+i]!="nop":
		    self.regFileWidget.item(i+1,1).setData(0,r[308+i])
		    self.regFileWidget.item(i+1,1).setToolTip(r[308+i])
		else:
		    self.regFileWidget.item(i+1,1).setData(0,"0")
		    self.regFileWidget.item(i+1,1).setToolTip("")
	        self.regFileWidget.item(i+1,1).setTextColor(QColor(0,0,0))
	    else:
	        if r[308+i]!="nop":
		    self.regFileWidget.item(i+2,1).setData(0,r[308+i])
		    self.regFileWidget.item(i+2,1).setToolTip(r[308+i])
		else:
		    self.regFileWidget.item(i+2,1).setData(0,"0")
		    self.regFileWidget.item(i+2,1).setToolTip("")
	        self.regFileWidget.item(i+2,1).setTextColor(QColor(0,0,0))

    def updateSPURegWFlag(self,r):
	i=r[7]
	if i>=0 and i<=31:
	    self.regFileWidget.item(i+1,1).setTextColor(QColor(255,0,0))
	else:
	    self.regFileWidget.item(i+2,1).setTextColor(QColor(255,0,0))

    def updateSPURegRFlag(self,i):
	if i>=0 and i<=31:
	    if self.regFileWidget.item(i+1,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+1,1).setTextColor(QColor(255,153,18))
	else:
	    if self.regFileWidget.item(i+2,1).textColor()==QColor(255,0,0):
	    	self.regFileWidget.item(i+2,1).setTextColor(QColor(255,153,18))



