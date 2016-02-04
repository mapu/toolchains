# -*- coding: utf-8 -*-   
import sys
from PyQt4.QtGui import QTableWidget, QAbstractItemView, QApplication, QPainter, QPen, QTableWidgetSelectionRange, QBrush, QTableWidgetItem
from PyQt4.QtCore import Qt, pyqtSignal, pyqtSlot, SIGNAL, QStringList, QString
from SetFSMNameWidget import SetFSMNameWidget
from view.MicrocodeTable.InsertDialog import InsertDialog
from view.MicrocodeTable.DeleteDialog import DeleteDialog
from view.MicrocodeTable.FloatDialog import FloatDialog
from view.MicrocodeTable.CellDelegate import CellDelegate
from view.Utils import warning
sys.path.append("../..")
from data.RectInfo import RectInfo
import re

class MicrocodeTable(QTableWidget):  
    floatDialogShowSignal = pyqtSignal(int, int, list)
    itemRegStateSignal = pyqtSignal(list)
    def __init__(self, parent = None):  
        super(MicrocodeTable, self).__init__(parent)   

        self.setSelectionMode(QAbstractItemView.ContiguousSelection)

        self.RowCount = 100
        self.ColumnCount = 10
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
        self.connect(self.horizontalScrollBar(), SIGNAL("valueChanged(int)"), self.horizontalScrollBarChangedSlot)
        self.connect(self.verticalScrollBar(), SIGNAL("valueChanged(int)"), self.verticalScrollBarChangedSlot)

        self.setAttribute(Qt.WA_PaintOutsidePaintEvent)
        #record the previous point row
        self.previousPointRow = -1

        #get default background
        item = QTableWidgetItem("")
        self.defaultBackgroundColor = item.background()
        #init array data
        self.array = [x[:] for x in [["...."]*(self.ColumnCount)]*(self.RowCount)]
        self.loopBodyList = [[]for i in xrange(self.ColumnCount)]
        cellDelegate = CellDelegate(self.array, self.loopBodyList)
        self.setItemDelegate(cellDelegate)
        cellDelegate.floatDialogShowSignal.connect(self.floatDialogShowSlot)
        cellDelegate.floatDialogCloseSignal.connect(self.floatDialogCloseSlot)
     
    def currentCellChangedSlot(self, currentRow, currentColumn, previousRow, previousColumn):
        self.floatDialogCloseSlot()
        self.CurrentRow = previousRow
        self.CurrentColumn = previousColumn
        #show reg info
        text = self.array[currentRow][currentColumn]
        textList = text.split(".")
        regList = textList[1].split(",")
        if len(regList) > 0:
            del regList[-1]
            self.itemRegStateSignal.emit(regList)   
        else:
            self.itemRegStateSignal.emit([])
        #show code input row
        if self.previousPointRow != -1:
            self.earserWholeRowColor(self.previousPointRow)   
        item = self.item(currentRow, currentColumn)
        if item != None:
            #check microcode is legal?

            if item.text() == "":
                return
             
            self.setWholeRowColor(currentRow, Qt.blue) 
            self.previousPointRow = currentRow     
        else:
            self.previousPointRow = -1 

    def horizontalScrollBarChangedSlot(self, i):
        self.floatDialogCloseSlot()

    def verticalScrollBarChangedSlot(self, i):
        self.floatDialogCloseSlot()

    def getRowCount(self):
        return self.rowCount()

    def getColumnCount(self):
        return self.columnCount()

    def clearTable(self):
        self.clear()
        self.setRowCount(self.RowCount)
        self.setColumnCount(self.ColumnCount)
        self.headerList.clear()
        for i in xrange(0, self.ColumnCount):
            self.headerList.append(QString("nonameFSM" + str(i)))
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
            QMessageBox.information(self, 'MicrocodeTable', "The information cannot be passted because the copy and paste areas aren't the same size.")
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
        for i in xrange(0, self.currentRowNum):
            if i> 0:
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
            if i> 0:
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

    def insertColumns(self):
        for i in xrange(self.currentColumnNum):
            self.insertColumn(self.currentLeftColumn + i)

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
            if i> 0:
                string += '\n'
            for j in xrange(0, self.ColumnCount - self.currentLeftColumn - self.currentColumnNum):
                if j > 0:
                    string += '\t'
                item = self.item(self.currentTopRow + i, self.currentLeftColumn + self.currentColumnNum + j)
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
                column = self.currentLeftColumn + j
                if row < self.RowCount and column < self.ColumnCount:
                     self.setItem(row, column, QTableWidgetItem(columns[j]))

    def deleteRows(self):
        for i in xrange(self.currentRowNum):
            self.removeRow(self.currentTopRow)

    def deleteColumns(self):
        for i in xrange(self.currentColumnNum):
            self.removeColumn(self.currentLeftColumn)		

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Down:
            if self.floatDialog.isVisible() == True:
                if self.floatDialogFocus == 0:
                    item = self.floatDialog.item(0)
                    if item != None:
                        self.floatDialog.setCurrentItem(item)
                        self.floatDialogFocus = 1
                elif self.floatDialogFocus == 1:
                    row = self.floatDialog.currentRow()
                    num = self.floatDialog.count()
	    	    if row + 1 < num:
                        self.floatDialog.setCurrentRow(row + 1)	
        elif event.key() == Qt.Key_Up and self.floatDialogFocus == 1:
            row = self.floatDialog.currentRow()
            if row > 0:
                self.floatDialog.setCurrentRow(row - 1)
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
        if item != None:
            string = item.text()
        else:
            string = ""
        if row == self.CurrentRow and column == self.CurrentColumn:
            self.setItem(row, column, QTableWidgetItem(string + event.text()))
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
            if row + 4 < self.RowCount:
                item = self.item(row + 1, column)
                if item == None:
                    item = QTableWidgetItem("")
                    self.setItem(row + 1, column, item)
                rect = self.visualItemRect(item)
                floatDialog_x = rect.topLeft().x() + self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.topLeft().y() + self.horizontalHeader().sizeHint().height()  
            else:
                item = self.item(row - 1, column)
                if item == None:
                    item = QTableWidgetItem("")
		    self.setItem(row - 1, column, item)	
                rect = self.visualItemRect(item)
                floatDialog_x = rect.topLeft().x() + self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.bottomLeft().y() + self.horizontalHeader().sizeHint().height() - 80		    	
        else:
            rect = self.visualItemRect(item)
            if row + 4 < self.RowCount:
                floatDialog_x = rect.bottomLeft().x() + self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.bottomLeft().y() + self.horizontalHeader().sizeHint().height()
            else:
                floatDialog_x = rect.bottomLeft().x() + self.verticalHeader().sizeHint().width()
                floatDialog_y = rect.topLeft().y() + self.horizontalHeader().sizeHint().height() - 80
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
        if self.floatDialog != 0:
            self.floatDialog.setVisible(False)
            self.floatDialogFocus = 0	

    def setRectInfo(self, reg):
        rectInfo = RectInfo()
        rectInfo.reg = reg

        selRange = self.selectedRange()
        self.currentRowNum = selRange.rowCount()
        self.currentColumnNum = selRange.columnCount()
        if self.currentColumnNum == 0:
            return -1
        if self.currentColumnNum > 1:
            return 0
        self.currentTopRow = selRange.topRow()
        self.currentLeftColumn = selRange.leftColumn()  
        rectInfo.startRow = self.currentTopRow
        rectInfo.endRow = self.currentTopRow + self.currentRowNum - 1
        rectInfo.column = self.currentLeftColumn
        #check selected cells
        columnList = self.loopBodyList[self.currentLeftColumn]
        num = len(columnList)
        for i in range(num):
            cmpInfo = columnList[i]
            if cmpInfo.startRow < rectInfo.startRow and cmpInfo.endRow < rectInfo.endRow and rectInfo.startRow < cmpInfo.endRow:
                return -2
        return rectInfo 

    def setItemInfo(self, info) :
        #update the items reg and frame info
        for i in xrange(info.startRow, info.endRow + 1):  
            if info.startRow == info.endRow:
                text = self.array[info.startRow][info.column]
                textList = text.split(".")
                num = len(textList)
                for i in xrange(num - 1):
                    regText = textList[i]
                    regText = "%s%s,"%(regText, str(info.reg))
                    textList[i] = regText                             
                text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4])  
                self.array[info.startRow][info.column] = text     
                break
            if i == info.startRow:
                text = self.array[i][info.column]
                textList = text.split(".")
                #up, right, left
                for j in [0, 1, 3]:
                    regText = textList[j]
                    regText = "%s%s,"%(regText,str(info.reg))
                    textList[j] = regText 
                text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4])  
                self.array[i][info.column] = text
            elif i == info.endRow:
                text = self.array[i][info.column]
                textList = text.split(".")
                #right, bottom, left
                for j in [1, 2, 3]:
                    regText = textList[j]
                    regText = "%s%s,"%(regText,str(info.reg))
                    textList[j] = regText 
                text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4])  
                self.array[i][info.column] = text
            else:
                text = self.array[i][info.column]
                textList = text.split(".")
                #right, left
                for j in [1, 3]:
                    regText = textList[j]
                    regText = "%s%s,"%(regText,str(info.reg))
                    textList[j] = regText 
                text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4])  
                self.array[i][info.column] = text                    
        
    def paintRect(self, reg):
        rectInfo = self.setRectInfo(reg)
        if rectInfo == 0 or rectInfo == -1 or rectInfo == -2:
            return rectInfo
        #add loopBodyList
        self.loopBodyList[rectInfo.column].append(rectInfo)
        self.setItemInfo(rectInfo) 
        self.viewport().update() 
        return 1

    def eraserRect(self, reg):
        selRange = self.selectedRange()
        self.currentTopRow = selRange.topRow()
        self.currentLeftColumn = selRange.leftColumn()
        columnList = self.loopBodyList[self.currentLeftColumn]
        num = len(columnList)
        if num == 0:
            return              
        for i in xrange(num, 0, -1):
           rectInfo = columnList[i - 1]
           if rectInfo.reg == reg and rectInfo.startRow <= self.currentTopRow and rectInfo.endRow >= self.currentTopRow and rectInfo.column == self.currentLeftColumn:
                columnList.remove(rectInfo)        
                #remove item reg info and frame                
                for i in xrange(rectInfo.startRow, rectInfo.endRow + 1):
                    if rectInfo.startRow == rectInfo.endRow:
                        text = self.array[i][rectInfo.column]
                        textList = text.split(".")
                        num = len(textList)
                        for j in xrange(num - 1):
                            regText = textList[j]
                            regList = regText.split(",")
                            regNum = len(regList)
                            for k in xrange(regNum - 1):
                                if regList[k] == str(reg):
                                    regList.remove(str(reg))
                                    break
                            regNum = len(regList)
                            regText = ""
                            for k in xrange(regNum - 1):
                                regText = "%s%s,"%(regText, regList[k])
                            textList[j] = regText                             
                        text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4])  
                        self.array[rectInfo.startRow][rectInfo.column] = text 
                        break    
                    if i == rectInfo.startRow:
                        text = self.array[i][rectInfo.column]
                        textList = text.split(".")
                        #up, right, left
                        for j in [0, 1, 3]:
                            regText = textList[j]
                            regList = regText.split(",")
                            regNum = len(regList)
                            for k in xrange(regNum - 1):
                                if regList[k] == str(reg):
                                    regList.remove(str(reg))
                                    break
                            regNum = len(regList)
                            regText = ""
                            for k in xrange(regNum - 1):
                                regText = "%s%s,"%(regText, regList[k])
                            textList[j] = regText                                             
                        text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4]) 
                        self.array[i][rectInfo.column] = text
                    elif i == rectInfo.endRow:
                        text = self.array[i][rectInfo.column]
                        textList = text.split(".")
                        #right, bottom, left
                        for j in [1, 2, 3]:
                            regText = textList[j]
                            regList = regText.split(",")
                            regNum = len(regList)
                            for k in xrange(regNum - 1):
                                if regList[k] == str(reg):
                                    regList.remove(str(reg))
                                    break
                            regNum = len(regList)
                            regText = ""
                            for k in xrange(regNum - 1):
                                regText = "%s%s,"%(regText, regList[k])
                            textList[j] = regText                                             
                        text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4])  
                        self.array[i][rectInfo.column] = text
                    else:
                        text = self.array[i][rectInfo.column]
                        textList = text.split(".")
                        #right, left
                        for j in [1, 3]:
                            regText = textList[j]
                            regList = regText.split(",")
                            regNum = len(regList)
                            for k in xrange(regNum - 1):
                                if regList[k] == str(reg):
                                    regList.remove(str(reg))
                                    break
                            regNum = len(regList)
                            regText = ""
                            for k in xrange(regNum - 1):
                                regText = "%s%s,"%(regText, regList[k])
                            textList[j] = regText                                             
                        text = "%s.%s.%s.%s.%s"%(textList[0], textList[1], textList[2], textList[3], textList[4]) 
                        self.array[i][rectInfo.column] = text 
        self.viewport().update()                              

    def setWholeRowColor(self, row, color):
        count = self.getColumnCount()
        for i in xrange(0, count):
            item = self.item(row, i)
            if item == None:
                item = QTableWidgetItem("")
                self.setItem(row, i, item)
            item.setBackground(QBrush(color))
        
    def earserWholeRowColor(self, row):
        count = self.getColumnCount()
        for i in xrange(0, count):
            item = self.item(row, i)
            if item == None:
                item = QTableWidgetItem("")
                self.setItem(row, i, item)
            item.setBackground(self.defaultBackgroundColor)

    def searchLPStart(self, columnList, startRow):
        num = len(columnList)
        regList = []
        for i in xrange(num):
            info = columnList[i]
            if info.startRow == startRow:
                regList.append(info)
        return regList

    def searchLPEnd(self, columnList, endRow):
        num = len(columnList)
        regList = []
        for i in xrange(num):
            info = columnList[i]
            if info.endRow == endRow:
                regList.append(info)
        return regList

    def saveFile(self, fileName):
        fp = open(fileName, "w")
        for column in xrange(self.ColumnCount):
            columnList = self.loopBodyList[column]
            lines = []
            line = ".hmacro "
            item = self.horizontalHeaderItem(column)
            line += item.text()
            line += "\n"
            lines.append(line)
            for row in xrange(self.RowCount): 
                #get loop start info
                regList = self.searchLPStart(columnList, row)
                num = len(regList)
                if num == 0 and row != 0:
                    if str(line).find(":") < 0:
                        line += ";"
                    line += "\n"
                    lines.append(line)               
                for i in xrange(num):
                    if row != 0 and str(line).find(":") < 0:
                        line += " || " 
                    elif str(line).find(":") >= 0:
                        line += "\n"
                        lines.append(line)
                        line = ""
                    else:
                        line = ""
                    loop = "LPTO (%df ) @ (%s)"%(regList[i].num, str(regList[i].reg))   
                    line += loop                
                    line += ";\n"
                    lines.append(line)                     
                item = self.item(row, column)
                if item == None or item.text() == "":
                    line = "NOP"
                else:
                    line = item.text()
                #get loop end info
                regList = self.searchLPEnd(columnList, row)
                num = len(regList)
                if num > 0:
                    line += ";\n"
                    lines.append(line)
                else:
                    continue       
                for i in xrange(num):                  
                    line = str(regList[i].num) + ":"
            if num == 0:
                line += ";"
            line += "\n"
            lines.append(line)
            lines.append(".endhmacro\n\n")
            fp.writelines(lines)

        fp.close()

    def openFile(self, fileName): 
        headerPattern = re.compile("\.hmacro ([a-zA-Z]+[0-9]+)")
        endPattern = re.compile("\.endhmacro")
        startLPPattern = re.compile("LPTO \((\d+)f \) @ \((\w+)\);")
        endLPPattern = re.compile("(\d+):")
        rowPattern = re.compile("(.+);")

        self.loopBodyList = [[]for i in xrange(self.ColumnCount)]

        fp = open(fileName, "r")
        stringList = fp.readlines()   
        column = 0
        row = 0
        for string in stringList:
            if headerPattern.search(string) != None:        
                record = headerPattern.search(string)
                self.setHorizontalHeaderItem(column, QTableWidgetItem(record.group(1)))
            elif endPattern.search(string) != None:
                record = endPattern.search(string)
                column += 1
                if column > self.ColumnCount:
                    self.setColumnCount(column)
                    self.loopBodyList.append([])
                row = 0
            elif startLPPattern.search(string) != None:
                if string.find("||") >= 0:
                    record = string.split(" || ")
                    if record[0] == "NOP":
                        text = ""
                    else:
                        text = record[0]
                    self.setItem(row, column, QTableWidgetItem(text))
                    row += 1
                    string = record[1]
                record = startLPPattern.search(string)           
                info = RectInfo()
                info.startRow = row
                info.num = int(record.group(1))
                info.reg = int(record.group(2))
                info.column = column
                item = QTableWidgetItem("")
                self.setItem(row, column, item)
                self.loopBodyList[column].append(info)
            elif endLPPattern.search(string) !=None:        
                record = endLPPattern.search(string)
                columnList = self.loopBodyList[column]
                for info in columnList:
                    if info.num == int(record.group(1)):
                        info.endRow = (row - 1)
                        break
                #set item reg value
                for i in range(info.startRow, info.endRow + 1):
                    item = self.item(i, column)
                    item.reg.append(info.reg)
            elif rowPattern.search(string) != None:
                record = rowPattern.search(string)
                text = record.group(1)
                if text == "NOP":
                    text = ""
                self.setItem(row, column, QTableWidgetItem(text))
                row += 1

        fp.close()

