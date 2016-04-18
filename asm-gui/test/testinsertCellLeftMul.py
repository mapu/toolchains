#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange
from PyQt4.QtCore import Qt, QString
from PyQt4.QtTest import QTest
import unittest
import main
import sys
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

    def testinsertLeftCellfirst(self):
        selrange = QTableWidgetSelectionRange(0,0,1,3)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(0,1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(0,2,QTableWidgetItem("china"))
        self.inittablewidget.currentRowNum = selrange.rowCount() - 1
        self.inittablewidget.currentTopRow = selrange.topRow()
        self.inittablewidget.currentColumnNum = selrange.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selrange.leftColumn()
        self.inittablewidget.insertLeftCell()
        self.headeritem = self.inittablewidget.horizontalHeaderItem(20).text()
        self.expectheaderitem = QString("nonameFSM20")
        """self.headeritemfirst = self.inittablewidget.horizontalHeaderItem(21).text()
        self.expectheaderitemfirst = QString("nonameFSM21")
        self.headeritemsecond = self.inittablewidget.horizontalHeaderItem(22).text()
        self.expectheaderitemsecond = QString("nonameFSM22")"""
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
        self.expecttext = QString("hello")
        self.expecttextfirst = QString("world")
        self.expecttextsecond = QString("china")
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.resultfirst,self.expectresult)
        self.assertEqual(self.resultsecond,self.expectresult)
        self.assertEqual(self.headeritem,self.expectheaderitem)
        self.assertEqual(self.text,self.expecttext)
        self.assertEqual(self.textfirst,self.expecttextfirst)
        self.assertEqual(self.textsecond,self.expecttextsecond)
        #self.assertEqual(self.headeritemfirst,self.expectheaderitemfirst)
        #self.assertEqual(self.headeritemsecond,self.expectheaderitemsecond)
        #self.assertEqual(self.headeritems,self.expectheaderitems)

    def testinsertLeftCellsecond(self):
        rows = random.randint(0,1999)
        columns = random.randint(0,16)
        selranges = QTableWidgetSelectionRange(rows, columns, rows + 1, columns + 3)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("hello"))
        self.inittablewidget.setItem(rows,columns +1,QTableWidgetItem("world"))
        self.inittablewidget.setItem(rows,columns +2,QTableWidgetItem("china"))
        self.inittablewidget.currentRowNum = selranges.rowCount() - 1
        self.inittablewidget.currentTopRow = selranges.topRow()
        self.inittablewidget.currentColumnNum = selranges.columnCount() - 1
        self.inittablewidget.currentLeftColumn = selranges.leftColumn()
        self.inittablewidget.insertLeftCell()
        self.headeritem = self.inittablewidget.horizontalHeaderItem(20).text()
        self.expectheaderitem = QString("nonameFSM20")
        """self.headeritemfirst = self.inittablewidget.horizontalHeaderItem(21).text()
        self.expectheaderitemfirst = QString("nonameFSM21")
        self.headeritemsecond = self.inittablewidget.horizontalHeaderItem(22).text()
        self.expectheaderitemsecond = QString("nonameFSM22")"""
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
        self.expecttext = QString("hello")
        self.expecttextfirst = QString("world")
        self.expecttextsecond = QString("china")
        self.assertEqual(self.result,self.expectresult)
        self.assertEqual(self.resultfirst,self.expectresult)
        self.assertEqual(self.resultsecond,self.expectresult)
        self.assertEqual(self.headeritem,self.expectheaderitem)
        self.assertEqual(self.text,self.expecttext)
        self.assertEqual(self.textfirst,self.expecttextfirst)
        self.assertEqual(self.textsecond,self.expecttextsecond)
        #self.assertEqual(self.headeritemfirst,self.expectheaderitemfirst)
        #self.assertEqual(self.headeritemsecond,self.expectheaderitemsecond)
        #self.assertEqual(self.headeritems,self.expectheaderitems)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)

