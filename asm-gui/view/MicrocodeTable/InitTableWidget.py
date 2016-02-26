#from -*- coding: utf-8 -*-

from PyQt4.QtGui import QTableWidget, QApplication, QTableWidgetItem, QTableWidgetSelectionRange, QAbstractItemView, QFont
from PyQt4.QtCore import Qt, pyqtSignal, pyqtSlot, SIGNAL, QStringList, QString
from view.MicrocodeTable.SetFSMNameWidget import SetFSMNameWidget
from view.MicrocodeTable.InsertDialog import InsertDialog
from view.MicrocodeTable.DeleteDialog import DeleteDialog
from view.MicrocodeTable.CellDelegate import CellDelegate
import sys
sys.path.append("..")
from view.Utils import warning

class InitTableWidget(QTableWidget):
    def __init__(self, parent = None):
        super(InitTableWidget, self).__init__(parent)

        #self.setStyleSheet("QTableWidget::item{text-align: center;}")

        self.RowCount = 2000
        self.ColumnCount = 20
        self.currentRowNum = 0
        self.currentColumnNum = 0
        self.currentTopRow = 0
        self.currentLeftColumn = 0
        self.CurrentRow = -1
        self.CurrentColumn = -1
        self.setRowCount(self.RowCount)
        self.setColumnCount(self.ColumnCount)
        self.connect(self.horizontalHeader(), SIGNAL("sectionDoubleClicked(int)"), self.setHeader)
        self.setSelectionMode(QAbstractItemView.ContiguousSelection)
        #get default background
        item = QTableWidgetItem("")
        self.defaultBackgroundColor = item.background()
        #set font
        self.setFont(QFont("Monospace", 10))
        self.horizontalHeader().setFont(QFont("Monospace", 10))
        #set Item delegate
        self.cellDelegate = CellDelegate()
        self.setItemDelegate(self.cellDelegate)
        self.cellDelegate.floatDialogShowSignal.connect(self.floatDialogShowSlot)
        self.cellDelegate.floatDialogCloseSignal.connect(self.floatDialogCloseSlot)
        #init table
        self.initTable()

    def initTable(self):
        self.clear()
        self.headerList = QStringList()
        for i in xrange(0, self.ColumnCount):
            self.headerList.append(QString("nonameFSM" + str(i)))
        self.setHorizontalHeaderLabels(self.headerList)
        #init array data
        self.array = [["...."]*(self.ColumnCount)]
        self.loopEndRow = 0
        self.loopBodyList = [[]for i in xrange(self.ColumnCount)]
        self.cellDelegate.setArray(self.array, self.loopBodyList)

    def setHeader(self, column):
        setFSMNameWidget = SetFSMNameWidget(column, self)
        setFSMNameWidget.setFSMNameSignal.connect(self.setFSMNameSlot)
        setFSMNameWidget.exec_()

    def setFSMNameSlot(self, column, string):
        item = QTableWidgetItem(string)
        self.setHorizontalHeaderItem(column, item)

    def getRowCount(self):
        return self.rowCount()

    def getColumnCount(self):
        return self.columnCount()

    def cut(self):
        self.copy()
        self.delete()

    def copy(self):	
        selRange = self.selectedRange()
        string = QString()
        for i in xrange(0, selRange.rowCount()):
            if i> 0:
                string += '\n'
            for j in xrange(0, selRange.columnCount()):
                if j > 0:
                    string += '\t'
                item = self.item(selRange.topRow() + i, selRange.leftColumn() + j)
                if item == None:
                    string += ""
                else:
                    string += self.item(selRange.topRow() + i, selRange.leftColumn() + j).text()
        QApplication.clipboard().setText(string)

    def paste(self):
        selRange = self.selectedRange()
        string = QApplication.clipboard().text()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t') + 1
        if selRange.rowCount()*selRange.columnCount() != 1 and (selRange.rowCount() != numRows or selRange.columnCount() != numColumns):
            warning("The information cannot be pasted because the copy and paste areas aren't the same size.")
            return
        for i in xrange(0, numRows):
            columns = rows[i].split('\t')
            for j in xrange(0, numColumns):
                row = selRange.topRow() + i
                column = selRange.leftColumn() + j
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))

    def delete(self):
        items = self.selectedItems()
        for i in xrange(len(items)):
            items[i].setText("")

    def selectCurrentRow(self):
        self.selectRow(self.currentRow())

    def selectCurrentColumn(self):
        self.selectColumn(self.currentColumn())

    def selectedRange(self):
        ranges = self.selectedRanges()
        if len(ranges) == 0:
            return QTableWidgetSelectionRange()
        return ranges[0]

    def insertDialogShow(self):
        selRange = self.selectedRange()
        self.currentRowNum = selRange.rowCount()
        self.currentColumnNum = selRange.columnCount()
        self.currentTopRow = selRange.topRow()
        self.currentLeftColumn = selRange.leftColumn()
        insertDialog = InsertDialog(self)
        insertDialog.insertOperateSignal.connect(self.insertOperateSlot)
        insertDialog.exec_()

    def deleteDialogShow(self):
        selRange = self.selectedRange()
        self.currentRowNum = selRange.rowCount()
        self.currentColumnNum = selRange.columnCount()
        self.currentTopRow = selRange.topRow()
        self.currentLeftColumn = selRange.leftColumn()
        deleteDialog = DeleteDialog(self)
        deleteDialog.deleteOperateSignal.connect(self.deleteOperateSlot)
        deleteDialog.exec_()
		    
    def insertOperateSlot(self, index):
        if index == 0:
            self.insertLeftCell()
        elif index == 1:
            self.insertUpCell()
        elif index == 2:
            self.insertRows()
        elif index == 3:
            self.insertColumns()

    def insertLeftCell(self):
        self.insertColumn(self.ColumnCount)
        self.setHorizontalHeaderItem(self.ColumnCount, QTableWidgetItem("nonameFSM" + str(self.ColumnCount)))
        num = len(self.array)
        for i in xrange(num):
            data = self.array[i]
            data.append("....")
        self.loopBodyList.append([])
        self.ColumnCount += 1

        string = QString()
        for i in xrange(0, self.currentRowNum):
            if i > 0:
                string += '\n'
            for j in xrange(0, self.ColumnCount - self.currentLeftColumn):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow + i, self.currentLeftColumn + j)
                if item == None:
                    string += ""
                else:
                    string += item.text()
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t') + 1
        for i in xrange(0, numRows):
            columns = rows[i].split('\t')
            for j in xrange(0, numColumns):
                row = self.currentTopRow + i
                column = self.currentLeftColumn + j + self.currentColumnNum
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))

    def insertUpCell(self):
        self.insertRow(self.RowCount)
        self.RowCount += 1

        string = QString()
        for i in xrange(0, self.ColumnCount - self.currentTopRow):
            if i > 0:
                string += '\n'
            for j in xrange(0, self.currentColumnNum):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow + i, self.currentLeftColumn + j)
                if item == None:
                    string += ""
                else:
                    string += item.text()
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t') + 1
        for i in xrange(0, numRows):
            columns = rows[i].split('\t')
            for j in xrange(0, numColumns):
                row = self.currentTopRow + i + self.currentRowNum
                column = self.currentLeftColumn + j
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))
  
    def insertRows(self):
        for i in xrange(self.currentRowNum):
            self.insertRow(self.currentTopRow + i)
            if self.loopEndRow >= (self.currentTopRow + i):
                self.array.insert(self.currentTopRow + i, ["...."]*(self.ColumnCount))
                self.loopEndRow += 1

    def insertColumns(self):
        for i in xrange(self.currentColumnNum):
            self.insertColumn(self.currentLeftColumn + i)
            self.setHorizontalHeaderItem(self.currentLeftColumn + i, QTableWidgetItem("nonameFSM" + str(self.currentLeftColumn + i)))
            self.loopBodyList.append([])
        num = len(self.array)
        for i in xrange(num):
            data = self.array[i]
            for j in xrange(self.currentColumnNum):
                data.append("....")

    def deleteOperateSlot(self, index):
        if index == 0:
            self.deleteCellLeft()
        elif index == 1:
            self.deleteCellUp()
        elif index == 2:
            self.deleteRows()
        elif index == 3:
            self.deleteColumns()

    def deleteCellLeft(self):
        string = QString()
        for i in xrange(0, self.currentRowNum):
            if i > 0:
                string += '\n'
            for j in xrange(0, self.ColumnCount - self.currentLeftColumn - self.currentColumnNum):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow + i, self.currentLeftColumn + self.currentColumnNum + j)
                if item == None:
                    string += ""
                    self.setItem(self.currentTopRow + i, self.currentLeftColumn + self.currentColumnNum + j, QTableWidgetItem(""))
                else:
                    string += item.text()
                item.setText("")
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t') + 1
        for i in xrange(0, numRows):
            columns = rows[i].split('\t')
            for j in xrange(0, numColumns):
                row = self.currentTopRow + i
                column = self.currentLeftColumn + j
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))

    def deleteCellUp(self):
        string = QString()
        for i in xrange(0, self.RowCount - self.currentTopRow - self.currentRowNum):
            if i > 0:
                string += '\n'
            for j in xrange(0, self.currentColumnNum):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow + self.currentRowNum + i, self.currentLeftColumn + j)
                if item == None:
                    string += ""
                    self.setItem(self.currentTopRow + i, self.currentLeftColumn + self.currentColumnNum + j, QTableWidgetItem(""))
                else:
                    string += item.text()
                item.setText("")
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t') + 1
        for i in xrange(0, numRows):
            columns = rows[i].split('\t')
            for j in xrange(0, numColumns):
                row = self.currentTopRow + i
                column = self.currentLeftColumn + j
                if row < self.RowCount and column < self.ColumnCount:
                     self.setItem(row, column, QTableWidgetItem(columns[j]))

    def deleteRows(self):
        for i in xrange(self.currentRowNum):
            self.removeRow(self.currentTopRow)
            if self.loopEndRow >= self.currentTopRow:
                del self.array[self.currentTopRow]
                self.loopEndRow -= 1

    def deleteColumns(self):
        for i in xrange(self.currentColumnNum):
            self.removeColumn(self.currentLeftColumn)
            del self.loopBodyList[self.currentLeftColumn]
        num = len(self.array)
        for i in xrange(num):
            data = self.array[i]
            for j in xrange(self.currentColumnNum):
                del data[self.currentLeftColumn]
		

