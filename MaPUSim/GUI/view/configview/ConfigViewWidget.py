# -*- coding =utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
import time

class ConfigViewWidget(QMainWindow):
    #define signal
    APCSimulatorDoneSignal=pyqtSignal(int)
    APCSimulatorShowSignal=pyqtSignal(int,str)
    ARMSimulatorShowSignal=pyqtSignal(int,str)
    ARMUart0StartProcess=pyqtSignal(str)
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

	#APC GROUP
	self.traceFlagsButton=QPushButton(self.tr("Trace flags..."))
	self.traceFlagsButton.setFixedSize(150,30)
	self.traceFileLabel=QLabel(self.tr("  Trace file"))
	self.traceFileLabel.setFixedHeight(30)
	self.traceFileLabel.setFixedWidth(900)
	self.traceFileEdit=QLineEdit(self.tr("aaa.out"))
	self.traceFileEdit.setFixedSize(500,30)	

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
	self.APE0SPUEdit.setFixedSize(500,30)
	self.APE0SPUButton=QPushButton(self.tr("Browse"))
	self.APE0SPUButton.setFixedSize(100,30)
	blank0=QLabel()
	blank0.setFixedSize(600,30)
	APE0SPULay=QHBoxLayout()
	APE0SPULay.addWidget(self.APE0SPULabel)
	APE0SPULay.addWidget(self.APE0SPUEdit)
	APE0SPULay.addWidget(self.APE0SPUButton)
	APE0SPULay.addWidget(blank0)
	self.APE0MPULabel=QLabel(self.tr("     MPU"))
	self.APE0MPULabel.setFixedSize(60,30)
	self.APE0MPUEdit=QLineEdit(self.tr("/home/litt/tlb.m.out"))
	self.APE0MPUEdit.setFixedSize(500,30)
	self.APE0MPUButton=QPushButton(self.tr("Browse"))
	self.APE0MPUButton.setFixedSize(100,30)
	blank1=QLabel()
	blank1.setFixedSize(600,30)
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
	self.APE1SPUEdit.setFixedSize(500,30)
	self.APE1SPUButton=QPushButton(self.tr("Browse"))
	self.APE1SPUButton.setFixedSize(100,30)
	blank2=QLabel()
	blank2.setFixedSize(600,30)
	APE1SPULay=QHBoxLayout()
	APE1SPULay.addWidget(self.APE1SPULabel)
	APE1SPULay.addWidget(self.APE1SPUEdit)
	APE1SPULay.addWidget(self.APE1SPUButton)
	APE1SPULay.addWidget(blank2)
	self.APE1MPULabel=QLabel(self.tr("     MPU"))
	self.APE1MPULabel.setFixedSize(60,30)
	self.APE1MPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE1MPUEdit.setFixedSize(500,30)
	self.APE1MPUButton=QPushButton(self.tr("Browse"))
	self.APE1MPUButton.setFixedSize(100,30)
	blank3=QLabel()
	blank3.setFixedSize(600,30)
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
	self.APE2SPUEdit.setFixedSize(500,30)
	self.APE2SPUButton=QPushButton(self.tr("Browse"))
	self.APE2SPUButton.setFixedSize(100,30)
	blank4=QLabel()
	blank4.setFixedSize(600,30)
	APE2SPULay=QHBoxLayout()
	APE2SPULay.addWidget(self.APE2SPULabel)
	APE2SPULay.addWidget(self.APE2SPUEdit)
	APE2SPULay.addWidget(self.APE2SPUButton)
	APE2SPULay.addWidget(blank4)
	self.APE2MPULabel=QLabel(self.tr("     MPU"))
	self.APE2MPULabel.setFixedSize(60,30)
	self.APE2MPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE2MPUEdit.setFixedSize(500,30)
	self.APE2MPUButton=QPushButton(self.tr("Browse"))
	self.APE2MPUButton.setFixedSize(100,30)
	blank5=QLabel()
	blank5.setFixedSize(600,30)
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
	self.APE3SPUEdit.setFixedSize(500,30)
	self.APE3SPUButton=QPushButton(self.tr("Browse"))
	self.APE3SPUButton.setFixedSize(100,30)
	blank6=QLabel()
	blank6.setFixedSize(600,30)
	APE3SPULay=QHBoxLayout()
	APE3SPULay.addWidget(self.APE3SPULabel)
	APE3SPULay.addWidget(self.APE3SPUEdit)
	APE3SPULay.addWidget(self.APE3SPUButton)
	APE3SPULay.addWidget(blank6)
	self.APE3MPULabel=QLabel(self.tr("     MPU"))
	self.APE3MPULabel.setFixedSize(60,30)
	self.APE3MPUEdit=QLineEdit(self.tr("Trace file name"))
	self.APE3MPUEdit.setFixedSize(500,30)
	self.APE3MPUButton=QPushButton(self.tr("Browse"))
	self.APE3MPUButton.setFixedSize(100,30)
	blank7=QLabel()
	blank7.setFixedSize(600,30)
	APE3MPULay=QHBoxLayout()
	APE3MPULay.addWidget(self.APE3MPULabel)
	APE3MPULay.addWidget(self.APE3MPUEdit)
	APE3MPULay.addWidget(self.APE3MPUButton)
	APE3MPULay.addWidget(blank7)
	APE3MPULay.addWidget(self.startButton)
	APE3MPULay.addWidget(self.stopButton)

	#APC Layout
	APCGroup=QGroupBox()
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

	#full group
	self.fullBrowserButton=QPushButton("Browser")
	self.fullBrowserButton.setFixedSize(100,30)
	self.fullEdit=QLineEdit()
	self.fullEdit.setFixedSize(500,30)
	blank8=QLabel()
	blank8.setFixedSize(700,30)
	#full Layout
	fullGroup=QGroupBox()
	fullLay=QHBoxLayout()
	fullLay.addWidget(self.fullEdit)
	fullLay.addWidget(self.fullBrowserButton)
	fullLay.addWidget(blank8)
	fullGroup.setLayout(fullLay)
	
	#main layout
	mainLay=QVBoxLayout()
	mainLay.addWidget(self.fullRadio)
	mainLay.addWidget(fullGroup)
	mainLay.addWidget(self.APCRadio)
	mainLay.addWidget(APCGroup)
	mainLay.addLayout(buttonLay)
	widget.setLayout(mainLay)
     
	#scroll
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
	self.connect(self.fullBrowserButton,SIGNAL("clicked()"),self.fullBrowser)
	self.connect(self.startButton,SIGNAL("clicked()"),self.startProcess)
	self.connect(self.stopButton,SIGNAL("clicked()"),self.stopProcess)
	
	#set default status
	if 0:
	    self.APCRadio.setChecked(True)
	    self.APCRadioSlot()
	    self.APE1Check.setCheckState(Qt.Unchecked)
	    self.APE2Check.setCheckState(Qt.Unchecked)
	    self.APE3Check.setCheckState(Qt.Unchecked)
	    self.APE1CheckSlot()
	    self.APE2CheckSlot()
	    self.APE3CheckSlot()
	else:
	    self.fullRadio.setChecked(True)
	    self.fullRadioSlot()

    def fullRadioSlot(self):
	self.fullBrowserButton.setEnabled(True)
	self.fullEdit.setEnabled(True)
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
	if self.APE1Check.isChecked()==True:
	    self.APE1SPUEdit.setEnabled(True)
	    self.APE1SPUButton.setEnabled(True)
	    self.APE1MPUEdit.setEnabled(True)
	    self.APE1MPUButton.setEnabled(True)
	self.APE2Check.setEnabled(True)
	if self.APE2Check.isChecked()==True:
	    self.APE2SPUEdit.setEnabled(True)
	    self.APE2SPUButton.setEnabled(True)
	    self.APE2MPUEdit.setEnabled(True)
	    self.APE2MPUButton.setEnabled(True)
	self.APE3Check.setEnabled(True)
	if self.APE3Check.isChecked()==True:
	    self.APE3SPUEdit.setEnabled(True)
	    self.APE3SPUButton.setEnabled(True)
	    self.APE3MPUEdit.setEnabled(True)
	    self.APE3MPUButton.setEnabled(True)
	self.fullBrowserButton.setEnabled(False)
	self.fullEdit.setEnabled(False)

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

    def fullBrowser(self):
	path=QFileDialog.getOpenFileName(self,self.tr("select file"),"/")
	self.fullEdit.setText(path)	

    def startProcess(self):
	self.startButton.setEnabled(False)
	self.stopButton.setEnabled(True)
	if self.fullRadio.isChecked()==True:
	    self.ARMCommand=self.simulatorPath+"/arm/gem5.opt"+" "+self.simulatorPath+"/arm/system/fs.py"+" --bare-metal --machine-type=MaPU_Board"
	    self.ARMProcess=QProcess()
            self.connect(self.ARMProcess,SIGNAL("readyReadStandardOutput()"),self.ARMStartReadOutput)
            self.connect(self.ARMProcess,SIGNAL("readyReadStandardError()"),self.ARMStartReadErrOutput,Qt.DirectConnection)
	    self.connect(self.ARMProcess,SIGNAL("finished(int,QProcess::ExitStatus)"),self.ARMFinishProcess)
            self.ARMProcess.start(self.ARMCommand)
            if False==self.ARMProcess.waitForStarted():
	        self.ARMSimulatorShowSignal.emit(0,"this process can not be called.")
	else:
	    string="--debug-flags=MapuGUI,MapuMem "+self.simulatorPath+"/apc/system/se.py -c" #
	    self.APCCommand=self.simulatorPath+"/apc/gem5.opt"   
	    self.APCCommand=self.APCCommand+" "+"--trace-file="+self.traceFileEdit.text()+" "+string+" "+"\""+self.APE0SPUEdit.text()+","+self.APE0MPUEdit.text()
	    self.num=1
	    if self.APE1Check.checkState()==Qt.Checked:
	        self.APCCommand=self.APCCommand+";"+self.APE1SPUEdit.text()+","+self.APE1MPUEdit.text()
	        self.num=self.num+1
	        if self.APE2Check.checkState()==Qt.Checked:
		    self.num=self.num+1
		    self.APCCommand=self.APCCommand+";"+self.APE2SPUEdit.text()+","+self.APE2MPUEdit.text()
	            if self.APE3Check.checkState()==Qt.Checked:
		        self.num=self.num+1
		        self.APCCommand=self.APCCommand+";"+self.APE3SPUEdit.text()+","+self.APE3MPUEdit.text()
	    self.APCCommand=self.APCCommand+"\""
	    if self.num>1:
	        self.APCCommand=self.APCCommand+" "+"-n"+" "+QString.number(self.num,10)
	    self.APCProcess=QProcess()
            self.connect(self.APCProcess,SIGNAL("readyReadStandardOutput()"),self.APCStartReadOutput)
            self.connect(self.APCProcess,SIGNAL("readyReadStandardError()"),self.APCStartReadErrOutput)
	    self.connect(self.APCProcess,SIGNAL("finished(int,QProcess::ExitStatus)"),self.APCFinishProcess)
            self.APCProcess.start(self.APCCommand)
            if False==self.APCProcess.waitForStarted():
	        self.APCSimulatorShowSignal.emit(0,"this process can not be called.")

    def ARMAPCSimulator(self,key,port):
	string="--debug-flags=MapuGUI "+"--trace-file="+self.traceFileEdit.text()+" "+self.simulatorPath+"/apc/system/ms.py -c " #
	self.ARMAPCCommand=self.simulatorPath+"/apc/gem5.opt"  
	self.ARMAPCCommand=self.ARMAPCCommand+" "+string+port+" -k "+key
	self.ARMAPCProcess=QProcess()
        self.connect(self.ARMAPCProcess,SIGNAL("readyReadStandardOutput()"),self.ARMAPCStartReadOutput)
        self.connect(self.ARMAPCProcess,SIGNAL("readyReadStandardError()"),self.ARMAPCStartReadErrOutput)
	self.connect(self.ARMAPCProcess,SIGNAL("finished(int,QProcess::ExitStatus)"),self.ARMAPCFinishProcess)
        self.ARMAPCProcess.start(self.ARMAPCCommand)
        if False==self.ARMAPCProcess.waitForStarted():
	    self.ARMSimulatorShowSignal.emit(0,"this process can not be called.")

    def APCFinishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.APCSimulatorShowSignal.emit(0,"process exit normal")
	    #simulator exit normal,then emit signal to create data base
	    self.APCSimulatorDoneSignal.emit(self.num) 
        else:
	    self.APCSimulatorShowSignal.emit(0,"process exit crash")
	    QMessageBox.about(self,"APC Exit","    1    ")
	self.startButton.setEnabled(True)
	self.stopButton.setEnabled(False)

    def APCStartReadOutput(self):
        ba=self.APCProcess.readAllStandardOutput()
	self.APCSimulatorShowSignal.emit(0,ba.data())

    def APCStartReadErrOutput(self):
        ba=self.APCProcess.readAllStandardError()
	self.APCSimulatorShowSignal.emit(1,ba.data())

    def stopProcess(self):
	self.startButton.setEnabled(True)
	self.stopButton.setEnabled(False)
        self.APCProcess.write("quit")
        self.APCProcess.kill()
        self.ARMProcess.write("quit")
        self.ARMProcess.kill()	
        self.ARMAPCProcess.write("quit")
        self.ARMAPCProcess.kill()	

    def ARMFinishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.ARMSimulatorShowSignal.emit(0,"process exit normal")
        else:
	    self.ARMSimulatorShowSignal.emit(0,"process exit crash")
	    QMessageBox.about(self,"ARM Exit","    1    ")

    def ARMStartReadOutput(self):
        ba=self.ARMProcess.readAllStandardOutput()
	self.ARMSimulatorShowSignal.emit(0,ba.data())

    def ARMStartReadErrOutput(self):
        ba=self.ARMProcess.readAllStandardError()
	string=QString(ba.data())
	if string.indexOf("Share memory key")>=0:
	    num=string.count("\n")
	    key=""
	    apcport=""
	    for i in range(0,num):
	        pos=string.indexOf("\n")
	        str1=string.left(pos)
	        string=string.right(string.size()-pos-1)
	        if str1.indexOf("Share memory key")>=0:
		    pos=str1.indexOf("is")
		    key=str1.right(str1.length()-pos-3)
	        elif str1.indexOf("vncserver")>=0:
		    pos=str1.indexOf("port")
		    port=str1.right(str1.length()-pos-5)
	        elif str1.indexOf("terminal")>=0:
		    pos=str1.indexOf("port")
		    uart0port=str1.right(str1.length()-pos-5)
	        elif str1.indexOf("realview.apc")>=0:
		    pos=str1.indexOf("port")
		    apcport=str1.right(str1.length()-pos-5)
	        elif str1.indexOf("remote gdb")>=0:
		    pos=str1.indexOf("port")
		    port=str1.right(str1.length()-pos-5)
	        str1=str1+"\n"
	        self.ARMSimulatorShowSignal.emit(1,str1)
	    self.ARMAPCSimulator(key,apcport)
	    self.ARMUart0StartProcess.emit(uart0port)
	else:
	    self.ARMSimulatorShowSignal.emit(1,string)

    def ARMAPCFinishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.ARMSimulatorShowSignal.emit(0,"process exit normal")
        else:
	    self.ARMSimulatorShowSignal.emit(0,"process exit crash")
	    QMessageBox.about(self,"ARM APC Exit","    1    ")
	self.startButton.setEnabled(True)
	self.stopButton.setEnabled(False)

    def ARMAPCStartReadOutput(self):
        ba=self.ARMAPCProcess.readAllStandardOutput()
	self.ARMSimulatorShowSignal.emit(0,ba.data())

    def ARMAPCStartReadErrOutput(self):
        ba=self.ARMAPCProcess.readAllStandardError()
	self.ARMSimulatorShowSignal.emit(1,ba.data())

    def m5termSimulator(self,uart0port):
	self.m5termCommand=self.simulatorPath+"/arm/utils/m5term localhost "+uart0port 
	print self.m5termCommand
	self.m5termProcess=QProcess()
        self.connect(self.m5termProcess,SIGNAL("readyReadStandardOutput()"),self.m5termStartReadOutput)
        self.connect(self.m5termProcess,SIGNAL("readyReadStandardError()"),self.m5termStartReadErrOutput)
	self.connect(self.m5termProcess,SIGNAL("finished(int,QProcess::ExitStatus)"),self.m5termFinishProcess)
        self.m5termProcess.start(self.m5termCommand)
        if False==self.m5termProcess.waitForStarted():
	    print "this process can not be called."

    def m5termFinishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    print "process exit normal"
        else:
	    print "process exit crash"
	    QMessageBox.about(self,"ARM APC Exit","    1    ")

    def m5termStartReadOutput(self):
        ba=self.m5termProcess.readAllStandardOutput()
	print "output",ba.data()

    def m5termStartReadErrOutput(self):
        ba=self.m5termProcess.readAllStandardError()
	print "error",ba.data()
  
