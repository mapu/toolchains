'''
Created on Apr 20, 2016
@author Litt
'''
from PyQt4.QtGui import QProgressDialog
from PyQt4.QtCore import SIGNAL

class ProgressDialog(QProgressDialog):
    def __init__(self, config, control, parent = None):
	super(ProgressDialog, self).__init__(parent)
	self.config = config
	self.control = control
	self.setWindowTitle("Data base progress")
	self.setFixedSize(300, 100)
	self.control.simDB.traceAnalyzeProgress.connect(self.valueChanged)
	self.control.simDB.traceAnalyzeStart.connect(self.showDialog)
	self.control.simDB.traceAnalyzeDone.connect(self.close)
	self.setCancelButton(None)
	
    def valueChanged(self, text, value):
	self.setLabelText(text)
	self.setValue(value)
	
    def showDialog(self, minValue, maxValue):
	self.setMaximum(maxValue)
	self.setMinimum(minValue)  
	self.show()