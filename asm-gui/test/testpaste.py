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
        self.inittablewidget.setSelectionMode(QAbstractItemView.ContiguousSelection)

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testPaste(self):
        self.main.newFile()
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.copy()
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(1,0,1,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(1,0).text()
        self.result = QString("r0.m[0]->m[0]")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result)

    def testPaste_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = 0
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row,column,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.copy()
        self.inittablewidget.setRangeSelected(selrange,False)
        selrange = QTableWidgetSelectionRange(row + 1,column,row + 2,column)
        self.inittablewidget.setRangeSelected(selrange, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(row + 1,column).text()
        self.result = QString("r0.m[0]->m[0]")
        self.assertEqual(self.text, self.result, 'test fail')

    def testPaste_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = 0
        #selRange = self.inittablewidget.selectedRange()
        selrange = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(row,column,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.copy()
        self.inittablewidget.setRangeSelected(selrange,False)
        selrange = QTableWidgetSelectionRange(row + 1,column,row + 1,column)
        self.inittablewidget.setRangeSelected(selrange, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(row + 1,column).text()
        self.result = QString("r0.m[0]->m[0]")
        self.assertEqual(self.text, self.result)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
