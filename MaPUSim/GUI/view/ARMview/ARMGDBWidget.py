'''
Created on Jan 19, 2016

@author: wangl
'''
from PyQt4.QtGui import QFont
from QTermWidget import QTermWidget
from PyQt4.QtCore import pyqtSlot

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
        self.setShellProgram(self.config.getConfig("ARMGDBpath"))
        self.setArgs([])
        self.startShellProgram()
        self.sendText("target remote localhost:%d\n" % port)