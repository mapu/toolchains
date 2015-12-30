# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import* 

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class Cell(QTableWidgetItem):  
    def __init__(self, string = ""):  
        super(Cell, self).__init__()  
        self.setText(string)


