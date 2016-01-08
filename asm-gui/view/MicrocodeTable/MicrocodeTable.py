# -*- coding: utf-8 -*-   
import sys
from PyQt4.QtGui import QTableWidget, QAbstractItemView, QTableWidgetItem, QApplication, QPainter, QPen
from PyQt4.QtCore import Qt, pyqtSignal, pyqtSlot, SIGNAL, QStringList, QString
from SetFSMNameWidget import SetFSMNameWidget
from InsertDialog import InsertDialog
from DeleteDialog import DeleteDialog
from FloatDialog import FloatDialog
from CellDelegate import CellDelegate
from view.Utils import warning
sys.path.append("../..")
from data.RectInfo import RectInfo

class MicrocodeTable(QTableWidget):  
    floatDialogShowSignal = pyqtSignal(int, int, list)
    def __init__(self, parent = None):  
        super(MicrocodeTable, self).__init__(parent)   

        self.setSelectionMode(QAbstractItemView.ContiguousSelection)
        cellDelegate = CellDelegate()
        self.setItemDelegate(cellDelegate)
        cellDelegate.floatDialogShowSignal.connect(self.floatDialogShowSlot)
        cellDelegate.floatDialogCloseSignal.connect(self.floatDialogCloseSlot)
        self.RowCount = 100
        self.ColumnCount = 100
        self.currentRowNum = 0
        self.currentColumnNum = 0
        self.currentTopRow = 0
        self.currentLeftColumn = 0
        self.CurrentRow = -1
        self.CurrentColumn = -1
        self.headerList = QStringList()
        self.clearTable()
        self.connect(self.horizontalHeader(), SIGNAL("sectionDoubleClicked(int)"), self.setHeader)
        self.floatDialog = 0
        self.floatDialogFocus = 0
        self.connect(self, SIGNAL("currentCellChanged(int, int, int, int)"), self.currentCellChangedSlot)
        self.connect(self, SIGNAL("itemClicked(QTableWidgetItem)"), self.itemClickedSlot)
        self.connect(self.horizontalScrollBar(), SIGNAL("valueChanged(int)"), self.scrollBarChangedSlot)
        self.connect(self.verticalScrollBar(), SIGNAL("valueChanged(int)"), self.scrollBarChangedSlot)

        self.setAttribute(Qt.WA_PaintOutsidePaintEvent)
        self.rectFrame = []

    def itemClickedSlot(self, item):
        self.repaint()

    def currentCellChangedSlot(self, currentRow, currentColumn, previousRow, previousColumn):
        self.viewport().update()
        self.floatDialogCloseSlot()
        self.CurrentRow = previousRow
        self.CurrentColumn = previousColumn

    def scrollBarChangedSlot(self, i):
        self.floatDialogCloseSlot()

    def clearTable(self):
        self.clear()
        self.setRowCount(self.RowCount)
        self.setColumnCount(self.ColumnCount)
        self.headerList.clear()
        for i in range(0, self.ColumnCount):
            self.headerList.append(QString("nonameFSM"+str(i)))
        self.setHorizontalHeaderLabels(self.headerList)

    def setHeader(self, column):
        setFSMNameWidget = SetFSMNameWidget(column, self)
        setFSMNameWidget.setFSMNameSignal.connect(self.setFSMNameSlot)
        setFSMNameWidget.exec_()

    def setFSMNameSlot(self, column, string):
        item = QTableWidgetItem(string)
        self.setHorizontalHeaderItem(column, item)

    def cut(self):
        self.copy()
        self.delete()

    def copy(self):	
        selRange = self.selectedRange()
        string = QString()
        for i in range(0, selRange.rowCount()):
            if i> 0:
                string += '\n'
            for j in range(0, selRange.columnCount()):
                if j > 0:
                    string += '\t'
                item = self.item(selRange.topRow()+i, selRange.leftColumn()+j)
                if item == None:
                    string += ""
                else:
                    string += self.item(selRange.topRow()+i, selRange.leftColumn()+j).text()
        QApplication.clipboard().setText(string)

    def paste(self):
        selRange = self.selectedRange()
        string = QApplication.clipboard().text()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t')+1
        if selRange.rowCount()*selRange.columnCount()!= 1 and (selRange.rowCount()!= numRows or selRange.columnCount()!= numColumns):
            QMessageBox.information(self, 'MicrocodeTable', "The information cannot be passted because the copy and paste areas aren't the same size.")
            return
        for i in range(0, numRows):
            columns = rows[i].split('\t')
            for j in range(0, numColumns):
                row = selRange.topRow()+i
                column = selRange.leftColumn()+j
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))

    def delete(self):
        items = self.selectedItems()
        for i in range(len(items)):
            items[i].setText("")

    def selectCurrentRow(self):
        self.selectRow(self.currentRow())

    def selectCurrentColumn(self):
        self.selectColumn(self.currentColumn())

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

    def selectedRange(self):
        ranges = self.selectedRanges()
        if len(ranges) == 0:
            return QTableWidgetSelectionRange()
        return ranges[0]
		    
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
        self.ColumnCount += 1

        string = QString()
        for i in range(0, self.currentRowNum):
            if i> 0:
                string += '\n'
            for j in range(0, self.ColumnCount-self.currentLeftColumn):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow+i, self.currentLeftColumn+j)
                if item == None:
                    string += ""
                else:
                    string += item.text()
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t')+1
        for i in range(0, numRows):
            columns = rows[i].split('\t')
            for j in range(0, numColumns):
                row = self.currentTopRow+i
                column = self.currentLeftColumn+j+self.currentColumnNum
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))

    def insertUpCell(self):
        self.insertRow(self.RowCount)
        self.RowCount += 1

        string = QString()
        for i in range(0, self.ColumnCount-self.currentTopRow):
            if i> 0:
                string += '\n'
            for j in range(0, self.currentColumnNum):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow+i, self.currentLeftColumn+j)
                if item == None:
                    string += ""
                else:
                    string += item.text()
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t')+1
        for i in range(0, numRows):
            columns = rows[i].split('\t')
            for j in range(0, numColumns):
                row = self.currentTopRow+i+self.currentRowNum
                column = self.currentLeftColumn+j
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))
  
    def insertRows(self):
        for i in range(self.currentRowNum):
            self.insertRow(self.currentTopRow+i)

    def insertColumns(self):
        for i in range(self.currentColumnNum):
            self.insertColumn(self.currentLeftColumn+i)

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
        for i in range(0, self.currentRowNum):
            if i> 0:
                string += '\n'
            for j in range(0, self.ColumnCount-self.currentLeftColumn-self.currentColumnNum):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow+i, self.currentLeftColumn+self.currentColumnNum+j)
                if item == None:
                    string += ""
                else:
                    string += item.text()
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t')+1
        for i in range(0, numRows):
            columns = rows[i].split('\t')
            for j in range(0, numColumns):
                row = self.currentTopRow+i
                column = self.currentLeftColumn+j
                if row < self.RowCount and column < self.ColumnCount:
                    self.setItem(row, column, QTableWidgetItem(columns[j]))	

    def deleteCellUp(self):
        string = QString()
        for i in range(0, self.RowCount-self.currentTopRow-self.currentRowNum):
            if i > 0:
                string += '\n'
            for j in range(0, self.currentColumnNum):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow+self.currentRowNum+i, self.currentLeftColumn+j)
                if item == None:
                    string += ""
                else:
                    string += item.text()
        self.delete()
        rows = string.split('\n')
        numRows = rows.count()
        numColumns = rows.first().count('\t')+1
        for i in range(0, numRows):
            columns = rows[i].split('\t')
            for j in range(0, numColumns):
                row = self.currentTopRow+i
                column = self.currentLeftColumn+j
                if row < self.RowCount and column < self.ColumnCount:
                     self.setItem(row, column, QTableWidgetItem(columns[j]))

    def deleteRows(self):
        for i in range(self.currentRowNum):
            self.removeRow(self.currentTopRow)

    def deleteColumns(self):
        for i in range(self.currentColumnNum):
            self.removeColumn(self.currentLeftColumn)		

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Down:
            if self.floatDialog.isVisible() == True:
                if self.floatDialogFocus == 0:
                    item = self.floatDialog.item(0)
                    if item!= None:
                        self.floatDialog.setCurrentItem(item)
                        self.floatDialogFocus = 1
                elif self.floatDialogFocus == 1:
                    row = self.floatDialog.currentRow()
                    num = self.floatDialog.count()
	    	    if row+1 < num:
                        self.floatDialog.setCurrentRow(row+1)	
        elif event.key() == Qt.Key_Up and self.floatDialogFocus == 1:
            row = self.floatDialog.currentRow()
            if row > 0:
                self.floatDialog.setCurrentRow(row-1)
        elif event.key() == Qt.Key_Return and self.floatDialogFocus == 1:
            item = self.floatDialog.currentItem()
            text = item.text()
            column = self.currentColumn()
            row = self.currentRow()
            item = self.item(row, column)	   
            item.setText(text) 
            self.floatDialogCloseSlot()
        if event.key() < 0x20 or event.key() > 0xff:
            return
        stringList = []
        stringList.append(event.text())
        stringList.append(event.text())
        stringList.append(event.text())
        column = self.currentColumn()
        row = self.currentRow()
        item = self.item(row, column)
        if item!= None:
            string = item.text()
        else:
            string = ""
        if row == self.CurrentRow and column == self.CurrentColumn:
            self.setItem(row, column, QTableWidgetItem(string+event.text()))
        else:
            self.setItem(row, column, QTableWidgetItem(event.text()))
        self.CurrentRow = row
        self.CurrentColumn = column
        self.floatDialogShow(row, column, stringList)

    def floatDialogShow(self, row, column, stringList):
        self.floatDialogCloseSlot()
        self.floatDialog = FloatDialog(self)
        item = self.item(row, column)
        floatDialog_x = 0
        floatDialog_y = 0
        if item == None:
            if row+4 < self.RowCount:
                item = self.item(row+1, column)
                if item == None:
                    item = QTableWidgetItem("")
                    self.setItem(row+1, column, item)
                rect = self.visualItemRect(item)
                floatDialog_x = rect.topLeft().x()+self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.topLeft().y()+self.horizontalHeader().sizeHint().height()  
            else:
                item = self.item(row-1, column)
                if item == None:
                    item = QTableWidgetItem("")
		    self.setItem(row-1, column, item)	
                rect = self.visualItemRect(item)
                floatDialog_x = rect.topLeft().x()+self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.bottomLeft().y()+self.horizontalHeader().sizeHint().height()-80		    	
        else:
            rect = self.visualItemRect(item)
            if row+4 < self.RowCount:
                floatDialog_x = rect.bottomLeft().x()+self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.bottomLeft().y()+self.horizontalHeader().sizeHint().height()
            else:
                floatDialog_x = rect.bottomLeft().x()+self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.topLeft().y()+self.horizontalHeader().sizeHint().height()-80
        self.floatDialog.setGeometry(floatDialog_x, floatDialog_y, self.horizontalHeader().length()/self.ColumnCount, 80)
        self.floatDialog.initDialog(stringList)
        self.floatDialog.setVisible(True)

    def floatDialogShowSlot(self, row, column, text):
        stringList = []
        stringList.append(text)
        stringList.append(text)
        stringList.append(text)
        self.floatDialogShow(row, column, stringList)

    def floatDialogCloseSlot(self):
        if self.floatDialog!= 0:
            self.floatDialog.setVisible(False)
            self.floatDialogFocus = 0	

    def getRectInfo(self, check, color):
        rectInfo = RectInfo()
        rectInfo.check = check
        rectInfo.color = color

        selRange = self.selectedRange()
        self.currentRowNum = selRange.rowCount()
        self.currentColumnNum = selRange.columnCount()
        if self.currentColumnNum > 1:
            return 0
        self.currentTopRow = selRange.topRow()
        self.currentLeftColumn = selRange.leftColumn()  
        item = self.item(self.currentTopRow, self.currentLeftColumn)
        if item == None:
            item = QTableWidgetItem("")
            self.setItem(self.currentTopRow, self.currentLeftColumn, item)
        rect = self.visualItemRect(item)
        rectInfo.topLeft_x = rect.topLeft().x()
        rectInfo.topLeft_y = rect.topLeft().y()
        rectInfo.topRight_x = rect.topRight().x()
        rectInfo.topRight_y = rect.topRight().y()  
        item = self.item(self.currentTopRow + self.currentRowNum - 1, self.currentLeftColumn + self.currentColumnNum - 1)
        if item == None:
           item = QTableWidgetItem("")
           self.setItem(self.currentTopRow + self.currentRowNum - 1, self.currentLeftColumn + self.currentColumnNum - 1,item)  
        rect = self.visualItemRect(item)
        rectInfo.bottomLeft_x = rect.bottomLeft().x()
        rectInfo.bottomLeft_y = rect.bottomLeft().y()
        rectInfo.bottomRight_x = rect.bottomRight().x()
        rectInfo.bottomRight_y = rect.bottomRight().y() 
        return rectInfo   

    def paintRect(self, check, color):
        rectInfo = self.getRectInfo(check, color)
        if rectInfo == 0:
            return 0
        #add rectFrame list
        self.rectFrame.append(rectInfo)
        self.viewport().update()
        return 1

    def eraserRect(self,check, color):
        num = len(self.rectFrame)
        if num == 0:
            return
        for i in range(num, 0, -1):
           rectInfo = self.rectFrame[i-1]
           if rectInfo.check == check and rectInfo.color == color:
                self.rectFrame.remove(rectInfo)
                self.viewport().update()

    def paintLine(self, x1, y1, x2, y2, color):
        qp = QPainter()
        qp.begin(self.viewport())
        pen = QPen(color,1.5,Qt.SolidLine)     
        qp.setPen(pen)      
        qp.translate(0,0)
        qp.drawLine(x1, y1, x2, y2)
        qp.end()

    def paintEvent(self,event):
        #draw rect
        num = len(self.rectFrame)
        if num == 0:
            QTableWidget.paintEvent(self, event)
            return
        for i in range(num):
            rectInfo = self.rectFrame[i]
            self.paintLine(rectInfo.topLeft_x, rectInfo.topLeft_y, rectInfo.topRight_x, rectInfo.topRight_y, rectInfo.color)
            self.paintLine(rectInfo.topRight_x, rectInfo.topRight_y, rectInfo.bottomRight_x, rectInfo.bottomRight_y, rectInfo.color)
            self.paintLine(rectInfo.bottomRight_x, rectInfo.bottomRight_y, rectInfo.bottomLeft_x, rectInfo.bottomLeft_y, rectInfo.color)
            self.paintLine(rectInfo.bottomLeft_x, rectInfo.bottomLeft_y, rectInfo.topLeft_x, rectInfo.topLeft_y, rectInfo.color)

        QTableWidget.paintEvent(self, event)


