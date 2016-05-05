# -*- coding: utf-8 -*-
'''
Created on May 04, 2016

@author: litt 
'''

from PyQt4.QtGui import QWidget, QTableWidget, QTableWidgetItem, QApplication, QTableWidgetSelectionRange, QAbstractItemView, QBrush, QColor
from PyQt4.QtCore import Qt, QString, QStringList
from PyQt4.QtTest import QTest
import unittest
import sys 
sys.path.append("..")
import main
import random
from view.MainWindow import MainWindow
from view.Utils import initParent

class TestOutput(unittest.TestCase): 
    def setUp(self):
        '''
        test FMAC code
        '''
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def test(self):
        self.main.newFile()
        textList = ["r0.m[0]->m[0]", "t0+t0(t)->biu0", "shu0.t2 ind tbh(tb=+2)->ialu.t1(i0)", "double t0(t)->biu0"]
        for k in xrange(2):
	    row  = 0
	    column = 0
            for i in xrange(4):
                self.inittablewidget.setItem(row, column, QTableWidgetItem(textList[0]))
                self.inittablewidget.setItem(row + 1, column, QTableWidgetItem(textList[1]))
                self.inittablewidget.setItem(row + 2, column, QTableWidgetItem(textList[2]))
                self.inittablewidget.setItem(row + 3, column, QTableWidgetItem(textList[3]))
		selranges = QTableWidgetSelectionRange(row, column, row + 3 , column)
		self.inittablewidget.setRangeSelected(selranges, True)	
		self.inittablewidget.paintRect(i, str(i))
		self.inittablewidget.setRangeSelected(selranges, False)
		row = row + 5
  
	    for i in xrange(4):
		self.inittablewidget.setItem(row, column, QTableWidgetItem(textList[0]))
		self.inittablewidget.setItem(row + 1, column, QTableWidgetItem(textList[1]))
		self.inittablewidget.setItem(row + 2, column, QTableWidgetItem(textList[2]))
		self.inittablewidget.setItem(row + 3, column, QTableWidgetItem(textList[3]))
		selranges = QTableWidgetSelectionRange(row, column, row + 3 , column)
		self.inittablewidget.setRangeSelected(selranges, True)
		for j in xrange(i + 1):
		    self.inittablewidget.paintRect(j, str(j))
		self.inittablewidget.setRangeSelected(selranges, False)
		row = row + 5 + i

	    for i in xrange(4):
		self.inittablewidget.setItem(row, column, QTableWidgetItem(textList[0]))
		self.inittablewidget.setItem(row + 1, column, QTableWidgetItem(textList[1]))
		self.inittablewidget.setItem(row + 2, column, QTableWidgetItem(textList[2]))
		self.inittablewidget.setItem(row + 3, column, QTableWidgetItem(textList[3]))
		selranges = QTableWidgetSelectionRange(row, column, row + 3 , column)
		self.inittablewidget.setRangeSelected(selranges, True)
		for j in xrange(4 - i):
		    self.inittablewidget.paintRect(j, str(j))
		self.inittablewidget.setRangeSelected(selranges, False)
		row = row + 5 + i + 1
     
	    selranges = QTableWidgetSelectionRange(0, 0, 0, 0)
	    self.inittablewidget.setRangeSelected(selranges, True)
	    self.inittablewidget.currentColumnNum = selranges.columnCount() 
	    self.inittablewidget.currentLeftColumn = selranges.leftColumn()
	    self.inittablewidget.insertColumns()
	    self.inittablewidget.setRangeSelected(selranges,False)

        
        self.inittablewidget.saveFile("/home/litt/toolchains/trunk/asm-gui/test/test.mpu.s")


suite = unittest.TestLoader().loadTestsFromTestCase(TestOutput)
unittest.TextTestRunner(verbosity=2).run(suite)
