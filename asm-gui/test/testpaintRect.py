#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QLineEdit
from PyQt4.QtCore import Qt, QString
from PyQt4.QtTest import QTest
import unittest
import main
import sys
import random
from view.MainWindow import MainWindow
from view.Utils import initParent
from data.RectInfo import RectInfo

class mytest(unittest.TestCase): 
    def setUp(self):
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        #self.inittablewidget.setSelectionMode(QAbstractItemView.ContiguousSelection)

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testpaintRect(self):
        selrange = QTableWidgetSelectionRange(0,0,1,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText("3")
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.startRow = self.inittablewidget.currentTopRow
        self.inittablewidget.endRow = self.inittablewidget.currentTopRow + self.inittablewidget.currentRowNum - 1
        self.inittablewidget.column = self.inittablewidget.currentLeftColumn
        self.inittablewidget.startRow = self.inittablewidget.currentTopRow
        self.inittablewidget.endRow = self.inittablewidget.currentTopRow + self.inittablewidget.currentRowNum - 1
        self.inittablewidget.column = self.inittablewidget.currentLeftColumn
        #self.inittablewidget.ColumnCount = selrange.columnCount() - 1
        self.inittablewidget.paintRect(self.main.register[0], self.main.register0Text.text())
        self.assertTrue(self.inittablewidget.paintRect(self.main.register[0], self.main.register0Text.text()))
        #self.inittablewidget.setItemInfo(self.main.register[0])
        #self.expectarray = "0.0.0.0."
        #self.assertEqual(self.inittablewidget.array[0][0], self.expectarray)
        print self.inittablewidget.array[0][0]

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
