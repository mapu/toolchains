# -*- coding =utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys

class ConfigViewWidget(QMainWindow):
    #define signal
    simulatorDoneSignal=pyqtSignal(int)
    simulatorShowSignal=pyqtSignal(int,str)
    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)

        centralWidget = QWidget()
        self.setCentralWidget(centralWidget)
        
	self.simulatorPath=""	

        widget = QWidget()
        widget.setMinimumSize (1000,500)

	self.fullRadio=QRadioButton(self.tr("Full system"))
	self.fullRadio.setFixedHeight(30)
	self.fullRadio.setFixedWidth(900)
	
	self.APCRadio=QRadioButton(self.tr("APC standalone system"))
	self.APCRadio.setFixedHeight(30)
	self.APCRadio.setFixedWidth(900)
	self.traceFlagsButton=QPushButton(self.tr("Trace flags..."))
	self.traceFlagsButton.setFixedSize(150,30)
	self.traceFileLabel=QLabel(self.tr("  Trace file"))
	self.traceFileLabel.setFixedHeight(30)
	self.traceFileLabel.setFixedWidth(900)
	self.traceFileEdit=QLineEdit(self.tr("aaa.out"))
	self.traceFileEdit.setFixedSize(400,30)	

	self.startButton=QPushButton("Start")
	self.startButton.setFixedSize(100,30)
	self.stopButton=QPushButton("Stop")
	self.stopButton.setFixedSize(100,30)
	buttonLay=QHBoxLayout()
	buttonLay.addWidget(self.startButton)
	buttonLay.addWidget(self.stopButton)
	self.APE0Label=QLabel(self.tr("       APE0"))
	self.APE0Label.setFixedHeight(30)
	self.APE0SPULabel=QLabel(self.tr("     SPU"))
	self.APE0SPULabel.setFixedSize(60,30)
	self.APE0SPUEdit=QLineEdit(self.tr("/home/litt/tlb.s.out"))
	self.APE0SPUEdit.setFixedSize(400,30)
	self.APE0SPUButton=QPushButton(self.tr("Browse"))
	self.APE0SPUButton.setFixedSize(100,30)
	blank0=QLabel()
	blank0.setFixedSize(550,30)
	APE0SPULay=QHBoxLayout()
	APE0SPULay.addWidget(self.APE0SPULabel)
	APE0SPULay.addWidget(self.APE0SPUEdit)
	APE0SPULay.addWidget(self.APE0SPUButton)
	APE0SPULay.addWidget(blank0)
	self.APE0MPULabel=QLabel(self.tr("     MPU"))
	self.APE0MPULabel.setFixedSize(60,30)
	self.APE0MPUEdit=QLineEdit(self.tr("/home/litt/tlb.m.out"))
	self.APE0MPUEdit.setFixedSize(400,30)
	self.APE0MPUButton=QPushButton(self.tr("Browse"))
	self.APE0MPUButton.setFixedSize(100,30)
	blank1=QLabel()
	blank1.setFixedSize(550,30)
	APE0MPULay=QHBoxLayout()
	APE0MPULay.addWidget(self.APE0MPULabel)
	APE0MPULay.addWidget(self.APE0MPUEdit)
	APE0MPULay.addWidget(self.APE0MPUButton)
	APE0MPULay.addWidget(blank1)

	self.APE1Check=QCheckBox(self.tr("APE1"))
	self.APE1Check.setFixedHeight(30)
	self.APE1SPULabel=QLabel(self.tr("     SPU"))
	self.APE1SPULabel.setFixedSize(60,30)
	self.APE1SPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE1SPUEdit.setFixedSize(400,30)
	self.APE1SPUButton=QPushButton(self.tr("Browse"))
	self.APE1SPUButton.setFixedSize(100,30)
	blank2=QLabel()
	blank2.setFixedSize(550,30)
	APE1SPULay=QHBoxLayout()
	APE1SPULay.addWidget(self.APE1SPULabel)
	APE1SPULay.addWidget(self.APE1SPUEdit)
	APE1SPULay.addWidget(self.APE1SPUButton)
	APE1SPULay.addWidget(blank2)
	self.APE1MPULabel=QLabel(self.tr("     MPU"))
	self.APE1MPULabel.setFixedSize(60,30)
	self.APE1MPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE1MPUEdit.setFixedSize(400,30)
	self.APE1MPUButton=QPushButton(self.tr("Browse"))
	self.APE1MPUButton.setFixedSize(100,30)
	blank3=QLabel()
	blank3.setFixedSize(550,30)
	APE1MPULay=QHBoxLayout()
	APE1MPULay.addWidget(self.APE1MPULabel)
	APE1MPULay.addWidget(self.APE1MPUEdit)
	APE1MPULay.addWidget(self.APE1MPUButton)
	APE1MPULay.addWidget(blank3)

	self.APE2Check=QCheckBox(self.tr("APE2"))
	self.APE2Check.setFixedHeight(30)
	self.APE2SPULabel=QLabel(self.tr("     SPU"))
	self.APE2SPULabel.setFixedSize(60,30)
	self.APE2SPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE2SPUEdit.setFixedSize(400,30)
	self.APE2SPUButton=QPushButton(self.tr("Browse"))
	self.APE2SPUButton.setFixedSize(100,30)
	blank4=QLabel()
	blank4.setFixedSize(550,30)
	APE2SPULay=QHBoxLayout()
	APE2SPULay.addWidget(self.APE2SPULabel)
	APE2SPULay.addWidget(self.APE2SPUEdit)
	APE2SPULay.addWidget(self.APE2SPUButton)
	APE2SPULay.addWidget(blank4)
	self.APE2MPULabel=QLabel(self.tr("     MPU"))
	self.APE2MPULabel.setFixedSize(60,30)
	self.APE2MPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE2MPUEdit.setFixedSize(400,30)
	self.APE2MPUButton=QPushButton(self.tr("Browse"))
	self.APE2MPUButton.setFixedSize(100,30)
	blank5=QLabel()
	blank5.setFixedSize(550,30)
	APE2MPULay=QHBoxLayout()
	APE2MPULay.addWidget(self.APE2MPULabel)
	APE2MPULay.addWidget(self.APE2MPUEdit)
	APE2MPULay.addWidget(self.APE2MPUButton)
	APE2MPULay.addWidget(blank5)

	self.APE3Check=QCheckBox(self.tr("APE3"))
	self.APE3Check.setFixedHeight(30)
	self.APE3SPULabel=QLabel(self.tr("     SPU"))
	self.APE3SPULabel.setFixedSize(60,30)
	self.APE3SPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE3SPUEdit.setFixedSize(400,30)
	self.APE3SPUButton=QPushButton(self.tr("Browse"))
	self.APE3SPUButton.setFixedSize(100,30)
	blank6=QLabel()
	blank6.setFixedSize(550,30)
	APE3SPULay=QHBoxLayout()
	APE3SPULay.addWidget(self.APE3SPULabel)
	APE3SPULay.addWidget(self.APE3SPUEdit)
	APE3SPULay.addWidget(self.APE3SPUButton)
	APE3SPULay.addWidget(blank6)
	self.APE3MPULabel=QLabel(self.tr("     MPU"))
	self.APE3MPULabel.setFixedSize(60,30)
	self.APE3MPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE3MPUEdit.setFixedSize(400,30)
	self.APE3MPUButton=QPushButton(self.tr("Browse"))
	self.APE3MPUButton.setFixedSize(100,30)
	blank7=QLabel()
	blank7.setFixedSize(550,30)
	APE3MPULay=QHBoxLayout()
	APE3MPULay.addWidget(self.APE3MPULabel)
	APE3MPULay.addWidget(self.APE3MPUEdit)
	APE3MPULay.addWidget(self.APE3MPUButton)
	APE3MPULay.addWidget(blank7)
	APE3MPULay.addWidget(self.startButton)
	APE3MPULay.addWidget(self.stopButton)

	APCGroupWidget=QWidget()
	APCGroup=QGroupBox(APCGroupWidget)
	APCLay=QVBoxLayout()
	APCLay.addWidget(self.traceFlagsButton)	
	APCLay.addWidget(self.traceFileLabel)
	APCLay.addWidget(self.traceFileEdit)
	APCLay.addWidget(self.APE0Label)
	APCLay.addLayout(APE0SPULay)
	APCLay.addLayout(APE0MPULay)
	APCLay.addWidget(self.APE1Check)
	APCLay.addLayout(APE1SPULay)
	APCLay.addLayout(APE1MPULay)
	APCLay.addWidget(self.APE2Check)
	APCLay.addLayout(APE2SPULay)
	APCLay.addLayout(APE2MPULay)
	APCLay.addWidget(self.APE3Check)
	APCLay.addLayout(APE3SPULay)
	APCLay.addLayout(APE3MPULay)
	APCGroup.setLayout(APCLay)	

	mainLay=QVBoxLayout()
	mainLay.addWidget(self.fullRadio)
	mainLay.addWidget(self.APCRadio)
	mainLay.addWidget(APCGroup)
	mainLay.addLayout(buttonLay)
	widget.setLayout(mainLay)
     
        scroll = QScrollArea()
        scroll.setWidget(widget)
        scroll.setAutoFillBackground(True)
        scroll.setWidgetResizable(True)

        vbox = QVBoxLayout()
        vbox.addWidget(scroll)  
        centralWidget.setLayout(vbox)

	self.connect(self.fullRadio,SIGNAL("clicked(bool)"),self.fullRadioSlot)
	self.connect(self.APCRadio,SIGNAL("clicked(bool)"),self.APCRadioSlot)
	self.connect(self.APE1Check,SIGNAL("clicked()"),self.APE1CheckSlot)
	self.connect(self.APE2Check,SIGNAL("clicked()"),self.APE2CheckSlot)
	self.connect(self.APE3Check,SIGNAL("clicked()"),self.APE3CheckSlot)
	self.connect(self.APE0SPUButton,SIGNAL("clicked()"),self.APE0SPUBrowse)
	self.connect(self.APE0MPUButton,SIGNAL("clicked()"),self.APE0MPUBrowse)
	self.connect(self.APE1SPUButton,SIGNAL("clicked()"),self.APE1SPUBrowse)
	self.connect(self.APE1MPUButton,SIGNAL("clicked()"),self.APE1MPUBrowse)
	self.connect(self.APE2SPUButton,SIGNAL("clicked()"),self.APE2SPUBrowse)
	self.connect(self.APE2MPUButton,SIGNAL("clicked()"),self.APE2MPUBrowse)
	self.connect(self.APE3SPUButton,SIGNAL("clicked()"),self.APE3SPUBrowse)
	self.connect(self.APE3MPUButton,SIGNAL("clicked()"),self.APE3MPUBrowse)
	self.connect(self.startButton,SIGNAL("clicked()"),self.startProcess)
	self.connect(self.stopButton,SIGNAL("clicked()"),self.stopProcess)
	
	#set default status
	self.APCRadio.setChecked(True)
	self.APCRadioSlot()
	self.APE1Check.setCheckState(Qt.Unchecked)
	self.APE2Check.setCheckState(Qt.Unchecked)
	self.APE3Check.setCheckState(Qt.Unchecked)
	self.APE1CheckSlot()
	self.APE2CheckSlot()
	self.APE3CheckSlot()

    def fullRadioSlot(self):
	self.traceFlagsButton.setEnabled(False)
	self.traceFileEdit.setEnabled(False)
	self.APE0SPUEdit.setEnabled(False)
	self.APE0SPUButton.setEnabled(False)
	self.APE0MPUEdit.setEnabled(False)
	self.APE0MPUButton.setEnabled(False)
	self.APE1Check.setEnabled(False)
	self.APE1SPUEdit.setEnabled(False)
	self.APE1SPUButton.setEnabled(False)
	self.APE1MPUEdit.setEnabled(False)
	self.APE1MPUButton.setEnabled(False)
	self.APE2Check.setEnabled(False)
	self.APE2SPUEdit.setEnabled(False)
	self.APE2SPUButton.setEnabled(False)
	self.APE2MPUEdit.setEnabled(False)
	self.APE2MPUButton.setEnabled(False)
	self.APE3Check.setEnabled(False)
	self.APE3SPUEdit.setEnabled(False)
	self.APE3SPUButton.setEnabled(False)
	self.APE3MPUEdit.setEnabled(False)
	self.APE3MPUButton.setEnabled(False)

    def APCRadioSlot(self):
	self.traceFlagsButton.setEnabled(True)
	self.traceFileEdit.setEnabled(True)
	self.APE0SPUEdit.setEnabled(True)
	self.APE0SPUButton.setEnabled(True)
	self.APE0MPUEdit.setEnabled(True)
	self.APE0MPUButton.setEnabled(True)
	self.APE1Check.setEnabled(True)
	self.APE1SPUEdit.setEnabled(True)
	self.APE1SPUButton.setEnabled(True)
	self.APE1MPUEdit.setEnabled(True)
	self.APE1MPUButton.setEnabled(True)
	self.APE2Check.setEnabled(True)
	self.APE2SPUEdit.setEnabled(True)
	self.APE2SPUButton.setEnabled(True)
	self.APE2MPUEdit.setEnabled(True)
	self.APE2MPUButton.setEnabled(True)
	self.APE3Check.setEnabled(True)
	self.APE3SPUEdit.setEnabled(True)
	self.APE3SPUButton.setEnabled(True)
	self.APE3MPUEdit.setEnabled(True)
	self.APE3MPUButton.setEnabled(True)

    def APE1CheckSlot(self):
	if self.APE1Check.checkState()==Qt.Checked:
	    self.APE1SPUEdit.setEnabled(True)
	    self.APE1SPUButton.setEnabled(True)
	    self.APE1MPUEdit.setEnabled(True)
	    self.APE1MPUButton.setEnabled(True)
	else:
	    self.APE1SPUEdit.setEnabled(False)
	    self.APE1SPUButton.setEnabled(False)
	    self.APE1MPUEdit.setEnabled(False)
	    self.APE1MPUButton.setEnabled(False)
	    self.APE2Check.setCheckState(Qt.Unchecked)
	    self.APE3Check.setCheckState(Qt.Unchecked)
	    self.APE2CheckSlot()
	    self.APE3CheckSlot()

    def APE2CheckSlot(self):
	if self.APE2Check.checkState()==Qt.Checked:
	    if self.APE1Check.checkState()==Qt.Checked:
	        self.APE2SPUEdit.setEnabled(True)
	        self.APE2SPUButton.setEnabled(True)
	        self.APE2MPUEdit.setEnabled(True)
	        self.APE2MPUButton.setEnabled(True)
	    else:
	        self.APE2Check.setCheckState(Qt.Unchecked)
	else:
	    self.APE2SPUEdit.setEnabled(False)
	    self.APE2SPUButton.setEnabled(False)
	    self.APE2MPUEdit.setEnabled(False)
	    self.APE2MPUButton.setEnabled(False)
	    self.APE3Check.setCheckState(Qt.Unchecked)
	    self.APE3CheckSlot()

    def APE3CheckSlot(self):
	if self.APE3Check.checkState()==Qt.Checked:
	    if self.APE1Check.checkState()==Qt.Checked and self.APE2Check.checkState()==Qt.Checked:
	        self.APE3SPUEdit.setEnabled(True)
	        self.APE3SPUButton.setEnabled(True)
	        self.APE3MPUEdit.setEnabled(True)
	        self.APE3MPUButton.setEnabled(True)
	    else:
	        self.APE3Check.setCheckState(Qt.Unchecked)
	else:
	    self.APE3SPUEdit.setEnabled(False)
	    self.APE3SPUButton.setEnabled(False)
	    self.APE3MPUEdit.setEnabled(False)
	    self.APE3MPUButton.setEnabled(False)	

    def APE0SPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE0SPUEdit.setText(path)

    def APE0MPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE0MPUEdit.setText(path)

    def APE1SPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE1SPUEdit.setText(path)

    def APE1MPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE1MPUEdit.setText(path)

    def APE2SPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE2SPUEdit.setText(path)

    def APE2MPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE2MPUEdit.setText(path)

    def APE3SPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE3SPUEdit.setText(path)

    def APE3MPUBrowse(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.APE3MPUEdit.setText(path)

    def startProcess(self):
	self.startButton.setEnabled(False)
	self.stopButton.setEnabled(True)
	string="--debug-flags=MapuGUI "+self.simulatorPath+"/apc/system/se.py -c"
	self.process=QProcess()
        self.connect(self.process,SIGNAL("readyReadStandardOutput()"),self.startReadOutput)
        self.connect(self.process,SIGNAL("readyReadStandardError()"),self.startReadErrOutput)
	self.connect(self.process,SIGNAL("finished(int,QProcess::ExitStatus)"),self.finishProcess)
	self.command=self.simulatorPath+"/apc/gem5.opt"   
	self.command=self.command+" "+"--trace-file="+self.traceFileEdit.text()+" "+string+" "+"\""+self.APE0SPUEdit.text()+","+self.APE0MPUEdit.text()
	self.num=1
	if self.APE1Check.checkState()==Qt.Checked:
	    self.command=self.command+";"+self.APE1SPUEdit.text()+","+self.APE1MPUEdit.text()
	    self.num=self.num+1
	    if self.APE2Check.checkState()==Qt.Checked:
		self.num=self.num+1
		self.command=self.command+";"+self.APE2SPUEdit.text()+","+self.APE2MPUEdit.text()
	        if self.APE3Check.checkState()==Qt.Checked:
		    self.num=self.num+1
		    self.command=self.command+";"+self.APE3SPUEdit.text()+","+self.APE3MPUEdit.text()
	self.command=self.command+"\""
	if self.num>1:
	    self.command=self.command+" "+"-n"+" "+QString.number(self.num,10)
        self.process.start(self.command)
        if False==self.process.waitForStarted():
	    self.simulatorShowSignal.emit(0,"this process can not be called.")

    def finishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.simulatorShowSignal.emit(0,"process exit normal")
	    #simulator exit normal,then emit signal to create data base
	    self.simulatorDoneSignal.emit(self.num) 
        else:
	    self.simulatorShowSignal.emit(0,"process exit crash")
	    QMessageBox.about(self,"Exit","    1    ")
	self.startButton.setEnabled(True)
	self.stopButton.setEnabled(False)

    def startReadOutput(self):
        ba=self.process.readAllStandardOutput()
	self.simulatorShowSignal.emit(0,ba.data())

    def startReadErrOutput(self):
        ba=self.process.readAllStandardError()
	self.simulatorShowSignal.emit(1,ba.data())

    def stopProcess(self):
	self.startButton.setEnabled(True)
	self.stopButton.setEnabled(False)
        self.process.write("quit")
        self.process.kill()

	

  
