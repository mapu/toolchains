#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class TableModel(QAbstractTableModel):
    def __init__(self, subArray, parent=None,):
        QAbstractTableModel.__init__(self, parent,)
	self.horizontalHeaderList=[]
	self.verticalHeaderList=[]
        self.subArray=subArray
	self.key=""
	self.curValue=0

    def setModalDatas(self,subArray):
         self.subArray=subArray

    def setHorizontalHeader(self,horizontalHeaderList):
	self.horizontalHeaderList=horizontalHeaderList

    def setVerticalHeader(self,verticalHeaderList):
        self.verticalHeaderList=verticalHeaderList

    def rowCount(self, parent):
        return len(self.subArray)

    def columnCount(self, parent):
        return len(self.subArray[0])

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
	else:
	    if self.curValue<=index.row()<=self.curValue+40:
                if role == Qt.DisplayRole:
		    text=self.subArray[index.row()][index.column()]
		    if text.find("rw")>=0 or text.find("wr")>=0:
			text=text[:(len(text)-2)]
		    elif text.find("r")>=0:
			text=text[:(len(text)-1)]
		    elif text.find("w")>=0:
			text=text[:(len(text)-1)]
                    return QVariant(text)
    	        elif role==Qt.TextAlignmentRole:
                    return int(Qt.AlignHCenter)
                elif role == Qt.BackgroundRole:
	            if index.data().toString()!="":
		        text=self.subArray[index.row()][index.column()]
			if text.find("rw")>=0 or text.find("wr")>=0:
			    return QColor(255,153,18)
			elif text.find("r")>=0:
			    return QColor(0,255,0)
			elif text.find("w")>=0:
			    return QColor(255,0,0)
			else:
                            if text=="FG" or text=="FS" or text=="FW" or text=="FR" or text=="DP":
                                return QColor("gray")
                            else:
                                return QColor(193,210,255) 

    def flags(self,index):
	if not index.isValid():
	    return Qt.NoItemFlags
	return Qt.ItemIsSelectable|Qt.ItemIsEnabled|Qt.ItemIsEditable

    def headerData(self,section,orientation,role):
        if role==Qt.DisplayRole:
            if orientation==Qt.Horizontal:
                if len(self.horizontalHeaderList)>section:
                    return self.horizontalHeaderList[section]
                else:
                    return QVariant() 
            else:
                if len(self.verticalHeaderList)>section:
                    return self.verticalHeaderList[section]
                else:
                    return QVariant()
	elif role == Qt.TextColorRole and orientation==Qt.Vertical:
	    if self.key!="":
		if len(self.verticalHeaderList)>section:
		    if self.verticalHeaderList[section].find(self.key)>=0:
		        return QColor(255,153,18)
        elif role == Qt.BackgroundRole and orientation==Qt.Vertical:
	    if self.key!="":
		if len(self.verticalHeaderList)>section:
		    if self.verticalHeaderList[section].find(self.key)>=0:
			brush=QBrush()
			brush.setColor(QColor(255,153,18))
		        return brush
	elif role==Qt.FontRole and orientation==Qt.Vertical:
	    if self.key!="":
		if len(self.verticalHeaderList)>section:
		    if self.verticalHeaderList[section].find(self.key)>=0:
			font=QFont()
			font.setFamily("Monospace")
			font.setBold(True)
			return font
    	return QVariant()

    def refrushModel(self):
        self.beginResetModel()
        self.endResetModel()









