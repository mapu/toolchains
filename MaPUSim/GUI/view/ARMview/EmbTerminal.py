#!/usr/bin/env python 
#-*- coding:utf-8 -*- 

import sys 
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 

class EmbTerminal(QWidget): 
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
	#self.connect(self.process,SIGNAL("finished(int,QProcess::ExitStatus)"),self.finishProcess)
	self.simulatorPath="/arm/utils/m5term"
	self.flag=0

    def startProcess(self,string): 
	self.flag=1
   	self.process.start('xterm',['-into', str(self.terminal.winId()),'-geometry','250x15','-e', str(self.simulatorPath), 'localhost', str(string)]) 
        if False==self.process.waitForStarted():
	    print "this process can not be called."

    #def finishProcess(self,exitCode,exitStatus):
        #if exitStatus==QProcess.NormalExit:
	    #print "uart0 process exit normal"
        #else:
	    #print "uart0 process exit crash"

    def startReadOutput(self):
        ba=self.process.readAllStandardOutput()

    def startReadErrOutput(self):
        ba=self.process.readAllStandardError()

    def stopProcess(self):
	if self.flag==1:
            self.process.close()


