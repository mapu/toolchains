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
 
    def testdeleteUpCellfirst(self):
        self.inittablewidget.setItem(1,1,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(2,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(3,1,QTableWidgetItem("china"))
        selrange = QTableWidgetSelectionRange(2,1,2,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentRowNum = selrange.rowCount()  
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.deleteCellUp()
        self.result = self.inittablewidget.item(1,1).text()
        self.results = self.inittablewidget.item(2,1).text()
        self.resultss = self.inittablewidget.item(3,1).text()
        self.expectresult = QString("hello")
        self.expectresults = QString("china")
        self.expectresultss = QString("")
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)

    def testdeleteUpCellsecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        self.inittablewidget.setItem(row - 1, column , QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row, column, QTableWidgetItem("world"))
        self.inittablewidget.setItem(row + 1, column, QTableWidgetItem("china"))
        selrange = QTableWidgetSelectionRange(row, column, row , column )
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.deleteCellUp()
        self.result = self.inittablewidget.item(row, column).text()
        self.results = self.inittablewidget.item(row + 1, column).text()
        self.expectresult = QString("china")
        self.expectresults = QString("")
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
