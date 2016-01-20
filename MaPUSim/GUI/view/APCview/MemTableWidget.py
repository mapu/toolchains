'''
Created on Jan 18, 2016

@author: litt
'''
from PyQt4.QtCore import pyqtSlot, SIGNAL, Qt
from PyQt4.QtGui import QTableView, QAbstractItemView, QColor,\
    QFont, QHeaderView
from data.MemTableModel import MemTableModel
import re

class MemTableWidget(QTableView):
    '''
    This widget is used for showing mem files.
    '''
    def __init__(self, mem_table, inst_table, parent = None):
        '''
        Constructor
        '''
        super(MemTableWidget, self).__init__(parent)
        
        self.memTable = mem_table
        self.instTable = inst_table
        self.mem = [[]]
        self.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.setShowGrid(True)
        self.verticalHeader().setDefaultSectionSize(16)
        self.setFont(QFont("Monospace", 10))
        self.horizontalHeader().setStretchLastSection(True)
        #self.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
        self.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
        
    def initTable(self):
        '''
        Initialize the table content
        '''
        self.memModel = MemTableModel(self.mem, self)
        self.header = ["Time", "PC", "Disassemble", "OP", "Address", "Value"]
        self.memModel.setHorizontalHeader(self.header)
        self.mem = [[]]
        self.setModel(self.memModel)

    @pyqtSlot()
    def updateTableSlot(self):
        '''
        update mem table by analysis trace signal
        '''
        self.initTable()

        records = self.memTable.getAll()
        if len(records) == 0:
            return
        textList = []
        self.mem = []
        for record in records:
            textList = [str(record[1]), "", "", record[3], record[5], record[4]]
            sears = self.instTable.getInstPcDis(record[1], record[2])
            sear = sears[0]
            textList[1] = sear[0]
            textList[2] = sear[1]
            self.mem.append(textList)
        self.memModel.setModelData(self.mem)
        self.memModel.refrushModel()

class SPUMemTableWidget(MemTableWidget):
    '''
    This widget is used for showing Mem reg.
    '''


    def __init__(self, mem_table, inst_table, parent = None):
        '''
        Constructor
        '''
        super(SPUMemTableWidget, self).__init__(mem_table, inst_table, parent)
       

class MPUMemTableWidget(MemTableWidget):
    '''
    This widget is used for showing Mem reg.
    '''


    def __init__(self, mem_table, inst_table, parent = None):
        '''
        Constructor
        '''
        super(MPUMemTableWidget, self).__init__(mem_table, inst_table, parent)
       

