# -*- coding: utf-8 -*-   
# from SimuInfoWidget import*
from APCview.APCViewWidget import APCViewWidget
from ARMview.ARMViewWidget import ARMViewWidget
from ConfigView.ConfigViewWidget import ConfigViewWidget
from PyQt4.QtCore import QProcess, SIGNAL, Qt
from PyQt4.QtGui import QMainWindow, QTabWidget, QAction, QIcon, \
    QLabel, QFileDialog, QInputDialog, QPixmap, QLineEdit
from view.Utils import warning
from res import qrc_resources
import os

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MainWindow(QMainWindow):  
    #closeChildDialogAPE0 = pyqtSignal()
    #closeChildDialogAPE1 = pyqtSignal()
    #closeChildDialogAPE2 = pyqtSignal()
    #closeChildDialogAPE3 = pyqtSignal()

    def __init__(self, config, control, parent = None):
        super(MainWindow, self).__init__(parent)
        self.config = config
        self.control = control
        self.setWindowTitle(self.tr("MaPU Simulator"))
        self.createActions()
        self.createMenus()
        self.createToolBars()
        #---Temporarily removed status bar-------------------------------------- 
        #self.createStatusBar()
        self.perspTabs = QTabWidget()
        self.setCentralWidget(self.perspTabs)
        self.armViewWidget = ARMViewWidget(config, control)
        self.apcViewWidget = APCViewWidget(config, control)
        self.configWidget = ConfigViewWidget(config, control)
        
        # Fix me: this is ugly...
        self.configWidget.ARMmodeGroup.buttonClicked[int].connect(self.armViewWidget.UART0Widget.switchMode)
        #======================================================================
        # Simulator information is currently removed
        #======================================================================
        #self.simuInfoWidget = SimuInfoWidget()
        # self.tabWidget.addTab(self.simuInfoWidget,self.tr("Simulator Information"))
        self.perspTabs.addTab(self.armViewWidget, self.tr("ARM Perspective"))
        self.perspTabs.addTab(self.apcViewWidget, self.tr("APC Perspective"))
        self.perspTabs.addTab(self.configWidget, self.tr("Configuration"))
        
        self.control.ARMGem5Process.stateChanged.connect(self.ARMStatus)
        self.control.ARMQemuProcess.stateChanged.connect(self.ARMStatus)
        self.control.APCProcess.stateChanged.connect(self.APCStatus)
    
        self.perspTabs.setCurrentIndex(2)
      
    def createMenus(self):  
        fileMenu = self.menuBar().addMenu(self.tr("File"))
        fileMenu.addAction(self.fileOpenAction)
        setMenu = self.menuBar().addMenu(self.tr("Settings"))
        setMenu.addAction(self.setAction)
    
    def createActions(self):
        self.fileOpenAction = QAction(QIcon(":/open.png"), self.tr("&Open"), self)
        self.fileOpenAction.setToolTip(self.tr("Open trace file"))
        self.connect(self.fileOpenAction, SIGNAL("triggered()"), self.fileOpenSlot)
        self.setAction = QAction(self.tr("&Simulator path ..."), self)
        self.connect(self.setAction, SIGNAL("triggered()"), self.setSimulatorPath)
    
        self.startAction = QAction(self.tr("Start"), self)
        self.startAction.setToolTip(self.tr("Start the simulation"))
        self.stopAction = QAction(self.tr("Stop"), self)
        self.stopAction.setToolTip(self.tr("Terminate the simulation"))
        self.startAction.triggered.connect(self.startProcess)
        self.stopAction.triggered.connect(self.stopProcess)

        self.startAction.setEnabled(True)
        self.stopAction.setEnabled(False)

    def startProcess(self):
        if self.control.start():
            self.startAction.setEnabled(False)
            self.stopAction.setEnabled(True)
            #---Force the configuration to be readonly during the simulation---- 
        else:
            warning(self.tr("Failed to launch the simulation!"),
                    self.tr("Invalid configuration"))
                        
    def stopProcess(self):
        self.startAction.setEnabled(True)
        self.stopAction.setEnabled(False)
        self.control.stop()
                        
    def ARMStatus(self, status):
        if status == QProcess.Running:
            self.ARMLabel.setPixmap(self.GreenPixmap)
            self.startAction.setEnabled(False)
            self.stopAction.setEnabled(True)
        elif status == QProcess.NotRunning:
            self.ARMLabel.setPixmap(self.RedPixmap)
            self.startAction.setEnabled(True)
            self.stopAction.setEnabled(False)

    def APCStatus(self,status):
        if status == QProcess.Running:
            self.APCLabel.setPixmap(self.GreenPixmap)
        elif status == QProcess.NotRunning:
            self.APCLabel.setPixmap(self.RedPixmap)
        
    def createToolBars(self):  
        toolBar = self.addToolBar(self.tr("Trace file bar"))
        toolBar.addAction(self.fileOpenAction)
        self.RedPixmap = \
            QPixmap(":/armred.png").scaledToHeight(25, Qt.SmoothTransformation)
        self.GreenPixmap = \
            QPixmap(":/armgreen.png").scaledToHeight(25, Qt.SmoothTransformation)
        self.ARMTextLabel = QLabel("ARM")
        self.ARMLabel = QLabel()
        self.ARMLabel.setPixmap(self.RedPixmap)
        self.APCTextLabel = QLabel("APC")
        self.APCLabel = QLabel()
        self.APCLabel.setPixmap(self.RedPixmap)
        toolBar = self.addToolBar(self.tr("Simulator status"))
        toolBar.addWidget(self.ARMLabel)
        toolBar.addWidget(self.ARMTextLabel)
        toolBar.insertSeparator(toolBar.addWidget(self.APCLabel))
        toolBar.addWidget(self.APCTextLabel)
        toolBar = self.addToolBar(self.tr("Simulator control"))
        toolBar.addAction(self.startAction)
        toolBar.addAction(self.stopAction)

    def createStatusBar(self):
        self.statusLabel = QLabel()
        self.statusText = self.tr("Status")
        self.statusLabel.setText(self.statusText)
        self.statusBar().addWidget(self.statusLabel)

    def fileOpenSlot(self):
        tracefile = QFileDialog.getOpenFileName(self, self.tr("select file"), os.getcwd())
        if ((tracefile != None) and os.path.isfile(tracefile)):
            self.control.simDB.analyzeTraceFile(4, tracefile)

    def setSimulatorPath(self):
        self.setDialog = QInputDialog(self)
        self.setDialog.setWindowTitle(self.tr("Setting"))
        self.setDialog.setFixedSize(400, 200)
        path, ok = QInputDialog.getText(self, self.tr("Simulator path setting"), 
                                        self.tr("Simulator path:"), 
                                        mode = QLineEdit.Normal, 
                                        text = self.config.
                                        getConfig("simulatorpath"))
        if ok:
            self.config.setConfig("simulatorpath", path)
 
    def closeEvent(self, event):
        self.config.writeXML()
        self.control.stop()
        event.accept()



