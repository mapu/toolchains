from PyQt4.QtGui import*
from PyQt4.QtCore import*
import time
import sys
sys.path.append("../..")
from control.DataBase import*

class Thread(QThread):
    def __init__(self,parent=None):
	super(Thread,self).__init__(parent)
	self.num=0
	
    def run(self):
	self.dataBase=DataBase()
	self.dataBase.createDatabase(self.num)
