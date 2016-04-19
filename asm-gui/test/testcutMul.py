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
        selranges = QTableWidgetSelectionRange(0, 0, 0, 2)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        selrange = QTableWidgetSelectionRange(1,0,1,2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(1,0).text()
        self.textfirst = self.inittablewidget.item(1,1).text()
        self.textsecond = self.inittablewidget.item(1,2).text()
        self.result = QString("hello")
        self.resultfirst = QString("world")
        self.resultsecond = QString("china")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result)
        self.assertEqual(self.textfirst, self.resultfirst)
        self.assertEqual(self.textsecond, self.resultsecond)

    def testCutfirst(self):
        #selRange = self.inittablewidget.selectedRange()
        selranges = QTableWidgetSelectionRange(0, 0, 0, 2)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        selrange = QTableWidgetSelectionRange(1,0,2,3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(1,0).text()
        self.textfirst = self.inittablewidget.item(1,1).text()
        self.textsecond = self.inittablewidget.item(1,2).text()
        self.result = QString("hello")
        self.resultfirst = QString("world")
        self.resultsecond = QString("china")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result,'test fail')
        self.assertEqual(self.textfirst, self.resultfirst,'test fail')
        self.assertEqual(self.textsecond, self.resultsecond,'test fail')

    def testCutsecond(self):
        selranges = QTableWidgetSelectionRange(0, 0, 0, 2)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        row = random.randint(0,1999)
        column = random.randint(0,16)
        selrange = QTableWidgetSelectionRange(row, column, row , column + 2)
        self.inittablewidget.setRangeSelected(selrange, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(row, column).text()
        self.textfirst = self.inittablewidget.item(row, column + 1).text()
        self.textsecond = self.inittablewidget.item(row, column + 2).text()
        self.result = QString("hello")
        self.resultfirst = QString("world")
        self.resultsecond = QString("china")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result)
        self.assertEqual(self.textfirst, self.resultfirst)
        self.assertEqual(self.textsecond, self.resultsecond)
       
    def testCutthird(self):
        #selRange = self.inittablewidget.selectedRange()
        rows = random.randint(0,1999)
        columns = random.randint(0,16)
        selranges = QTableWidgetSelectionRange(rows, columns, rows , columns + 2)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(rows,columns +1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(rows,columns +2,QTableWidgetItem("china"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        row = random.randint(0,1999)
        column = random.randint(0,16)
        selranges = QTableWidgetSelectionRange(row, column, row , column + 2)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(row, column).text()
        self.textfirst = self.inittablewidget.item(row, column + 1).text()
        self.textsecond = self.inittablewidget.item(row, column + 2).text()
        self.result = QString("hello")
        self.resultfirst = QString("world")
        self.resultsecond = QString("china")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result)
        self.assertEqual(self.textfirst, self.resultfirst)
        self.assertEqual(self.textsecond, self.resultsecond) 

    def testCutforth(self):
        #selRange = self.inittablewidget.selectedRange()
        rows = random.randint(0,1999)
        columns = random.randint(0,16)
        selranges = QTableWidgetSelectionRange(rows, columns, rows , columns + 2)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(rows,columns +1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(rows,columns +2,QTableWidgetItem("china"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        row = random.randint(0,1999)
        column = random.randint(0,16)
        selranges = QTableWidgetSelectionRange(row, column, row , column + 3)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(row, column).text()
        self.textfirst = self.inittablewidget.item(row, column + 1).text()
        self.textsecond = self.inittablewidget.item(row, column + 2).text()
        self.result = QString("hello")
        self.resultfirst = QString("world")
        self.resultsecond = QString("china")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result, 'test fail')
        self.assertEqual(self.textfirst, self.resultfirst ,'test fail')
        self.assertEqual(self.textsecond, self.resultsecond, ' test fail')           


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
