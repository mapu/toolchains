# -*- coding: utf-8 -*-
from PyQt4.QtCore import SIGNAL, pyqtSlot
from PyQt4.QtGui import QWidget, QPushButton, QVBoxLayout, QTabWidget,\
    QTableWidget, QHBoxLayout
from view.APCview.StageDialog import StageDialog
from view.APCview.RegfileWidget import SPURegfileWidget, SpecialRegfileWidget

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class SPUViewWidget(QWidget):
    def __init__(self, config, control, idx, parent = None):
        super(SPUViewWidget, self).__init__(parent)

        self.leftWidget = QWidget()
        self.stageButton = QPushButton(self.tr("Instruction Pipeline Diagram"))
        self.stageButton.setFixedSize(200, 40)
        self.stageButton.setEnabled(False)
        self.connect(self.stageButton, SIGNAL("clicked()"), self.stageButtonSlot)
        leftLay = QVBoxLayout()
        leftLay.addWidget(self.stageButton)
        self.leftWidget.setLayout(leftLay)
        
        # Get tables
        inst_table = control.simDB.getTable("APE%dMPUInstTable" % idx)
        stage_table = control.simDB.getTable("APE%dMPUStageTable" % idx)
        reg_table = control.simDB.getTable("APE%dMPURegTable" % idx)
        time_table = control.simDB.getTable("APE%dMPUTimeTable" % idx)
        self.stageDialog = StageDialog(inst_table, stage_table, reg_table, self)
        self.stageDialog.setWindowTitle(self.tr("SPU Instruction Pipeline Diagram"))
        self.stageDialog.updateTimePointSignal.connect(control.simDB.setTimePointSlot)
        control.simDB.timeChanged.connect(self.timeChangedSlot)

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

    @pyqtSlot()
    def stageButtonSlot(self):
        self.stageDialog.show()
        
    @pyqtSlot(int)
    def timeChangedSlot(self, time):
        self.stageButton.setEnabled(True)

