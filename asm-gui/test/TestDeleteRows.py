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

    def testdeleteRowsfirst(self):
        selrange = QTableWidgetSelectionRange(0,0,1,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("world"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.deleteRows()
        self.results = self.inittablewidget.item(0,0).text()
        self.expectresults = QString("world")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 1999
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.results,self.expectresults)

    def testdeleteRowssecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row, column, row + 1, column )
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row + 1, column, QTableWidgetItem("world"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row , column, row , column )
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.deleteRows()
        self.result = self.inittablewidget.item(row, column).text()
        self.expectresult = QString("world")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 1999
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
