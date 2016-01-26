# -*- coding: utf-8 -*_
from PyQt4.QtGui import QWidget, QVBoxLayout, QSplitter
from PyQt4.QtCore import Qt
from view.APCview.MPUCoreWidget import MPUCoreWidget

class MPUAllWidget(QSplitter):
    def __init__(self, inst_table, parent = None):
        super(MPUAllWidget, self).__init__(parent)

        self.leftWidget = MPUCoreWidget(inst_table)
        self.rightWidget = QWidget()

        self.setOrientation(Qt.Horizontal)
        self.setChildrenCollapsible(True)
        self.addWidget(self.leftWidget)
        self.addWidget(self.rightWidget)
        self.setStretchFactor(0, 15)
        self.setStretchFactor(1, 1)



