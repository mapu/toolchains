# -*- coding: utf-8 -*-
'''
Created on May 17, 2016

@author: litt 
'''

from PyQt4.QtGui import QWidget, QTableWidget, QTableWidgetItem, QApplication, QTableWidgetSelectionRange, QAbstractItemView, QBrush, QColor, QApplication
from PyQt4.QtCore import Qt, QString, QStringList
from PyQt4.QtTest import QTest
import unittest
import sys 
sys.path.append("..")
import random
from view.MainWindow import MainWindow
from view.Utils import initParent


class TestOutput(unittest.TestCase): 
    def setUp(self):

        self.mainWindow = None
        self.inittablewidget = None
        
    def tearDown(self):
        self.mainWindow = None
        self.inittablewidget = None

    def test(self):
        app = QApplication(sys.argv)
        self.mainWindow = MainWindow()
        self.inittablewidget = self.mainWindow.microcodeTableWidget
	self.inittablewidget.initTable()
	self.inittablewidget.openFile("test.asm.s")
         
        self.mainWindow.show()
        app.exec_()

suite = unittest.TestLoader().loadTestsFromTestCase(TestOutput)
unittest.TextTestRunner(verbosity=2).run(suite)
