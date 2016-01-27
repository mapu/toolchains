'''
Created on Jan 19, 2016

@author: wangl
'''
from PyQt4.QtGui import QFont
from QTermWidget import QTermWidget
from PyQt4.QtCore import pyqtSlot, QTimer, SIGNAL
import time

class ARMGDBWidget(QTermWidget):
    def __init__(self, config, control, parent = None):
        super(QTermWidget, self).__init__()

        self.config = config
        self.control = control
        self.setScrollBarPosition(QTermWidget.ScrollBarRight)
        font = QFont("Monospace")
        self.setTerminalFont(font)
        self.control.signalGDBStart.connect(self.startProcess)

    @pyqtSlot(int)
    def startProcess(self, port):
        '''
        Start ARM GDB process in terminal
        '''
        self.port = port
        self.timer = QTimer()
        self.timer.setInterval(5000)#delay 5s
        self.timer.start()
        self.connect(self.timer, SIGNAL("timeout()"), self.timeOut)

    @pyqtSlot()
    def timeOut(self):
        self.timer.stop()
        self.setShellProgram(self.config.getConfig("ARMGDBpath"))
        self.setArgs([])
        self.startShellProgram()
        self.sendText("target remote localhost:%d\n" % self.port)
