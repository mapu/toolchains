from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
sys.path.append("../..")
from control.DataBase import*

class Thread(QThread):
    def __init__(self,parent=None):
	super(Thread,self).__init__(parent)
	
    def run(self):
	self.dataBase=DataBase()
	self.dataBase.snTableInit()
	self.dataBase.regTableInit()
	self.dataBase.timeTableInit()
