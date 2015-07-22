#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class TableModel(QAbstractTableModel):
    def __init__(self, arraydata, parent=None,):
        QAbstractTableModel.__init__(self, parent,)
	self.horizontalHeaderList=[]
	self.verticalHeaderList=[]
        self.arraydata=arraydata

    def setModalDatas(self,arraydata):
         self.arraydata=arraydata

    def setHorizontalHeader(self,horizontalHeaderList):
	self.horizontalHeaderList=horizontalHeaderList

    def setVerticalHeader(self,verticalHeaderList):
        self.verticalHeaderList=verticalHeaderList

    def rowCount(self, parent):
        return len(self.arraydata)

    def columnCount(self, parent):
        return len(self.arraydata[0])

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
        if role == Qt.DisplayRole:
            return QVariant(self.arraydata[index.row()][index.column()])
        #if role == Qt.BackgroundRole:
            #if int(index.data().toString().toInt())<=0:
               # return QColor(Qt.blue);
            #else:
                #return QColor(Qt.white);   

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
    	return QVariant()

    def refrushModel(self):
        self.beginResetModel()
        self.endResetModel()









