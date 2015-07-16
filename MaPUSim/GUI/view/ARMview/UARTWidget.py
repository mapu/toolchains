# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from EmbTerminal import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class UARTWidget(QWidget):
    def __init__(self,parent=None):
	super(UARTWidget,self).__init__(parent)

	self.embTerminal=EmbTerminal()
	layout=QVBoxLayout()
	layout.addWidget(self.embTerminal)
	self.setLayout(layout)

    def m5termProcessStart(self,string):
	self.embTerminal.startProcess(string)


