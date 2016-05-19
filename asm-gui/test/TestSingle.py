#!/usr/bin/env python
#-*-coding: utf-8 -*-

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView
from PyQt4.QtCore import Qt, QString,QStringList,QPoint,QObject
from PyQt4.QtTest import QTest
import unittest
import sys
sys.path.append("..")
import random
from view.MainWindow import MainWindow

class mytest(unittest.TestCase):
    def setUp(self):
        self.main = None
        self.inittablewidget = None

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testSoftware(self):
        app = QApplication(sys.argv)
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,7):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,7)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,20)
        column = random.randint(0,7)
        m = random.randint(0,3)
        self.inittablewidget.setCurrentCell(row,column)
        QTest.keyClicks(self.inittablewidget,'R0.M[0]->M[0]@(C)')
        text = QString("R0.M[0]->M[0]@(C)")
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        QTest.mouseClick(self.main.registerText[m], Qt.LeftButton)
        QTest.keyClick(self.main.registerText[m],Qt.Key_Backspace)
        QTest.keyClicks(self.main.registerText[m], str(m))
        QTest.mouseClick(self.main.registerCheck[m], Qt.LeftButton)
        self.inittablewidget.setRangeSelected(selranges, False)

        self.inittablewidget.setCurrentCell(row + 1,column)
        QTest.keyClicks(self.inittablewidget,'R0.M[0]->M[0]@(C)')
        text = QString("R0.M[0]->M[0]@(C)")
        selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        QTest.mouseClick(self.main.registerText[m], Qt.LeftButton)
        QTest.keyClick(self.main.registerText[m],Qt.Key_Backspace)
        QTest.keyClicks(self.main.registerText[m], str(m))
        QTest.mouseClick(self.main.registerCheck[m], Qt.LeftButton)
        
        filename = "%s || Repeat @ (%s - %d).s"%(text,self.main.register[m],m)
        self.inittablewidget.saveFile(filename)
  
	self.main.show()
	app.exec_()

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
