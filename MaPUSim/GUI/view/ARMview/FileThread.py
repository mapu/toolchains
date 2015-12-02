# -*- coding=utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import os

class FileThread(QThread):
    fileExistSignal=pyqtSignal(list)
    def __init__(self,path,parent=None):
	super(FileThread,self).__init__(parent)
	self.path=path

    def run(self):
	while True:
	    if os.path.exists(self.path)==True:
		while True:
		    b=self.GetAPCParameter()
		    if b==True:
			self.fileExistSignal.emit(self.lines)
		   	break
		break

    def GetAPCParameter(self):
	b=os.path.exists(self.path)
	if b==False:
	    return
        f=open(self.path,"r")
        self.lines=f.readlines()
	f.close()
	key=""
	apcport=""
        for line in self.lines:
	    str1=QString(line)
	    if str1.indexOf("Share memory key")>=0:
		pos=str1.indexOf("is")
		key=str1.right(str1.length()-pos-3)
		key=key[:len(key)-1]
	    elif str1.indexOf("realview.apc")>=0:
		pos=str1.indexOf("port")
		apcport=str1.right(str1.length()-pos-5)
		apcport=apcport[:len(apcport)-1]
	if key!="" and apcport!="":
	    return True
	return False

	
