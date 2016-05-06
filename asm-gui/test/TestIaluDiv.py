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
        bytelist = ["","(u)"]
        for m in xrange(0,4):
            for n in xrange(0,4):
                for byte in bytelist:
                    for cond in condlist:
                        text = "t%s divs t%s%s->divr%s"%(m,n,byte,cond)
                        #print text
                        selranges = QTableWidgetSelectionRange(row, column, row, column)
                        self.inittablewidget.setRangeSelected(selranges, True)
                        self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                        self.inittablewidget.dataParser(row, column)
                        self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                        self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "1")

    def testDataIalu_0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        dplist = ["mdivr","mdivq","divq"]
        for dp in dplist:
            for cond in condlist:
                text = "%s->divr%s"%(dp,cond)
                #print text
                selranges = QTableWidgetSelectionRange(row, column, row, column)
                self.inittablewidget.setRangeSelected(selranges, True)
                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                self.inittablewidget.dataParser(row, column)
                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "1")

    def testDataIalu_1(self):
        self.main.newFile()
        row =random.randint(0,1999)
        column = random.randint(0,19)
        biulist = ["biu0","biu1","biu2"]
        for biu in biulist:
            for cond in condlist:
                text = "divr->%s%s"%(biu,cond)
                #print text
                selranges = QTableWidgetSelectionRange(row, column, row, column)
                self.inittablewidget.setRangeSelected(selranges, True)
                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                self.inittablewidget.dataParser(row, column)
                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")

    def testDataIalu_2(self):
        self.main.newFile()
        row =random.randint(0,1999)
        column = random.randint(0,19)
        for t in xrange(0,127):
            for cond in condlist:
                text = "divr->m[%s]%s"%(t,cond)
                #print text
                selranges = QTableWidgetSelectionRange(row, column, row, column)
                self.inittablewidget.setRangeSelected(selranges, True)
                self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                self.inittablewidget.dataParser(row, column)
                self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")

    def testDataIalu_3(self):
        self.main.newFile()
        row =random.randint(0,1999)
        column = random.randint(0,19)
        macclist = ["ialu","imac","falu"]
        for macc in macclist:
            for t in xrange(0,4):
                for cond in condlist:
                    text = "divr->%s.t%s%s"%(macc,t,cond)
                    #print text
                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                    self.inittablewidget.setRangeSelected(selranges, True)
                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                    self.inittablewidget.dataParser(row, column)
                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "2")

    def testDataIalu_4(self):
        self.main.newFile()
        row =random.randint(0,1999)
        column = random.randint(0,19)
        shulist = ["shu0","shu1"]
        for shu in shulist:
            for t in xrange(0,4):
                for cond in condlist:
                    text = "divr->%s.t%s%s"%(shu,t,cond)
                    #print text
                    selranges = QTableWidgetSelectionRange(row, column, row, column)
                    self.inittablewidget.setRangeSelected(selranges, True)
                    self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                    self.inittablewidget.dataParser(row, column)
                    self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                    self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "3")

suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
