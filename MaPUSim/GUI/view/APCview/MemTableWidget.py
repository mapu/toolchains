'''
Created on Jan 18, 2016

@author: litt
'''
from PyQt4.QtCore import pyqtSlot, pyqtSignal
from PyQt4.QtGui import QTableView, QAbstractItemView, QFont
from data.MemTableModel import MemTableModel

class MemTableWidget(QTableView):
    updateMemTableSignal = pyqtSignal()
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
        self.memModel = MemTableModel(self.memTable, self.instTable, self)
        self.updateMemTableSignal.connect(self.memModel.getMemData)
        self.header = ["Time", "PC", "Disassemble", "OP", "Address", "Value"]
        self.memModel.setHorizontalHeader(self.header)
        self.setModel(self.memModel)

    @pyqtSlot()
    def updateTableSlot(self):
        '''
        update mem table by analysis trace signal
        '''
        self.initTable()
        self.updateMemTableSignal.emit()

class SPUMemTableWidget(MemTableWidget):
    '''
    This widget is used for showing memory access.
    '''


    def __init__(self, mem_table, inst_table, parent = None):
        '''
        Constructor
        '''
        super(SPUMemTableWidget, self).__init__(mem_table, inst_table, parent)
       

class MPUMemTableWidget(MemTableWidget):
    '''
    This widget is used for showing memory access.
    '''


    def __init__(self, mem_table, inst_table, parent = None):
        '''
        Constructor
        '''
        super(MPUMemTableWidget, self).__init__(mem_table, inst_table, parent)
       

