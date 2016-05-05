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
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testFalu(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["abs","recip","rsqrt"]
        prelist = ["s","d"]
        biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        text = "%st%s(%s)->%s"%(ury,m,pre,biu)
                        text0 = "%st%s(%s)->%s@(c)"%(ury,m,pre,biu)
                        text1 = "%st%s(%s)->%s@(!c)"%(ury,m,pre,biu)
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
   
    def testFalu_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["abs","recip","rsqrt"]
        prelist = ["s","d"]
        #biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for m in xrange(0,4):
                    for t in xrange(0,127):
                        text = "%st%s(%s)->m[%s]"%(ury,m,pre,t)
                        text0 = "%st%s(%s)->m[%s]@(c)"%(ury,m,pre,t)
                        text1 = "%st%s(%s)->m[%s]@(!c)"%(ury,m,pre,t)
                        #print text
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

    def testFalu_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["abs","recip","rsqrt"]
        prelist = ["s","d"]
        macclist = ["ialu","imac","falu","fmac"]
        for ury in urylist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,macc,t)
                            text0 = "%st%s(%s)->%s.t%s@(c)"%(ury,m,pre,macc,t)
                            text1 = "%st%s(%s)->%s.t%s@(!c)"%(ury,m,pre,macc,t)
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

    def testFalu_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["abs","recip","rsqrt"]
        prelist = ["s","d"]
        shulist = ["shu0","shu1"]
        for ury in urylist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,shu,t)
                            text0 = "%st%s(%s)->%s.t%s@(c)"%(ury,m,pre,shu,t)
                            text1 = "%st%s(%s)->%s.t%s@(!c)"%(ury,m,pre,shu,t)
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

    def testFalu_3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        biulist = ["biu0","biu1","biu2"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        for cond in condlist:
                            text = "%st%s(%s)->%s%s"%(ury,m,pre,biu,cond)
                            text1 = "%st%s->%s%s"%(ury,m,biu,cond)
                            print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testFalu_4(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        condlist = ["","@(c)","@(!c)"]
        #biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for m in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "%st%s(%s)->m[%s]%s"%(ury,m,pre,t,cond)
                            text1 = "%st%s->m[%s]%s"%(ury,m,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_5(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        macclist = ["ialu","imac","falu","fmac"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "%st%s(%s)->%s.t%s%s"%(ury,m,pre,macc,t,cond)
                                text1 = "%st%s->%s.t%s%s"%(ury,m,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 2, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testFalu_6(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        shulist = ["shu0","shu1"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "%st%s(%s)->%s.t%s%s"%(ury,m,pre,shu,t,cond)
                                text1 = "%st%s->%s.t%s%s"%(ury,m,shu,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 2, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_7(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        biulist = ["biu0","biu1","biu2"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        for cond in condlist:
                            text = "%st%s(%s)->%s%s"%(ury,m,pre,biu,cond)
                            text1 = "%st%s->%s%s"%(ury,m,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testFalu_8(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        #biulist = ["biu0","biu1","biu2"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for m in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "%st%s(%s)->m[%s]%s"%(ury,m,pre,t,cond)
                            text1 = "%st%s->m[%s]%s"%(ury,m,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_9(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        macclist = ["ialu","imac","falu","fmac"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "%st%s(%s)->%s.t%s%s"%(ury,m,pre,macc,t,cond)
                                text1 = "%st%s->%s.t%s%s"%(ury,m,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 2, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testFalu_10(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        shulist = ["shu0","shu1"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "%st%s(%s)->%s.t%s%s"%(ury,m,pre,shu,t,cond)
                                text1 = "%st%s->%s.t%s%s"%(ury,m,shu,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 2, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_11(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","t,d"]
        biulist = ["biu0","biu1","biu2"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        for cond in condlist:
                            text = "%st%s(%s)->%s%s"%(ury,m,pre,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testFalu_12(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","T,D"]
        condlist = ["","@(c)","@(!c)"]
        #biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for m in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "%st%s(%s)->m[%s]%s"%(ury,m,pre,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testFalu_13(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","T,D"]
        macclist = ["ialu","imac","falu","fmac"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "%st%s(%s)->%s.t%s%s"%(ury,m,pre,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testFalu_14(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","T,D"]
        shulist = ["shu0","shu1"]
        condlist = ["","@(c)","@(!c)"]
        for ury in urylist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "%st%s(%s)->%s.t%s%s"%(ury,m,pre,shu,t,cond)
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
