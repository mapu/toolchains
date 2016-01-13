# -*- coding: utf-8 -*-   
from PyQt4.QtGui import QHeaderView, QFont
from PyQt4.QtCore import QSize

class HeaderView(QHeaderView):  
    def __init__(self, model, orientation, parent = None):  
        super(HeaderView, self).__init__(orientation, parent)  
        self.orientation = orientation
        self.model = model
        self.height = 20
        self.setFont(QFont("Monospace"))
        
    def paintSection(self, painter, rect, logicalIndex):
        QHeaderView.paintSection(self, painter, rect, logicalIndex)
        painter.save()
        painter.translate(rect.x() + 5, rect.y())
        contentHeight = self.fontMetrics().width(self.model.hHeaderList[logicalIndex]) * 1.1
        if contentHeight > self.minimumHeight():
            self.setMinimumHeight(contentHeight)
            self.parent().updateGeometries()
        painter.rotate(80)
        painter.drawText(0, 0, self.model.hHeaderList[logicalIndex])
        painter.restore()
    
    def sizeHint(self):
        if self.minimumHeight() > 0:
            return QSize(self.width(), self.minimumHeight())
        else:
            return QHeaderView.sizeHint(self)
