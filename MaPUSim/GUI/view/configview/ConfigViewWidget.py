# -*- coding =utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
import time
import shutil
import os

class ConfigViewWidget(QMainWindow):
    #define signal
    APCSimulatorDoneSignal=pyqtSignal(int,str)
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

	#APC GROUP
	self.traceFlagsButton=QPushButton(self.tr("Trace flags..."))
	self.traceFlagsButton.setFixedSize(150,30)
	self.traceFileLabel=QLabel(self.tr("Trace file name:"))
	self.traceFileLabel.setFixedSize(110,30)
	self.traceFileEdit=QLineEdit(self.tr(""))
	self.traceFileEdit.setFixedSize(500,30)	
	blank10=QLabel()
	blank10.setFixedSize(920,30)
	traceLay=QHBoxLayout()
	traceLay.addWidget(self.traceFileLabel)
	traceLay.addWidget(self.traceFileEdit)
	traceLay.addWidget(blank10)

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
	self.APE0SPUEdit=QLineEdit(self.tr(""))
	self.APE0SPUEdit.setFixedSize(540,30)
	self.APE0SPUButton=QPushButton(self.tr("Browse"))
	self.APE0SPUButton.setFixedSize(100,30)
	blank0=QLabel()
	blank0.setFixedSize(800,30)
	APE0SPULay=QHBoxLayout()
	APE0SPULay.addWidget(self.APE0SPULabel)
	APE0SPULay.addWidget(self.APE0SPUEdit)
	APE0SPULay.addWidget(self.APE0SPUButton)
	APE0SPULay.addWidget(blank0)
	self.APE0MPULabel=QLabel(self.tr("     MPU"))
	self.APE0MPULabel.setFixedSize(60,30)
	self.APE0MPUEdit=QLineEdit(self.tr(""))
	self.APE0MPUEdit.setFixedSize(540,30)
	self.APE0MPUButton=QPushButton(self.tr("Browse"))
	self.APE0MPUButton.setFixedSize(100,30)
	blank1=QLabel()
	blank1.setFixedSize(800,30)
	APE0MPULay=QHBoxLayout()
	APE0MPULay.addWidget(self.APE0MPULabel)
	APE0MPULay.addWidget(self.APE0MPUEdit)
	APE0MPULay.addWidget(self.APE0MPUButton)
	APE0MPULay.addWidget(blank1)

	self.APE1Check=QCheckBox(self.tr("APE1"))
	self.APE1Check.setFixedHeight(30)
	self.APE1SPULabel=QLabel(self.tr("     SPU"))
	self.APE1SPULabel.setFixedSize(60,30)
	self.APE1SPUEdit=QLineEdit(self.tr(""))
	self.APE1SPUEdit.setFixedSize(540,30)
	self.APE1SPUButton=QPushButton(self.tr("Browse"))
	self.APE1SPUButton.setFixedSize(100,30)
	blank2=QLabel()
	blank2.setFixedSize(800,30)
	APE1SPULay=QHBoxLayout()
	APE1SPULay.addWidget(self.APE1SPULabel)
	APE1SPULay.addWidget(self.APE1SPUEdit)
	APE1SPULay.addWidget(self.APE1SPUButton)
	APE1SPULay.addWidget(blank2)
	self.APE1MPULabel=QLabel(self.tr("     MPU"))
	self.APE1MPULabel.setFixedSize(60,30)
	self.APE1MPUEdit=QLineEdit(self.tr(""))
	self.APE1MPUEdit.setFixedSize(540,30)
	self.APE1MPUButton=QPushButton(self.tr("Browse"))
	self.APE1MPUButton.setFixedSize(100,30)
	blank3=QLabel()
	blank3.setFixedSize(800,30)
	APE1MPULay=QHBoxLayout()
	APE1MPULay.addWidget(self.APE1MPULabel)
	APE1MPULay.addWidget(self.APE1MPUEdit)
	APE1MPULay.addWidget(self.APE1MPUButton)
	APE1MPULay.addWidget(blank3)

	self.APE2Check=QCheckBox(self.tr("APE2"))
	self.APE2Check.setFixedHeight(30)
	self.APE2SPULabel=QLabel(self.tr("     SPU"))
	self.APE2SPULabel.setFixedSize(60,30)
	self.APE2SPUEdit=QLineEdit(self.tr(""))
	self.APE2SPUEdit.setFixedSize(540,30)
	self.APE2SPUButton=QPushButton(self.tr("Browse"))
	self.APE2SPUButton.setFixedSize(100,30)
	blank4=QLabel()
	blank4.setFixedSize(800,30)
	APE2SPULay=QHBoxLayout()
	APE2SPULay.addWidget(self.APE2SPULabel)
	APE2SPULay.addWidget(self.APE2SPUEdit)
	APE2SPULay.addWidget(self.APE2SPUButton)
	APE2SPULay.addWidget(blank4)
	self.APE2MPULabel=QLabel(self.tr("     MPU"))
	self.APE2MPULabel.setFixedSize(60,30)
	self.APE2MPUEdit=QLineEdit(self.tr(""))
	self.APE2MPUEdit.setFixedSize(540,30)
	self.APE2MPUButton=QPushButton(self.tr("Browse"))
	self.APE2MPUButton.setFixedSize(100,30)
	blank5=QLabel()
	blank5.setFixedSize(800,30)
	APE2MPULay=QHBoxLayout()
	APE2MPULay.addWidget(self.APE2MPULabel)
	APE2MPULay.addWidget(self.APE2MPUEdit)
	APE2MPULay.addWidget(self.APE2MPUButton)
	APE2MPULay.addWidget(blank5)

	self.APE3Check=QCheckBox(self.tr("APE3"))
	self.APE3Check.setFixedHeight(30)
	self.APE3SPULabel=QLabel(self.tr("     SPU"))
	self.APE3SPULabel.setFixedSize(60,30)
	self.APE3SPUEdit=QLineEdit(self.tr(""))
	self.APE3SPUEdit.setFixedSize(540,30)
	self.APE3SPUButton=QPushButton(self.tr("Browse"))
	self.APE3SPUButton.setFixedSize(100,30)
	blank6=QLabel()
	blank6.setFixedSize(800,30)
	APE3SPULay=QHBoxLayout()
	APE3SPULay.addWidget(self.APE3SPULabel)
	APE3SPULay.addWidget(self.APE3SPUEdit)
	APE3SPULay.addWidget(self.APE3SPUButton)
	APE3SPULay.addWidget(blank6)
	self.APE3MPULabel=QLabel(self.tr("     MPU"))
	self.APE3MPULabel.setFixedSize(60,30)
	self.APE3MPUEdit=QLineEdit(self.tr(""))
	self.APE3MPUEdit.setFixedSize(540,30)
	self.APE3MPUButton=QPushButton(self.tr("Browse"))
	self.APE3MPUButton.setFixedSize(100,30)
	blank7=QLabel()
	blank7.setFixedSize(800,30)
	APE3MPULay=QHBoxLayout()
	APE3MPULay.addWidget(self.APE3MPULabel)
	APE3MPULay.addWidget(self.APE3MPUEdit)
	APE3MPULay.addWidget(self.APE3MPUButton)
	APE3MPULay.addWidget(blank7)
	APE3MPULay.addWidget(self.startButton)
	APE3MPULay.addWidget(self.stopButton)

	#APC Layout
	self.APCGroup=QGroupBox("APC standalone system")
	self.APCGroup.setAlignment(Qt.AlignLeft)
	self.APCGroup.setCheckable(True)
	self.APCGroup.setChecked(False)
	APCLay=QVBoxLayout()	
	APCLay.addLayout(traceLay)
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
	self.APCGroup.setLayout(APCLay)	

	#full group
	fullImageLabel=QLabel("Image file name:")
	fullImageLabel.setFixedSize(110,30)
	self.fullBrowserButton=QPushButton("Browser")
	self.fullBrowserButton.setFixedSize(100,30)
	self.fullEdit=QLineEdit()
	self.fullEdit.setFixedSize(500,30)
	blank8=QLabel()
	blank8.setFixedSize(805,30)
	fullTraceLabel=QLabel("Trace file name:")
	fullTraceLabel.setFixedSize(110,30)
	self.fullTracefile=QLineEdit()
	self.fullTracefile.setFixedSize(500,30)
	blank9=QLabel()
	blank9.setFixedSize(920,30)	
	#full Layout
	self.fullGroup=QGroupBox("Full system")
	self.fullGroup.setAlignment(Qt.AlignLeft)
	self.fullGroup.setCheckable(True)
	self.fullGroup.setChecked(True)
	fullLay1=QHBoxLayout()
	fullLay1.addWidget(fullImageLabel)
	fullLay1.addWidget(self.fullEdit)
	fullLay1.addWidget(self.fullBrowserButton)
	fullLay1.addWidget(blank8)
	fullLay2=QHBoxLayout()
	fullLay2.addWidget(fullTraceLabel)
	fullLay2.addWidget(self.fullTracefile)
	fullLay2.addWidget(blank9)
	fullLay=QVBoxLayout()
	fullLay.addLayout(fullLay1)
	fullLay.addLayout(fullLay2)
	self.fullGroup.setLayout(fullLay)
	
	#main layout
	mainLay=QVBoxLayout()
	mainLay.addWidget(self.fullGroup)
	mainLay.addWidget(self.APCGroup)
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

	self.connect(self.fullGroup,SIGNAL("clicked()"),self.fullCheckedSlot)
	self.connect(self.APCGroup,SIGNAL("clicked()"),self.APCCheckedSlot)
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
	
	self.ARMAPCProcess=QProcess()
	self.ARMProcess=QProcess()
	self.key=""
	self.apcport=""
	self.uart0port=""
	self.exitFlag=0
	self.startButton.setEnabled(True)
	self.stopButton.setEnabled(False)

	#set default status
	self.fullCheckedSlot()

    def fullCheckedSlot(self):
	if self.fullGroup.isChecked()==True:
	    self.flag=1
	    self.fullBrowserButton.setEnabled(True)
	    self.fullEdit.setEnabled(True)
	    self.fullTracefile.setEnabled(True)
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
	    self.APCGroup.setChecked(False)  
	else:
	    self.APCGroup.setChecked(True)    

    def APCCheckedSlot(self):
	if self.APCGroup.isChecked()==True:
	    self.flag=0
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
	    self.fullTracefile.setEnabled(False)
	    self.fullGroup.setChecked(False)  
	else:
	    self.fullGroup.setChecked(True)  

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
	if self.simulatorPath=="":
 	    QMessageBox.warning(self,"Warning","Simulator path is not set!")
	    return
	if self.fullGroup.isChecked()==True:
	    #check path is legal
	    if os.path.isfile(str(self.fullEdit.text()))==False:
	        QMessageBox.warning(self,"Warning","Image file is not exist!")
		return
	    if self.fullTracefile.text()=="":
	        QMessageBox.warning(self,"Warning","Trace file is not input!")
		return
	    self.startButton.setEnabled(False)
	    self.stopButton.setEnabled(True)
	    self.processFlag=0
	    #copy image.bin file
	    path=os.path.realpath(sys.path[0])
	    if os.path.isfile(path):
	        path=os.path.dirname(path)
	    self.mainPath=os.path.abspath(path)
	    srcPath=str(self.fullEdit.text())
	    destPath=self.mainPath+"/images"
	    if os.path.exists(destPath)<=0:
	    	os.mkdir(destPath)
	    if os.path.exists(srcPath):
		shutil.copy(srcPath,destPath)
	    self.ARMCommand=self.simulatorPath+"/arm/gem5.opt"+" "+self.simulatorPath+"/arm/system/fs.py"+" --bare-metal --machine-type=MaPU_Board"
            self.connect(self.ARMProcess,SIGNAL("readyReadStandardOutput()"),self.ARMStartReadOutput)
            self.connect(self.ARMProcess,SIGNAL("readyReadStandardError()"),self.ARMStartReadErrOutput,Qt.DirectConnection)
	    self.connect(self.ARMProcess,SIGNAL("finished(int,QProcess::ExitStatus)"),self.ARMFinishProcess)
            self.ARMProcess.start(self.ARMCommand)
            if False==self.ARMProcess.waitForStarted():
	        self.ARMSimulatorShowSignal.emit(0,"ARM process can not be called.")
	else:
	    if self.traceFileEdit.text()=="":
	        QMessageBox.warning(self,"Warning","Trace file is not input!")
		return
	    string="--debug-flags=MapuGUI,MapuMem "+self.simulatorPath+"/apc/system/se.py -c" #
	    self.APCCommand=self.simulatorPath+"/apc/gem5.opt"
	    #check path is legal
   	    if os.path.isfile(str(self.APE0SPUEdit.text()))==False:
	        QMessageBox.warning(self,"Warning","APE0 SPU file is not exist!")
		return
   	    if os.path.isfile(str(self.APE0MPUEdit.text()))==False:
	        QMessageBox.warning(self,"Warning","APE0 MPU file is not exist!")
		return
	    self.APCCommand=self.APCCommand+" "+"--trace-file="+self.traceFileEdit.text()+" "+string+" "+"\""+self.APE0SPUEdit.text()+","+self.APE0MPUEdit.text()
	    self.num=1
	    if self.APE1Check.checkState()==Qt.Checked:
	        #check path is legal
   	        if os.path.isfile(str(self.APE1SPUEdit.text()))==False:
	            QMessageBox.warning(self,"Warning","APE1 SPU file is not exist!")
		    return
   	        if os.path.isfile(str(self.APE1MPUEdit.text()))==False:
	            QMessageBox.warning(self,"Warning","APE1 MPU file is not exist!")
		    return
	        self.APCCommand=self.APCCommand+";"+self.APE1SPUEdit.text()+","+self.APE1MPUEdit.text()
	        self.num=self.num+1
	        if self.APE2Check.checkState()==Qt.Checked:
	            #check path is legal
   	            if os.path.isfile(str(self.APE2SPUEdit.text()))==False:
	                QMessageBox.warning(self,"Warning","APE2 SPU file is not exist!")
		        return
   	            if os.path.isfile(str(self.APE2MPUEdit.text()))==False:
	                QMessageBox.warning(self,"Warning","APE2 MPU file is not exist!")
		        return
		    self.num=self.num+1
		    self.APCCommand=self.APCCommand+";"+self.APE2SPUEdit.text()+","+self.APE2MPUEdit.text()
	            if self.APE3Check.checkState()==Qt.Checked:
	                #check path is legal
   	                if os.path.isfile(str(self.APE3SPUEdit.text()))==False:
	                    QMessageBox.warning(self,"Warning","APE3 SPU file is not exist!")
		            return
   	                if os.path.isfile(str(self.APE3MPUEdit.text()))==False:
	                    QMessageBox.warning(self,"Warning","APE3 MPU file is not exist!")
		            return
		        self.num=self.num+1
		        self.APCCommand=self.APCCommand+";"+self.APE3SPUEdit.text()+","+self.APE3MPUEdit.text()
	    self.startButton.setEnabled(False)
	    self.stopButton.setEnabled(True)
	    self.APCCommand=self.APCCommand+"\""
	    if self.num>1:
	        self.APCCommand=self.APCCommand+" "+"-n"+" "+QString.number(self.num,10)
	    self.APCProcess=QProcess()
            self.connect(self.APCProcess,SIGNAL("readyReadStandardOutput()"),self.APCStartReadOutput)
            self.connect(self.APCProcess,SIGNAL("readyReadStandardError()"),self.APCStartReadErrOutput)
	    self.connect(self.APCProcess,SIGNAL("finished(int,QProcess::ExitStatus)"),self.APCFinishProcess)
            self.APCProcess.start(self.APCCommand)
            if False==self.APCProcess.waitForStarted():
	        self.APCSimulatorShowSignal.emit(0,"APC process can not be called.")

    def ARMAPCSimulator(self,key,port):
	string="--debug-flags=MapuGUI "+"--trace-file="+self.fullTracefile.text()+" "+self.simulatorPath+"/apc/system/ms.py -c " #
	self.ARMAPCCommand=self.simulatorPath+"/apc/gem5.opt"  
	self.ARMAPCCommand=self.ARMAPCCommand+" "+string+port+" -k "+key+" -n 4"
        self.connect(self.ARMAPCProcess,SIGNAL("readyReadStandardOutput()"),self.ARMAPCStartReadOutput)
        self.connect(self.ARMAPCProcess,SIGNAL("readyReadStandardError()"),self.ARMAPCStartReadErrOutput)
	self.connect(self.ARMAPCProcess,SIGNAL("finished(int,QProcess::ExitStatus)"),self.ARMAPCFinishProcess)
        self.ARMAPCProcess.start(self.ARMAPCCommand)
        if False==self.ARMAPCProcess.waitForStarted():
	    self.APCSimulatorShowSignal.emit(0,"ARM APC process can not be called.")

    def APCFinishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.APCSimulatorShowSignal.emit(0,"process exit normal")
	    #simulator exit normal,then emit signal to create data base
	    self.APCSimulatorDoneSignal.emit(self.num,self.traceFileEdit.text()) 
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

    def ARMFinishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.ARMSimulatorShowSignal.emit(0,"process exit normal")
        else:
	    self.ARMSimulatorShowSignal.emit(0,"process exit crash")

    def ARMStartReadOutput(self):
        ba=self.ARMProcess.readAllStandardOutput()
	self.ARMSimulatorShowSignal.emit(0,ba.data())

    def ARMStartReadErrOutput(self):
        ba=self.ARMProcess.readAllStandardError()
	string=QString(ba.data())
	num=string.count("\n")
	for i in range(0,num):
	    pos=string.indexOf("\n")
	    str1=string.left(pos)
	    string=string.right(string.size()-pos-1)
	    if str1.indexOf("Share memory key")>=0:
		pos=str1.indexOf("is")
		self.key=str1.right(str1.length()-pos-3)
	    elif str1.indexOf("vncserver")>=0:
		pos=str1.indexOf("port")
		port=str1.right(str1.length()-pos-5)
	    elif str1.indexOf("terminal")>=0:
		pos=str1.indexOf("port")
		self.uart0port=str1.right(str1.length()-pos-5)
	    elif str1.indexOf("realview.apc")>=0:
		pos=str1.indexOf("port")
		self.apcport=str1.right(str1.length()-pos-5)
	    elif str1.indexOf("remote gdb")>=0:
		pos=str1.indexOf("port")
		port=str1.right(str1.length()-pos-5)
	    str1=str1+"\n"
	    self.ARMSimulatorShowSignal.emit(1,str1)

	if self.key!="" and self.apcport!="" and self.uart0port!="":
	    if self.processFlag==0:
	        self.ARMAPCSimulator(self.key,self.apcport)
	        self.ARMUart0StartProcess.emit(self.uart0port)
		self.processFlag=1

    def ARMAPCFinishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.APCSimulatorShowSignal.emit(0,"process exit normal")
        else:
	    self.APCSimulatorShowSignal.emit(0,"process exit crash")
	#simulator exit normal,then emit signal to create data base
	if self.exitFlag==0:
	    self.APCSimulatorDoneSignal.emit(4,self.fullTracefile.text())
 
    def ARMAPCStartReadOutput(self):
        ba=self.ARMAPCProcess.readAllStandardOutput()
	self.APCSimulatorShowSignal.emit(0,ba.data())

    def ARMAPCStartReadErrOutput(self):
        ba=self.ARMAPCProcess.readAllStandardError()
	self.APCSimulatorShowSignal.emit(1,ba.data())
  
    def stopProcess(self):
	self.startButton.setEnabled(True)
	self.stopButton.setEnabled(False)
	if self.flag==1:
	    self.ARMProcess.close()	
 	    self.ARMAPCProcess.close()	

    def stopProcessExit(self,exitFlag):
	self.exitFlag=exitFlag
	if self.flag==1:
	    self.ARMProcess.close()	
 	    self.ARMAPCProcess.close()	
	

