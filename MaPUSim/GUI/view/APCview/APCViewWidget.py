# -*- coding: utf-8 -*-
from PyQt4.QtCore import pyqtSignal, Qt, SIGNAL, QDir, pyqtSlot
from PyQt4.QtGui import QWidget, QSlider, QSpinBox, QTabWidget, QHBoxLayout,\
    QVBoxLayout
from view.APCview.APCMutiCoreWidget import APCMutiCoreWidget
from view.APCview.APECoreWidget import APECoreWidget
from view.SimLogWidget import SimLogWidget

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class APCViewWidget(QWidget):
    progressShowSignal = pyqtSignal(int, str, int)
    def __init__(self, config, control, parent = None):
        super(APCViewWidget, self).__init__(parent)
        self.config = config
        self.control = control

        self.timeSlider = QSlider(Qt.Horizontal)
        self.timeSlider.setTickPosition(QSlider.TicksBothSides)
        self.timeSlider.setMaximumHeight(30)
        self.timeSlider.setEnabled(False)
            
        self.timeSpinBox = QSpinBox()
        self.timeSpinBox.setMaximumHeight(40)
        self.timeSpinBox.setEnabled(False)
        self.connect(self.timeSpinBox, SIGNAL("valueChanged(int)"),
                     self.control.simDB.setTimePointSlot)
        
        # Synchonize the values of the two widgets -----------------------------
        self.connect(self.timeSlider, SIGNAL("valueChanged(int)"),
                     self.timeSpinBox.valueChanged)
        
        self.control.simDB.timeChanged.connect(self.timeChangedSlot)

        self.APCWidget = APCMutiCoreWidget(self)  # APC mutiple core view
        self.logWidget = SimLogWidget()
        self.control.APCProcess.updateLog.connect(self.logWidget.logUpdate)
        self.control.APCProcess.stateChanged.connect(self.logWidget.updateSwitch)

        # define left tab
        self.APCViewTab = QTabWidget()
        self.APCViewTab.addTab(self.APCWidget, self.tr("APC"))

        self.APETabWidget = []
        for i in range(4):
            self.APETabWidget.append(APECoreWidget(config, control, i))
            self.APCViewTab.addTab(self.APETabWidget[i], self.tr("APE%d" % i))
            
        self.APCWidget.APEButtonGroup.buttonPressed[int].connect(self.APEWidgetSlot)

        self.APCViewTab.addTab(self.logWidget, self.tr("Simulator log"))

        # lay out all widget
        bottomLay = QHBoxLayout()
        bottomLay.addWidget(self.timeSlider)
        bottomLay.addWidget(self.timeSpinBox)
        bottomLay.setStretchFactor(self.timeSlider, 7)
        bottomLay.setStretchFactor(self.timeSpinBox, 1)
        self.mainLayout = QVBoxLayout()
        self.mainLayout.addWidget(self.APCViewTab)
        self.mainLayout.addLayout(bottomLay)
        self.setLayout(self.mainLayout)

        self.mainOpen = 0
        self.databaseFlag = 0

        
    @pyqtSlot(int)
    def APEWidgetSlot(self, i):
        self.APCViewTab.setCurrentWidget(self.APETabWidget[i])
        
    @pyqtSlot()
    def timeChangedSlot(self):
        if self.control.simDB.startPoint <= self.control.simDB.endPoint:
            self.timeSlider.setEnabled(True)
            self.timeSlider.setRange(self.control.simDB.startPoint,
                                     self.control.simDB.endPoint)
            self.timeSpinBox.setEnabled(True)
            self.timeSpinBox.setRange(self.control.simDB.startPoint,
                                      self.control.simDB.endPoint)
            self.timeSpinBox.setValue(self.control.simDB.curTime)
        else:
            self.timeSlider.setEnabled(False)
            self.timeSpinBox.setEnabled(False)

        # update DM bin file
        #dirpath = QDir(".")
        #stringFile = dirpath.entryList(QDir.Files, QDir.Time)
        #for j in range (0, stringFile.count()):
        #    if stringFile[j].indexOf(".bin") > 0:
        #        filename = stringFile[j]
        #        for ape in self.APETabWidget:
        #            ape.MPUWidget.coreWidget.setHexFileStart(filename,  16 * 1024 * 1024 * self.APETabWidget.index(ape))
        #        break

