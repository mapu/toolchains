#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange
from PyQt4.QtCore import Qt, QString
from PyQt4.QtTest import QTest
import unittest
import main
import sys
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

    def testinsertColumns(self):
        selrange = QTableWidgetSelectionRange(0,0,1,3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(0,3).text()
        self.results = self.inittablewidget.item(0,4).text()
        self.resultss = self.inittablewidget.item(0,5).text()
        self.expectresult = QString("hello")
        self.expectresults = QString("world")
        self.expectresultss = QString("china")
        self.headeritem = self.inittablewidget.horizontalHeaderItem(0).text()
        self.headeritems = self.inittablewidget.horizontalHeaderItem(1).text()
        self.headeritemss = self.inittablewidget.horizontalHeaderItem(2).text()
        self.headerItem = self.inittablewidget.horizontalHeaderItem(3).text()
        self.headerItems = self.inittablewidget.horizontalHeaderItem(4).text()
        self.headerItemss = self.inittablewidget.horizontalHeaderItem(5).text()
        self.expectheaderitem = QString("nonameFSM0")
        self.expectheaderitems = QString("nonameFSM1")
        self.expectheaderitemss = QString("nonameFSM2")
        for i in range(6, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i-3))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 23
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)
        self.assertEqual(self.headeritem,self.expectheaderitem)
        self.assertEqual(self.headeritems,self.expectheaderitems)
        self.assertEqual(self.headeritemss,self.expectheaderitemss)
        self.assertEqual(self.headerItem,self.expectheaderitem)
        self.assertEqual(self.headerItems,self.expectheaderitems)
        self.assertEqual(self.headerItemss,self.expectheaderitemss)

    def testinsertColumnsfirst(self):
        selrange = QTableWidgetSelectionRange(0,0,1,3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,3,1,6)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(0,6).text()
        self.results = self.inittablewidget.item(0,7).text()
        self.resultss = self.inittablewidget.item(0,8).text()
        self.expectresult = QString("hello")
        self.expectresults = QString("world")
        self.expectresultss = QString("china")
        for i in range(0, 6):
            self.headeritem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.originaltext = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritem, self.originaltext)
        self.headerItem = self.inittablewidget.horizontalHeaderItem(6).text()
        self.headerItems = self.inittablewidget.horizontalHeaderItem(7).text()
        self.headerItemss = self.inittablewidget.horizontalHeaderItem(8).text()
        self.expectheaderitem = QString("nonameFSM0")
        self.expectheaderitems = QString("nonameFSM1")
        self.expectheaderitemss = QString("nonameFSM2")
        for i in range(9, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i-6))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 26
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)
        self.assertEqual(self.headerItem,self.expectheaderitem)
        self.assertEqual(self.headerItems,self.expectheaderitems)
        self.assertEqual(self.headerItemss,self.expectheaderitemss)

    def testinsertColumnssecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row, column, row + 1, column + 3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row, column + 1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(row, column + 2,QTableWidgetItem("china"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row, column + 3, row + 1, column + 6)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(row, column + 6).text()
        self.results = self.inittablewidget.item(row, column + 7).text()
        self.resultss = self.inittablewidget.item(row, column + 8).text()
        self.expectresult = QString("hello")
        self.expectresults = QString("world")
        self.expectresultss = QString("china")
        for i in range(0, column):
            self.headeritems = self.inittablewidget.horizontalHeaderItem(i).text()
            self.originaltexts = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritems, self.originaltexts)
        for i in range(column, column + 6):
            self.headeritem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.originaltext = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritem, self.originaltext)
        self.headerItem = self.inittablewidget.horizontalHeaderItem(column + 6).text()
        self.headerItems = self.inittablewidget.horizontalHeaderItem(column + 7).text()
        self.headerItemss = self.inittablewidget.horizontalHeaderItem(column + 8).text()
        self.expectheaderitem = QString("nonameFSM" + str(column))
        self.expectheaderitems = QString("nonameFSM" + str(column + 1))
        self.expectheaderitemss = QString("nonameFSM" + str(column + 2))
        for i in range(column + 9, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i-6))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 26
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)
        self.assertEqual(self.headerItem,self.expectheaderitem)
        self.assertEqual(self.headerItems,self.expectheaderitems)
        self.assertEqual(self.headerItemss,self.expectheaderitemss)

    def testinsertColumnsthird(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varRow = random.randint(1,2000)
        varColumn = random.randint(1,20)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column + varColumn)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row, column + varColumn, row + varRow, column + varColumn * 2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        for i in range(0, column):
            self.headeritems = self.inittablewidget.horizontalHeaderItem(i).text()
            self.originaltexts = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritems, self.originaltexts)
        for i in range(column, column + varColumn * 2):
            self.headeritem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.originaltext = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritem, self.originaltext)
        for i in range(column + varColumn * 2, self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i-varColumn * 2))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 20 + varColumn * 2
        self.assertEqual(self.column, self.Columncount)
        
suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
