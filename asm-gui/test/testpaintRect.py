#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QLineEdit
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

    def testpaintRect(self):
        selrange = QTableWidgetSelectionRange(0,1,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText("3")
        self.main.register1Text.setText("5")
        self.main.register2Text.setText("7")
        self.main.register3Text.setText("9")
        #self.inittablewidget.paintRect(self.main.register[0], self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectarray = "0,1,2,3,.0,1,2,3,.0,1,2,3,.0,1,2,3,."
        self.assertEqual(self.inittablewidget.array[0][1], self.expectarray)

    def testpaintRectfirst(self):
        row = random.randint(0, 1999)
        column = random.randint(0, 19)
        varint = random.randint(0,99)
        selrange = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        #self.inittablewidget.paintRect(self.main.register[0], self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectarray = "0,1,2,3,.0,1,2,3,.0,1,2,3,.0,1,2,3,."
        self.assertEqual(self.inittablewidget.array[row][column], self.expectarray)

    def testpaintRectsecoond(self):
        row = random.randint(0, 1999)
        column = random.randint(0, 19)
        varint = random.randint(0,99)
        varColumn = random.randint(1, 20 - column)
        selrange = QTableWidgetSelectionRange(row, column, row, column + varColumn)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        #self.inittablewidget.paintRect(self.main.register[0], self.main.register0Text.text())
        re0 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        re1 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        re2 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        re3 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectresult = 0
        self.assertEqual(re0, self.expectresult)
        self.assertEqual(re1, self.expectresult)
        self.assertEqual(re2, self.expectresult)
        self.assertEqual(re3, self.expectresult)

    def testpaintRectthird(self):
        row = random.randint(0, 1999)
        column = random.randint(0, 19)
        varint = random.randint(0,99)
        selrange = QTableWidgetSelectionRange(row, column, row, column )
        self.inittablewidget.setRangeSelected(selrange,False)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        #self.inittablewidget.paintRect(self.main.register[0], self.main.register0Text.text())
        re0 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        re1 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        re2 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        re3 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectresult = -1
        self.assertEqual(re0, self.expectresult)
        self.assertEqual(re1, self.expectresult)
        self.assertEqual(re2, self.expectresult)
        self.assertEqual(re3, self.expectresult)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
