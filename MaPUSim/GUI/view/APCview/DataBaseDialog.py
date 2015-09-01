from PyQt4.QtGui import*
from PyQt4.QtCore import*

class DataBaseDialog(QProgressDialog):
    def __init__(self,parent=None):
	super(DataBaseDialog,self).__init__(parent)

	self.setFixedSize(300,100)
	self.setWindowTitle("Information")
	self.setAttribute (Qt.WA_DeleteOnClose);
	self.setLabelText("Data base is building, please wait......")
	self.setCancelButton(None)

    def updateDataBaseDialog(self,value,string,maxValue):
	if maxValue!=-1:
	    self.setRange(value,maxValue)
	self.setValue(value)
	self.setLabelText(string)
  

