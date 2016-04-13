#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView
from PyQt4.QtCore import Qt, QString,QStringList
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
        self.inittablewidget.setSelectionMode(QAbstractItemView.ContiguousSelection)

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testCut(self):
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(0,0,1,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("world"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selrange,False)
        selrange = QTableWidgetSelectionRange(0,1,1,2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(0,1).text()
        self.result = QString("world")
        self.assertEqual(self.text, self.result)

    def testCutfirst(self):
        #selRange = self.inittablewidget.selectedRange()
        rows = random.randint(0,1999)
        columns = random.randint(0,19)
        selranges = QTableWidgetSelectionRange(rows, columns, rows + 1, columns + 1)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("world"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selranges = QTableWidgetSelectionRange(row, column, row + 1, column + 1)
        self.inittablewidget.setRangeSelected(selranges,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(row,column).text()
        self.result = QString("world")
        self.assertEqual(self.text, self.result)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
