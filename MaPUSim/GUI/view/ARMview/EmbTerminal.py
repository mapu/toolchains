#!/usr/bin/env python 
#-*- coding:utf-8 -*- 
from PyQt4.QtCore import pyqtSignal
from PyQt4.QtGui import QVBoxLayout, QFont, QWidget
from QTermWidget import QTermWidget

class EmbTerminal(QWidget):
    ARMSimulatorStatusSignal = pyqtSignal(bool)
    ARMSimulatorShowSignal = pyqtSignal(int, str)
    APCSimulatorSignal = pyqtSignal(str, str)
    def __init__(self, config, control, parent = None):
        super(EmbTerminal, self).__init__(parent)

        self.config = config
        self.control = control

        self.lay = QVBoxLayout()
        self.setLayout(self.lay)
        self.termWidget = QTermWidget(0)
        self.termWidget.setScrollBarPosition(2)
        font = QFont("Monospace")
        self.termWidget.setTerminalFont(font)
        self.lay.addWidget(self.termWidget)
        
    def switchMode(self, buttonid):
        '''
        Switch the UART working mode between Gem5 and QEMU
        '''
        if buttonid == 0:
            self.control.ARMQemuProcess.signalUARTStart.connect(
                self.startProcess)
            try:
                self.control.ARMGem5Process.signalUARTStart.disconnect(
                    self.startm5term)
            except Exception: pass
        elif buttonid == 1:
            self.control.ARMGem5Process.signalUARTStart.connect(
                self.startm5term)
            try:
                self.control.ARMQemuProcess.signalUARTStart.disconnect(
                    self.startProcess)
            except Exception: pass
        return
    
    def startm5term(self, port):
        '''
        Start UART simulation for Gem5 mode
        '''
        self.lay.removeWidget(self.termWidget)
        self.termWidget.close()
        del self.termWidget
        self.termWidget = QTermWidget(0)
        self.termWidget.setScrollBarPosition(QTermWidget.ScrollBarRight)
        font = QFont("Monospace")
        self.termWidget.setTerminalFont(font)
        self.lay.addWidget(self.termWidget)
        self.termWidget.setShellProgram(self.config.getConfig("simulatorpath") +
                                        "/arm/utils/m5term")
        args = ["localhost", ("%d" % port)]
        self.termWidget.setArgs(args)
        self.termWidget.startShellProgram()

    def startProcess(self, command, args):
        self.ARMSimulatorStatusSignal.emit(True)
        self.lay.removeWidget(self.termWidget)
        self.termWidget.close()
        del self.termWidget
        self.termWidget = 0
        self.termWidget = QTermWidget(0)
        self.control.bindARMQemuProcess(self.termWidget.getProcess())
        self.termWidget.setScrollBarPosition(2)
        font = QFont("Monospace")
        self.termWidget.setTerminalFont(font)
        self.lay.addWidget(self.termWidget)

        # ARMCommand=self.simulatorPath+"/arm/bin/qemu-system-arm -M mapu -m 512 -pflash "+path+" -serial stdio 2>"+self.errorFile+"\n"  # -gdb tcp::1234 -S
        # self.termWidget.sendText(ARMCommand)
    
        # qemu-system-arm -M mapu -m 512 -pflash /home/litt/simulator/sim_dmac.bin -serial stdio
        #path = self.config.getConfig("simulatorpath")
        #image = self.config.getConfig("flashimage")
        #self.termWidget.setShellProgram(path + "/arm/bin/qemu-system-arm")
        #args = ["-M", "mapu", "-m", "512", "-pflash", image, "-serial", "stdio", ""]
        self.termWidget.setShellProgram(command)
        self.termWidget.setArgs(args)
        self.termWidget.startShellProgram()
    
        #self.watcher = QFileSystemWatcher()
        #self.watcher.addPath(self.errorFile)
        #self.connect(self.watcher, SIGNAL("fileChanged(QString)"), self.fileChangedSlot)
        # self.fileChangedSlot()
