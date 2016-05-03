#!/usr/bin/env python
#encoding: utf-8

from PyQt4.QtGui import QWidget,QTableWidget,QTableWidgetItem,QApplication,QTableWidgetSelectionRange,QAbstractItemView
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
        "test FALU register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testfalu(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["+","-"]
        prelist = ["s","d","t,s","t,d"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for sym in symlist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            text = "t%s%st%s(%s)->%s"%(m,sym,n,pre,biu)
                            text0 = "t%s%st%s(%s)->%s@(c|!c)"%(m,sym,n,pre,biu)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu0(self):
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
                            text = "t%s%st%s(%s)->m[%s]"%(m,sym,n,pre,t)
                            text0 = "t%s%st%s(%s)->m[%s]@(c|!c)"%(m,sym,n,pre,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu1(self):
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
                                text = "t%s%st%s(%s)->%s.t%s"%(m,sym,n,pre,macc,t)
                                text0 = "t%s%st%s(%s)->%s.t%s@(c|!c)"%(m,sym,n,pre,macc,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu2(self):
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
                                text = "t%s%st%s(%s)->%s.t%s"%(m,sym,n,pre,shu,t)
                                text0 = "t%s%st%s(%s)->%s.t%s@(c|!c)"%(m,sym,n,pre,shu,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
    def testfalu3(self):
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
                            text = "t%s%st%s(%s)->%s"%(m,sym,n,pre,biu)
                            text0 = "t%s%st%s(%s)->%s@(c|!c)"%(m,sym,n,pre,biu)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu4(self):
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
                            text = "t%s%st%s(%s)->m[%s]"%(m,sym,n,pre,t)
                            text0 = "t%s%st%s(%s)->m[%s]@(c|!c)"%(m,sym,n,pre,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu5(self):
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
                                text = "t%s%st%s(%s)->%s.t%s"%(m,sym,n,pre,macc,t)
                                text0 = "t%s%st%s(%s)->%s.t%s@(c|!c)"%(m,sym,n,pre,macc,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu6(self):
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
                                text = "t%s%st%s(%s)->%s.t%s"%(m,sym,n,pre,shu,t)
                                text0 = "t%s%st%s(%s)->%s.t%s@(c|!c)"%(m,sym,n,pre,shu,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6") 

    def testfalu7(self):
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
                            text = "t%s%st%s(%s)->%s"%(m,sym,n,pre,biu)
                            text0 = "t%s%st%s(%s)->%s@(c|!c)"%(m,sym,n,pre,biu)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu8(self):
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
                            text = "t%s%st%s(%s)->m[%s]"%(m,sym,n,pre,t)
                            text0 = "t%s%st%s(%s)->m[%s]@(c|!c)"%(m,sym,n,pre,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu9(self):
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
                                text = "t%s%st%s(%s)->%s.t%s"%(m,sym,n,pre,macc,t)
                                text0 = "t%s%st%s(%s)->%s.t%s@(c|!c)"%(m,sym,n,pre,macc,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu10(self):
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
                                text = "t%s%st%s(%s)->%s.t%s"%(m,sym,n,pre,shu,t)
                                text0 = "t%s%st%s(%s)->%s.t%s@(c|!c)"%(m,sym,n,pre,shu,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")       

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
