#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView,QIcon,QCursor
from PyQt4.QtCore import Qt, QString,QStringList,QPoint,QObject
from PyQt4.QtTest import QTest
import unittest
import sys
sys.path.append("..")
#import main
import random
from view.MainWindow import MainWindow
from view.Utils import initParent

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
        for i in range(0,3):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,3)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = 0
        column = 0
    
        for i in xrange(4):
            row = row + 4
            self.inittablewidget.setCurrentCell(row,column)
            QTest.keyClicks(self.inittablewidget,'merge(t%s,t0,t0)->M[0]@(C)'% i)
            self.inittablewidget.floatDialog.close()
            self.inittablewidget.setCurrentCell(row + 1,column)
            QTest.keyClicks(self.inittablewidget,'t%s|t1->shu0.t0'% i)
            self.inittablewidget.floatDialog.close()
            self.inittablewidget.setCurrentCell(row + 2,column)
            QTest.keyClicks(self.inittablewidget,'max(t%s,t3)(u,b)->falu.t2'% i)
            self.inittablewidget.floatDialog.close()
            self.inittablewidget.setCurrentCell(row + 3,column)
            QTest.keyClicks(self.inittablewidget,'max(t%s,t3)(u,b)->ialu.t2'% i)
            self.inittablewidget.floatDialog.close()
            selranges = QTableWidgetSelectionRange(row, column, row + 3 , column)
            self.inittablewidget.setRangeSelected(selranges, True)
            for j in xrange(i + 1):
                QTest.mouseClick(self.main.registerText[j], Qt.LeftButton)
                QTest.keyClick(self.main.registerText[j],Qt.Key_Backspace)
                QTest.keyClicks(self.main.registerText[j], str(j + 4))
                QTest.mouseClick(self.main.registerCheck[j], Qt.LeftButton)
            self.inittablewidget.setRangeSelected(selranges, False)

        self.inittablewidget.saveFile("test.pp.s")

	self.main.show()
	app.exec_()
	
suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
