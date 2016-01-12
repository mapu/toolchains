# -*- coding: utf-8 -*-  
from PyQt4.QtGui import QWidget, QTabWidget, QVBoxLayout
from EmbTerminal import EmbTerminal
from view.SimLogWidget import SimLogWidget

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class ARMViewWidget(QWidget):  
    def __init__(self, config, control, parent = None):
        super(ARMViewWidget, self).__init__(parent)
        self.config = config
        self.control = control
        self.UART0Widget = EmbTerminal(config, control)
        if self.config.getConfig("ARMSimType") == "QEMU":
            self.UART0Widget.switchMode(0)
        else:
            self.UART0Widget.switchMode(1)
            
        self.UART1Widget = EmbTerminal(config, control)
        self.LCDWidget = QWidget()
        self.logWidget = SimLogWidget()
        self.control.ARMGem5Process.updateLog.connect(self.logWidget.logUpdate)
        self.control.ARMQemuProcess.updateLog.connect(self.logWidget.logUpdate)
        self.control.ARMGem5Process.stateChanged.connect(self.logWidget.updateSwitch)
        self.control.ARMQemuProcess.stateChanged.connect(self.logWidget.updateSwitch)

        self.topTabWidget = QTabWidget()
        self.topTabWidget.addTab(self.UART0Widget, "UART0")
        self.topTabWidget.addTab(self.UART1Widget, "UART1")
        self.topTabWidget.addTab(self.LCDWidget, "LCD")
        self.topTabWidget.addTab(self.logWidget, self.tr("Simulator log"))

        self.GDBWidget = QWidget()
        self.ConsoleWidget = QWidget()
        self.bottomTabWidget = QTabWidget()
        self.bottomTabWidget.setTabPosition(QTabWidget.South)
        self.bottomTabWidget.addTab(self.GDBWidget, "GDB")
        self.bottomTabWidget.addTab(self.ConsoleWidget, self.tr("Console"))

        self.layout = QVBoxLayout()
        self.layout.addWidget(self.topTabWidget)
        self.layout.addWidget(self.bottomTabWidget)
        self.layout.setStretchFactor(self.topTabWidget, 3)
        self.layout.setStretchFactor(self.bottomTabWidget, 2)
        self.setLayout(self.layout)

        #self.UART0Widget.embTerminal.ARMSimulatorShowSignal.connect(self.statusWidget.simulatorShowText)

