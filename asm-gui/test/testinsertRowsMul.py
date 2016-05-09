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

    def testInsertRows(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,1,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        #print self.inittablewidget.currentRowNum
        self.inittablewidget.insertRows()
        self.result = self.inittablewidget.item(2,0).text()
        self.results = self.inittablewidget.item(3,0).text()
        self.expectresult = QString("r0.m[0]->m[0]")
        self.expectresults = QString("r0.m[0]->m[1]")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2002
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults) 

    def testInsertRows_0(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,2,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(2,0,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(1,0,2,0)
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.insertRows()
        self.results = self.inittablewidget.item(0,0).text()
        self.result = self.inittablewidget.item(3,0).text()
        self.resultss = self.inittablewidget.item(4,0).text()
        self.expectresult = QString("r0.m[0]->m[1]")
        self.expectresults = QString("r0.m[0]->m[0]")
        self.expectresultss = QString("r0.m[0]->m[2]")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2002
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)

    def testInsertRows_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = 0
        selrange = QTableWidgetSelectionRange(row, column, row + 2, column )
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row, column, QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(row + 1, column, QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(row + 2, column, QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row + 1, column, row + 2, column )
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.insertRows()
        self.result = self.inittablewidget.item(row, column).text()
        self.results = self.inittablewidget.item(row + 3, column).text()
        self.resultss = self.inittablewidget.item(row + 4, column).text()
        self.expectresult = QString("r0.m[0]->m[0]")
        self.expectresults = QString("r0.m[0]->m[1]")
        self.expectresultss = QString("r0.m[0]->m[2]")
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2002
        self.assertEqual(self.row, self.Rowcount)
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.results,self.expectresults)
        self.assertEqual(self.resultss,self.expectresultss)

    def testInsertRows_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = 0
        varRow = random.randint(1,2000)
        varColumn = 0
        selrange = QTableWidgetSelectionRange(row, column, row + varRow - 1, column + varColumn)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.insertRows()
        self.row = self.inittablewidget.rowCount()
        self.Rowcount = 2000 + varRow
        self.assertEqual(self.row, self.Rowcount)


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
