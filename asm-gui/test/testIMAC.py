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

    def testImac(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["t","u,t","t,i","t,b","t,h"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for cond in condlist:
                            text = "t%s*t%s(%s)->%s%s"%(m,n,pre,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6")
   
    def testImac_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["t","u,t","t,i","t,b","t,h"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "t%s*t%s(%s)->m[%s]%s"%(m,n,pre,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6")

    def testImac_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["t","u,t","t,i","t,b","t,h"]
        macclist = ["ialu","imac","falu"]
        for pre in prelist:
            for macc in macclist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "5")

    def testImac_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["t","u,t","t,i","t,b","t,h"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,shu,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6")

    def testImac_3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["b","h","u,b","u,h","i,b","i,h"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for cond in condlist:
                            text = "t%s*t%s(%s)->%s%s"%(m,n,pre,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6-7")
   
    def testImac_4(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["b","h","u,b","u,h","i,b","i,h"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "t%s*t%s(%s)->m[%s]%s"%(m,n,pre,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6-7")

    def testImac_5(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["b","h","u,b","u,h","i,b","i,h"]
        macclist = ["ialu","imac","falu"]
        for pre in prelist:
            for macc in macclist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "5-6")

    def testImac_6(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["b","h","u,b","u,h","i,b","i,h"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,shu,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6-7")

    def testImac_7(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        biulist = ["biu0","biu1","biu2"]
        for p in xrange (0,4):
            for biu in biulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for cond in condlist:
                            text = "t%s+-t%s*t%s(h)->%s%s"%(p,m,n,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "8")
   
    def testImac_8(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        #biulist = ["biu0","biu1","biu2"]
        for p in xrange(0,4):
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "t%s+-t%s*t%s(h)->m[%s]%s"%(p,m,n,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "8")

    def testImac_9(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        macclist = ["ialu","imac","falu"]
        for p in xrange(0,4):
            for macc in macclist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s+-t%s*t%s(h)->%s.t%s%s"%(p,m,n,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "7")

    def testImac_10(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        shulist = ["shu0","shu1"]
        for p in xrange(0,4):
            for shu in shulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s+-t%s*t%s(h)->%s.t%s%s"%(p,m,n,shu,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "8")

    def testImac_11(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["u","b","h","u,b","u,h"]
        for p in xrange(0,4):
             for m in xrange(0,4):
                 for n in xrange(0,4):
                     for pre in prelist:
                        for cond in condlist:
                            text = "mr<-t%s+T%s*t%s%s"%(p,m,n,cond)
                            text1 = "mr<-t%s+T%s*t%s(%s)%s"%(p,m,n,pre,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 2, column, QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6")

    def testImac_12(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["U","CR","B","H","U,CR","U,B","U,H","CR,B","CR,H"]
        biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for biu in biulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for cond in condlist:
                            text = "mr+=t%s*t%s(%s)->%s%s"%(m,n,pre,biu,cond)
                            text1 = "mr+=t%s*t%s->%s%s"%(m,n,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            #print self.inittablewidget.item(row,column).text()
                            self.inittablewidget.setItem(row + 1, column + 1, QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column + 1)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "9")
   
    def testImac_13(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["U","CR","B","H","U,CR","U,B","U,H","CR,B","CR,H"]
        #biulist = ["biu0","biu1","biu2"]
        for pre in prelist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist:
                            text = "mr+=t%s*t%s(%s)->m[%s]%s"%(m,n,pre,t,cond)
                            text1 = "mr+=t%s*t%s->m[%s]%s"%(m,n,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 1, column + 1, QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column + 1)
                            self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(), self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "9")

    def testImac_14(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["U","CR","B","H","U,CR","U,B","U,H","CR,B","CR,H"]
        macclist = ["ialu","imac","falu"]
        for pre in prelist:
            for macc in macclist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "mr+=t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,macc,t,cond)
                                text1 = "mr+=t%s*t%s->%s.t%s%s"%(m,n,macc,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column + 1,QTableWidgetItem(text1))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column + 1)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "8")

    def testImac_15(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["U","CR","B","H","U,CR","U,B","U,H","CR,B","CR,H"]
        shulist = ["shu0","shu1"]
        for pre in prelist:
            for shu in shulist:
                for m in xrange(0,4):
                    for n in xrange(0,4):
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "mr+=t%s*t%s(%s)->%s.t%s%s"%(m,n,pre,shu,t,cond)
                                text1 = "mr+=t%s*t%s->%s.t%s%s"%(m,n,shu,t,cond)
                                #print text
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                                #print self.inittablewidget.item(row,column).text()
                                self.inittablewidget.setItem(row + 1, column + 1, QTableWidgetItem(text1))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column + 1)
                                self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(), self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "9")

    def testImac_16(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        prelist = ["U","CR","B","H","U,CR","U,B","U,H","CR,B","CR,H"]
        for pre in prelist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for cond in condlist:
                        text = "mr+=t%s*t%s(%s)%s"%(m,n,pre,cond)
                        text1 = "mr+=t%s*t%s%s"%(m,n,cond)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column, QTableWidgetItem(text))
                        #print self.inittablewidget.item(row,column).text()
                        self.inittablewidget.setItem(row + 1, column + 1, QTableWidgetItem(text1))
                        self.inittablewidget.dataParser(row, column)
                        self.inittablewidget.dataParser(row + 1, column + 1)
                        self.assertEqual(self.inittablewidget.item(row, column).background(), self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.item(row + 1, column + 1).background(), self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "6")

    

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)

