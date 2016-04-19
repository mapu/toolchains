#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView
from PyQt4.QtCore import Qt, QString,QStringList
from PyQt4.QtTest import QTest
import unittest
import main
import sys
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

    def testCopy(self):
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(0,0,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.copy()
        self.originalText = QApplication.clipboard().text()
        #print self.originalText
        self.result = QString("hello\tworld")
        self.expectresult = QString("HELLO")
        self.assertTrue(self.inittablewidget.copy)
        self.assertEqual(self.originalText,self.result)
        #self.assertEqual(self.originalText,self.expectresult,"test copy fail")

    def testcopy(self):
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.copy()
        self.inittablewidget.setRangeSelected(selrange,False)
        self.txt = QApplication.clipboard().text()
        selrange = QTableWidgetSelectionRange(0,1,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.copy()
        self.originalText = QApplication.clipboard().text()
        self.result = QString("world")
        self.assertEqual(self.originalText,self.result)
        self.assertTrue(self.inittablewidget.copy)
        

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
