'''
Created on Dec 24, 2015

@author: wangl
'''
from PyQt4.QtGui import QWidget, QLabel, QHBoxLayout, QPainter, QLinearGradient, QBrush

class LegendWidget(QWidget):
    '''
    Widget provide a legend for stage diagram
    '''


    def __init__(self, row_color, parent = None):
        '''
        Constructor
        '''
        super(LegendWidget, self).__init__(parent)

        self.rowColor = row_color
        self.legendLabel = QLabel(self.tr("Legend:"))
        self.legendLabel.setFixedSize(130, 25)
        self.redLabel = QLabel()
        self.redLabel.setFixedSize(40, 20)
        self.redTip = QLabel(self.tr("Register is written"))
        self.redTip.setFixedSize(130, 25)
        self.greenLabel = QLabel()
        self.greenLabel.setFixedSize(40, 20)
        self.greenTip = QLabel(self.tr("Register is read"))
        self.greenTip.setFixedSize(130, 25)
        self.yellowLabel = QLabel()
        self.yellowLabel.setFixedSize(40, 20)
        self.yellowTip = QLabel(self.tr("Register is read and written"))
        self.yellowTip.setFixedSize(180, 25)
        
        self.legendLayout = QHBoxLayout()
        
        self.legendLayout.addWidget(self.legendLabel)
        self.legendLayout.addWidget(self.redLabel)
        self.legendLayout.addWidget(self.redTip)
        self.legendLayout.addWidget(self.greenLabel)
        self.legendLayout.addWidget(self.greenTip)
        self.legendLayout.addWidget(self.yellowLabel)
        self.legendLayout.addWidget(self.yellowTip)
        
        self.setLayout(self.legendLayout)
   
    def paintEvent(self, event):
        painter = QPainter()
        painter.begin(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)
        
        rect = self.redLabel.geometry()
        redGradient = self.rowColor["W"]
        redGradient.setStart(rect.topLeft())
        redGradient.setFinalStop(rect.bottomRight())
        painter.setBrush(QBrush(redGradient)) 
        painter.drawRect(rect)

        rect = self.greenLabel.geometry()
        greenGradient = self.rowColor["R"]
        greenGradient.setStart(rect.topLeft())
        greenGradient.setFinalStop(rect.bottomRight())

        painter.setBrush(QBrush(greenGradient)) 
        painter.drawRect(rect)

        rect = self.yellowLabel.geometry()
        yellowGradient = self.rowColor["RW"]
        yellowGradient.setStart(rect.topLeft())
        yellowGradient.setFinalStop(rect.bottomRight())
        painter.setBrush(QBrush(yellowGradient)) 
        painter.drawRect(rect)

        painter.end()
     
