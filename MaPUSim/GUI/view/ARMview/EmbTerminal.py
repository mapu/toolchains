#!/usr/bin/env python 
#-*- coding:utf-8 -*- 

import sys 
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 

class EmbTerminal(QWidget): 
    ARMSimulatorStatusSignal=pyqtSignal(bool)
    ARMSimulatorShowSignal=pyqtSignal(int,str)
    APCSimulatorSignal=pyqtSignal(str,str)
    def __init__(self): 
	super(EmbTerminal,self).__init__()

    	QWidget.__init__(self) 
	self.resize(1000,200)
    	self.process = QProcess(self) 
    	self.terminal = QWidget(self) 
	self.terminal.resize(1000,200)
    	layout = QVBoxLayout(self) 
    	layout.addWidget(self.terminal)
        self.connect(self.process,SIGNAL("readyReadStandardOutput()"),self.startReadOutput)
        self.connect(self.process,SIGNAL("readyReadStandardError()"),self.startReadErrOutput)
	self.connect(self.process,SIGNAL("finished(int,QProcess::ExitStatus)"),self.finishProcess)
	self.simulatorPath=""
	self.flag=0
	self.errorFile="main.out"

    def startProcess(self): 
	self.flag=1
	self.ARMCommand=self.simulatorPath+"/arm_qemu/bin/qemu-system-arm -M mapu -m 512 -pflash "+self.simulatorPath+"/sim_dmac.bin -serial stdio 2>"+self.errorFile
	#/home/litt/simulator/arm_qemu/bin/qemu-system-arm -M mapu -m 512 -pflash /home/litt/simulator/sim_dmac.bin -serial stdio
   	#self.process.start('gnome-terminal',['-into', str(self.terminal.winId()),'--geometry=GEOMETRY',"255x50",'-e', str(self.ARMCommand)]) 
	self.process.start('xterm',['-into', str(self.terminal.winId()),'-geometry',"100x200",'-e', str(self.ARMCommand)]) 
        if False==self.process.waitForStarted():
	    self.ARMSimulatorShowSignal.emit(0,"ARM process can not be called.")
	else:
	    self.ARMSimulatorStatusSignal.emit(True)
	self.GetAPCParameter()

    def finishProcess(self,exitCode,exitStatus):
        if exitStatus==QProcess.NormalExit:
	    self.ARMSimulatorShowSignal.emit(0,"process exit normal")
        else:
	    self.ARMSimulatorShowSignal.emit(0,"process exit crash")
	self.ARMSimulatorStatusSignal.emit(False)

    def startReadOutput(self):
        ba=self.process.readAllStandardOutput()
	print ba
	self.ARMSimulatorShowSignal.emit(0,ba.data())

    def startReadErrOutput(self):
        ba=self.process.readAllStandardError()

    def GetAPCParameter(self):
        f=open(self.errorFile,"r")
        lines=f.readlines()
        for line in lines:
	    str1=QString(line)
	    self.ARMSimulatorShowSignal.emit(1,str1)
	    if str1.indexOf("Share memory key")>=0:
		pos=str1.indexOf("is")
		key=str1.right(str1.length()-pos-3)
	    elif str1.indexOf("realview.apc")>=0:
		pos=str1.indexOf("port")
		apcport=str1.right(str1.length()-pos-5)
	if key!="" and apcport!="":
	    self.APCSimulatorSignal.emit(key,apcport)

    def stopProcess(self):
	if self.flag==1:
            self.process.close()


