#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange
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

    def testInsertLeftCell(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        selrange = QTableWidgetSelectionRange(0,0,0,2)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.currentRowNum = selrange.rowCount() 
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertLeftCell()
        for i in range(0,21):
            self.headeritems = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritems,self.expectheaderitems)
        self.result = self.inittablewidget.item(0,0).text()
        self.resultfirst = self.inittablewidget.item(0,1).text()
        self.resultsecond = self.inittablewidget.item(0,2).text()
        self.text = self.inittablewidget.item(0,3).text()
        self.textfirst = self.inittablewidget.item(0,4).text()
        self.textsecond = self.inittablewidget.item(0,5).text()
        self.expectresult = QString("")
        self.expecttext = QString("r0.m[0]->m[0]")
        self.expecttextfirst = QString("r0.m[0]->m[1]")
        self.expecttextsecond = QString("r0.m[0]->m[2]")
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.resultfirst,self.expectresult)
        self.assertEqual(self.resultsecond,self.expectresult)
        self.assertEqual(self.text,self.expecttext)
        self.assertEqual(self.textfirst,self.expecttextfirst)
        self.assertEqual(self.textsecond,self.expecttextsecond)
        
    def testInsertLeftCell_0(self):
        self.main.newFile()
        selrange = QTableWidgetSelectionRange(0,0,0,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.currentColumnNum = selrange.columnCount() 
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        for i in range(0,19):
            self.inittablewidget.insertColumns()
        selrange = QTableWidgetSelectionRange(0,0,0,19)
        self.inittablewidget.setRangeSelected(selrange, False)
        rows = random.randint(0,1999)
        columns = random.randint(0,16)
        selranges = QTableWidgetSelectionRange(rows, columns, rows , columns + 2)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(rows,columns + 1,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(rows,columns + 2,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.currentRowNum = selranges.rowCount() 
        self.inittablewidget.currentTopRow = selranges.topRow()
        self.inittablewidget.currentColumnNum = selranges.columnCount() 
        self.inittablewidget.currentLeftColumn = selranges.leftColumn()
        self.inittablewidget.insertLeftCell()
        for i in range(0,21):
            self.headeritems = self.inittablewidget.horizontalHeaderItem(i).text()
            self.expectheaderitems = QString("nonameFSM" + str(i))
            self.assertEqual(self.headeritems,self.expectheaderitems)
        self.result = self.inittablewidget.item(rows, columns).text()
        self.resultfirst = self.inittablewidget.item(rows, columns + 1).text()
        self.resultsecond = self.inittablewidget.item(rows, columns + 2).text()
        self.text = self.inittablewidget.item(rows, columns + 3).text()
        self.textfirst = self.inittablewidget.item(rows, columns + 4).text()
        self.textsecond = self.inittablewidget.item(rows, columns + 5).text()
        self.expectresult = QString("")
        self.expecttext = QString("r0.m[0]->m[0]")
        self.expecttextfirst = QString("r0.m[0]->m[1]")
        self.expecttextsecond = QString("r0.m[0]->m[2]")
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.resultfirst,self.expectresult)
        self.assertEqual(self.resultsecond,self.expectresult)
        self.assertEqual(self.text,self.expecttext)
        self.assertEqual(self.textfirst,self.expecttextfirst)
        self.assertEqual(self.textsecond,self.expecttextsecond)
        
suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)

