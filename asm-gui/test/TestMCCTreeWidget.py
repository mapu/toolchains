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
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.result = QString("r")
        self.mcctreewidget.searchMcc(row,column,self.result)
        print self.mcctreewidget.result

    def testMCCTreeWidget_0(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"t")
        print self.mcctreewidget.result
  
    def testMCCTreeWidget_1(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"s")
        print self.mcctreewidget.result

    def testMCCTreeWidget_2(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"b")
        print self.mcctreewidget.result

    def testMCCTreeWidget_3(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"m")
        print self.mcctreewidget.result

    def testMCCTreeWidget_4(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges,True)
        self.mcctreewidget.searchMcc(row,column,"a")
        print self.mcctreewidget.result

    def testMCCTreeWidget_5(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges,True)
        self.mcctreewidget.searchMcc(row,column,"n")
        print self.mcctreewidget.result

    def testMCCTreeWidget_6(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges,True)
        self.mcctreewidget.searchMcc(row,column,"e")
        print self.mcctreewidget.result
        
    def testMCCTreeWidget_7(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"c")
        print self.mcctreewidget.result
    
    def testMCCTreeWidget_8(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"d")
        print self.mcctreewidget.result 
     
    def testMCCTreeWidget_9(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"i")
        print self.mcctreewidget.result
        
    def testMCCTreeWidget_10(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"u")
        print self.mcctreewidget.result
        
    def testMCCTreeWidget_11(self):
        self.main.newFile()
        column = 0
        row = random.randint(0,2000)
        selranges = QTableWidgetSelectionRange(row,column,row,column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.mcctreewidget.searchMcc(row,column,"l")
        print self.mcctreewidget.result     
        
suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
