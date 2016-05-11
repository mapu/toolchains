#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QLineEdit
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

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testEraserRect(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,1,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText("3")
        self.main.register1Text.setText("5")
        self.main.register2Text.setText("7")
        self.main.register3Text.setText("9")
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[0]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[1]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[2]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[3]))
        self.expectarray = "...."
        self.assertEqual(self.inittablewidget.array[0][1], self.expectarray)

    def testEraserRect_0(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0, 1999)
        column = random.randint(0, 19)
        varint = random.randint(0,99)
        selrange = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[0]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[1]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[2]))
        self.inittablewidget.eraserRect(self.main.registerCheck.index(self.main.registerCheck[3]))
        self.expectarray = "...."
        self.assertEqual(self.inittablewidget.array[row][column], self.expectarray)


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
