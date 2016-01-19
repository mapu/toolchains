#!/usr/bin/env python 
#-*- coding:utf-8 -*- 
from PyQt4.QtGui import QFont
from QTermWidget import QTermWidget

class EmbTerminal(QTermWidget):
    def __init__(self, config, control, parent = None):
        super(QTermWidget, self).__init__()

        self.config = config
        self.control = control
        self.setScrollBarPosition(QTermWidget.ScrollBarRight)
        font = QFont("Monospace")
        self.setTerminalFont(font)
        
    def switchMode(self, buttonid):
        '''
        Switch the UART working mode between Gem5 and QEMU
        '''
        if buttonid == 0:
            self.control.ARMQemuProcess.signalUARTStart.connect(
                self.startProcess)
            self.control.bindARMQemuProcess(self.getProcess())
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
            try:
                self.control.unbindARMQemuProcess()
            except Exception: pass
        return
    
    def startm5term(self, port):
        '''
        Start UART simulation for Gem5 mode
        '''
        self.setShellProgram(self.config.getConfig("simulatorpath") +
                             "/arm/utils/m5term")
        args = ["localhost", ("%d" % port)]
        self.setArgs(args)
        self.startShellProgram()

    def startProcess(self, command, args):

        # ARMCommand=self.simulatorPath+"/arm/bin/qemu-system-arm -M mapu -m 512 -pflash "+path+" -serial stdio 2>"+self.errorFile+"\n"  # -gdb tcp::1234 -S
        # self.termWidget.sendText(ARMCommand)
    
        # qemu-system-arm -M mapu -m 512 -pflash /home/litt/simulator/sim_dmac.bin -serial stdio
        #path = self.config.getConfig("simulatorpath")
        #image = self.config.getConfig("flashimage")
        #self.termWidget.setShellProgram(path + "/arm/bin/qemu-system-arm")
        #args = ["-M", "mapu", "-m", "512", "-pflash", image, "-serial", "stdio", ""]
        self.setShellProgram(command)
        self.setArgs(args)
        self.startShellProgram()
        self.control.ARMQemuProcess.signalStarted.emit()
