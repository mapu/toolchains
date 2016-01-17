'''
Created on Dec 27, 2015

@author: wangl
'''
from PyQt4.QtCore import pyqtSlot, SIGNAL, Qt
from PyQt4.QtGui import QTableWidget, QAbstractItemView, QColor,\
    QTableWidgetItem, QFont, QComboBox
from data import RegNames
from view.APCview.RegfileTableWidgetItem import ScalarRegfileTableItem,\
    VectorRegfileTableItem
import re
from view.APCview.IndexComboBox import IndexComboBox

class RegfileWidget(QTableWidget):
    '''
    This widget is used for showing register files.
    '''


    def __init__(self, reg_table, time_table, parent = None):
        '''
        Constructor
        '''
        super(RegfileWidget, self).__init__(parent)
        
        self.regTable = reg_table
        self.timeTable = time_table
        
        self.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.horizontalHeader().setVisible(False)
        self.verticalHeader().setVisible(False)
        self.setShowGrid(True)
        self.verticalHeader().setDefaultSectionSize(16)
        self.setFont(QFont("Monospace", 10))
        self.horizontalHeader().setStretchLastSection(True)
        self.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
        self.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
        
        self.classColor = QColor(192, 192, 192)
        
        self.classes = [("name", 0)]
        self.regNames = [""]
        self.regOffset = 0

        self.condition = ""
        
        self.itemType = QTableWidgetItem
        
    def setClassItem(self, row, col, item):
        '''
        Set the item as a register file class name
        '''
        self.setItem(row, col, item)
        self.item(row, col).setBackgroundColor(self.classColor)
        self.setSpan(row, col, 1, 3)
        font = QFont()
        font.setBold(True)
        self.item(row, col).setFont(font)
        
    def initTable(self):
        '''
        Initialize the table content
        '''
        self.setRowCount(len(self.regNames) + len(self.classes))
        i = 0
        j = 0
        valueIdx = self.columnCount() - 1
        for name in self.regNames:
            if j < len(self.classes) and i == self.classes[j][1]:
                self.setClassItem(i, 0, QTableWidgetItem(self.classes[j][0]))
                j += 1
                i += 1
            name = self.nameDecorate(name)
            self.setItem(i, 0, QTableWidgetItem(name))
        
            typeComboBox = QComboBox()
            typeComboBox.insertItem(0, "1 byte")
            typeComboBox.insertItem(1, "2 bytes")
            typeComboBox.insertItem(2, "4 bytes")
            typeComboBox.insertItem(3, "float")
            typeComboBox.insertItem(4, "double")
            
            self.connect(typeComboBox, SIGNAL("currentIndexChanged(int)"),
                         self.typeChangedSlot)
            self.setCellWidget(i, 1, typeComboBox)
            
            if valueIdx == 3:
                idxComboBox = IndexComboBox()
            
                self.connect(typeComboBox, SIGNAL("currentIndexChanged(int)"),
                             idxComboBox.typeChangedSlot)
                self.connect(idxComboBox, SIGNAL("currentIndexChanged(int)"),
                             self.typeChangedSlot)
                idxComboBox.typeChangedSlot(0)
                
                self.setCellWidget(i, 2, idxComboBox)
                
                self.setItem(i, valueIdx, self.itemType(typeComboBox, idxComboBox))
            else:
                self.setItem(i, valueIdx, self.itemType(typeComboBox))
                
            if "NA" in name:
                self.setRowHidden(i, True)
            i += 1
        self.resizeColumnToContents(valueIdx)
        
    @pyqtSlot(int)
    def timeChangedSlot(self, time):
        '''
        Update the register value and R/W operations on it triggered by time
        change signal
        '''
        values = self.timeTable.getValue(time)
        if values == []:
            #===================================================================
            # No record found in DB, the 'time' is earlier than the earliest
            # record, so use the initial value
            #===================================================================
            values = [[0] + self.initValues] 
        j = 0  # The class No.
        i = 0  # The item row No. = register No. + class No.
        valueIdx = self.columnCount() - 1
        for value in values[0][(1 + self.regOffset):self.regEnd + 1]:
            if j < len(self.classes) and i == self.classes[j][1]:
                j += 1
                i += 1
            if value != "N/A":
                self.item(i, valueIdx).setValueByString(value)
            else:
                self.item(i, valueIdx).value = self.getInitValue(i - j)
            self.item(i, valueIdx).op = 0
            i += 1
        
        ops = self.regTable.getOperations(time, self.condition)
        # op = [op, class, no]
        for op in ops:
            idx = op[2] + self.timeTable.regOffset[op[1]] - self.regOffset
            regcl = 0
            for cl in self.classes:
                if idx + regcl >= cl[1]:
                    regcl += 1
            if op[0] == "R":
                self.item(idx + regcl, valueIdx).op |= 1
            elif op[0] == "W":
                self.item(idx + regcl, valueIdx).op |= 2
        self.resizeColumnToContents(2)
        self.viewport().update()
                
    def getInitValue(self, idx):
        '''
        Get the initial value for register 'idx'
        '''
        return 0
    
    @pyqtSlot(int)
    def typeChangedSlot(self, datatype):
        '''
        Update the register value form according to the new selected data type
        '''
        self.viewport().update()
        
    def nameDecorate(self, name):
        '''
        Present the register name in more readable format
        '''
        return re.sub("_", " ", name)


class SPURegfileWidget(RegfileWidget):
    '''
    This widget is used for showing SPU R and J register files.
    '''


    def __init__(self, reg_table, time_table, parent = None):
        '''
        Constructor
        '''
        super(SPURegfileWidget, self).__init__(reg_table, time_table, parent)
        self.setColumnCount(3)
        self.setColumnWidth(0, 40)
        self.regNames = RegNames.SPURegNames[:64]
        self.regOffset = 0
        self.regEnd = 64
        self.classes = [(self.tr("R register file"), 0),
                        (self.tr("J register file"), 33)]
        self.initValues = ["0x0"] * 64
        self.condition = "(class == 'R' OR class == 'J')"
        self.itemType = ScalarRegfileTableItem
        
        self.initTable()


class SpecialRegfileWidget(RegfileWidget):
    '''
    This widget is used for showing special registers.
    '''


    def __init__(self, reg_table, time_table, parent = None):
        '''
        Constructor
        '''
        super(SpecialRegfileWidget, self).__init__(reg_table, time_table, parent)
        self.setColumnCount(3)
        self.setColumnWidth(0, 150)
        self.regNames = RegNames.SPURegNames[64:]
        self.regOffset = 64
        self.regEnd = len(time_table.itemsid)
        self.classes = [(self.tr("SPU special registers"), 0),
                        (self.tr("M RF port control register"),
                         RegNames.SPURegNames.index("MC_r0") + 1 - 64),
                        (self.tr("K registers"),
                         RegNames.SPURegNames.index("KB0") + 2 - 64),
                        (self.tr("MPU control registers"),
                         RegNames.SPURegNames.index("MPUCondition") + 3 - 64)]
        self.initValues = ["0x0"] * (self.regEnd - self.regOffset)
        self.condition = "(class == 'Misc')"
        self.itemType = ScalarRegfileTableItem
        
        self.initTable()


class MRegfileWidget(RegfileWidget):
    '''
    This widget is used for showing M registers.
    '''


    def __init__(self, reg_table, time_table, parent = None):
        '''
        Constructor
        '''
        super(MRegfileWidget, self).__init__(reg_table, time_table, parent)
        self.setColumnCount(4)
        self.setColumnWidth(0, 50)
        self.classes = [(self.tr("M register"), 0)]
        self.regNames = RegNames.MPURegNames[:128]
        self.regOffset = 0
        self.regEnd = 64
        self.initValues = ["0x00 " * 64] * (self.regEnd - self.regOffset)
        self.condition = "class == 'MPU' AND no < %d " % self.regEnd
        self.itemType = VectorRegfileTableItem
        
        self.initTable()
        
    def getInitValue(self, idx):
        '''
        Get the initial value for register 'idx'
        '''
        strlist = self.initValues[idx].split()
        return [int(s, 16) for s in strlist]


class MPURegfileWidget(RegfileWidget):
    '''
    This widget is used for showing M registers.
    '''

    def __init__(self, reg_table, time_table, parent = None):
        '''
        Constructor
        '''
        super(MPURegfileWidget, self).__init__(reg_table, time_table, parent)
        self.setColumnCount(4)
        self.setColumnWidth(0, 50)
        self.regNames = RegNames.MPURegNames[128:]
        self.regOffset = 128
        self.regEnd = len(time_table.itemsid)
        self.classes = [(self.tr("SHU0 T register"), 0),
                        (self.tr("SHU1 T register"), 
                         RegNames.MPURegNames.index("SHU1_T0") + 1 - self.regOffset),
                        (self.tr("IALU T register"), 
                         RegNames.MPURegNames.index("IALU_T0") + 2 - self.regOffset),
                        (self.tr("IMAC T register"), 
                         RegNames.MPURegNames.index("IMAC_T0") + 3 - self.regOffset),
                        (self.tr("FALU T register"), 
                         RegNames.MPURegNames.index("FALU_T0") + 4 - self.regOffset),
                        (self.tr("FMAC T register"), 
                         RegNames.MPURegNames.index("FMAC_T0") + 5 - self.regOffset),
                        (self.tr("Other register"), 
                         RegNames.MPURegNames.index("IMRL") + 6 - self.regOffset)]
        self.initValues = ["0x00 " * 64] * (self.regEnd - self.regOffset)
        self.condition = "class == 'MPU' AND no >= %d " % self.regOffset
        self.itemType = VectorRegfileTableItem
        
        self.initTable()
        
    def getInitValue(self, idx):
        '''
        Get the initial value for register 'idx'
        '''
        strlist = self.initValues[idx].split()
        return [int(s, 16) for s in strlist]
        
    def nameDecorate(self, name):
        '''
        Present the register name in more readable format
        '''
        if "_" in name:
            return name.split("_")[1]
        else:
            return name
        