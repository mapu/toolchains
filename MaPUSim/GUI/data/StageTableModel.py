# -*- coding: utf-8 -*-
from PyQt4.QtCore import QAbstractTableModel, QVariant, Qt, QString, pyqtSlot
from PyQt4.QtGui import QColor, QFont, QBrush, QLinearGradient
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
        self.allVHeaderList = []
        self.currentPage = 0
        self.stages = [[]]
        self.key = ""
        self.curValue = 0
        self.curMin = 0
        self.curMax = 0

        green = QLinearGradient()
        green.setColorAt(0.0, Qt.green)
        green.setColorAt(0.2, QColor(113, 255, 147,))
        green.setColorAt(0.4, QColor(137, 255, 165,))
        green.setColorAt(0.6, QColor(167, 255, 188,))
        green.setColorAt(0.8, QColor(209, 255, 218,))
        green.setColorAt(1.0, QColor(229, 255, 235))

        red = QLinearGradient()
        red.setColorAt(0.0, QColor(216, 64, 3))
        red.setColorAt(0.2, QColor(252, 86, 20))
        red.setColorAt(0.4, QColor(253, 157, 119))
        red.setColorAt(0.6, QColor(254, 191, 186))
        red.setColorAt(0.8, QColor(254, 219, 216))
        red.setColorAt(1.0, QColor(255, 244, 243))

        yellow = QLinearGradient()
        yellow.setColorAt(0.0, QColor(244, 238, 4))
        yellow.setColorAt(0.2, QColor(255, 251, 33))
        yellow.setColorAt(0.4, QColor(255, 253, 153))
        yellow.setColorAt(0.6, QColor(251, 254, 172))
        yellow.setColorAt(0.8, QColor(251, 254, 210))
        yellow.setColorAt(1.0, QColor(253, 255, 229))

        grey = QLinearGradient()
        grey.setColorAt(0.0, QColor(194, 194, 194))
        grey.setColorAt(0.2, QColor(207, 207, 207))
        grey.setColorAt(0.4, QColor(230, 230, 230))
        grey.setColorAt(0.6, QColor(243, 243, 243))
        grey.setColorAt(0.8, QColor(248, 248, 248))
        grey.setColorAt(1.0, QColor(253, 253, 253))

        blue = QLinearGradient()
        blue.setColorAt(0.0, QColor(71, 124, 255))
        blue.setColorAt(0.2, QColor(121, 159, 255))  
        blue.setColorAt(0.4, QColor(189, 208, 255)) 
        blue.setColorAt(0.6, QColor(213, 255, 255))         
        blue.setColorAt(0.8, QColor(220, 229, 255))
        blue.setColorAt(1.0, QColor(235, 241, 255))

        self.rwColors = {"R" : green,
                         "W" : red,
                         "RW" : yellow,
                         "WR" : yellow,
                         "Fetch" : grey,
                         "Ex" : blue,
                         "Matched": QColor(255, 153, 18)}
        '''
        self.rwColors = {"R" : QColor(0, 255, 0),
                         "W" : QColor(255, 0, 0),
                         "RW" : QColor(255, 153, 18),
                         "WR" : QColor(255, 153, 18),
                         "Fetch" : QColor("light gray"),
                         "Ex" : QColor(193, 210, 255),
                         "Matched": QColor(255, 153, 18)}
        '''
  
    @pyqtSlot(int)
    def switchPageSlot(self, index):
        '''
        Switch a selected page from stage table
        '''
        if index == -1:
            return
        self.currentPage = index
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
                    gradient = self.rwColors[text[1]]
                    gradient.setStart(index.column() * 22, index.row() * 20)
                    gradient.setFinalStop((index.column() + 1) * 22, (index.row() + 1) * 20)
                    brush = QBrush(gradient)
                    brush.setStyle(Qt.LinearGradientPattern)
                    return brush
                elif text[0] in ["FG", "FS", "FW", "FR", "DP"]:
                    gradient = self.rwColors["Fetch"]
                    gradient.setStart(index.column() * 22, index.row() * 20)
                    gradient.setFinalStop((index.column() + 1) * 22, (index.row() + 1) * 20)
                    brush = QBrush(gradient)
                    brush.setStyle(Qt.LinearGradientPattern)
                    return brush
                else:
                    gradient = self.rwColors["Ex"]
                    gradient.setStart(index.column() * 22, index.row() * 20)
                    gradient.setFinalStop((index.column() + 1) * 22, (index.row() + 1) * 20)
                    brush = QBrush(gradient)
                    brush.setStyle(Qt.LinearGradientPattern)
                    return brush
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
        i = 0
        for item in self.allVHeaderList:
            if item.find(key) >= 0:
                result.append(i)
            i += 1
        return result 

    def getAllVHeaderList(self):
        self.allVHeaderList, self.idList = self.stageTable.getVHeaderList()

