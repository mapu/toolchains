# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt
from PyQt4.QtGui import QSplitter, QWidget, QVBoxLayout, QTabWidget, QTableWidget
from view.APCview.RegfileWidget import MRegfileWidget, MPURegfileWidget
from view.APCview.StageWidget import StageWidget

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPUViewWidget(QWidget):
    def __init__(self, config, control, idx, parent = None):
        super(MPUViewWidget, self).__init__(parent)
        
        # Get tables
        inst_table = control.simDB.getTable("APE%dMPUInstTable" % idx)
        stage_table = control.simDB.getTable("APE%dMPUStageTable" % idx)
        reg_table = control.simDB.getTable("APE%dMPURegTable" % idx)
        time_table = control.simDB.getTable("APE%dMPUTimeTable" % idx)
        self.stageWidget = StageWidget(inst_table, stage_table, reg_table, self)
        self.stageWidget.updateTimePointSignal.connect(control.simDB.setTimePointSlot)
        control.simDB.traceAnalyzeDone.connect(self.stageWidget.updatePageListSlot)
        
        # define left Widget
        self.leftWidget = QWidget()
        # layout left Widget
        self.leftLayout = QVBoxLayout()
        self.leftLayout.addWidget(self.stageWidget)
        
        self.leftWidget.setLayout(self.leftLayout)

        # define rightTab
        self.rightTab = QTabWidget()
        self.rightTab.setMinimumWidth(100)
        
        self.MRegWidget = MRegfileWidget(reg_table, time_table)
        self.MPURegWidget = MPURegfileWidget(reg_table, time_table)
        control.simDB.timeChanged.connect(self.MRegWidget.timeChangedSlot)
        control.simDB.timeChanged.connect(self.MPURegWidget.timeChangedSlot)
        
        self.traceWidget = QTableWidget()
        self.rightTab.addTab(self.MRegWidget, self.tr("M RF"))
        self.rightTab.addTab(self.MPURegWidget, self.tr("MPU Registers"))
        self.rightTab.addTab(self.traceWidget, self.tr("Trace"))
        self.traceWidget.horizontalHeader().setStretchLastSection(True)

        self.splitter = QSplitter(Qt.Horizontal)
        self.splitter.setChildrenCollapsible(False)
        self.splitter.addWidget(self.leftWidget)
        self.splitter.addWidget(self.rightTab)
        self.splitter.setStretchFactor(0, 2)
        self.splitter.setStretchFactor(1, 1)
        
        self.vbox = QVBoxLayout()
        self.vbox.addWidget(self.splitter)
        self.setLayout(self.vbox)
