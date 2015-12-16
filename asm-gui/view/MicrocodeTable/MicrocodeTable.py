# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import* 
from SetFSMNameWidget import*
from InsertDialog import*
from DeleteDialog import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MicrocodeTable(QTableWidget):  
    def __init__(self,parent=None):  
        super(MicrocodeTable,self).__init__(parent)   

	self.setSelectionMode(QAbstractItemView.ContiguousSelection)
	
	self.RowCount=100
	self.ColumnCount=100
	self.currentRowNum=0
	self.currentColumnNum=0
	self.currentTopRow=0
	self.currentLeftColumn=0
	self.clearTable()

	self.connect(self.horizontalHeader(),SIGNAL("sectionDoubleClicked(int)"),self.setHeader)

    def clearTable(self):
	self.setRowCount(0)
	self.setColumnCount(0)
	self.setRowCount(self.RowCount)
	self.setColumnCount(self.ColumnCount)

    def setHeader(self,column):
	setFSMNameWidget=SetFSMNameWidget(column)
	setFSMNameWidget.setFSMNameSignal.connect(self.setFSMNameSlot)
	setFSMNameWidget.exec_()

    def setFSMNameSlot(self,column,string):
	item=QTableWidgetItem(string)
	self.setHorizontalHeaderItem(column,item)

    def cut(self):
	self.copy()
	self.delete()

    def copy(self):	
	selRange=self.selectedRange()
	string=QString()
	for i in range(0,selRange.rowCount()):
	    if i>0:
		string+='\n'
	    for j in range(0,selRange.columnCount()):
		if j >0:
		    string+='\t'
		item=self.item(selRange.topRow()+i,selRange.leftColumn()+j)
		if item==None:
		    string+=""
		else:
		    string+=self.item(selRange.topRow()+i,selRange.leftColumn()+j).text()
	QApplication.clipboard().setText(string)

    def paste(self):
	selRange=self.selectedRange()
	string=QApplication.clipboard().text()
	rows=string.split('\n')
	numRows=rows.count()
	numColumns=rows.first().count('\t')+1
	if selRange.rowCount()*selRange.columnCount()!=1 and (selRange.rowCount()!=numRows or selRange.columnCount()!=numColumns):
	    QMessageBox.information(self,'MicrocodeTable',"The information cannot be passted because the copy and paste areas aren't the same size.")
	    return
	for i in range(0,numRows):
	    columns=rows[i].split('\t')
	    for j in range(0,numColumns):
		row=selRange.topRow()+i
		column=selRange.leftColumn()+j
		if row<self.RowCount and column<self.ColumnCount:
		    self.setItem(row,column,QTableWidgetItem(columns[j]))

    def delete(self):
	items=self.selectedItems()
	for i in range(len(items)):
	    items[i].setText("")

    def selectCurrentRow(self):
	self.selectRow(self.currentRow())

    def selectCurrentColumn(self):
	self.selectColumn(self.currentColumn())

    def insertDialogShow(self):
	selRange=self.selectedRange()
	self.currentRowNum=selRange.rowCount()
	self.currentColumnNum=selRange.columnCount()
	self.currentTopRow=selRange.topRow()
	self.currentLeftColumn=selRange.leftColumn()
	insertDialog=InsertDialog(self)
	insertDialog.insertOperateSignal.connect(self.insertOperateSlot)
	insertDialog.exec_()

    def deleteDialogShow(self):
	selRange=self.selectedRange()
	self.currentRowNum=selRange.rowCount()
	self.currentColumnNum=selRange.columnCount()
	self.currentTopRow=selRange.topRow()
	self.currentLeftColumn=selRange.leftColumn()
	deleteDialog=DeleteDialog(self)
	deleteDialog.deleteOperateSignal.connect(self.deleteOperateSlot)
	deleteDialog.exec_()

    def selectedRange(self):
	ranges=self.selectedRanges()
	if len(ranges)==0:
	    return QTableWidgetSelectionRange()
	return ranges[0]
		    
    def insertOperateSlot(self,index):
	if index==0:
	    self.insertLeftCell()
	elif index==1:
 	    self.insertUpCell()
	elif index==2:
	    self.insertRows()
	elif index==3:
	    self.insertColumns()

    def insertLeftCell(self):
	self.insertColumn(self.ColumnCount)
	self.ColumnCount+=1

	string=QString()
	for i in range(0,self.currentRowNum):
	    if i>0:
		string+='\n'
	    for j in range(0,self.ColumnCount-self.currentLeftColumn):
		if j >0:
		    string+='\t'
		item=self.item(self.currentTopRow+i,self.currentLeftColumn+j)
		if item==None:
		    string+=""
		else:
		    string+=item.text()
	self.delete()
	rows=string.split('\n')
	numRows=rows.count()
	numColumns=rows.first().count('\t')+1
	for i in range(0,numRows):
	    columns=rows[i].split('\t')
	    for j in range(0,numColumns):
		row=self.currentTopRow+i
		column=self.currentLeftColumn+j+self.currentColumnNum
		if row<self.RowCount and column<self.ColumnCount:
		    self.setItem(row,column,QTableWidgetItem(columns[j]))

    def insertUpCell(self):
	self.insertRow(self.RowCount)
	self.RowCount+=1

	string=QString()
	for i in range(0,self.ColumnCount-self.currentTopRow):
	    if i>0:
		string+='\n'
	    for j in range(0,self.currentColumnNum):
		if j >0:
		    string+='\t'
		item=self.item(self.currentTopRow+i,self.currentLeftColumn+j)
		if item==None:
		    string+=""
		else:
		    string+=item.text()
	self.delete()
	rows=string.split('\n')
	numRows=rows.count()
	numColumns=rows.first().count('\t')+1
	for i in range(0,numRows):
	    columns=rows[i].split('\t')
	    for j in range(0,numColumns):
		row=self.currentTopRow+i+self.currentRowNum
		column=self.currentLeftColumn+j
		if row<self.RowCount and column<self.ColumnCount:
		    self.setItem(row,column,QTableWidgetItem(columns[j]))
  
    def insertRows(self):
	for i in range(self.currentRowNum):
	    self.insertRow(self.currentTopRow+i)

    def insertColumns(self):
	for i in range(self.currentColumnNum):
	    self.insertColumn(self.currentLeftColumn+i)

    def deleteOperateSlot(self,index):
	if index==0:
	    self.deleteCellLeft()
	elif index==1:
 	    self.deleteCellUp()
	elif index==2:
	    self.deleteRows()
	elif index==3:
	    self.deleteColumns()

    def deleteCellLeft(self):
	string=QString()
	for i in range(0,self.currentRowNum):
	    if i>0:
		string+='\n'
	    for j in range(0,self.ColumnCount-self.currentLeftColumn-self.currentColumnNum):
		if j >0:
		    string+='\t'
		item=self.item(self.currentTopRow+i,self.currentLeftColumn+self.currentColumnNum+j)
		if item==None:
		    string+=""
		else:
		    string+=item.text()
	self.delete()
	rows=string.split('\n')
	numRows=rows.count()
	numColumns=rows.first().count('\t')+1
	for i in range(0,numRows):
	    columns=rows[i].split('\t')
	    for j in range(0,numColumns):
		row=self.currentTopRow+i
		column=self.currentLeftColumn+j
		if row<self.RowCount and column<self.ColumnCount:
		    self.setItem(row,column,QTableWidgetItem(columns[j]))	

    def deleteCellUp(self):
	string=QString()
	for i in range(0,self.RowCount-self.currentTopRow-self.currentRowNum):
	    if i>0:
		string+='\n'
	    for j in range(0,self.currentColumnNum):
		if j >0:
		    string+='\t'
		item=self.item(self.currentTopRow+self.currentRowNum+i,self.currentLeftColumn+j)
		if item==None:
		    string+=""
		else:
		    string+=item.text()
	self.delete()
	rows=string.split('\n')
	numRows=rows.count()
	numColumns=rows.first().count('\t')+1
	for i in range(0,numRows):
	    columns=rows[i].split('\t')
	    for j in range(0,numColumns):
		row=self.currentTopRow+i
		column=self.currentLeftColumn+j
		if row<self.RowCount and column<self.ColumnCount:
		    self.setItem(row,column,QTableWidgetItem(columns[j]))

    def deleteRows(self):
	for i in range(self.currentRowNum):
	    self.removeRow(self.currentTopRow)

    def deleteColumns(self):
	for i in range(self.currentColumnNum):
	    self.removeColumn(self.currentLeftColumn)		





