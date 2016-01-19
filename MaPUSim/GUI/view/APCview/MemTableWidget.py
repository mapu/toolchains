'''
Created on Jan 18, 2016

@author: litt
'''
from PyQt4.QtCore import pyqtSlot, SIGNAL, Qt
from PyQt4.QtGui import QTableView, QAbstractItemView, QColor,\
    QStandardItemModel, QFont, QStandardItem, QHeaderView
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
        
        self.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.horizontalHeader().setVisible(True)
        self.verticalHeader().setVisible(False)
        self.setShowGrid(True)
        self.verticalHeader().setDefaultSectionSize(16)
        self.setFont(QFont("Monospace", 10))
        self.horizontalHeader().setStretchLastSection(True)
        self.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
        self.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
        
    def appendRow(self, record):
        '''
        Set the item as a register file class name
        '''
        num = len(record)
        itemList = []
        for i in range(0, num):
            item = QStandardItem(record[i])
            itemList.append(item)
        self.memModel.appendRow(itemList)
        
    def initTable(self):
        '''
        Initialize the table content
        '''
        self.memModel = QStandardItemModel(self)
        header = ["Time", "PC", "Disassemble", "OP", "Value", "Address"]
        self.memModel.setHorizontalHeaderLabels(header)

        self.setModel(self.memModel)

    @pyqtSlot()
    def updateTableSlot(self):
        '''
        update mem table by analysis trace signal
        '''
        records = self.memTable.getAll()
        if len(records) == 0:
            return
        textList = []
        for record in records:
            textList = [str(record[1]), "", "", record[3], record[4], record[5]]
            sears = self.instTable.getInstPcDis(record[1], record[2])
            sear = sears[0]
            textList[1] = sear[4]
            textList[2] = sear[6]
            self.appendRow(textList)

class SPUMemTableWidget(MemTableWidget):
    '''
    This widget is used for showing Mem reg.
    '''


    def __init__(self, mem_table, inst_table, parent = None):
        '''
        Constructor
        '''
        super(SPUMemTableWidget, self).__init__(mem_table, inst_table, parent)
       
        self.initTable()

class MPUMemTableWidget(MemTableWidget):
    '''
    This widget is used for showing Mem reg.
    '''


    def __init__(self, mem_table, inst_table, parent = None):
        '''
        Constructor
        '''
        super(MPUMemTableWidget, self).__init__(mem_table, inst_table, parent)
       
        self.initTable()

