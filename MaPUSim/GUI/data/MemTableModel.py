# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt, QAbstractTableModel, QVariant, QString
from PyQt4.QtGui import QFont

class MemTableModel(QAbstractTableModel):
    def __init__(self, mem_table, inst_table, parent = None):
        QAbstractTableModel.__init__(self, parent)

        self.memTable = mem_table
        self.instTable = inst_table
        self.mem = [[]]
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

    def getMemData(self):
        sql_query = "select " + self.memTable.Name + ".time, pc, dis, op, addr, value from " + self.memTable.Name + " , "\
                     + self.instTable.Name + " where " + self.memTable.Name + ".sn = " + self.instTable.Name + ".sn"
        cur = self.instTable.DBConn.execute(sql_query)
        self.mem = cur.fetchall() 
        if len(self.mem) == 0:
            return
        self.refrushModel()


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
