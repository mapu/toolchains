# -*- coding: utf-8 -*_
from PyQt4.QtGui import QWidget, QVBoxLayout, QSplitter
from PyQt4.QtCore import Qt
from view.APCview.MPUCoreWidget import MPUCoreWidget

class MPUAllWidget(QWidget):
    def __init__(self, inst_table, parent = None):
        super(MPUAllWidget, self).__init__(parent)

        self.leftWidget = MPUCoreWidget(inst_table)
        self.rightWidget = QWidget()

        self.splitter = QSplitter(Qt.Horizontal)
        self.splitter.setChildrenCollapsible(False)
        self.splitter.addWidget(self.leftWidget)
        self.splitter.addWidget(self.rightWidget)
        self.splitter.setStretchFactor(0, 15)
        self.splitter.setStretchFactor(1, 1)

        self.vbox = QVBoxLayout()
        self.vbox.addWidget(self.splitter)
        self.setLayout(self.vbox)


