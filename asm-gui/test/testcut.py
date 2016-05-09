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

    def testCut(self):
        #selRange = self.inittablewidget.selectedRange()
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selrange,False)
        selrange = QTableWidgetSelectionRange(1,0,1,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(1,0).text()
        self.text0 = self.inittablewidget.item(0,0).text()
        self.result = QString("r0.m[0]->m[0]")
        self.assertEqual(self.text, self.result)
        self.assertEqual(self.text0, str(""))

    def testCutfirst(self):
        #selRange = self.inittablewidget.selectedRange()
        self.main.newFile()
        rows = random.randint(0,1999)
        columns = 0
        selranges = QTableWidgetSelectionRange(rows, columns, rows , columns )
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        selranges = QTableWidgetSelectionRange(rows + 1, columns, rows + 1, columns )
        self.inittablewidget.setRangeSelected(selranges,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(rows + 1,columns).text()
        self.text0 = self.inittablewidget.item(rows,columns).text()
        self.result = QString("r0.m[0]->m[0]")
        self.assertEqual(self.text, self.result)
        self.assertEqual(self.text0, str(""))

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
