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
        dplist = ["max","min"]
        prelist = ["s","d"]
        biulist = ["biu0","biu1","biu2"]
        for dp in dplist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            text = "%s(t%s,t%s)(%s)->%s"%(dp,m,n,pre,biu)
                            text0 = "%s(t%s,t%s)(%s)->%s@(c)"%(dp,m,n,pre,biu)
                            text1 = "%s(t%s,t%s)(%s)->%s@(!c)"%(dp,m,n,pre,biu)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 1, column + 1,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 1, column + 1)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        dplist = ["max","min"]
        prelist = ["s","d"]
        #biulist = ["biu0","biu1","biu2"]
        for dp in dplist:
            for pre in prelist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,127):
                            text = "%s(t%s,t%s)(%s)->m[%s]"%(dp,m,n,pre,t)
                            text0 = "%s(t%s,t%s)(%s)->m[%s]@(c)"%(dp,m,n,pre,t)
                            text1 = "%s(t%s,t%s)(%s)->m[%s]@(!c)"%(dp,m,n,pre,t)
                            #print text0
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 1, column + 1,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 1, column + 1)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        dplist = ["max","min"]
        prelist = ["s","d"]
        macclist = ["ialu","imac","falu","fmac"]
        for dp in dplist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                text = "%s(t%s,t%s)(%s)->%s.t%s"%(dp,m,n,pre,macc,t)
                                text0 = "%s(t%s,t%s)(%s)->%s.t%s@(!c)"%(dp,m,n,pre,macc,t)
                                text1 = "%s(t%s,t%s)(%s)->%s.t%s@(c)"%(dp,m,n,pre,macc,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.setItem(row + 1, column + 1,QTableWidgetItem(text1))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.inittablewidget.dataParser(row + 1, column + 1)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        dplist = ["max","min"]
        prelist = ["s","d"]
        shulist = ["shu0","shu1"]
        for dp in dplist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for n in xrange(0,4):
                            for t in xrange(0,4):
                                text = "%s(t%s,t%s)(%s)->%s.t%s"%(dp,m,n,pre,shu,t)
                                text0 = "%s(t%s,t%s)(%s)->%s.t%s@(c)"%(dp,m,n,pre,shu,t)
                                text1 = "%s(t%s,t%s)(%s)->%s.t%s@(!c)"%(dp,m,n,pre,shu,t)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.setItem(row + 1, column + 1,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.inittablewidget.dataParser(row + 1, column + 1)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
