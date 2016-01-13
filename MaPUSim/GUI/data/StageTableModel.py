# -*- coding: utf-8 -*-
from PyQt4.QtCore import QAbstractTableModel, QVariant, Qt, QString, pyqtSlot
from PyQt4.QtGui import QColor, QFont, QBrush

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class StageTableModel(QAbstractTableModel):
    def __init__(self, stage_table, reg_table, parent = None):
        QAbstractTableModel.__init__(self, parent)
        self.stageTable = stage_table
        self.regTable = reg_table
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
        self.hHeaderList = map(str, range(self.curMin, self.curMax + 1))
        self.dataChanged.emit(self.index(0, 0), 
                              self.index(self.rowCount() - 1, 
                                         self.columnCount() - 1))
        self.refrushModel()

    def rowCount(self, parent=None):
        return len(self.stages)

    def columnCount(self, parent=None):
        return len(self.stages[0])

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
        if 1 or self.curValue <= index.row() <= self.curValue + 40:
            if role == Qt.DisplayRole:
                text = self.stages[index.row()][index.column()].split(".")
                return QVariant(text[0])
            elif role == Qt.TextAlignmentRole:
                return int(Qt.AlignHCenter)
            elif role == Qt.BackgroundRole:
                if self.stages[index.row()][index.column()] != "":
                    text = self.stages[index.row()][index.column()].split(".")
                    if len(text) > 1:
                        return self.rwColors[text[1]]
                    elif text[0] in ["FG", "FS", "FW", "FR", "DP"]:
                        return self.rwColors["Fetch"]
                    else:
                        return self.rwColors["Ex"]
            elif role == Qt.ToolTipRole:
                if self.stages[index.row()][index.column()] != "":
                    text = self.stages[index.row()][index.column()].split(".")
                    if len(text) > 1:
                        return QString(text[0])

    def flags(self, index):
        if not index.isValid():
            return Qt.NoItemFlags
        return Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsEditable

    def headerData(self, section, orientation, role):
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
        return self.stageTable.getStart(row) - self.curMin

    def refrushModel(self):
        self.beginResetModel()
        self.endResetModel()






