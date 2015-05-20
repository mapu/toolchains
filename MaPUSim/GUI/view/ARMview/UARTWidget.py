# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class UARTWidget(QWidget):
    def __init__(self,parent=None):
	super(UARTWidget,self).__init__(parent)

	self.text=QTextEdit()
	self.text.setReadOnly(True)
	self.edit=QLineEdit()
	
	layout=QVBoxLayout()
	layout.addWidget(self.text)
	layout.addWidget(self.edit)
	self.setLayout(layout)

