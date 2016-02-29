# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt

class RectInfo:
    def __init__(self):
        self.reg = -1
        self.topLeft_x = 0
        self.topLeft_y = 0
        self.bottomRight_x = 0
        self.bottomRight_y = 0
        self.num = 0
        self.column = -1
        self.startRow = -1
        self.endRow = -1
        self.color = Qt.black


