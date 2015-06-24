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

    def simulatorShowText(self,code,string):
	if code==0:
	    self.text.append(string)
	else:
	    defaultColor=self.text.textColor() 
	    num=string.count("\n")
	    for i in range(0,num):
		pos=string.indexOf("\n")
		str1=string.left(pos)
		string=string.right(string.size()-pos-1)
		if str1.contains("fatal")==True:
		    self.text.setTextColor(QColor("red"))
		    self.text.append(str1)
		    self.text.setTextColor(defaultColor)
		else:
		    self.text.append(str1)

