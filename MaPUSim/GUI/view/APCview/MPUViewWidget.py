# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt, SIGNAL, pyqtSlot
from PyQt4.QtGui import QSplitter, QPushButton, QWidget, QVBoxLayout, \
    QTabWidget, QTableWidget
from view.APCview.MPUCoreWidget import MPUCoreWidget
from view.APCview.RegfileWidget import MRegfileWidget, MPURegfileWidget
from view.APCview.StageDialog import StageDialog

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPUViewWidget(QWidget):
    def __init__(self, config, control, idx, parent = None):
        super(MPUViewWidget, self).__init__(parent)

        #centralWidget = QSplitter(Qt.Horizontal)
        #self.setCentralWidget(centralWidget)
        widget = QSplitter(Qt.Horizontal)
        widget.setMinimumSize(1400, 600)

        self.stageButton = QPushButton(self.tr("Instruction Pipeline Diagram"))
        self.stageButton.setFixedSize(200, 40)
        self.stageButton.setEnabled(False)
        self.connect(self.stageButton, SIGNAL("clicked()"), self.stageButtonSlot)
        
        # No. 7 is the stage table, and No. 5 is the register table 
        # No. 4 is the inst table 
        self.stageDialog = StageDialog(control.simDB.Tables[idx * 8 + 4],
                                       control.simDB.Tables[idx * 8 + 7],
                                       control.simDB.Tables[idx * 8 + 5],
                                       self)
        self.stageDialog.updateTimePointSignal.connect(control.simDB.setTimePointSlot)
        self.stageDialog.setWindowTitle(self.tr("MPU Instruction Pipeline Diagram"))
        control.simDB.timeChanged.connect(self.timeChangedSlot)
        
        self.coreWidget = MPUCoreWidget(control.simDB.Tables[idx * 8 + 4])
        control.simDB.timeChanged.connect(self.coreWidget.timeChangedSlot)

        # define left Widget
        self.leftWidget = QWidget()
        # layout left Widget
        self.leftLayout = QVBoxLayout()
        self.leftLayout.addWidget(self.stageButton)
        # scroll
        self.leftLayout.addWidget(self.coreWidget)
        self.leftWidget.setLayout(self.leftLayout)

        # define rightTab
        self.rightTab = QTabWidget()
        self.rightTab.setMinimumWidth(100)
        
        self.MRegWidget = MRegfileWidget(control.simDB.Tables[idx * 8 + 5],
                                         control.simDB.Tables[idx * 8 + 6])
        self.MPURegWidget = MPURegfileWidget(control.simDB.Tables[idx * 8 + 5],
                                             control.simDB.Tables[idx * 8 + 6])
        #control.simDB.timeChanged.connect(self.MRegWidget.timeChangedSlot)
        #control.simDB.timeChanged.connect(self.MPURegWidget.timeChangedSlot)
        
        self.traceWidget = QTableWidget()
        self.rightTab.addTab(self.MRegWidget, self.tr("M RF"))
        self.rightTab.addTab(self.MPURegWidget, self.tr("MPU Registers"))
        self.rightTab.addTab(self.traceWidget, self.tr("Trace"))
        self.traceWidget.horizontalHeader().setStretchLastSection(True)

        splitter = QSplitter(Qt.Horizontal, widget)
        splitter.setChildrenCollapsible(False)
        splitter.addWidget(self.leftWidget)
        splitter.addWidget(self.rightTab)
        splitter.setStretchFactor(0, 1)
        splitter.setStretchFactor(1, 1)
        
        vbox = QVBoxLayout()
        vbox.addWidget(splitter)
        self.setLayout(vbox)

    @pyqtSlot()
    def stageButtonSlot(self):
        self.stageDialog.show()
        self.stageDialog.raise_()
        self.stageDialog.activateWindow()
        
    @pyqtSlot()
    def timeChangedSlot(self):
        self.stageButton.setEnabled(True)
