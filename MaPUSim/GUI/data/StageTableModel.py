# -*- coding: utf-8 -*-
from PyQt4.QtCore import QAbstractTableModel, QVariant, Qt, QString, pyqtSlot
from PyQt4.QtGui import QColor, QFont, QBrush
from data.RegNames import getRegName
from view.APCview.CellDelegate import CellDelegate

class StageTableModel(QAbstractTableModel):
    def __init__(self, stage_table, reg_table, delegate, parent = None):
        QAbstractTableModel.__init__(self, parent)
        self.stageTable = stage_table
        self.regTable = reg_table
        self.itemDelegate = delegate
        self.hHeaderList = []
        self.vHeaderList = []
        self.stages = [[]]
        self.key = ""
        self.curValue = 0
        self.curMin = 0
        self.curMax = 0
        self.rwColors = {"R" : QColor(0, 255, 0),
                         "W" : QColor(255, 0, 0),
                         "RW" : QColor(255, 153, 18),
                         "WR" : QColor(255, 153, 18),
                         "Fetch" : QColor("light gray"),
                         "Ex" : QColor(193, 210, 255),
                         "Matched": QColor(255, 153, 18)}
    
    @pyqtSlot(int)
    def switchPageSlot(self, index):
        '''
        Switch a selected page from stage table
        '''
        if index == -1:
            return
        self.curMin, self.curMax, self.vHeaderList, self.stages = \
            self.stageTable.getTablePage(index)
        self.itemDelegate.setStages(self.stages)
        self.hHeaderList = map(str, range(self.curMin, self.curMax + 1))
        self.dataChanged.emit(self.index(0, 0), 
                              self.index(self.rowCount() - 1, 
                                         self.columnCount() - 1))
        self.refrushModel()

    def rowCount(self, parent = None):
        return len(self.stages)

    def columnCount(self, parent = None):
        return len(self.stages[0])

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
        if role == Qt.DisplayRole:
            text = self.stages[index.row()][index.column()].split(".")
            return QVariant(text[0])
        elif role == Qt.TextAlignmentRole:
            return int(Qt.AlignCenter)
        elif role == Qt.FontRole:
            text = self.stages[index.row()][index.column()].split(".")
            if len(text[0]) > 2:
                return QFont("Monospace", 7)
        elif role == Qt.ToolTipRole:
            if self.stages[index.row()][index.column()] != "":
                text = self.stages[index.row()][index.column()].split(".")
                if len(text) > 1:
                    sn = self.stageTable.getSn(index.row())
                    time = index.column() + self.curMin
                    condition = "sn == %s" % sn
                    regOps = self.regTable.getOperations(time, condition)
                    tip = "Regster operations:\n"
                    for op in regOps:
                        name = getRegName(op[1], op[2])
                        tip += op[0] + " " + name + "\n"
                    return QString(tip)

    '''
        elif role == Qt.BackgroundRole:
            if self.stages[index.row()][index.column()] != "":
                text = self.stages[index.row()][index.column()].split(".")
                if len(text) > 1:
                    return self.rwColors[text[1]]
                elif text[0] in ["FG", "FS", "FW", "FR", "DP"]:
                    return self.rwColors["Fetch"]
                else:
                    return self.rwColors["Ex"]
    '''

    def flags(self, index):
        if not index.isValid():
            return Qt.NoItemFlags
        return Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsEditable

    def headerData(self, section, orientation, role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return QVariant()
            else:
                if len(self.vHeaderList) > section:
                    return self.vHeaderList[section]
                else:
                    return QVariant()
        elif role == Qt.TextColorRole and orientation == Qt.Vertical:
            if self.key != "":
                if len(self.vHeaderList) > section:
                    if self.vHeaderList[section].find(self.key) >= 0:
                        return self.rwColors["Matched"]
        elif role == Qt.BackgroundRole and orientation == Qt.Vertical:
            if self.key != "":
                if len(self.vHeaderList) > section:
                    if self.vHeaderList[section].find(self.key) >= 0:
                        brush = QBrush()
                        brush.setColor(self.rwColors["Matched"])
                        return brush
        elif role == Qt.FontRole and orientation == Qt.Vertical:
            if self.key != "":
                if len(self.vHeaderList) > section:
                    if self.vHeaderList[section].find(self.key) >= 0:
                        font = QFont()
                        font.setFamily("Monospace")
                        font.setBold(True)
                        return font
            return QVariant()
        
    def getStart(self, row):
        '''
        Get the start column index of the given inst at the "row"
        '''
        start = self.stageTable.getStart(row) - self.curMin
        while self.stages[row][start] != "" and self.stages[row][start].find("RR"):
            start += 1
        if self.stages[row][start] == "":
            return self.stageTable.getStart(row) - self.curMin
        else:
            return start
        

    def refrushModel(self):
        self.beginResetModel()
        self.endResetModel()

    def searchHeader(self, key):
        result = []
        for item in self.vHeaderList:
            if item.find(key):
                result.append(self.vHeaderList.index(item))
        return result





