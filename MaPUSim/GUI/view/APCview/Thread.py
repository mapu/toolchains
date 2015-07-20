from PyQt4.QtGui import*
from PyQt4.QtCore import*
import time
import sys
sys.path.append("../..")
from control.DataBase import*
import os

class Thread(QThread):
    def __init__(self,parent=None):
	super(Thread,self).__init__(parent)
	self.num=0
	self.path="m5out/aaa.out"	
    def progressCall(self,callback):
	self.callback=callback

    def run(self):
	self.dataBase=DataBase()
	if os.path.exists(self.dataBase.APE0dbFilePath):
	    os.remove(self.dataBase.APE0dbFilePath)
	if os.path.exists(self.dataBase.APE0timeFilePath):
	    os.remove(self.dataBase.APE0timeFilePath)	
	if os.path.exists(self.dataBase.APE1dbFilePath):
	    os.remove(self.dataBase.APE1dbFilePath)
	if os.path.exists(self.dataBase.APE1timeFilePath):
	    os.remove(self.dataBase.APE1timeFilePath)
	if os.path.exists(self.dataBase.APE2dbFilePath):
	    os.remove(self.dataBase.APE2dbFilePath)
	if os.path.exists(self.dataBase.APE2timeFilePath):
	    os.remove(self.dataBase.APE2timeFilePath)
	if os.path.exists(self.dataBase.APE3dbFilePath):
	    os.remove(self.dataBase.APE3dbFilePath)
	if os.path.exists(self.dataBase.APE3timeFilePath):
	    os.remove(self.dataBase.APE3timeFilePath)
	self.dataBase.createDatabase(self.num,self.path,self.callback)
