 # -*- coding: utf-8 -*-   
import sys
from PyQt4.QtGui import QHeaderView, QPainter, QPen, QFont
from PyQt4.QtCore import Qt, pyqtSignal, pyqtSlot, SIGNAL, QStringList, QString

class HeaderView(QHeaderView):  
    def __init__(self, model, orientation, parent = None):  
        super(HeaderView, self).__init__(orientation, parent)  
        self.orientation = orientation
        self.model = model
        self.height = 20
        self.setFont(QFont("Monospace"))
        
    def paintSection(self, painter, rect, logicalIndex): 
        painter.save()
        painter.translate(rect.x()+5, rect.y())
        tmp = self.sectionSizeHint(logicalIndex)
        if tmp > self.height:
            self.height = tmp
            self.setMinimumHeight(self.height)
        painter.rotate(95)
        painter.drawText(0, 0, self.model.headerData(logicalIndex, self.orientation, Qt.DisplayRole).toString())
        painter.restore()

