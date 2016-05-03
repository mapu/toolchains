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
        urylist = ["abs","recip","rsqrt"]
        prelist = ["s","d"]
        biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        text = "%st%s(%s)->%s"%(ury,m,pre,biu)
                        text0 = "%st%s(%s)->%s@(c|!c)"%(ury,m,pre,biu)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        #print self.inittablewidget.item(row,column).text()
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu0(self):
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
                        text0 = "%st%s(%s)->m[%s]@(c|!c)"%(ury,m,pre,t)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu1(self):
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
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,macc,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu2(self):
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
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,shu,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        text = "%st%s(%s)->%s"%(ury,m,pre,biu)
                        text0 = "%st%s(%s)->%s@(c|!c)"%(ury,m,pre,biu)
                        text1 = "%st%s->%s"%(ury,m,biu)
                        text2 = "%st%s->%s@(c|!c)"%(ury,m,biu)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        #print self.inittablewidget.item(row,column).text()
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                        self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text0))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.inittablewidget.dataParser(row + 2, column)
                        self.inittablewidget.dataParser(row + 3, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 3, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu4(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        #biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for m in xrange(0,4):
                    for t in xrange(0,127):
                        text = "%st%s(%s)->m[%s]"%(ury,m,pre,t)
                        text0 = "%st%s(%s)->m[%s]@(c|!c)"%(ury,m,pre,t)
                        text1 = "%st%s->m[%s]"%(ury,m,t)
                        text2 = "%st%s->m[%s]@(c|!c)"%(ury,m,t)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                        self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text2))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.inittablewidget.dataParser(row + 2, column)
                        self.inittablewidget.dataParser(row + 3, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu5(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        macclist = ["ialu","imac","falu","fmac"]
        for ury in urylist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,macc,t)
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,macc,t)
                            text1 = "%st%s->%s.t%s"%(ury,m,macc,t)
                            text2 = "%st%s->%s.t%s@(c|!c)"%(ury,m,macc,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text2))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.inittablewidget.dataParser(row + 3, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 3, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu6(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["single"]
        prelist = ["t","d","u","t,d","t,u"]
        shulist = ["shu0","shu1"]
        for ury in urylist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,shu,t)
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,shu,t)
                            text1 = "%st%s->%s.t%s"%(ury,m,shu,t)
                            text2 = "%st%s->%s.t%s@(c|!c)"%(ury,m,shu,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.inittablewidget.dataParser(row + 3, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 3, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu7(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        text = "%st%s(%s)->%s"%(ury,m,pre,biu)
                        text0 = "%st%s(%s)->%s@(c|!c)"%(ury,m,pre,biu)
                        text1 = "%st%s->%s"%(ury,m,biu)
                        text2 = "%st%s->%s@(c|!c)"%(ury,m,biu)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        #print self.inittablewidget.item(row,column).text()
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                        self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text0))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.inittablewidget.dataParser(row + 2, column)
                        self.inittablewidget.dataParser(row + 3, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 3, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu8(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        #biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for m in xrange(0,4):
                    for t in xrange(0,127):
                        text = "%st%s(%s)->m[%s]"%(ury,m,pre,t)
                        text0 = "%st%s(%s)->m[%s]@(c|!c)"%(ury,m,pre,t)
                        text1 = "%st%s->m[%s]"%(ury,m,t)
                        text2 = "%st%s->m[%s]@(c|!c)"%(ury,m,t)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                        self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text2))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.inittablewidget.dataParser(row + 2, column)
                        self.inittablewidget.dataParser(row + 3, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu9(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        macclist = ["ialu","imac","falu","fmac"]
        for ury in urylist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,macc,t)
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,macc,t)
                            text1 = "%st%s->%s.t%s"%(ury,m,macc,t)
                            text2 = "%st%s->%s.t%s@(c|!c)"%(ury,m,macc,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text2))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.inittablewidget.dataParser(row + 3, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 3, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu10(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["double"]
        prelist = ["t","s","u","t,s","t,u"]
        shulist = ["shu0","shu1"]
        for ury in urylist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,shu,t)
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,shu,t)
                            text1 = "%st%s->%s.t%s"%(ury,m,shu,t)
                            text2 = "%st%s->%s.t%s@(c|!c)"%(ury,m,shu,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.setItem(row + 3, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.inittablewidget.dataParser(row + 3, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 3, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu11(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","t,d"]
        biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for biu in biulist:
                    for m in xrange(0,4):
                        text = "%st%s(%s)->%s"%(ury,m,pre,biu)
                        text0 = "%st%s(%s)->%s@(c|!c)"%(ury,m,pre,biu)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        #print self.inittablewidget.item(row,column).text()
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")
   
    def testfalu12(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","T,D"]
        #biulist = ["biu0","biu1","biu2"]
        for ury in urylist:
            for pre in prelist:
                for m in xrange(0,4):
                    for t in xrange(0,127):
                        text = "%st%s(%s)->m[%s]"%(ury,m,pre,t)
                        text0 = "%st%s(%s)->m[%s]@(c|!c)"%(ury,m,pre,t)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

    def testfalu13(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","T,D"]
        macclist = ["ialu","imac","falu","fmac"]
        for ury in urylist:
            for pre in prelist:
                for macc in macclist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,macc,t)
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,macc,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"5")

    def testfalu14(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        urylist = ["INT","UINT"]
        prelist = ["s","d","T,S","T,D"]
        shulist = ["shu0","shu1"]
        for ury in urylist:
            for pre in prelist:
                for shu in shulist:
                    for m in xrange(0,4):
                        for t in xrange(0,4):
                            text = "%st%s(%s)->%s.t%s"%(ury,m,pre,shu,t)
                            text0 = "%st%s(%s)->%s.t%s@(c|!c)"%(ury,m,pre,shu,t)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result),"6")

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
