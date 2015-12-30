# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class RectInfo:
    def __init__(self):
        self.check = -1
        self.topLeft_x = 0
        self.topLeft_y = 0
        self.topRight_x = 0
        self.topRight_y = 0
        self.bottomLeft_x = 0
        self.bottomLeft_y = 0
        self.bottomRight_x = 0
        self.bottomRight_y = 0
        self.color = Qt.black


