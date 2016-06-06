#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QTreeWidget, QTreeWidgetItem
from PyQt4.QtCore import Qt, QString,QStringList
from PyQt4.QtTest import QTest
import unittest
import sys
sys.path.append("..")
import main
import random
from view.MainWindow import MainWindow


class mytest(unittest.TestCase): 
    def setUp(self):
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        self.mcctreewidget = self.main.mccTreeWidget
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None
        self.mcctreewidget = None

    def testMCCTreeWidget(self):
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(0, 0, 0, 0)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.result = QString("r0")
        self.mcctreewidget.searchMcc(0,0,self.result)
        print self.result, self.mcctreewidget.result


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
