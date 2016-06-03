# -*- coding: utf-8 -*-

from PyQt4.QtGui import QLineEdit, QItemDelegate, QPainter, QPen, QStyleOptionViewItem
from PyQt4.QtCore import Qt, pyqtSignal, pyqtSlot, SIGNAL
import data.RegColor

class CellDelegate(QItemDelegate):
    searchTreeSignal = pyqtSignal(int, int, str)
    floatDialogCloseSignal = pyqtSignal()
    def __init__(self, parent = None):
        super(CellDelegate, self).__init__(parent)
        self.regColor = data.RegColor.initRegColor()
        self.flagList = [[1, 1, 0, 0],
                         [0, 0, 1, 1],
                         [0, 0, 0, 0],
                         [1, 1, 1, 1]]

    def createEditor(self, parent, option, index):
        self.lineEdit = QLineEdit(parent)
        self.tableRow = index.row()
        self.tableColumn = index.column()
        self.connect(self.lineEdit, SIGNAL("editingFinished()"), self.commitAndCloseEditor)
        self.connect(self.lineEdit, SIGNAL("textChanged(QString)"), self.textChangedSlot)
        return self.lineEdit

    def commitAndCloseEditor(self):
        self.floatDialogCloseSignal.emit()

    def textChangedSlot(self, text):
        self.searchTreeSignal.emit(self.tableRow, self.tableColumn, str(text))

    def lineEditClear(self):
        self.lineEdit.clear()

    def setArray(self, array, loopList):
        self.array = array
        self.loopBodyList = loopList

    def paint(self, painter, option, index):
        myOption = QStyleOptionViewItem(option)
        step = 2 #indent
        if index.row() >= len(self.array):
            QItemDelegate.paint(self, painter, myOption, index) 
            return
        if self.array[index.row()][index.column()] != "....":
            columnList = self.loopBodyList[index.column()]
            columnNum = len(columnList)
            cmpList = []
            for i in range(columnNum):
                info = columnList[i]
                if info.startRow <= index.row() and info.endRow >= index.row():
                    cmpList.append(info)
            text = self.array[index.row()][index.column()]
            textList = text.split(".")
            pen = QPen()
            for cmpInfo in cmpList:
                if cmpInfo.startRow == index.row() and cmpInfo.endRow == index.row():
                    flag = self.flagList[3]
                elif cmpInfo.startRow == index.row():
                    flag = self.flagList[0]
                elif cmpInfo.endRow == index.row():
                    flag = self.flagList[1]
                else:
                    flag = self.flagList[2]                
                reg = cmpInfo.reg
                pen.setColor(self.regColor[str(reg)])
                painter.setPen(pen)
                if textList[0] != "":
                    regList = textList[0].split(",")
                    if str(reg) in regList:
                        painter.drawLine(option.rect.topLeft().x() + (reg * step), option.rect.topLeft().y() + (reg * step * flag[0]), \
                                         option.rect.topRight().x() - (reg * step), option.rect.topRight().y() + (reg * step * flag[1]))
                if textList[1] != "":
                    regList = textList[1].split(",")
                    if str(reg) in regList:
                        painter.drawLine(option.rect.topRight().x() - (reg * step), option.rect.topRight().y() + (reg * step * flag[1]), \
                                         option.rect.bottomRight().x() - (reg * step), option.rect.bottomRight().y() - (reg * step * flag[2])) 
                if textList[2] != "":
                    regList = textList[2].split(",")
                    if str(reg) in regList:
                        painter.drawLine(option.rect.bottomRight().x() - (reg * step), option.rect.bottomRight().y() - (reg * step * flag[2]), \
                                         option.rect.bottomLeft().x() + (reg * step), option.rect.bottomLeft().y() - (reg * step * flag[3]))
                if textList[3] != "":
                    regList = textList[3].split(",")
                    if str(reg) in regList:
                        painter.drawLine(option.rect.bottomLeft().x() + (reg * step), option.rect.bottomLeft().y() - (reg * step * flag[3]), \
                                         option.rect.topLeft().x() + (reg * step), option.rect.topLeft().y() + (reg * step * flag[0]))       
        QItemDelegate.paint(self, painter, myOption, index)   

