#!/usr/bin/env python 
#-*- coding:utf-8 -*- 
import re
import os
import sys
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 
sys.path.append("/usr/local/lib64/")
from QTermWidget import*
import time

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

    def startProcess(self,path):
	self.ARMSimulatorStatusSignal.emit(True) 
	self.flag=1
	ARMCommand=self.simulatorPath+"/arm_qemu/bin/qemu-system-arm -M mapu -m 512 -pflash "+path+" -serial stdio 2>"+self.errorFile+"\n"
	self.termWidget.sendText(ARMCommand)	

	#/home/litt/simulator/arm_qemu/bin/qemu-system-arm -M mapu -m 512 -pflash /home/litt/simulator/sim_dmac.bin -serial stdio
	if os.path.exists(self.pidFile)==True:
	    os.remove(self.pidFile)
	self.commandWidget=QTermWidget()
	self.commandWidget.sendText("ps -ef | grep "+self.simulatorPath+"/arm_qemu/bin/qemu-system-arm | awk '{print $2 , $3}'>>"+self.pidFile+"\n")
	self.GetAPCParameter()

    def GetAPCParameter(self):
	b=os.path.exists(self.errorFile)
	if b==False:
  	    return
        f=open(self.errorFile,"r")
        lines=f.readlines()
	key=""
	apcport=""
        for line in lines:
	    str1=QString(line)
	    self.ARMSimulatorShowSignal.emit(1,str1)
	    if str1.indexOf("Share memory key")>=0:
		pos=str1.indexOf("is")
		key=str1.right(str1.length()-pos-3)
		key=key[:len(key)-1]
	    elif str1.indexOf("realview.apc")>=0:
		pos=str1.indexOf("port")
		apcport=str1.right(str1.length()-pos-5)
		apcport=apcport[:len(apcport)-1]
	if key!="" and apcport!="":
	    self.APCSimulatorSignal.emit(key,apcport)

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
	    #self.commandWidget.resize(800,600)
	    #self.commandWidget.show()
	    self.termWidget.sendText("   \n")
	    self.termWidget.sendText("reset\n")
	self.ARMSimulatorStatusSignal.emit(False) 




