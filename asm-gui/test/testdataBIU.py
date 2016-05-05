#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView,QBrush, QColor
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
        "test BIU register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        global condlist
        condlist = ["","@(c)","@(!c)"]
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testDataBiu(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        #t = random.randint(0,127)
        biulist = ["biu0","biu1","biu2"]
        ldlist = ["","(A++)","(K++)","(M)","(BR)","(a++,K++)","(K++,M)","(M,BR)","(A++,M)","(A++,BR)","(K++,BR)"]
        for biu in biulist:
            for ld in ldlist:
                for t in xrange(0,127):
                    for cond in condlist:
                        text = "%s.dm%s->m[%s]%s"%(biu,ld,t,cond)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "12")
        for biu in biulist:
            for ld in ldlist:
                for cond in condlist:
                    text = "%s.dm %s->m[I++]%s"%(biu,ld,cond)
                    selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column)
                    self.inittablewidget.setRangeSelected(selranges, True)
                    self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                    self.inittablewidget.dataParser(row + 1, column)
                    self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "12")

    def testDataBiu_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        #t = random.randint(0,127)
        biulist = ["biu0","biu1","biu2"]
        ldlist = ["","(A++)","(K++)","(M)","(BR)","(a++,K++)","(K++,M)","(M,BR)","(A++,M)","(A++,BR)","(K++,BR)"]
        macclist = ["ialu", "imac", "falu", "fmac"]
        ilist = ["i0", "i1", "i2"]
        for biu in biulist:
            for ld in ldlist:
                for macc in macclist:
                    for t in xrange(0,3):
                        for i in ilist:
                            for cond in condlist:
                                text = "%s.dm%s->%s.t%s(%s)%s"%(biu,ld,macc,t,i,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "11")
    def testDataBiu_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        #t = random.randint(0,127)
        biulist = ["biu0","biu1","biu2"]
        ldlist = ["","(A++)","(K++)","(M)","(BR)","(a++,K++)","(K++,M)","(M,BR)","(A++,M)","(A++,BR)","(K++,BR)"]
        shulist = ["shu0", "shu1"]
        for biu in biulist:
            for ld in ldlist:
                for shu in shulist:
                    for t in xrange(0,3):
                        for cond in condlist:
                            text = "%s.dm%s->%s.t%s%s"%(biu,ld,shu,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "11")

    def testDataBiu_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        #t = random.randint(0,127)
        biulist = ["biu0","biu1","biu2"]
        ldlist = ["","(A++)","(K++)","(M)","(BR)","(a++,K++)","(K++,M)","(M,BR)","(I)","(A++,M)","(A++,BR)","(A++,I)","(K++,BR)","(K++,I)","(M,I)","(BR,I)"]
        for biu in biulist:
            for ld in ldlist:
                for cond in condlist:
                    text = "%s->dm%s%s"%(biu,ld,cond)
                    #print text
                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                    self.inittablewidget.setRangeSelected(selranges, True)
                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                    self.inittablewidget.dataParser(row, column)
                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "8")

    def testDataBiu_3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        #t = random.randint(0,127)
        biulist = ["biu0","biu1","biu2"]
        for biu in biulist:
            for t in xrange(0,127):
                for cond in condlist:
                    text = "%s(kg)->m[%s]%s"%(biu,t,cond)
                    #print text
                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                    self.inittablewidget.setRangeSelected(selranges, True)
                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                    self.inittablewidget.dataParser(row, column)
                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")
        for biu in biulist:
            for cond in condlist:
                text0 = "%s(kg)->m[i++]%s"%(biu,cond)
                print text0
                selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column)
                self.inittablewidget.setRangeSelected(selranges, True)
                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                self.inittablewidget.dataParser(row + 1, column)
                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
