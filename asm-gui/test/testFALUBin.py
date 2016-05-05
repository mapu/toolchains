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
        "test FALU register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        global condlist
        condlist = ["","@(c)","@(!c)"]
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testFalu(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+","-"]
        prelist = ["s","d","t,s","t,d"]
        biulist = ["biu0","biu1","biu2"]
        #condlist = ["","@(c)","@(!c)"]
        for pre in prelist:
            for biu in biulist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for cond in condlist:
                                text = "t%s%st%s(%s)->%s%s"%(m,sym,n,pre,biu,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testFalu_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+","-"]
        prelist = ["s","d","t,s","t,d"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for sym in symlist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,127):
                            for cond in condlist:
                                text = "t%s%st%s(%s)->m[%s]%s"%(m,sym,n,pre,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+","-"]
        prelist = ["s","d","t,s","t,d"]
        macclist = ["ialu","imac","falu","fmac"]
        for pre in prelist:
            for macc in macclist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                for cond in condlist:
                                    text = "t%s%st%s(%s)->%s.t%s%s"%(m,sym,n,pre,macc,t,cond)
                                    #print text
                                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                                    self.inittablewidget.setRangeSelected(selranges, True)
                                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                    #print self.inittablewidget.item(row,column).text()
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testFalu_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+","-"]
        prelist = ["s","d","t,s","t,d"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                for cond in condlist:
                                    text = "t%s%st%s(%s)->%s.t%s%s"%(m,sym,n,pre,shu,t,cond)
                                    #print text
                                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                                    self.inittablewidget.setRangeSelected(selranges, True)
                                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                    #print self.inittablewidget.item(row,column).text()
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
    def testFalu_3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+-"]
        prelist = ["s"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for cond in condlist:
                                text = "t%s%st%s(%s)->%s%s"%(m,sym,n,pre,biu,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testFalu_4(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+-"]
        prelist = ["s"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for sym in symlist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,127):
                            for cond in condlist:
                                text = "t%s%st%s(%s)->m[%s]%s"%(m,sym,n,pre,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_5(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+-"]
        prelist = ["s"]
        macclist = ["ialu","imac","falu","fmac"]
        for pre in prelist:
            for macc in macclist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                for cond in condlist:
                                    text = "t%s%st%s(%s)->%s.t%s%s"%(m,sym,n,pre,macc,t,cond)
                                    #print text
                                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                                    self.inittablewidget.setRangeSelected(selranges, True)
                                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                    #print self.inittablewidget.item(row,column).text()
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testFalu_6(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+-"]
        prelist = ["s"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                for cond in condlist:
                                    text = "t%s%st%s(%s)->%s.t%s%s"%(m,sym,n,pre,shu,t,cond)
                                    #print text
                                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                                    self.inittablewidget.setRangeSelected(selranges, True)
                                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                    #print self.inittablewidget.item(row,column).text()
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6") 

    def testFalu_7(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["==","!=",">",">=","<","<="]
        prelist = ["s","d"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for cond in condlist:
                                text = "t%s%st%s(%s)->%s%s"%(m,sym,n,pre,biu,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testFalu_8(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["==","!=",">",">=","<","<="]
        prelist = ["s","d"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for sym in symlist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,127):
                            for cond in condlist:
                                text = "t%s%st%s(%s)->m[%s]%s"%(m,sym,n,pre,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_9(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["==","!=",">",">=","<","<="]
        prelist = ["s","d"]
        macclist = ["ialu","imac","falu","fmac"]
        for pre in prelist:
            for macc in macclist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                for cond in condlist:
                                    text = "t%s%st%s(%s)->%s.t%s%s"%(m,sym,n,pre,macc,t,cond)
                                    #print text
                                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                                    self.inittablewidget.setRangeSelected(selranges, True)
                                    self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                    #print self.inittablewidget.item(row,column).text()
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testFalu_10(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["==","!=",">",">=","<","<="]
        prelist = ["s","d"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                for cond in condlist:
                                    text = "t%s%st%s(%s)->%s.t%s%s"%(m,sym,n,pre,shu,t,cond)
                                    #print text
                                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                                    self.inittablewidget.setRangeSelected(selranges, True)
                                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                    #print self.inittablewidget.item(row,column).text()
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")       

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
