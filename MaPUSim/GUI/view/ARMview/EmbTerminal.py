#!/usr/bin/env python 
#-*- coding:utf-8 -*- 
import re
import os
import sys
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 
from QTermWidget import*

class EmbTerminal(QWidget): 
    ARMSimulatorStatusSignal=pyqtSignal(bool)
    ARMSimulatorShowSignal=pyqtSignal(int,str)
    APCSimulatorSignal=pyqtSignal(str,str)
    def __init__(self): 
	super(EmbTerminal,self).__init__()
    	QWidget.__init__(self) 
	
	self.simulatorPath=""
	self.flag=0
	self.errorFile="info.out"
	self.parameter=0
	self.m5termPath="/arm/utils/m5term"
	self.lay=QVBoxLayout()
	self.setLayout(self.lay)
	self.termWidget=QTermWidget(0)
	self.termWidget.setScrollBarPosition(2)
	font=QFont("Monospace")
	self.termWidget.setTerminalFont(font)
	self.lay.addWidget(self.termWidget)

    def startProcess(self,index,path):
	self.ARMSimulatorStatusSignal.emit(True) 
	self.lay.removeWidget(self.termWidget)
	self.termWidget.close()
	del self.termWidget
	self.termWidget=0
	self.termWidget=QTermWidget(0)
	self.termWidget.setScrollBarPosition(2)
	font=QFont("Monospace")
	self.termWidget.setTerminalFont(font)
	self.lay.addWidget(self.termWidget)
	if index==0:
	    self.flag=1
	    #ARMCommand=self.simulatorPath+self.m5termPath+" localhost "+path+"\n"
	    self.termWidget.setShellProgram(self.simulatorPath+self.m5termPath)
	    args=["localhost",path]
	    self.termWidget.setArgs(args)
	    self.termWidget.startShellProgram()	

	elif index==1:
	    self.flag=2

	    f=open(self.errorFile,"w")
	    f.close()
		
	    #ARMCommand=self.simulatorPath+"/arm/bin/qemu-system-arm -M mapu -m 512 -pflash "+path+" -serial stdio 2>"+self.errorFile+"\n"  # -gdb tcp::1234 -S
	    #self.termWidget.sendText(ARMCommand)	

	    #/home/litt/simulator/arm_qemu/bin/qemu-system-arm -M mapu -m 512 -pflash /home/litt/simulator/sim_dmac.bin -serial stdio
	    self.termWidget.setShellProgram(self.simulatorPath+"/arm/bin/qemu-system-arm")
	    args=["-M","mapu","-m","512","-pflash",path,"-serial","stdio",""]
	    self.termWidget.setArgs(args)
	    self.termWidget.startShellProgram()

	    self.watcher=QFileSystemWatcher()
	    self.watcher.addPath(self.errorFile)
	    self.connect(self.watcher,SIGNAL("fileChanged(QString)"),self.fileChangedSlot)
	    #self.fileChangedSlot()

    def showToWidget(self,lines):
	str1=""
        for line in lines:
	    str1+=QString(line)
	self.ARMSimulatorShowSignal.emit(2,str1)	

    def GetAPCParameter(self,lines):
	key=""
	apcport=""
        for line in lines:
	    str1=QString(line)
	    if str1.indexOf("Share memory key")>=0:
		pos=str1.indexOf("is")
		key=str1.right(str1.length()-pos-3)
		key=key[:len(key)-1]
	    elif str1.indexOf("realview.apc")>=0:
		pos=str1.indexOf("port")
		apcport=str1.right(str1.length()-pos-5)
		apcport=apcport[:len(apcport)-1]
	if key!="" and apcport!="":
	    self.parameter=1
	    self.APCSimulatorSignal.emit(key,apcport)


    def fileChangedSlot(self,string):
        f=open(self.errorFile,"r")
        lines=f.readlines()
	f.close()
	if self.parameter==0:
	    self.GetAPCParameter(lines)
	self.showToWidget(lines)

    def stopProcess(self):
	self.commandWidget=QTermWidget(0)
	self.commandWidget.setShellProgram("/bin/bash")
	self.commandWidget.setArgs([])
	self.commandWidget.startShellProgram()
	if self.flag==1:
	    pid=self.termWidget.getShellPID()
	    self.commandWidget.sendText("kill -s 9 "+str(pid)+"\n")
	elif self.flag==2:
	    pid=self.termWidget.getShellPID()
	    self.commandWidget.sendText("kill -s 9 "+str(pid)+"\n")
	    self.watcher.removePath(self.errorFile)
	    del self.watcher
	    self.watcher=0
	    #os.remove(self.errorFile)
	self.ARMSimulatorStatusSignal.emit(False) 
	self.parameter=0




