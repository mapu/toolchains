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
        selrange = QTableWidgetSelectionRange(0,0,3,0)
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
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[0]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[1]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[2]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[3]))
        self.expectarray0 = "...."
        self.assertEqual(self.inittablewidget.array[0][0], self.expectarray0)
        self.assertEqual(self.inittablewidget.array[1][0], self.expectarray0)
        self.assertEqual(self.inittablewidget.array[2][0], self.expectarray0)
        self.assertEqual(self.inittablewidget.array[3][0], self.expectarray0)

    def testpaintRectfirst(self):
        row = random.randint(0, 1999)
        column = random.randint(0, 19)
        varint = random.randint(0,99)
        varRow = random.randint(1, 2000 - row)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column)
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
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[0]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[1]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[2]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[3]))
        self.expectarray0 = "...."
        for i in xrange(row + 1, row + varRow):
            self.expectarray1 = "...."
            self.assertEqual(self.inittablewidget.array[i][column], self.expectarray1)
        self.assertEqual(self.inittablewidget.array[row][column], self.expectarray0)
        self.assertEqual(self.inittablewidget.array[row + varRow][column], self.expectarray0)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
