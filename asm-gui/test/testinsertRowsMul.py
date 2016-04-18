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

    def testinsertRows(self):
        selrange = QTableWidgetSelectionRange(0,0,2,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("world"))
        self.inittablewidget.currentRowNum = selrange.rowCount() - 1
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.insertRows()
        self.result = self.inittablewidget.item(2,0).text()
        self.results = self.inittablewidget.item(3,0).text()
        self.expectresult = QString("hello")
        self.expectresults = QString("world")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2002
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults) 

    def testinsertRowsfirst(self):
        selrange = QTableWidgetSelectionRange(0,0,3,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("world"))
        self.inittablewidget.setItem(2,0,QTableWidgetItem("china"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(1,0,3,1)
        self.inittablewidget.currentRowNum = selrange.rowCount() - 1
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.insertRows()
        self.results = self.inittablewidget.item(0,0).text()
        self.result = self.inittablewidget.item(3,0).text()
        self.resultss = self.inittablewidget.item(4,0).text()
        self.expectresult = QString("world")
        self.expectresults = QString("hello")
        self.expectresultss = QString("china")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2002
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)

    def testinsertRowssecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row, column, row + 3, column + 1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row + 1, column, QTableWidgetItem("world"))
        self.inittablewidget.setItem(row + 2, column, QTableWidgetItem("china"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row + 1, column, row + 3, column + 1)
        self.inittablewidget.currentRowNum = selrange.rowCount() - 1
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.insertRows()
        self.result = self.inittablewidget.item(row, column).text()
        self.results = self.inittablewidget.item(row + 3, column).text()
        self.resultss = self.inittablewidget.item(row + 4, column).text()
        self.expectresult = QString("hello")
        self.expectresults = QString("world")
        self.expectresultss = QString("china")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2002
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)

    def testinsertRowsthird(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varRow = random.randint(1,2000)
        varColumn = random.randint(1,20)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column + varColumn)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentRowNum = selrange.rowCount() - 1
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.insertRows()
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2000 + varRow
        self.assertEqual(self.row, self.Rowcount)


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
