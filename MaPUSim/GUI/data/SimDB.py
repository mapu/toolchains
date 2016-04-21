'''
Created on Dec 22, 2015

@author: wangl
'''
from PyQt4.QtCore import QObject, pyqtSlot, pyqtSignal, QThread, QMutex
from data.InstTable import SPUInstTable, MPUInstTable
from data.RegTable import SPURegTable, MPURegTable
from data.StageTable import SPUStageTable, MPUStageTable
from data.TimeTable import SPUTimeTable, MPUTimeTable
from data.MemTable import SPUMemTable, MPUMemTable
from timeit import Timer
from view.Utils import fatal
import os
import sqlite3
import sys

class TraceAnalyzeThread(QThread):
    '''
    The thread class for trace analyze
    '''

    def __init__(self, simdb, numofcores, tracefile, parent = None):
        '''
        Constructor
        '''
        super(TraceAnalyzeThread, self).__init__(parent)
        self.simDB = simdb
        self.numOfCores = numofcores
        self.traceFile = tracefile
        
    def run(self):
        '''
        Generate trace database from trace file
        '''
        self.simDB.lock.lock()
        self.simDB.traceAnalyzeStart.emit(0, len(self.simDB.Tables))
        try:
            DBConn = sqlite3.connect(self.simDB.DBName)
            trace = open(self.traceFile, "r")
            lines = trace.readlines()
            lines_wrapper = [lines]
            for i in xrange(len(self.simDB.Tables)):
                print "table", i
                self.simDB.traceAnalyzeProgress.emit("table %s"%i, i)
                self.simDB.Tables[i].DBConn = DBConn
                self.simDB.Tables[i].clearTable()
                t = Timer(lambda: self.simDB.Tables[i].traceAnalyze(lines_wrapper))
                print t.timeit(number = 1)
                self.simDB.Tables[i].DBConn = self.simDB.DBConn
            DBConn.close()
            for i in xrange(self.numOfCores):
                table = self.simDB.getTable("APE%dSPUStageTable" % i)
                if table.startPoint < self.simDB.startPoint:
                    self.simDB.startPoint = table.startPoint
                if table.endPoint > self.simDB.endPoint:
                    self.simDB.endPoint = table.endPoint
                table = self.simDB.getTable("APE%dMPUStageTable" % i)
                if table.startPoint < self.simDB.startPoint:
                    self.simDB.startPoint = table.startPoint
                if table.endPoint > self.simDB.endPoint:
                    self.simDB.endPoint = table.endPoint
            self.simDB.setTimePointSlot(self.simDB.startPoint)
            self.simDB.traceAnalyzeDone.emit()
        except Exception:
            self.simDB.analyzeFailed.emit()
        self.simDB.lock.unlock()
    

class SimDB(QObject):
    '''
    Simulation database
    '''

    timeChanged = pyqtSignal(int)
    traceAnalyzeDone = pyqtSignal()
    analyzeFailed = pyqtSignal()
    traceAnalyzeProgress = pyqtSignal(str, int)
    traceAnalyzeStart = pyqtSignal(int, int)

    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(SimDB, self).__init__(parent)
        self.DBName = "simgui.db"
        # Remove the existing db file first
        if (os.path.exists(self.DBName) and os.path.isfile(self.DBName)):
            os.remove(self.DBName)
        self.DBConn = sqlite3.connect(self.DBName)
        self.Tables = [None] * (4 * 10)
        self.tableIdx = {}
        for i in range(4):
            self.Tables[i * 10] = SPUInstTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 1] = SPURegTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 2] = SPUTimeTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 3] = SPUStageTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 4] = SPUMemTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 5] = MPUInstTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 6] = MPURegTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 7] = MPUTimeTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 8] = MPUStageTable(i, self.DBConn, parent)
            self.Tables[i * 10 + 9] = MPUMemTable(i, self.DBConn, parent)
            
        for table in self.Tables:
            self.tableIdx[table.Name] = self.Tables.index(table)
                        
        self.curTime = -1
        self.startPoint = sys.maxint
        self.endPoint = 0
        
        self.lock = QMutex()
        
        self.analyzeFailed.connect(self.analyzeFailedSlot)

    def analyzeTraceFile(self, num_of_cores, tracefile):
        '''
        Generate trace database from trace file
        '''
        self.curTime = -1
        self.startPoint = sys.maxint
        self.endPoint = 0
        thread = TraceAnalyzeThread(self, num_of_cores, tracefile, self)
        thread.start()
            
    @pyqtSlot(int)
    def setTimePointSlot(self, tp):
        '''
        Receive the current time value from GUI setting or simulation control,
        and then broadcast this change to all concerning modules
        '''
        if self.curTime != tp:
            self.curTime = tp
            self.timeChanged.emit(tp)
            
    @pyqtSlot()
    def analyzeFailedSlot(self):
        fatal("Broken trace file!")
        
    def getTable(self, name):
        return self.Tables[self.tableIdx[name]]
            
        
