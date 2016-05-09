#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView
from PyQt4.QtCore import Qt, QString,QStringList
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
        self.inittablewidget.setSelectionMode(QAbstractItemView.ContiguousSelection)
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testCut(self):
        #selRange = self.inittablewidget.selectedRange()
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(0, 0, 2, 0)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(2,0,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        selrange = QTableWidgetSelectionRange(3,0,5,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(3,0).text()
        self.textfirst = self.inittablewidget.item(4,0).text()
        self.textsecond = self.inittablewidget.item(5,0).text()
        self.result = QString("r0.m[0]->m[0]")
        self.resultfirst = QString("r0.m[0]->m[1]")
        self.resultsecond = QString("r0.m[0]->m[2]")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result)
        self.assertEqual(self.textfirst, self.resultfirst)
        self.assertEqual(self.textsecond, self.resultsecond)

    def testCut_0(self):
        #selRange = self.inittablewidget.selectedRange()
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(0, 0, 2, 0)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(1,0,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(2,0,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        selrange = QTableWidgetSelectionRange(3,0,6,0)
        self.inittablewidget.setRangeSelected(selrange,True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(3,0).text()
        self.textfirst = self.inittablewidget.item(4,0).text()
        self.textsecond = self.inittablewidget.item(5,0).text()
        self.result = QString("r0.m[0]->m[0]")
        self.resultfirst = QString("r0.m[0]->m[1]")
        self.resultsecond = QString("r0.m[0]->m[2]")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result,'test fail')
        self.assertEqual(self.textfirst, self.resultfirst,'test fail')
        self.assertEqual(self.textsecond, self.resultsecond,'test fail')
       
    def testCut_1(self):
        #selRange = self.inittablewidget.selectedRange()
        self.main.newFile()
        rows = random.randint(0,1999)
        columns = 0
        selranges = QTableWidgetSelectionRange(rows, columns, rows + 2, columns)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(rows + 1,columns,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(rows + 2,columns,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        selranges = QTableWidgetSelectionRange(rows + 3, columns, rows + 5, columns)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(rows + 3, columns).text()
        self.textfirst = self.inittablewidget.item(rows + 4, columns).text()
        self.textsecond = self.inittablewidget.item(rows + 5, columns).text()
        self.result = QString("r0.m[0]->m[0]")
        self.resultfirst = QString("r0.m[0]->m[1]")
        self.resultsecond = QString("r0.m[0]->m[2]")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result)
        self.assertEqual(self.textfirst, self.resultfirst)
        self.assertEqual(self.textsecond, self.resultsecond) 

    def testCut_2(self):
        #selRange = self.inittablewidget.selectedRange()
        self.main.newFile()
        rows = random.randint(0,1999)
        columns = random.randint(0,16)
        selranges = QTableWidgetSelectionRange(rows, columns, rows + 2, columns)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(rows,columns,QTableWidgetItem("r0.m[0]->m[0]"))
        self.inittablewidget.setItem(rows + 1,columns,QTableWidgetItem("r0.m[0]->m[1]"))
        self.inittablewidget.setItem(rows + 2,columns,QTableWidgetItem("r0.m[0]->m[2]"))
        self.inittablewidget.cut()
        self.inittablewidget.setRangeSelected(selranges, False)
        selranges = QTableWidgetSelectionRange(rows + 3, columns, rows + 6, columns)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.paste()
        self.text = self.inittablewidget.item(rows + 3, columns).text()
        self.textfirst = self.inittablewidget.item(rows + 4, columns).text()
        self.textsecond = self.inittablewidget.item(rows + 5, columns).text()
        self.result = QString("r0.m[0]->m[0]")
        self.resultfirst = QString("r0.m[0]->m[1]")
        self.resultsecond = QString("r0.m[0]->m[2]")
        self.assertTrue(self.inittablewidget.paste)
        self.assertEqual(self.text ,self.result, 'test fail')
        self.assertEqual(self.textfirst, self.resultfirst ,'test fail')
        self.assertEqual(self.textsecond, self.resultsecond, ' test fail')           


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
