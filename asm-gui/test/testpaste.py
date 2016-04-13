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

    def testPaste(self):
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(0,0,1,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.copy()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(2,2,3,3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(2,2).text()
        self.result = QString("hello")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result)

    def testPastefirst(self):
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(0,0,1,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("world"))
        self.inittablewidget.copy()
        self.inittablewidget.setRangeSelected(selrange,False)
        selrange = QTableWidgetSelectionRange(0,1,1,2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(0,1).text()
        self.result = QString("world")
        self.assertEqual(self.text, self.result)

    def testPastesecond(self):
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(0,0,1,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.copy()
        self.inittablewidget.setRangeSelected(selrange,False)
        selrange = QTableWidgetSelectionRange(0,0,2,2)
        self.inittablewidget.setRangeSelected(selrange, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(0,1).text()
        self.result = QString("hello")
        self.assertEqual(self.text, self.result, 'test fail')

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
