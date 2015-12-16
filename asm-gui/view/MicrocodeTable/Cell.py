# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import* 

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class Cell(QTableWidgetItem):  
    def __init__(self,parent=None):  
        super(Cell,self).__init__(parent)   
	
	self.setDirty()

    def setData(self,role,value):
	QTableWidgetItem.setData(role,value)
	if role==Qt.EditRole:
	    self.setDirty()

    def data(self,role):
	if role==Qt.DisplayRole:
	    if self.value().isValid():
		return self.value().toString()
	    else:
		return '####'
	elif role==Qt.TextAlignmentRole:
	    if self.value().type()==QVariant.String:
		return int(Qt.AlignLeft|Qt.AlignVCenter)
	    else:
		return int(Qt.AlignRight|Qt.AlignVCenter)
	else:
	    return QTableWidgetItem.data(role)

    def setFormula(self,formula):
	self.setData(Qt.EditRole,formula)

    def formula(self):
	return self.data(Qt.EditRole).toString()

    def setDirty(self):
	self.cacheIsDirty=True


