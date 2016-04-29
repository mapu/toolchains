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
        "test M register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testdataParser(self):
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(0, 0, 0, 0)
        self.inittablewidget.setRangeSelected(selranges, True)
        self.inittablewidget.setItem(0,0,QTableWidgetItem("r0.m[I++]->m[3]"))
        self.inittablewidget.dataParser(0,0)
        self.assertEqual(self.inittablewidget.item(0,0).background(),self.inittablewidget.defaultBackgroundColor)

    def testdataParserfirst(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        s = random.randint(0,127)
        t = random.randint(0,3)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row , column )
        self.inittablewidget.setRangeSelected(selranges, True)
	text = "r0.m[%s]->m[%s]"%(s, t)
        text0 = "r0.m[I++]->m[%s]"%(t)
        text1 = "r0.m[S++]->m[%s]"%(t)
        shut = random.choice(["shu0","shu1"])
        text2 = "r0.m[%s]->%s.t%s"%(s,shut,t)
        IS = random.choice(["I++", "S++"])
        text3 = "r0.m[%s]->%s.t%s"%(IS,shut,t)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
        self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text2))
        self.inittablewidget.setItem(row + 4, column,QTableWidgetItem(text2))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.inittablewidget.dataParser(row + 2, column)
        self.inittablewidget.dataParser(row + 3, column)
        self.inittablewidget.dataParser(row + 4, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 3, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 4, column).background(),self.inittablewidget.defaultBackgroundColor)

    def testdataParsersecond(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        s = random.randint(0,127)
        t = random.randint(0,3)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column )
        self.inittablewidget.setRangeSelected(selranges, True)
        macc = random.choice(["ialu", "imac", "falu", "fmac"])
        i = random.choice(["i0", "i1", "i2"])
        IS = random.choice(["I++", "S++"])
        R = random.choice(["r0","r1","r2"])
        r = random.choice(["r0","r1"])
        #shut = random.choice(["shu0","shu1"])
        text = "%s.m[%s]->%s.t%s(%s)"%(R,s,macc,t,i)
        text0 = "%s.m[%s]->%s.t%s(%s)"%(r,IS,macc,t,i)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)

    def testdataParserthird(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        s = random.randint(0,127)
        t = random.randint(0,3)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column )
        self.inittablewidget.setRangeSelected(selranges, True)
        shut = random.choice(["SHU0","shu1"])
        IS = random.choice(["I++", "S++"])
        text = "r1.m[%s]->%s.T%s"%(s,shut,t)
        text0 = "r1.m[%s]->%s.T%s"%(IS,shut,t)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)

    def testdataParserforth(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        s = random.randint(0,127)
        t = random.randint(0,3)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column )
        self.inittablewidget.setRangeSelected(selranges, True)
        macc = random.choice(["ialu", "imac", "falu", "fmac"])
        i = random.choice(["i0", "i1", "i2"])
        wfi = random.choice(["wf1", "wf2", "wf3", "wx1", "wx2", "wx3", "wx4"])
        text = "r2.m[%s](%s)->%s.t%s(%s)"%(s,wfi,macc,t,i)
        text0 = text.upper()
        #print text0
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)

    def testdataParserfive(self):
        row = random.randint(0,1999)
        column = random.randint(0,19)
        s = random.randint(0,127)
        self.main.newFile()
        selranges = QTableWidgetSelectionRange(row, column, row, column )
        self.inittablewidget.setRangeSelected(selranges, True)
        BIU = random.choice(["biu0","biu1","biu2","biu0(kg)","biu1(kg)","biu2(kg)"])
        text = "r3.m[%s]->%s"%(s,BIU)
        text0 = "m[%s]->%s"%(s,BIU)
        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
        self.inittablewidget.dataParser(row, column)
        self.inittablewidget.dataParser(row + 1, column)
        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
