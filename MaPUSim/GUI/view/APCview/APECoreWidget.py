# -*- coding: utf-8 -*-
from PyQt4.QtGui import QWidget, QTabWidget, QHBoxLayout
from view.APCview.SPUViewWidget import SPUViewWidget
from view.APCview.MPUViewWidget import MPUViewWidget

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class APECoreWidget(QWidget):
    def __init__(self, config, control, idx, parent = None):
        super(APECoreWidget, self).__init__(parent)

        ProcUnitTab = QTabWidget()
        self.SPUWidget = SPUViewWidget(config, control, idx)
        self.MPUWidget = MPUViewWidget(config, control, idx)
        ProcUnitTab.addTab(self.SPUWidget, self.tr("SPU"))
        ProcUnitTab.addTab(self.MPUWidget, self.tr("MPU"))
        ProcUnitTab.setTabPosition(QTabWidget.West)

        layout = QHBoxLayout()
        layout.addWidget(ProcUnitTab)
        self.setLayout(layout)


