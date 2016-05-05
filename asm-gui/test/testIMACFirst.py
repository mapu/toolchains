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
        "test IMAC register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        global condlist
        condlist = ["","@(c)","@(!c)"]
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testImac_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["l","u,l","l,cr","l,b","l,h"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for cond in condlist:
                            text = "mr+=t%s*t%s(%s)->%s%s"%(m,n,pre,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"9-10")
   
    def testImac_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["l","u,l","l,cr","l,b","l,h"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "mr+=t%s*t%s(%s)->m[%s]%s"%(m,n,pre,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"9-10")

    def testImac_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["l","u,l","l,cr","l,b","l,h"]
        macclist = ["ialu","imac","falu"]
        for pre in prelist:
            for macc in macclist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "mr += t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"8-9")

    def testImac_3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["l","u,l","l,cr","l,b","l,h"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "mr += t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,shu,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"9-10")


    def testImac_4(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["","(u)","(cr)","(b)","(h)","(u,cr)","(u,b)","(u,h)","(cr,b)","(cr,h)"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for m in xrange(0,4):
                    for cond in condlist:
                        text = "mr += t%s%s->%s%s"%(m,pre,biu,cond)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        #print self.inittablewidget.item(row,column).text()
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")
   
    def testImac_5(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["","(u)","(cr)","(b)","(h)","(u,cr)","(u,b)","(u,h)","(cr,b)","(cr,h)"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for m in xrange(0,4):
                for t in xrange(0,127):
                    for cond in condlist:
                        text = "mr += t%s%s->m[%s]%s"%(m,pre,t,cond)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testImac_6(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["","(u)","(cr)","(b)","(h)","(u,cr)","(u,b)","(u,h)","(cr,b)","(cr,h)"]
        macclist = ["ialu","imac","falu"]
        for pre in prelist:
            for macc in macclist:
                for m in xrange(0,4):
                    for t in xrange(0,4):
                        for cond in condlist:
                            text = "mr += t%s%s->%s.t%s%s"%(m,pre,macc,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"4")

    def testImac_7(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["","(u)","(cr)","(b)","(h)","(u,cr)","(u,b)","(u,h)","(cr,b)","(cr,h)"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for m in xrange(0,4):
                    for t in xrange(0,4):
                        for cond in condlist:
                            text = "mr += t%s%s->%s.t%s%s"%(m,pre,shu,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5") 
    
    def testImac_8(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["","(u)","(cr)","(b)","(h)","(u,cr)","(u,b)","(u,h)","(cr,b)","(cr,h)"]
        for pre in prelist:
            for m in xrange(0,4):
                for cond in condlist:
                    text = "mr += t%s%s%s"%(m,pre,cond)
                    #print text
                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                    self.inittablewidget.setRangeSelected(selranges, True)
                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                    #print self.inittablewidget.item(row,column).text()
                    self.inittablewidget.dataParser(row, column)
                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"1")
        

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)

