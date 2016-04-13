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

    def testDelete(self):
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        selrange = QTableWidgetSelectionRange(0,0,1,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.delete() 
        stringss = self.inittablewidget.item(0,0).text()
        self.item = QString("")
        self.assertTrue(self.inittablewidget.delete)
        self.assertEqual(stringss, self.item)
  
    def testdeletefirst(self):
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        selrange = QTableWidgetSelectionRange(0,0,1,3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.delete()
        string = self.inittablewidget.item(0,0).text()
        strings = self.inittablewidget.item(0,1).text()
        stringss = self.inittablewidget.item(0,2).text()
        self.item = QString("")
        self.assertEqual(string, self.item)
        self.assertEqual(strings, self.item)
        self.assertEqual(stringss, self.item) 

    def testdeletesecond(self): 
        row = random.randint(0,1999)
        column = random.randint(0,19)      
        self.inittablewidget.setItem(row, column, QTableWidgetItem("hello"))
        self.inittablewidget.setItem(row, column + 1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(row, column + 2,QTableWidgetItem("china"))
        selrange = QTableWidgetSelectionRange(row, column, row + 1, column + 3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.delete()
        string = self.inittablewidget.item(row, column).text()
        strings = self.inittablewidget.item(row, column + 1).text()
        stringss = self.inittablewidget.item(row, column + 2).text()
        self.item = QString("")
        self.assertEqual(string, self.item)
        self.assertEqual(strings, self.item)
        self.assertEqual(stringss, self.item) 

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
