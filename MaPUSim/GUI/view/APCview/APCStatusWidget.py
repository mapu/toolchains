from PyQt4.QtGui import*
from PyQt4.QtCore import*

class APCStatusWidget(QWidget):
    def __init__(self,parent=None):
	super(APCStatusWidget,self).__init__(parent)

	self.fullText=QTextEdit()
	self.fullText.setReadOnly(True)
	gridLay=QGridLayout()
	gridLay.addWidget(self.fullText,0,0)
	self.setLayout(gridLay)

    def simulatorShowText(self,code,string):
	if code==0:    
	    self.fullText.append(string)
	else:  
	    defaultColor=self.fullText.textColor() 
	    num=string.count("\n")
	    for i in range(0,num):
		pos=string.indexOf("\n")
		str1=string.left(pos)
		string=string.right(string.size()-pos-1)
		if str1.contains("fatal")==True:
		    self.fullText.setTextColor(QColor("red"))
		    self.fullText.append(str1)
		    self.fullText.setTextColor(defaultColor)
		else:
		    self.fullText.append(str1) 	
