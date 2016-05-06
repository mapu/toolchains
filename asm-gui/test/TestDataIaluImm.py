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
        "test IALU register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        global condlist
        condlist = ["","@(c)","@(!c)"]

    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testDataIalu(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)","(u)","(u,b)","(u,h)"]
        biulist = ["biu0","biu1","biu2"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for biu in biulist:
                        for cond in condlist:
                            text = "t%s>>%s%s->%s%s"%(m,imm5,byte,biu,cond)
                            text0 = "t%s>>%s%s->%s%s"%(m,hex(imm5),byte,biu,cond)
                            #print text0
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")

    def testDataIalu_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)","(u)","(u,b)","(u,h)"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for t in xrange(0,127):
                        for cond in condlist:
                                text = "t%s>>%s%s->m[%s]%s"%(m,imm5,byte,t,cond)
                                text0 = "t%s>>%s%s->m[%s]%s"%(m,hex(imm5),byte,t,cond)
                                #print text0
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")

    def testDataIalu_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)","(u)","(u,b)","(u,h)"]
        macclist = ["ialu","imac","falu"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for macc in macclist:
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s>>%s%s->%s.t%s%s"%(m,imm5,byte,macc,t,cond)
                                text0 = "t%s>>%s%s->%s.t%s%s"%(m,hex(imm5),byte,macc,t,cond)
                                #print text0
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")

    def testDataIalu_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)","(u)","(u,b)","(u,h)"]
        shulist = ["shu0","shu1"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for shu in shulist:
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s>>%s%s->%s.t%s%s"%(m,imm5,byte,shu,t,cond)
                                text0 = "t%s>>%s%s->%s.t%s%s"%(m,hex(imm5),byte,shu,t,cond)
                                #print text0
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")


    def testDataIalu_3(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)"]
        biulist = ["biu0","biu1","biu2"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for biu in biulist:
                        for cond in condlist:
                            text = "t%s<<%s%s->%s%s"%(m,imm5,byte,biu,cond)
                            text0 = "t%s<<%s%s->%s%s"%(m,hex(imm5),byte,biu,cond)
                            #print text0
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.dataParser(row, column)
                            self.inittablewidget.dataParser(row + 1, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")

    def testDataIalu_4(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for t in xrange(0,127):
                        for cond in condlist:
                                text = "t%s<<%s%s->m[%s]%s"%(m,imm5,byte,t,cond)
                                text0 = "t%s<<%s%s->m[%s]%s"%(m,hex(imm5),byte,t,cond)
                                #print text0
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")

    def testDataIalu_5(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)"]
        macclist = ["ialu","imac","falu"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for macc in macclist:
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s<<%s%s->%s.t%s%s"%(m,imm5,byte,macc,t,cond)
                                text0 = "t%s<<%s%s->%s.t%s%s"%(m,hex(imm5),byte,macc,t,cond)
                                #print text0
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")

    def testDataIalu_6(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        bytelist = ["","(B)","(H)"]
        shulist = ["shu0","shu1"]
        for m in xrange(0,4):
            for imm5 in xrange(9,32):
                for byte in bytelist:
                    for shu in shulist:
                        for t in xrange(0,4):
                            for cond in condlist:
                                text = "t%s<<%s%s->%s.t%s%s"%(m,imm5,byte,shu,t,cond)
                                text0 = "t%s<<%s%s->%s.t%s%s"%(m,hex(imm5),byte,shu,t,cond)
                                #print text0
                                selranges = QTableWidgetSelectionRange(row, column, row, column)
                                self.inittablewidget.setRangeSelected(selranges, True)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)

