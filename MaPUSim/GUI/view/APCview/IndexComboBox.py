'''
Created on Jan 17, 2016

@author: wangl
'''
from PyQt4.QtGui import QComboBox
from PyQt4.QtCore import pyqtSlot, QStringList

class IndexComboBox(QComboBox):
    '''
    Index selection combobox for vertor register view
    '''


    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(QComboBox, self).__init__(parent)
        self.itemLists = [
            ["all"] + ["%d" % i for i in xrange(64)],
            ["all"] + ["%d" % i for i in xrange(32)],
            ["all"] + ["%d" % i for i in xrange(16)],
            ["all"] + ["%d" % i for i in xrange(16)],
            ["all"] + ["%d" % i for i in xrange(8)]
        ]
        
    @pyqtSlot(int)
    def typeChangedSlot(self, ty):
        '''
        Reload indices according to current data type
        '''
        self.clear()
        qslist = QStringList()
        for s in self.itemLists[ty]:
            qslist.append(s)
        self.insertItems(0, qslist)
        