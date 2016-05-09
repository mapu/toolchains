#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange
from PyQt4.QtCore import Qt, QString
from PyQt4.QtTest import QTest
import unittest
import sys
sys.path.append("..")
import main
import random
from view.MainWindow import MainWindow
from view.Utils import initParent

class mytest(unittest.TestCase): 
    def setUp(self):
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        #self.inittablewidget.setSelectionMode(QAbstractItemView.ContiguousSelection)

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testInsertColumns(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,0,2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(0,3).text()
        self.results = self.inittablewidget.item(0,4).text()
        self.resultss = self.inittablewidget.item(0,5).text()
        self.expectresult = QString("r0.m[0]->m[0]")
        self.expectresults = QString("r0.m[0]->m[1]")
        self.expectresultss = QString("r0.m[0]->m[2]")
        for i in range(0, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 23
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)

    def testInsertColumns_0(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,0,2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,3,0,5)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(0,6).text()
        self.results = self.inittablewidget.item(0,7).text()
        self.resultss = self.inittablewidget.item(0,8).text()
        self.expectresult = QString("r0.m[0]->m[0]")
        self.expectresults = QString("r0.m[0]->m[1]")
        self.expectresultss = QString("r0.m[0]->m[2]")
        for i in range(0, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 26
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)

    def testInsertColumns_1(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row, column, row , column + 2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row, column + 1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(row, column + 2,QTableWidgetItem("china"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row, column + 3, row , column + 5)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(row, column + 6).text()
        self.results = self.inittablewidget.item(row, column + 7).text()
        self.resultss = self.inittablewidget.item(row, column + 8).text()
        self.expectresult = QString("hello")
        self.expectresults = QString("world")
        self.expectresultss = QString("china")
        for i in range(0, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 26
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)
        
    def testInsertColumns_2(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varRow = random.randint(1,2000)
        varColumn = random.randint(1,20)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column + varColumn - 1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row, column + varColumn, row + varRow, column + varColumn * 2 - 1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        for i in range(0, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        self.column = self.inittablewidget.columnCount()
        self.Columncount = 20 + varColumn * 2
        self.assertEqual(self.column, self.Columncount)
        
suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
