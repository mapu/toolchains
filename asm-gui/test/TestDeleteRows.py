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

    def testDeleteRows(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,1,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.deleteRows()
        self.results = self.inittablewidget.item(0,0).text()
        self.expectresults = QString("r0.m[0]->m[1]")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 1999
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.results,self.expectresults)

    def testDeleteRows_0(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        selrange = QTableWidgetSelectionRange(row, column, row + 1, column )
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(row + 1, column, QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row , column, row , column )
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.deleteRows()
        self.result = self.inittablewidget.item(row, column).text()
        self.expectresult = QString("r0.m[0]->m[1]")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 1999
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
