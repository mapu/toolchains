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
        selrange = QTableWidgetSelectionRange(0,0,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.deleteColumns()
        self.result = self.inittablewidget.item(0,0).text()
        self.expectresult = QString("world")
        for i in range(0,19):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i + 1))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        self.column = self.inittablewidget.columnCount()
        self.Columncount = 19
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)

    def testdeleteColumnssecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row ,column , row , column + 1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row, column + 1, QTableWidgetItem("world"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row, column ,row , column )
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.deleteColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(row, column).text()
        self.expectresult = QString("world")
        for i in xrange(0, column ):
            self.headeritem = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitem = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritem,self.expectheaderitem)
        for i in xrange(column , self.column):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i + 1))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 19
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
