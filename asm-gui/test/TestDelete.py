#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView
from PyQt4.QtCore import Qt, QString,QStringList
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

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testDelete(self):
        self.main.newFile()
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.delete() 
        stringss = self.inittablewidget.item(0,0).text()
        self.item = QString("")
        self.assertTrue(self.inittablewidget.delete)
        self.assertEqual(stringss, self.item)

    def testDelete_1(self):
        self.main.newFile() 
        row = random.randint(0,1999)
        column = 0      
        self.inittablewidget.setItem(row, column, QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(row + 2, column,QTableWidgetItem("r0.m[0]->m[2]"))
        selrange = QTableWidgetSelectionRange(row, column, row + 2, column)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.delete()
        string = self.inittablewidget.item(row, column).text()
        strings = self.inittablewidget.item(row + 1, column).text()
        stringss = self.inittablewidget.item(row + 2, column).text()
        self.item = QString("")
        self.assertEqual(string, self.item)
        self.assertEqual(strings, self.item)
        self.assertEqual(stringss, self.item) 

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
