# -*- coding: utf-8 -*-
from PyQt4.QtGui import QListWidget
from PyQt4.QtCore import Qt

class FloatDialog(QListWidget):
    def __init__(self, parent = None):
        super(FloatDialog, self).__init__(parent)
        self.setWindowFlags(Qt.WindowStaysOnTopHint)
        self.initDialog([])

    def initDialog(self, mList):
        self.clear()
        num = len(mList)
        for i in range(num):
            self.addItem(mList[i])

    def clearDialog(self):
        self.clear()
	
	
