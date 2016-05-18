#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView
from PyQt4.QtCore import Qt, QString,QStringList,QPoint,QObject
from PyQt4.QtTest import QTest
import unittest
import sys
sys.path.append("..")
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
        for i in range(0,7):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,7)
        self.inittablewidget.setRangeSelected(selrange, False)
        row = random.randint(0,20)
        column = random.randint(0,7)
        
        for i in xrange(4):
            self.inittablewidget.setCurrentCell(row,column)
            QTest.keyClicks(self.inittablewidget,'R0.M[0]->M[0]@(C)')
            self.inittablewidget.setCurrentCell(row + 1,column)
            QTest.keyClicks(self.inittablewidget,'shu0.t0 ind tsq->biu0')
            self.inittablewidget.setCurrentCell(row + 2,column)
            QTest.keyClicks(self.inittablewidget,'biu0.dm(A++,K++)->IALU.t0(i0)')
            self.inittablewidget.setCurrentCell(row + 3,column)
            QTest.keyClicks(self.inittablewidget,'t0 + t1(u,b)->shu0.t0@(!c)')
            selranges = QTableWidgetSelectionRange(row, column, row + 3 , column)
            self.inittablewidget.setRangeSelected(selranges, True)
            QTest.mouseClick(self.main.registerText[i], Qt.LeftButton)
            QTest.keyClick(self.main.registerText[i],Qt.Key_Backspace)
            QTest.keyClicks(self.main.registerText[i], str(i))
            QTest.mouseClick(self.main.registerCheck[i], Qt.LeftButton)
            self.inittablewidget.setRangeSelected(selranges, False)
        
        for i in xrange(4):
            m = random.randint(0,10)
            row = row + 4 + m
            self.inittablewidget.setCurrentCell(row,column)
            QTest.keyClicks(self.inittablewidget,'merge(t0,t0,t0)->M[0]@(C)')
            self.inittablewidget.setCurrentCell(row + 1,column)
            QTest.keyClicks(self.inittablewidget,'t0|t1->shu0.t0')
            self.inittablewidget.setCurrentCell(row + 2,column)
            QTest.keyClicks(self.inittablewidget,'max(t0,t3)(u,b)->falu.t2')
            self.inittablewidget.setCurrentCell(row + 3,column)
            QTest.keyClicks(self.inittablewidget,'mdivr->divr@(c)')
            selranges = QTableWidgetSelectionRange(row, column, row + 3 , column)
            self.inittablewidget.setRangeSelected(selranges, True)
            for j in xrange(i + 1):
                QTest.mouseClick(self.main.registerText[j], Qt.LeftButton)
                QTest.keyClick(self.main.registerText[j],Qt.Key_Backspace)
                QTest.keyClicks(self.main.registerText[j], str(j + 4))
                QTest.mouseClick(self.main.registerCheck[j], Qt.LeftButton)
            self.inittablewidget.setRangeSelected(selranges, False)

        for i in xrange(4):
            if column < 7:
                rightcolumn = 7 - column
                var = random.randint(1, rightcolumn)
                #print rightcolumn,var
                self.inittablewidget.setCurrentCell(row,column + var)
                QTest.keyClicks(self.inittablewidget,'R0.M[0]->M[0]@(C)')
                self.inittablewidget.setCurrentCell(row + 1,column + var)
                QTest.keyClicks(self.inittablewidget,'shu0.t0 ind tsq->biu0')
                self.inittablewidget.setCurrentCell(row + 2,column + var)
                QTest.keyClicks(self.inittablewidget,'biu0.dm(A++,K++)->IALU.t0(i0)')
                self.inittablewidget.setCurrentCell(row + 3,column + var)
                QTest.keyClicks(self.inittablewidget,'t0 + t1(u,b)->shu0.t0@(!c)')
                selranges = QTableWidgetSelectionRange(row, column + var, row + 3 , column + var)
                self.inittablewidget.setRangeSelected(selranges, True)
                QTest.mouseClick(self.main.registerText[i], Qt.LeftButton)
                QTest.keyClick(self.main.registerText[i],Qt.Key_Backspace)
                QTest.keyClicks(self.main.registerText[i], str(i))
                QTest.mouseClick(self.main.registerCheck[i], Qt.LeftButton)
                self.inittablewidget.setRangeSelected(selranges, False)
            else:
                break

        for i in xrange(4):
            m = random.randint(0,5)
            row = row + 4 + m
            if column < 7:
                rightcolumn = 7 - column
                var = random.randint(1, rightcolumn)
                #print rightcolumn,var
                self.inittablewidget.setCurrentCell(row,column + var)
                QTest.keyClicks(self.inittablewidget,'merge(t0,t0,t0)->M[0]@(C)')
                self.inittablewidget.setCurrentCell(row + 1,column + var)
                QTest.keyClicks(self.inittablewidget,'t0|t1->shu0.t0')
                self.inittablewidget.setCurrentCell(row + 2,column + var)
                QTest.keyClicks(self.inittablewidget,'max(t0,t3)(u,b)->falu.t2')
                self.inittablewidget.setCurrentCell(row + 3,column + var)
                QTest.keyClicks(self.inittablewidget,'mdivr->divr@(c)')
                selranges = QTableWidgetSelectionRange(row, column + var, row + 3 , column + var)
                self.inittablewidget.setRangeSelected(selranges, True)
                for j in xrange(i + 1):
                    QTest.mouseClick(self.main.registerText[j], Qt.LeftButton)
                    QTest.keyClick(self.main.registerText[j],Qt.Key_Backspace)
                    QTest.keyClicks(self.main.registerText[j], str(j + 4))
                    QTest.mouseClick(self.main.registerCheck[j], Qt.LeftButton)
                self.inittablewidget.setRangeSelected(selranges, False)
            else:
                break

        for i in xrange(4):
            if column >= 1:
                leftvar = random.randint(1,column)
                self.inittablewidget.setCurrentCell(row,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'R0.M[0]->M[0]@(C)')
                self.inittablewidget.setCurrentCell(row + 1,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'shu0.t0 ind tsq->biu0')
                self.inittablewidget.setCurrentCell(row + 2,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'biu0.dm(A++,K++)->IALU.t0(i0)')
                self.inittablewidget.setCurrentCell(row + 3,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'t0 + t1(u,b)->shu0.t0@(!c)')
                selranges = QTableWidgetSelectionRange(row, column - leftvar, row + 3 , column - leftvar)
                self.inittablewidget.setRangeSelected(selranges, True)
                QTest.mouseClick(self.main.registerText[i], Qt.LeftButton)
                QTest.keyClick(self.main.registerText[i],Qt.Key_Backspace)
                QTest.keyClicks(self.main.registerText[i], str(i))
                QTest.mouseClick(self.main.registerCheck[i], Qt.LeftButton)
                self.inittablewidget.setRangeSelected(selranges, False)
            else:
                break

        for i in xrange(4):
            m = random.randint(0,5)
            row = row + 4 + m
            if column >= 1:
                leftvar = random.randint(1,column)
                self.inittablewidget.setCurrentCell(row,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'merge(t0,t0,t0)->M[0]@(C)')
                self.inittablewidget.setCurrentCell(row + 1,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'t0|t1->shu0.t0')
                self.inittablewidget.setCurrentCell(row + 2,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'max(t0,t3)(u,b)->falu.t2')
                self.inittablewidget.setCurrentCell(row + 3,column - leftvar)
                QTest.keyClicks(self.inittablewidget,'mdivr->divr@(c)')
                selranges = QTableWidgetSelectionRange(row, column - leftvar, row + 3 , column - leftvar)
                self.inittablewidget.setRangeSelected(selranges, True)
                for j in xrange(i + 1):
                    QTest.mouseClick(self.main.registerText[j], Qt.LeftButton)
                    QTest.keyClick(self.main.registerText[j],Qt.Key_Backspace)
                    QTest.keyClicks(self.main.registerText[j], str(j + 4))
                    QTest.mouseClick(self.main.registerCheck[j], Qt.LeftButton)
                self.inittablewidget.setRangeSelected(selranges, False)
            else:
                break
            
        self.inittablewidget.saveFile("test.gui.s")

	self.main.show()
	app.exec_()
	
suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
