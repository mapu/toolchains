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

    def testdeleteColumnsfirst(self):
        selrange = QTableWidgetSelectionRange(0,0,0,2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,0,1)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.deleteColumns()
        self.result = self.inittablewidget.item(0,0).text()
        self.expectresult = QString("china")
        for i in range(0,18):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i + 2))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        self.column = self.inittablewidget.columnCount()
        self.Columncount = 18
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)

    def testdeleteColumnssecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,17)
        #print row,column
        selrange = QTableWidgetSelectionRange(row ,column , row , column + 2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row, column + 1, QTableWidgetItem("world"))
        self.inittablewidget.setItem(row, column + 2, QTableWidgetItem("china"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row, column ,row , column + 1)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.deleteColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(row, column).text()
        self.expectresult = QString("china")
        for i in xrange(0, column ):
            self.headeritem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitem = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritem,self.expectheaderitem)
        for i in xrange(column , self.column):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i + 2))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 18
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)

    def testdeleteColumnsthird(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varRow = random.randint(1,2000 - row)
        varColumn = random.randint(1,20 - column)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column + varColumn - 1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.deleteColumns()
        self.column = self.inittablewidget.columnCount()
        for i in range(0, column):
            self.headeritems = self.inittablewidget.horizontalHeaderItem(i).text()
            self.originaltexts = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritems, self.originaltexts)
        for i in range(column , self.column):
            self.horheaderitem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.horexpectheaderitems = QString("nonameFSM" + str(i + varColumn ))
            self.assertEqual(self.horheaderitem,self.horexpectheaderitems)
        self.column = self.inittablewidget.columnCount()
        self.Columncount = 20 - varColumn 
        self.assertEqual(self.column, self.Columncount)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
