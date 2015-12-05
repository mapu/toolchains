#!/usr/bin/env python 
#-*- coding:utf-8 -*- 
import re
import os
import sys
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 
sys.path.append("/usr/local/lib64/")
from QTermWidget import*

class EmbTerminal(QWidget): 
    ARMSimulatorStatusSignal=pyqtSignal(bool)
    ARMSimulatorShowSignal=pyqtSignal(int,str)
    APCSimulatorSignal=pyqtSignal(str,str)
    def __init__(self): 
	super(EmbTerminal,self).__init__()
    	QWidget.__init__(self) 
	self.termWidget=QTermWidget()
	self.lay=QVBoxLayout()
	self.lay.addWidget(self.termWidget)
	self.setLayout(self.lay)	
	self.simulatorPath=""
	self.flag=0
	self.errorFile="main.out"
	self.pidFile='a.dat'
	self.parameter=0

    def startProcess(self,path):
	self.ARMSimulatorStatusSignal.emit(True) 
	self.flag=1
	f=open(self.errorFile,"w")
	f.close()
	ARMCommand=self.simulatorPath+"/arm/bin/qemu-system-arm -M mapu -m 512 -pflash "+path+" -serial stdio 2>"+self.errorFile+"\n"
	self.termWidget.sendText(ARMCommand)	

	#/home/litt/simulator/arm_qemu/bin/qemu-system-arm -M mapu -m 512 -pflash /home/litt/simulator/sim_dmac.bin -serial stdio
	if os.path.exists(self.pidFile)==True:
	    os.remove(self.pidFile)
	self.commandWidget=QTermWidget()
	self.commandWidget.sendText("ps -ef | grep "+self.simulatorPath+"/arm/bin/qemu-system-arm | awk '{print $2 , $3}'>>"+self.pidFile+"\n")
	
	self.watcher=QFileSystemWatcher()
	self.watcher.addPath(self.errorFile)
	self.connect(self.watcher,SIGNAL("fileChanged(QString)"),self.fileChangedSlot)

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
	b=os.path.exists(self.errorFile)
	if b==False:
	    return False
        f=open(self.errorFile,"r")
        lines=f.readlines()
	f.close()
	if self.parameter==0:
	    self.GetAPCParameter(lines)
	self.showToWidget(lines)

    def stopProcess(self):
	if self.flag==1:
	    pid=self.termWidget.getShellPID()
	    b=os.path.exists(self.pidFile)
	    if b==True:
	    	f=open(self.pidFile,"r")
	    	lines=f.readlines()
	    	for line in lines:
		    strList=line.split(" ")
		    string=strList[1]
		    string=string[:len(string)-1]
		    if int(string)==pid:
		        self.commandWidget.sendText("kill -s 9 "+strList[0]+"\n")
	    self.termWidget.sendText("   \n")
	    self.termWidget.sendText("reset\n")
	    self.watcher.removePath(self.errorFile)
	    del self.watcher
	    self.watcher=0
	    os.remove(self.errorFile)
	self.ARMSimulatorStatusSignal.emit(False) 
	self.parameter=0




