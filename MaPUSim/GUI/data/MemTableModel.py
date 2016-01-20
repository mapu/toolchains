# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt, QAbstractTableModel, QVariant, QString
from PyQt4.QtGui import QFont

class MemTableModel(QAbstractTableModel):
    def __init__(self, mem, parent = None):
        QAbstractTableModel.__init__(self, parent)

        self.mem = mem
        self.hHeaderList = []
        self.vHeaderList = []

    def rowCount(self, parent = None):
        return len(self.mem)

    def columnCount(self, parent = None):
        return len(self.mem[0])

    def setHorizontalHeader(self, headerList):
        self.hHeaderList = headerList

    def setVerticalHeader(self, headerList):
        self.vHeaderList = headerList

    def setModelData(self, array):
        self.mem = array

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
	else:
            if role == Qt.DisplayRole:
		text = self.mem[index.row()][index.column()]
                return QVariant(text)
    	    elif role == Qt.TextAlignmentRole:
                return int(Qt.AlignHCenter)

    def headerData(self,section,orientation,role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                if len(self.hHeaderList) > section:
                    return self.hHeaderList[section]
                else:
                    return QVariant() 
            else:
                if len(self.vHeaderList) > section:
                    return self.vHeaderList[section]
                else:
                    return QVariant()

    	return QVariant()

    def refrushModel(self):
        self.beginResetModel()
        self.endResetModel()
