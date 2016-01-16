'''
Created on Dec 27, 2015

@author: wangl
'''
from PyQt4.QtGui import QTableWidgetItem, QColor
from PyQt4.QtCore import Qt
import ctypes

class ScalarRegfileTableItem(QTableWidgetItem):
    '''
    Display a scalar register table item
    '''


    def __init__(self, typecombo, itemtype = QTableWidgetItem.Type):
        '''
        Constructor
        '''
        super(ScalarRegfileTableItem, self).__init__(itemtype)
        self.value = 0
        self.typeCombo = typecombo
        self.op = 0 # 00: None 01: Read 10: Write 11: RW
        self.readColor = QColor(0, 255, 0)
        self.writeColor = QColor(255, 0, 0)
        self.rwColor = QColor(255, 153, 18)
        self.noneColor = QColor(0, 0, 0)
        
    def text(self):
        '''
        Format the value string
        '''
        datatype = self.typeCombo.currentText()
        if datatype == "1 byte":
            return "0x%02x" % self.value
        if datatype == "2 bytes":
            return "0x%04x" % self.value
        if datatype == "4 bytes":
            return "0x%08x" % self.value
        if datatype == "float":
            return "%f" % ctypes.cast(ctypes.pointer(ctypes.c_uint32(self.value)),
                                      ctypes.POINTER(ctypes.c_float)).contents.value
        if datatype == "double":
            return "%f" % ctypes.cast(ctypes.pointer(ctypes.c_uint64(self.value)),
                                      ctypes.POINTER(ctypes.c_double)).contents.value
    
    def data(self, role):
        '''
        Override the text display rules
        '''
        if role == Qt.DisplayRole:
            return self.text()
        elif role == Qt.TextColorRole:
            if self.op == 0:
                return self.noneColor
            elif self.op == 1:
                return self.readColor
            elif self.op == 2:
                return self.writeColor
            elif self.op == 3:
                return self.rwColor
            else:
                return self.noneColor
        else:
            return QTableWidgetItem.data(self, role)
        
    def setValueByString(self, value):
        '''
        Get value from the 'value' string
        '''
        self.value = int(value, 16)
        

class VectorRegfileTableItem(QTableWidgetItem):
    '''
    Display a vector register table item
    '''


    def __init__(self, typecombo, itemtype = QTableWidgetItem.Type):
        '''
        Constructor
        '''
        super(VectorRegfileTableItem, self).__init__(itemtype)
        self.value = [0] * 64
        self.typeCombo = typecombo
        self.op = 0 # 00: None 01: Read 10: Write 11: RW
        self.readColor = QColor(0, 255, 0)
        self.writeColor = QColor(255, 0, 0)
        self.rwColor = QColor(255, 153, 18)
        self.noneColor = QColor(0, 0, 0)
        
    def text(self):
        '''
        Format the value string
        '''
        datatype = self.typeCombo.currentText()
        if datatype == "1 byte":
            return " ".join(["0x%02x" % v for v in self.value])
        if datatype == "2 bytes":
            value = [(self.value[i * 2 + 1] << 8) | self.value[i * 2] for i in xrange(32)]
            return " ".join(["0x%04x" % v for v in value])
        if datatype == "4 bytes":
            value = [(self.value[i * 4 + 3] << 24) | (self.value[i * 4 + 2] << 16) |
                     (self.value[i * 4 + 1] << 8) | self.value[i * 4]
                      for i in xrange(16)]
            return " ".join(["0x%08x" % v for v in value])
        if datatype == "float":
            value = [(self.value[i * 4 + 3] << 24) | (self.value[i * 4 + 2] << 16) |
                     (self.value[i * 4 + 1] << 8) | self.value[i * 4]
                      for i in xrange(16)]
            return " ".join(["%f" % 
                             ctypes.cast(ctypes.pointer(ctypes.c_uint32(v)),
                             ctypes.POINTER(ctypes.c_float)).contents.value
                             for v in value])
        if datatype == "double":
            value = [(self.value[i * 4 + 3] << 24) | (self.value[i * 4 + 2] << 16) | 
                     (self.value[i * 4 + 1] << 8) | self.value[i * 4]
                      for i in xrange(16)]
            return " ".join(["%f" %
                             ctypes.cast(ctypes.pointer(ctypes.c_uint64(v)),
                             ctypes.POINTER(ctypes.c_double)).contents.value
                             for v in value])
    
    def data(self, role):
        '''
        Override the text display rules
        '''
        if role == Qt.DisplayRole:
            return self.text()
        elif role == Qt.TextColorRole:
            if self.op == 0:
                return self.noneColor
            elif self.op == 1:
                return self.readColor
            elif self.op == 2:
                return self.writeColor
            elif self.op == 3:
                return self.rwColor
            else:
                return self.noneColor
        else:
            return QTableWidgetItem.data(self, role)
        
    def setValueByString(self, value):
        '''
        Get value from the 'value' string
        '''
        strlist = value.split()
        self.value = [int(s, 16) for s in strlist]
        