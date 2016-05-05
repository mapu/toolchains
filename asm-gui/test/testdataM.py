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
        "test M register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        global condlist
        condlist = ["","@(c)","@(!c)"]
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testDataParser(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        Rlist = ["r0"]
        ISlist = ["i++", "s++"]
        #s = random.randint(0,127)
        #t = random.randint(0,3)
        for r in Rlist:
            for IS in ISlist:
                for t in xrange(0,127):
                    for cond in condlist:
                        selranges = QTableWidgetSelectionRange(row, column, row , column )
                        self.inittablewidget.setRangeSelected(selranges, True)
	                text = "%s.m[%s]->m[%s]%s"%(r, IS, t, cond)
                        #print text
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"3")
        for r in Rlist:
            for s in xrange(0,127):
                for t in xrange(0,127):
                    for cond in condlist:
                        selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column )
                        self.inittablewidget.setRangeSelected(selranges, True)
	                text = "%s.m[%s]->m[%s]%s"%(r, s, t, cond)
                        #print text
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row + 1, column)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"3")
     
    def testDataParser_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        Rlist = ["r0", "r1"]
        ISlist = ["i++", "s++"]
        shulist = ["shu0", "shu1"]
        #s = random.randint(0,127)
        #t = random.randint(0,3)
        for r in Rlist:
            for IS in ISlist:
                for shu in shulist:
                    for t in xrange(0,4):
                        for cond in condlist:
                            selranges = QTableWidgetSelectionRange(row, column, row , column )
                            self.inittablewidget.setRangeSelected(selranges, True)
	                    text = "%s.m[%s]->%s.t%s%s"%(r, IS, shu, t, cond)
                            #print text
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"3")
        for r in Rlist:
            for s in xrange(0,127):
                for shu in shulist:
                    for t in xrange(0,4):
                        for cond in condlist:
                            selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column )
                            self.inittablewidget.setRangeSelected(selranges, True)
	                    text = "%s.m[%s]->%s.t%s%s"%(r, s, shu, t, cond)
                            #print text
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"3")

    def testDataParser_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        Rlist = ["r0", "r1"]
        ISlist = ["i++", "s++"]
        macclist = ["ialu", "imac","falu","fmac"]
        ilist = ["i0", "i1", "i2"]
        #s = random.randint(0,127)
        #t = random.randint(0,3)
        for r in Rlist:
            for IS in ISlist:
                for macc in macclist:
                    for t in xrange(0,4):
                        for i in ilist:
                            for cond in condlist:
                                selranges = QTableWidgetSelectionRange(row, column, row , column )
                                self.inittablewidget.setRangeSelected(selranges, True)
	                        text = "%s.m[%s]->%s.t%s(%s)%s"%(r, IS, macc, t, i, cond)
                                #print text
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"2")
        for r in Rlist:
            for s in xrange(0,127):
                for macc in macclist:
                    for t in xrange(0,4):
                        for i in ilist:
                            for cond in condlist:
                                selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column )
                                self.inittablewidget.setRangeSelected(selranges, True)
	                        text = "%s.m[%s]->%s.t%s(%s)%s"%(r, s, macc, t, i, cond)
                                #print text
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"2")

    def testDataParser_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        Rlist = ["r2"]
        ISlist = ["i++", "s++"]
        macclist = ["ialu", "imac","falu","fmac"]
        ilist = ["i0", "i1", "i2"]
        for r in Rlist:
            for s in xrange(0,127):
                for macc in macclist:
                    for t in xrange(0,4):
                        for i in ilist:
                            for cond in condlist:
                                selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column )
                                self.inittablewidget.setRangeSelected(selranges, True)
	                        text = "%s.m[%s]->%s.t%s(%s)%s"%(r, s, macc, t, i, cond)
                                #print text
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"2")


    def testDataParser_3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        Rlist = ["r2"]
        wlist = ["wf1", "wf2", "wf3", "wx1", "wx2", "wx3", "wx4"]
        macclist = ["ialu", "imac","falu","fmac"]
        ilist = ["i0", "i1", "i2"]
        for r in Rlist:
            for s in xrange(0,127):
                for w in wlist:
                    for macc in macclist:
                        for t in xrange(0,4):
                            for i in ilist:
                                for cond in condlist:
                                    selranges = QTableWidgetSelectionRange(row, column, row , column )
                                    self.inittablewidget.setRangeSelected(selranges, True)
	                            text = "%s.m[%s](%s)->%s.t%s(%s)%s"%(r, s, w, macc, t, i, cond)
                                    #print text
                                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"4")

    def testDataParser_4(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        Rlist = ["r3.",""]
        biulist = ["biu0", "biu1", "biu2"]
        #kglist = ["","(kg)"]
        for r in Rlist:
            for s in xrange(0,127):
                for biu in biulist:
                    for cond in condlist:
                        selranges = QTableWidgetSelectionRange(row, column, row , column )
                        self.inittablewidget.setRangeSelected(selranges, True)
	                text = "%sm[%s]->%s%s"%(r, s, biu, cond)
                        #print text
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"3")
        for r in Rlist:
            for s in xrange(0,127):
                for biu in biulist:
                    for cond in condlist:
                        selranges = QTableWidgetSelectionRange(row + 1, column, row + 1, column )
                        self.inittablewidget.setRangeSelected(selranges, True)
	                text = "%sm[%s]->%s(kg)%s"%(r, s, biu, cond)
                        #print text
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row + 1, column)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"4")

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
