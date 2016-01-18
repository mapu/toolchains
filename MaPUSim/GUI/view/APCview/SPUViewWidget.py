# -*- coding: utf-8 -*-
from PyQt4.QtCore import SIGNAL, pyqtSlot
from PyQt4.QtGui import QWidget, QPushButton, QVBoxLayout, QTabWidget,\
    QTableWidget, QHBoxLayout
from view.APCview.RegfileWidget import SPURegfileWidget, SpecialRegfileWidget
from view.APCview.StageWidget import StageWidget

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class SPUViewWidget(QWidget):
    def __init__(self, config, control, idx, parent = None):
        super(SPUViewWidget, self).__init__(parent)

        self.leftWidget = QWidget()
        self.leftLayout = QVBoxLayout()
        self.leftWidget.setLayout(self.leftLayout)
        
        # Get tables
        inst_table = control.simDB.getTable("APE%dSPUInstTable" % idx)
        stage_table = control.simDB.getTable("APE%dSPUStageTable" % idx)
        reg_table = control.simDB.getTable("APE%dSPURegTable" % idx)
        time_table = control.simDB.getTable("APE%dSPUTimeTable" % idx)
        self.stageWidget = StageWidget(inst_table, stage_table, reg_table, self)
        self.stageWidget.updateTimePointSignal.connect(control.simDB.setTimePointSlot)
        control.simDB.traceAnalyzeDone.connect(self.stageWidget.updatePageListSlot)
        self.leftLayout.addWidget(self.stageWidget)

        self.rightTab = QTabWidget()

        # define rightTab
        self.genRegWidget = SPURegfileWidget(reg_table, time_table)
        self.specRegWidget = SpecialRegfileWidget(reg_table, time_table)
        control.simDB.timeChanged.connect(self.genRegWidget.timeChangedSlot)
        control.simDB.timeChanged.connect(self.genRegWidget.timeChangedSlot)
        
        self.traceWidget = QTableWidget()
        self.rightTab.addTab(self.genRegWidget, self.tr("General purpose RF"))
        self.rightTab.addTab(self.specRegWidget, self.tr("Special RF"))
        self.rightTab.addTab(self.traceWidget, self.tr("Trace"))
        self.traceWidget.horizontalHeader().setStretchLastSection(True)

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.leftWidget)
        mainLayout.addWidget(self.rightTab)
        mainLayout.setStretchFactor(self.leftWidget, 5)
        mainLayout.setStretchFactor(self.rightTab, 2)
        self.setLayout(mainLayout)

