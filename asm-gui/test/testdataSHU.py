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
        "test SHU register read"
        self.main = MainWindow()
        self.inittablewidget = self.main.microcodeTableWidget
        global condlist
        condlist = ["","@(c)","@(!c)"]
        
    def tearDown(self):
        self.main = None
        self.inittablewidget = None

    def testDataShu(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        shulist = ["shu0","shu1"]
        sbhlist = ["sq","bb","bh","bw","bd"]
        biulist = ["biu0", "biu1", "biu2"]
        for shu in shulist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for biu in biulist:
                        for cond in condlist: 
                            text = "%s.t%s IND t%s->%s%s"%(shu,m,n,biu,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column )
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")
                            for imm3 in xrange(0,8):          
                                text0 = "%s.t%s IND t%s(tb =+ %s)->%s%s"%(shu,m,n,imm3,biu,cond)
                                #print text0
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")
        for shu in shulist:
            for sbh in sbhlist:
                for biu in biulist:
                    for cond in condlist:
                        text ="%s.t%s IND t%s->%s%s"%(shu,sbh,sbh,biu,cond)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3") 
                        for m in xrange(0,4):
                            text0 = "%s.t%s IND t%s->%s%s"%(shu,m,sbh,biu,cond)
                            text1 = "%s.t%s IND t%s->%s%s"%(shu,sbh,m,biu,cond)
                            #print text0
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")
                            for imm3 in xrange(0,8):          
                                text2 = "%s.t%s IND t%s(tb =+ %s)->%s%s"%(shu,m,sbh,imm3,biu,cond)
                                text3 = "%s.t%s IND t%s(tb =+ %s)->%s%s"%(shu,sbh,sbh,imm3,biu,cond)
                                text4 = "%s.t%s IND t%s(tb =+ %s)->%s%s"%(shu,sbh,m,imm3,biu,cond)
                                #print text2
                                self.inittablewidget.setItem(row, column + 1,QTableWidgetItem(text2))
                                self.inittablewidget.setItem(row, column + 2,QTableWidgetItem(text3))
                                self.inittablewidget.setItem(row, column + 3,QTableWidgetItem(text4))
                                self.inittablewidget.dataParser(row, column + 1)
                                self.inittablewidget.dataParser(row, column + 2)
                                self.inittablewidget.dataParser(row, column + 3)
                                self.assertEqual(self.inittablewidget.item(row, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row, column + 2).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row, column + 3).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")

    def testDataShu_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        shulist = ["shu0","shu1"]
        sbhlist = ["sq","bb","bh","bw","bd"]
        for shu in shulist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for t in xrange(0,127):
                        for cond in condlist: 
                            text = "%s.t%s IND t%s->m[%s]%s"%(shu,m,n,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column )
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")
                            for imm3 in xrange(0,8):          
                                text0 = "%s.t%s IND t%s(tb =+ %s)->m[%s]%s"%(shu,m,n,imm3,t,cond)
                                #print text0
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")
        for shu in shulist:
            for sbh in sbhlist:
                for t in xrange(0,127):
                    for cond in condlist:
                        text ="%s.t%s IND t%s->m[%s]%s"%(shu,sbh,sbh,t,cond)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4") 
                        for m in xrange(0,4):
                            text0 = "%s.t%s IND t%s->m[%s]%s"%(shu,m,sbh,t,cond)
                            text1 = "%s.t%s IND t%s->m[%s]%s"%(shu,sbh,m,t,cond)
                            #print text0
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")
                            for imm3 in xrange(0,8):          
                                text2 = "%s.t%s IND t%s(tb =+ %s)->m[%s]%s"%(shu,m,sbh,imm3,t,cond)
                                text3 = "%s.t%s IND t%s(tb =+ %s)->m[%s]%s"%(shu,sbh,sbh,imm3,t,cond)
                                text4 = "%s.t%s IND t%s(tb =+ %s)->m[%s]%s"%(shu,sbh,m,imm3,t,cond)
                                #print text2
                                self.inittablewidget.setItem(row, column + 1,QTableWidgetItem(text2))
                                self.inittablewidget.setItem(row, column + 2,QTableWidgetItem(text3))
                                self.inittablewidget.setItem(row, column + 3,QTableWidgetItem(text4))
                                self.inittablewidget.dataParser(row, column + 1)
                                self.inittablewidget.dataParser(row, column + 2)
                                self.inittablewidget.dataParser(row, column + 3)
                                self.assertEqual(self.inittablewidget.item(row, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row, column + 2).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row, column + 3).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")

    def testDataShu_1(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        shulist = ["shu0","shu1"]
        sbhlist = ["sq","bb","bh","bw","bd"]
        macclist = ["ialu", "imac", "falu", "fmac"]
        ilist = ["i0", "i1", "i2"]
        for shu in shulist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for macc in macclist:
                        for t in xrange(0,4):
                            for i in ilist:
                                for cond in condlist: 
                                    text = "%s.t%s IND t%s->%s.t%s(%s)%s"%(shu,m,n,macc,t,i,cond)
                                    #print text
                                    selranges = QTableWidgetSelectionRange(row, column, row, column )
                                    self.inittablewidget.setRangeSelected(selranges, True)
                                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                    self.inittablewidget.dataParser(row, column)
                                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")
                                    for imm3 in xrange(0,8):          
                                        text0 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s(%s)%s"%(shu,m,n,imm3,macc,t,i,cond)
                                        #print text0
                                        self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                        self.inittablewidget.dataParser(row + 1, column)
                                        self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")
        for shu in shulist:
            for sbh in sbhlist:
                for macc in macclist:
                    for t in xrange(0,4):
                        for i in ilist:
                            for cond in condlist:
                                text ="%s.t%s IND t%s->%s.t%s(%s)%s"%(shu,sbh,sbh,macc,t,i,cond)
                                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                                self.inittablewidget.dataParser(row, column)
                                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3") 
                                for m in xrange(0,4):
                                    text0 = "%s.t%s IND t%s->%s.t%s(%s)%s"%(shu,m,sbh,macc,t,i,cond)
                                    text1 = "%s.t%s IND t%s->%s.t%s(%s)%s"%(shu,sbh,m,macc,t,i,cond)
                                    #print text0
                                    self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                    self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                                    self.inittablewidget.dataParser(row + 1, column)
                                    self.inittablewidget.dataParser(row + 2, column)
                                    self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")
                                    for imm3 in xrange(0,8):          
                                        text2 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s(%s)%s"%(shu,m,sbh,imm3,macc,t,i,cond)
                                        text3 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s(%s)%s"%(shu,sbh,sbh,imm3,macc,t,i,cond)
                                        text4 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s(%s)%s"%(shu,sbh,m,imm3,macc,t,i,cond)
                                        #print text2
                                        self.inittablewidget.setItem(row, column + 1,QTableWidgetItem(text2))
                                        self.inittablewidget.setItem(row, column + 2,QTableWidgetItem(text3))
                                        self.inittablewidget.setItem(row, column + 3,QTableWidgetItem(text4))
                                        self.inittablewidget.dataParser(row, column + 1)
                                        self.inittablewidget.dataParser(row, column + 2)
                                        self.inittablewidget.dataParser(row, column + 3)
                                        self.assertEqual(self.inittablewidget.item(row, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                                        self.assertEqual(self.inittablewidget.item(row, column + 2).background(),self.inittablewidget.defaultBackgroundColor)
                                        self.assertEqual(self.inittablewidget.item(row, column + 3).background(),self.inittablewidget.defaultBackgroundColor)
                                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")

    def testDataShu_2(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        shulist = ["shu0","shu1"]
        sbhlist = ["sq","bb","bh","bw","bd"]
        for shu in shulist:
            for m in xrange(0,4):
                for n in xrange(0,4):
                    for t in xrange(0,4):
                        for cond in condlist: 
                            text = "%s.t%s IND t%s->%s.t%s%s"%(shu,m,n,shu,t,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column )
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "2")
                            for imm3 in xrange(0,8):          
                                text0 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s%s"%(shu,m,n,imm3,shu,t,cond)
                                #print text0
                                self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                                self.inittablewidget.dataParser(row + 1, column)
                                self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "2")
        for shu in shulist:
            for sbh in sbhlist:
                for t in xrange(0,4):
                    for cond in condlist:
                        text ="%s.t%s IND t%s->%s.t%s%s"%(shu,sbh,sbh,shu,t,cond)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "2") 
                        for m in xrange(0,4):
                            text0 = "%s.t%s IND t%s->%s.t%s%s"%(shu,m,sbh,shu,t,cond)
                            text1 = "%s.t%s IND t%s->%s.t%s%s"%(shu,sbh,m,shu,t,cond)
                            #print text0
                            self.inittablewidget.setItem(row + 1, column,QTableWidgetItem(text0))
                            self.inittablewidget.setItem(row + 2, column,QTableWidgetItem(text1))
                            self.inittablewidget.dataParser(row + 1, column)
                            self.inittablewidget.dataParser(row + 2, column)
                            self.assertEqual(self.inittablewidget.item(row + 1, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.item(row + 2, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "2")
                            for imm3 in xrange(0,8):          
                                text2 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s%s"%(shu,m,sbh,imm3,shu,t,cond)
                                text3 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s%s"%(shu,sbh,sbh,imm3,shu,t,cond)
                                text4 = "%s.t%s IND t%s(tb =+ %s)->%s.t%s%s"%(shu,sbh,m,imm3,shu,t,cond)
                                #print text2
                                self.inittablewidget.setItem(row, column + 1,QTableWidgetItem(text2))
                                self.inittablewidget.setItem(row, column + 2,QTableWidgetItem(text3))
                                self.inittablewidget.setItem(row, column + 3,QTableWidgetItem(text4))
                                self.inittablewidget.dataParser(row, column + 1)
                                self.inittablewidget.dataParser(row, column + 2)
                                self.inittablewidget.dataParser(row, column + 3)
                                self.assertEqual(self.inittablewidget.item(row, column + 1).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row, column + 2).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.item(row, column + 3).background(),self.inittablewidget.defaultBackgroundColor)
                                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "2") 


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
