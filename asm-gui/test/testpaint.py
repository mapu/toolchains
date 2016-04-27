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
        #self.inittablewidget.setSelectionMode(QAbstractItemView.ContiguousSelection)

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testpaintRect(self):
        selrange = QTableWidgetSelectionRange(0,1,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        #self.main.register0Text.setText("3")
        self.inittablewidget.paintRect(self.main.registerCheck.index(self.main.registerCheck[0]), self.main.register0Text.text())
        self.inittablewidget.setRangeSelected(selrange,False)
        selranges = QTableWidgetSelectionRange(0,2,0,2)
        self.inittablewidget.setRangeSelected(selranges,False)
        selrange = QTableWidgetSelectionRange(0,1,0,1)
        self.inittablewidget.setRangeSelected(selrange,True)
        print self.main.register0Check.checkState()
        #self.assertEqual(self.main.register0Text.text(), "3")

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
