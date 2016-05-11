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

    def testPaintRect(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,3,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText("3")
        self.main.register1Text.setText("5")
        self.main.register2Text.setText("7")
        self.main.register3Text.setText("9")
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectarray0 = "0,1,2,3,.0,1,2,3,..0,1,2,3,."
        self.expectarray1 = ".0,1,2,3,..0,1,2,3,."
        self.expectarray2 = ".0,1,2,3,..0,1,2,3,."
        self.expectarray3 = ".0,1,2,3,.0,1,2,3,.0,1,2,3,."
        self.assertEqual(self.inittablewidget.array[0][0], self.expectarray0)
        self.assertEqual(self.inittablewidget.array[1][0], self.expectarray1)
        self.assertEqual(self.inittablewidget.array[2][0], self.expectarray2)
        self.assertEqual(self.inittablewidget.array[3][0], self.expectarray3)

    def testPaintRect_0(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varint = random.randint(0,99)
        varRow = random.randint(1,2000 - row)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectarray0 = "0,1,2,3,.0,1,2,3,..0,1,2,3,."
        for i in xrange(row + 1, row + varRow):
            self.expectarray1 = ".0,1,2,3,..0,1,2,3,."
            self.assertEqual(self.inittablewidget.array[i][column], self.expectarray1)
        self.expectarray3 = ".0,1,2,3,.0,1,2,3,.0,1,2,3,."
        self.assertEqual(self.inittablewidget.array[row][column], self.expectarray0)
        self.assertEqual(self.inittablewidget.array[row + varRow][column], self.expectarray3)

    def testPaintRect_1(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varint = random.randint(0,99)
        varRow = random.randint(1,2000 - row)
        varColumn = random.randint(1,20 - column)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column + varColumn)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        re0 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        re1 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        re2 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        re3 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectresult = 0
        self.assertEqual(re0, self.expectresult)
        self.assertEqual(re1, self.expectresult)
        self.assertEqual(re2, self.expectresult)
        self.assertEqual(re3, self.expectresult)

    def testPaintRect_2(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varint = random.randint(0,99)
        varRow = random.randint(1,2000 - row)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column )
        self.inittablewidget.setRangeSelected(selrange,False)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        re0 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        re1 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        re2 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        re3 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectresult = -1
        self.assertEqual(re0, self.expectresult)
        self.assertEqual(re1, self.expectresult)
        self.assertEqual(re2, self.expectresult)
        self.assertEqual(re3, self.expectresult)

    def testPaintRect_3(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,1999)
        column = random.randint(0,19)
        varint = random.randint(0,99)
        varRow = random.randint(1,2000 - row)
        selrange = QTableWidgetSelectionRange(row, column, row + varRow, column )
        self.inittablewidget.setRangeSelected(selrange, True)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(row + 1 , column, row + varRow + 3, column)
        self.inittablewidget.setRangeSelected(selrange, True)
        self.main.register0Text.setText(str(varint))
        self.main.register1Text.setText(str(varint))
        self.main.register2Text.setText(str(varint))
        self.main.register3Text.setText(str(varint))
        re0 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        re1 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[1]), self.main.register1Text.text())
        re2 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[2]), self.main.register1Text.text())
        re3 = self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[3]), self.main.register1Text.text())
        self.expectresult = -2
        self.assertEqual(re0, self.expectresult)
        self.assertEqual(re1, self.expectresult)
        self.assertEqual(re2, self.expectresult)
        self.assertEqual(re3, self.expectresult)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
