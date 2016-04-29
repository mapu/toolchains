#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView,QBrush, QColor
from PyQt4.QtCore import Qt, QString,QStringList
from PyQt4.QtTest import QTest
import unittest
import main
import sys
import random
from view.MainWindow import MainWindow
from view.Utils import initParent

class mytest(unittest.TestCase): 
    def setUp(self):
        "test BIU register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None
 
    def testdatabiu(self):
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(0, 0, 0, 0)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("biu0.dm->m[122]"))
        self.inittablewidget.dataParser(0,0)
        self.assertEqual(self.inittablewidget.item(0,0).background(),self.inittablewidget.defaultBackgroundColor)

    def testdatabiufirst(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        t = random.randint(0,127)
        BIU = random.choice(["biu0","biu1","biu2"])
        LD = random.choice(["A++","K++","M","BR","a++,K++","K++,M","M,BR"])
        text = "%s.dm->m[%s]"%(BIU,t)
        text0 = "%s.dm->m[i++]"%(BIU)
        text1 = "%s.dm(%s)->m[%s]"%(BIU,LD,t)
        text2 = "%s.dm(%s)->m[i++]"%(BIU,LD)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.setItem(row, column + 1,QTableWidgetItem(text1))
        self.inittablewidget.setItem(row + 1, column + 1,QTableWidgetItem(text1))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.inittablewidget.dataParser(row, column + 1)
        self.inittablewidget.dataParser(row + 1, column + 1)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(),self.inittablewidget.defaultBackgroundColor)

    def testdatabiusecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        t = random.randint(0,3)
        BIU = random.choice(["biu0","biu1","biu2"])
        LD = random.choice(["A++","K++","M","BR","a++,K++","K++,M","M,BR"])
        ld = random.choice(["ialu","imac","falu","fmac"])
        i = random.choice(["i0","i1","i2"])
        text = "%s.dm->%s.t%s(%s)"%(BIU,ld,t,i)
        text0 = "%s.dm(%s)->%s.t%s(%s)"%(BIU,LD,ld,t,i)
        print text0
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)

    def testdatabiuthird(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        t = random.randint(0,3)
        BIU = random.choice(["biu0","biu1","biu2"])
        LD = random.choice(["A++","K++","M","BR","a++,K++","K++,M","M,BR"])
        shu = random.choice(["shu0","shu1"])
        text = "%s.dm->%s.t%s"%(BIU,shu,t)
        text0 = "%s.dm(%s)->%s.t%s"%(BIU,LD,shu,t)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)

    def testdatabiuforth(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        BIU = random.choice(["biu0","biu1","biu2"])
        LD = random.choice(["A++","K++","M","BR","a++,K++","K++,M","M,BR"])
        text = "%s->dm@(c|!c)"%(BIU)
        text0 = "%s->dm(%s)@(c|!c)"%(BIU,LD)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)

    def testdatabiufive(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        BIU = random.choice(["biu0","biu1","biu2"])
        t = random.randint(0,127)
        text = "%s(kg)->m[%s]@(c|!c)"%(BIU,t)
        text0 = "%s(kg)->m[i++]@(c|!c)"%(BIU)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
