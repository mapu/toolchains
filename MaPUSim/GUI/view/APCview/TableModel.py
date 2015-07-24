#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class TableModel(QAbstractTableModel):
    def __init__(self, arrayData, parent=None,):
        QAbstractTableModel.__init__(self, parent,)
	self.horizontalHeaderList=[]
	self.verticalHeaderList=[]
        self.arrayData=arrayData
	self.key=""
	self.curValue=0

    def setDataBase(self,dataBase,APEdbFilePath,flag):
	self.dataBase=dataBase
	self.APEdbFilePath=APEdbFilePath
	self.flag=flag

    def setModalDatas(self,arrayData):
         self.arrayData=arrayData

    def setHorizontalHeader(self,horizontalHeaderList):
	self.horizontalHeaderList=horizontalHeaderList

    def setVerticalHeader(self,verticalHeaderList):
        self.verticalHeaderList=verticalHeaderList

    def rowCount(self, parent):
        return len(self.arrayData)

    def columnCount(self, parent):
        return len(self.arrayData[0])

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
	else:
	    if self.curValue<=index.row()<=self.curValue+40:
                if role == Qt.DisplayRole:
                    return QVariant(self.arrayData[index.row()][index.column()])
                elif role == Qt.BackgroundRole:
	            if index.data().toString()!="":
		        text=str(index.data().toString())
			if text.find("&")>=0:
			    return QColor(255,153,18)
			elif text.find("R")>=0:
			    return QColor(0,255,0)
			elif text.find("W")>=0:
			    return QColor(255,0,0)
			else:
                            if int(text)<0:
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
    	return QVariant()

    def refrushModel(self):
        self.beginResetModel()
        self.endResetModel()









