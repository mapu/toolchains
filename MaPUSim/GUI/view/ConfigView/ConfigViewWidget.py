# -*- coding =utf-8 -*-
from PyQt4.QtCore import pyqtSignal, Qt, QProcess
from PyQt4.QtGui import QWidget, QPushButton, QLabel, QLineEdit,\
    QHBoxLayout, QVBoxLayout, QCheckBox, QGroupBox, QRadioButton, QScrollArea,\
    QBoxLayout, QGridLayout, QFileDialog, QButtonGroup
from view.Utils import warning
import os

class ConfigViewWidget(QWidget):
    #define signal
    APCSimulatorDoneSignal = pyqtSignal(int, str)
    APCSimulatorShowSignal = pyqtSignal(int, str)
    ARMSimulatorShowSignal = pyqtSignal(int, str)
    ARMUart0StartProcess = pyqtSignal(int, str)  # 0,gem5; 1,qemu
    APCSimulatorStatusSignal = pyqtSignal(bool)
    ARMSimulatorStatusSignal = pyqtSignal(bool)
    ARMProcessEndSignal = pyqtSignal()
    
    def __init__(self, config, control, parent = None):
        super(ConfigViewWidget, self).__init__(parent)
        self.config = config
        self.control = control

        # Group for APC standalone mode
        #=======================================================================
        # Trace flags configuration is temporarily disabled now
        #=======================================================================
        #traceFlagsButton = QPushButton(self.tr("Trace flags..."))
        #traceFlagsButton.setFixedSize(150, 25)
        self.saTraceLabel = QLabel(self.tr("Trace file:"))
        self.saTraceLabel.setFixedSize(110, 25)
        self.saTraceEdit = QLineEdit(self.config.getConfig("standalonetrace"))
        self.saTraceEdit.setFixedSize(500, 25)
        self.saTraceEdit.textChanged.connect(self.imageUpdateSlot)
        self.saTraceLayout = QHBoxLayout()
        self.saTraceLayout.addWidget(self.saTraceLabel)
        self.saTraceLayout.addWidget(self.saTraceEdit)
        blank = QLabel()
        blank.setFixedSize(520, 25)
        self.saTraceLayout.addWidget(blank)
        
        self.APCLayout = QVBoxLayout()
        self.APCLayout.addLayout(self.saTraceLayout)
        
         
        self.APELayout = [QHBoxLayout() for _ in range(8)]
        self.SPULabel = [QLabel(self.tr("SPU")) for _ in range(4)]
        self.MPULabel = [QLabel(self.tr("MPU")) for _ in range(4)]
        self.SPUImage = [QLineEdit() for _ in range(4)]
        self.MPUImage = [QLineEdit() for _ in range(4)]
        self.browseBotton = [QPushButton(self.tr("Browse ..."))
                             for _ in range(8)]
        self.APECheckBox = [QCheckBox() for _ in range(3)]
        self.APECheckSlot = [self.APE1CheckSlot, self.APE2CheckSlot,
                             self.APE3CheckSlot]
        for i in range(4):
            if i == 0:
                self.APELabel = QLabel(self.tr("APE%d" % i))
                self.APELabel.setFixedHeight(25)
                self.APCLayout.addWidget(self.APELabel)
            else:
                self.APECheckBox[i - 1].setText(self.tr("APE%d" % i))
                self.APECheckBox[i - 1].setFixedHeight(25)
                self.APCLayout.addWidget(self.APECheckBox[i - 1])
                self.APECheckBox[i - 1].clicked.connect(self.APECheckSlot[i - 1])
            self.SPULabel[i].setFixedSize(60, 25)
            self.SPUImage[i].setText(self.config.getConfig("APE%dSPUimage" % i))
            self.SPUImage[i].setFixedSize(540, 25)
            self.SPUImage[i].textChanged.connect(self.imageUpdateSlot)
            self.MPULabel[i].setFixedSize(60, 25)
            self.MPUImage[i].setText(self.config.getConfig("APE%dMPUimage" % i))
            self.MPUImage[i].setFixedSize(540, 25)
            self.MPUImage[i].textChanged.connect(self.imageUpdateSlot)
            self.browseBotton[i << 1].setFixedSize(100, 25)
            self.browseBotton[i << 1].clicked.connect(self.imageBrowseSlot)
            self.browseBotton[(i << 1) + 1].setFixedSize(100, 25)
            self.browseBotton[(i << 1) + 1].clicked.connect(self.imageBrowseSlot)
            self.APELayout[i << 1].setAlignment(Qt.AlignLeft)
            self.APELayout[i << 1].addWidget(self.SPULabel[i])
            self.APELayout[i << 1].addWidget(self.SPUImage[i])
            self.APELayout[i << 1].addWidget(self.browseBotton[i << 1])
            self.APELayout[(i << 1) + 1].setAlignment(Qt.AlignLeft)
            self.APELayout[(i << 1) + 1].addWidget(self.MPULabel[i])
            self.APELayout[(i << 1) + 1].addWidget(self.MPUImage[i])
            self.APELayout[(i << 1) + 1].addWidget(self.browseBotton[(i << 1) + 1])
            self.APCLayout.addLayout(self.APELayout[i << 1])
            self.APCLayout.addLayout(self.APELayout[(i << 1) + 1])

        self.APCGroup = QGroupBox(self.tr("APC standalone system"))
        self.APCGroup.setAlignment(Qt.AlignLeft)
        self.APCGroup.setCheckable(True)
        self.GroupSwitchWidget = [QWidget(), QWidget()]
        self.GroupSwitchWidget[1].setLayout(self.APCLayout)
        self.GroupLayout = [QVBoxLayout(), QVBoxLayout()]
        self.GroupLayout[1].addWidget(self.GroupSwitchWidget[1])
        self.APCGroup.setLayout(self.GroupLayout[1])
        self.APCGroup.setStyleSheet("""
        .QWidget {
            border: 2px solid gray;
            border-radius: 6px;
            }
        """)

        # Group for full system mode
        self.gem5Radio = QRadioButton("GEM5")
        self.qemuRadio = QRadioButton("QEMU")
        self.ARMmodeGroup = QButtonGroup(parent)
        self.ARMmodeGroup.setExclusive(True)
        self.ARMmodeGroup.addButton(self.qemuRadio, 0)
        self.ARMmodeGroup.addButton(self.gem5Radio, 1)
        self.ARMmodeGroup.buttonClicked[int].connect(self.ARMSimulatorSwitchSlot)
        if self.config.getConfig("ARMSimType") == "QEMU":
            self.qemuRadio.setChecked(True)
        else:
            self.gem5Radio.setChecked(True)
        self.flashImageLabel = QLabel(self.tr("Flash image file:"))
        self.flashImageLabel.setFixedSize(110, 25)
        self.flashImageBrowseButton = QPushButton(self.tr("Browse ..."))
        self.flashImageBrowseButton.setFixedSize(100, 25)
        self.flashImageBrowseButton.clicked.connect(self.imageBrowseSlot)
        self.flashImageEdit = QLineEdit(self.config.getConfig("flashimage"))
        self.flashImageEdit.setFixedSize(500, 25)
        self.flashImageEdit.textChanged.connect(self.imageUpdateSlot)
        self.fullTraceLabel = QLabel(self.tr("Trace file:"))
        self.fullTraceLabel.setFixedSize(110, 25)
        self.fullTracefile = QLineEdit(self.config.getConfig("fulltrace"))
        self.fullTracefile.setFixedSize(500, 25)
        self.fullTracefile.textChanged.connect(self.imageUpdateSlot)

        self.fullGroup = QGroupBox(self.tr("Full system"))
        self.fullGroup.setAlignment(Qt.AlignLeft)
        self.fullGroup.setCheckable(True)
        self.fullGroup.setStyleSheet("""
        .QWidget {
            border: 2px solid gray;
            border-radius: 6px;
            }
        """)
        
        self.fullLayout = QVBoxLayout()
        self.ARMmodeLayout = QHBoxLayout()
        self.ARMmodeLayout.addWidget(self.gem5Radio)
        self.ARMmodeLayout.addWidget(self.qemuRadio)
        self.fullLayout.addLayout(self.ARMmodeLayout)

        self.flashImageLayout = QHBoxLayout()
        self.flashImageLayout.setAlignment(Qt.AlignLeft)
        self.flashImageLayout.addWidget(self.flashImageLabel)
        self.flashImageLayout.addWidget(self.flashImageEdit)
        self.flashImageLayout.addWidget(self.flashImageBrowseButton)
        self.fullLayout.addLayout(self.flashImageLayout)
        self.fullLayout.setDirection(QBoxLayout.TopToBottom)
        self.fullTraceLayout = QHBoxLayout()
        self.fullTraceLayout.setAlignment(Qt.AlignLeft)
        self.fullTraceLayout.addWidget(self.fullTraceLabel)
        self.fullTraceLayout.setDirection(QBoxLayout.LeftToRight)
        self.fullTraceLayout.addWidget(self.fullTracefile)
        self.fullLayout.addLayout(self.fullTraceLayout)

        self.GroupSwitchWidget[0].setLayout(self.fullLayout)
        self.GroupLayout[0].addWidget(self.GroupSwitchWidget[0])
        self.fullGroup.setLayout(self.GroupLayout[0])
        
        #main layout
        self.configLayout = QVBoxLayout()
        self.configLayout.addWidget(self.fullGroup)
        self.configLayout.addWidget(self.APCGroup)

        self.fixedWidget = QWidget()
        self.fixedWidget.setFixedSize(780, 590)
        self.fixedWidget.setLayout(self.configLayout)

        self.fixedLayout = QGridLayout()
        self.fixedLayout.addWidget(self.fixedWidget, 0, 0)    
        self.widget = QWidget()
        self.widget.setMinimumSize(800, 600)
        self.widget.setLayout(self.fixedLayout)
     
        #scroll
        self.scroll = QScrollArea()
        self.scroll.setWidget(self.widget)
        self.scroll.setAutoFillBackground(True)
        self.scroll.setWidgetResizable(True)
        self.vbox = QGridLayout()
        self.vbox.addWidget(self.scroll, 0, 0)  
        self.setLayout(self.vbox)

        self.fullGroup.clicked.connect(self.modeSwitchSlot)
        self.APCGroup.clicked.connect(self.modeSwitchSlot)
        
        self.modeSwitch()
        
    def modeSwitch(self):
        '''
        This method switches the configure view between full system mode
        and standalone mode
        '''
        if self.config.getConfig("isFullsystem") == "True":
            self.APCGroup.setChecked(False)
            self.fullGroup.setChecked(True)
            self.GroupSwitchWidget[0].setEnabled(True)
            self.GroupSwitchWidget[1].setEnabled(False)
        else:
            self.APCGroup.setChecked(True)
            self.fullGroup.setChecked(False)
            self.GroupSwitchWidget[0].setEnabled(False)
            self.GroupSwitchWidget[1].setEnabled(True)
            
    def modeSwitchSlot(self):
        '''
        This slot handles the mode selection
        '''
        if self.sender() == self.fullGroup:
            self.config.setConfig("isFullsystem", 
                                  "%s" % self.fullGroup.isChecked())
        elif self.sender() == self.APCGroup:
            self.config.setConfig("isFullsystem", 
                                  "%s" % (not self.APCGroup.isChecked()))
        else:
            warning(self.tr("Unknown signal sender!"), self.tr("Invalid configuration"))
            return
        self.modeSwitch()

    def APE1CheckSlot(self):
        if self.APECheckBox[0].isChecked():
            self.SPUImage[1].setEnabled(True)
            self.browseBotton[2].setEnabled(True)
            self.MPUImage[1].setEnabled(True)
            self.browseBotton[3].setEnabled(True)
            self.config.setConfig("numberofAPEs", "2")
        else:
            self.SPUImage[1].setEnabled(False)
            self.browseBotton[2].setEnabled(False)
            self.MPUImage[1].setEnabled(False)
            self.browseBotton[3].setEnabled(False)
            self.APECheckBox[2].setCheckState(Qt.Unchecked)
            self.APE2CheckSlot()
            self.config.setConfig("numberofAPEs", "1")

    def APE2CheckSlot(self):
        if self.APECheckBox[1].isChecked():
            self.SPUImage[2].setEnabled(True)
            self.browseBotton[4].setEnabled(True)
            self.MPUImage[2].setEnabled(True)
            self.browseBotton[5].setEnabled(True)
            self.config.setConfig("numberofAPEs", "3")
        else:
            self.SPUImage[2].setEnabled(False)
            self.browseBotton[4].setEnabled(False)
            self.MPUImage[2].setEnabled(False)
            self.browseBotton[5].setEnabled(False)
            self.APECheckBox[3].setCheckState(Qt.Unchecked)
            self.APE3CheckSlot()
            self.config.setConfig("numberofAPEs", "2")

    def APE3CheckSlot(self):
        if self.APECheckBox[2].isChecked():
            self.SPUImage[3].setEnabled(True)
            self.browseBotton[6].setEnabled(True)
            self.MPUImage[3].setEnabled(True)
            self.browseBotton[7].setEnabled(True)
            self.config.setConfig("numberofAPEs", "4")
        else:
            self.SPUImage[3].setEnabled(False)
            self.browseBotton[6].setEnabled(False)
            self.MPUImage[3].setEnabled(False)
            self.browseBotton[7].setEnabled(False)
            self.config.setConfig("numberofAPEs", "3")   

    def imageBrowseSlot(self):
        path = QFileDialog.getOpenFileName(self, self.tr("Select image file"), os.getcwd())
        if path == None:
            return
        if self.sender() in self.browseBotton:
            idx = self.browseBotton.index(self.sender())
            if idx & 1 == 0:
                self.SPUImage[idx >> 1].setText(path)
            else:
                self.MPUImage[idx >> 1].setText(path)
            return
        if self.sender() == self.flashImageBrowseButton:
            self.flashImageEdit.setText(path)
            return
    
    def ARMSimulatorSwitchSlot(self, buttonid):
        if buttonid == 0:
            self.config.setConfig("ARMSimType", "QEMU")
        elif buttonid == 1:
            self.config.setConfig("ARMSimType", "GEM5")
        self.control.ARMSimulatorSwitch()
        return

    def imageUpdateSlot(self):
        if self.sender() in self.SPUImage:
            i = self.SPUImage.index(self.sender())
            self.config.setConfig("APE%dSPUImage" % i, self.sender().text())
            return
        if self.sender() in self.MPUImage:
            i = self.MPUImage.index(self.sender())
            self.config.setConfig("APE%dMPUImage" % i, self.sender().text())
            return
        if self.sender() == self.flashImageEdit:
            self.config.setConfig("flashimage", self.sender().text())
            return
        if self.sender() == self.fullTracefile:
            self.config.setConfig("fulltrace", self.sender().text())
            return
        if self.sender() == self.saTraceEdit:
            self.config.setConfig("standalonetrace", self.sender().text())
            return


        
