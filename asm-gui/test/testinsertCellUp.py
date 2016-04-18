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

    def testinsertUpCell(self):
        selrange = QTableWidgetSelectionRange(0,0,1,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.currentRowNum = selrange.rowCount() - 1
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertUpCell()
        self.result = self.inittablewidget.item(0,0).text()
        self.results = self.inittablewidget.item(1,0).text()
        self.expectresult = QString("")
        self.expectresults = QString("hello")
        self.rowcount = 2001
        self.assertEqual(self.inittablewidget.RowCount,self.rowcount)
        self.assertTrue(self.inittablewidget.insertUpCell)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        
    def testinsertUpCellfirst(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row, column, row + 1, column + 1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.currentRowNum = selrange.rowCount() - 1
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertUpCell()
        self.result = self.inittablewidget.item(row, column).text()
        self.results = self.inittablewidget.item(row + 1, column).text()
        self.expectresult = QString("")
        self.expectresults = QString("hello")
        self.rowcount = 2001
        self.assertEqual(self.inittablewidget.RowCount,self.rowcount)
        self.assertTrue(self.inittablewidget.insertUpCell)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
