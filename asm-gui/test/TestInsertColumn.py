#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange
from PyQt4.QtCore import Qt, QString
from PyQt4.QtTest import QTest
import unittest
import sys
sys.path.append("..")
import main
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

    def testInsertColumns(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.result = self.inittablewidget.item(0,1).text()
        self.expectresult = QString("r0.m[0]->m[0]")
        for i in range(0,2):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        self.column = self.inittablewidget.columnCount()
        self.Columncount = 2
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)

    def testInsertColumns_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = 0
        selrange = QTableWidgetSelectionRange(row ,column , row , column)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertColumns()
        self.column = self.inittablewidget.columnCount()
        self.result = self.inittablewidget.item(row, column + 1).text()
        self.expectresult = QString("hello")
        for i in xrange(column, self.column):
            self.headeritemss = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritemss,self.expectheaderitems)
        #self.column = self.inittablewidget.columnCount()
        self.Columncount = 2
        self.assertEqual(self.column, self.Columncount)
        self.assertEqual(self.result,self.expectresult)
        
        
suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
