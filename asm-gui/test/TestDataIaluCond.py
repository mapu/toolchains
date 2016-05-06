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
 
    def testdataialu(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = [">",">=","<","<="]
        bytelist = ["","(u)","(b)","(h)","(u,b)","(u,h)"]
        for m in xrange(0,4):
            for sym in symlist:
                for n in xrange(0,4):
                    for byte in bytelist:
                        for cond in condlist:
                            text = "t%s%s t%s%s->cond%s"%(m,sym,n,byte,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")
            

    def testdataialu0(self):
        self.main.newFile()
        row = random.randint(0,1999)
        column = random.randint(0,19)
        symlist = ["==","!="]
        bytelist = ["","(b)","(h)"]
        for m in xrange(0,4):
            for sym in symlist:
                for n in xrange(0,4):
                    for byte in bytelist:
                        for cond in condlist:
                            text = "t%s%s t%s%s->cond%s"%(m,sym,n,byte,cond)
                            #print text
                            selranges = QTableWidgetSelectionRange(row, column, row, column)
                            self.inittablewidget.setRangeSelected(selranges, True)
                            self.inittablewidget.setItem(row, column,QTableWidgetItem(text))
                            self.inittablewidget.dataParser(row, column)
                            self.assertEqual(self.inittablewidget.item(row, column).background(),self.inittablewidget.defaultBackgroundColor)
                            self.assertEqual(self.inittablewidget.database.searchMcc(self.inittablewidget.mmpulite.result), "4")


suite = unittest.TestLoader().loadTestsFromTestCase(mytest)
unittest.TextTestRunner(verbosity=2).run(suite)
