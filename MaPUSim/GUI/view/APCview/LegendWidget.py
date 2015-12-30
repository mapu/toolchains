'''
Created on Dec 24, 2015

@author: wangl
'''
from PyQt4.QtGui import QWidget, QLabel, QHBoxLayout

class LegendWidget(QWidget):
    '''
    Widget provide a legend for stage diagram
    '''


    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(LegendWidget, self).__init__(parent)
        self.legendLabel = QLabel(self.tr("Legend:"))
        self.legendLabel.setFixedSize(130, 25)
        self.redLabel = QLabel()
        self.redLabel.setFixedSize(50, 20)
        self.redLabel.setStyleSheet("QLabel{background:rgb(255,0,0)}")
        self.redTip = QLabel(self.tr("Register is written"))
        self.redTip.setFixedSize(80, 25)
        self.greenLabel = QLabel()
        self.greenLabel.setFixedSize(50, 20)
        self.greenLabel.setStyleSheet("QLabel{background:rgb(0,255,0)}")
        self.greenTip = QLabel(self.tr("Register is read"))
        self.greenTip.setFixedSize(80, 25)
        self.yellowLabel = QLabel()
        self.yellowLabel.setFixedSize(50, 20)
        self.yellowLabel.setStyleSheet("QLabel{background:rgb(255,153,18)}")
        self.yellowTip = QLabel(self.tr("Register is read and written"))
        self.yellowTip.setFixedSize(150, 25)
        
        self.legendLayout = QHBoxLayout()
        
        self.legendLayout.addWidget(self.legendLabel)
        self.legendLayout.addWidget(self.redLabel)
        self.legendLayout.addWidget(self.redTip)
        self.legendLayout.addWidget(self.greenLabel)
        self.legendLayout.addWidget(self.greenTip)
        self.legendLayout.addWidget(self.yellowLabel)
        self.legendLayout.addWidget(self.yellowTip)
        
        self.setLayout(self.legendLayout)
        