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
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.result = self.inittablewidget.item(0,1).text()
        self.expectresult = QString("hello")
        self.headeritem = self.inittablewidget.horizontalHeaderItem(0).text()
        self.headeritems = self.inittablewidget.horizontalHeaderItem(1).text()
        self.expectheaderitem = QString("nonameFSM0")
        for i in range(2,21):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i-1))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        self.column = self.inittablewidget.columnCount()
        self.Columncount = 21
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.headeritem,self.expectheaderitem)
        self.assertEqual(self.headeritems,self.expectheaderitem)

    def testinsertColumnsfirst(self):
        selrange = QTableWidgetSelectionRange(0,0,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,1,0,1)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.results = self.inittablewidget.item(0,0).text()
        self.result = self.inittablewidget.item(0,2).text()
        self.expectresult = QString("world")
        self.expectresults = QString("hello")
        self.headeritem = self.inittablewidget.horizontalHeaderItem(1).text()
        self.headeritems = self.inittablewidget.horizontalHeaderItem(2).text()
        self.expectheaderitem = QString("nonameFSM1")
        for i in range(3,21):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i-1))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        self.column = self.inittablewidget.columnCount()
        self.Columncount = 21
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.headeritem,self.expectheaderitem)
        self.assertEqual(self.headeritems,self.expectheaderitem)

    def testinsertColumnssecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row ,column , row , column + 1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row, column + 1, QTableWidgetItem("world"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row, column + 1,row , column + 1)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.results = self.inittablewidget.item(row, column).text()
        self.result = self.inittablewidget.item(row, column + 2).text()
        self.expectresult = QString("world")
        self.expectresults = QString("hello")
        self.headeritem = self.inittablewidget.horizontalHeaderItem(column + 1).text()
        self.headeritems = self.inittablewidget.horizontalHeaderItem(column + 2).text()
        self.expectheaderitem = QString("nonameFSM" + str(column + 1))
        for i in xrange(column + 3, self.column):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i-1))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 21
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.headeritem,self.expectheaderitem)
        self.assertEqual(self.headeritems,self.expectheaderitem)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
