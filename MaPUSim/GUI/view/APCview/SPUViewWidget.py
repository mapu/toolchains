# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt, SIGNAL, pyqtSlot
from PyQt4.QtGui import QWidget, QPushButton, QVBoxLayout, QTabWidget,\
    QTableWidget, QHBoxLayout, QSplitter
from view.APCview.RegfileWidget import SPURegfileWidget, SpecialRegfileWidget
from view.APCview.StageWidget import StageWidget
from view.APCview.MemTableWidget import SPUMemTableWidget

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
        mem_table = control.simDB.getTable("APE%dSPUMemTable" % idx)
        self.stageWidget = StageWidget(inst_table, stage_table, reg_table, self)
        self.stageWidget.updateTimePointSignal.connect(control.simDB.setTimePointSlot)
        control.simDB.traceAnalyzeDone.connect(self.stageWidget.updatePageListSlot)
        control.simDB.traceAnalyzeDone.connect(self.stageWidget.updateVHeaderListSlot)
        self.leftLayout.addWidget(self.stageWidget)

        self.rightTab = QTabWidget()

        # define rightTab
        self.genRegWidget = SPURegfileWidget(reg_table, time_table)
        self.specRegWidget = SpecialRegfileWidget(reg_table, time_table)
        self.memTableWidget = SPUMemTableWidget(mem_table, inst_table)
        control.simDB.timeChanged.connect(self.genRegWidget.timeChangedSlot)
        control.simDB.timeChanged.connect(self.specRegWidget.timeChangedSlot)
        control.simDB.traceAnalyzeDone.connect(self.memTableWidget.updateTableSlot)
        
        self.traceWidget = QTableWidget()
        self.rightTab.addTab(self.genRegWidget, self.tr("General purpose RF"))
        self.rightTab.addTab(self.specRegWidget, self.tr("Special RF"))
        self.rightTab.addTab(self.memTableWidget, self.tr("Mem Info"))
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
