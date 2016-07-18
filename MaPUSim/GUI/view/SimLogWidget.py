'''
Created on Dec 20, 2015

@author: wangl
'''
from PyQt4.QtGui import QTextEdit, QColor
from PyQt4.QtCore import QTimer, QProcess

class SimLogWidget(QTextEdit):
    '''
    The configuration of the GUI
    '''


    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(SimLogWidget, self).__init__(parent)
        
        self.setReadOnly(True)
        self.updateTimer = QTimer(self)
        self.updateTimer.setSingleShot(False)
        self.updateTimer.timeout.connect(self.updateTextView)
        self.log = ""

    def updateTextView(self):
        '''
        Refresh the TextEdit every 1000ms for updated log
        '''
        string = self.log
        #self.clear()       
        if str(string).find("fatal") == True:
            defaultColor = self.textColor()
            num = string.count("\n")
            while num != 0:
                pos = string.indexOf("\n")
                str1 = string.left(pos)
                string = string.right(string.size() - pos - 1)
                if str1.contains("fatal") == True:
                    self.setTextColor(QColor("red"))
                    self.append(str1)
                    self.setTextColor(defaultColor)
                else:
                    self.append(str1)
                num -= 1
	else:
	    self.append(string)
	    
        self.log = ""
                    
    def updateSwitch(self, state):
        '''
        This slot start or stop the log updating
        '''
        if state == QProcess.Running:
            self.updateTimer.start(1000)
        elif state == QProcess.NotRunning:
            self.updateTimer.stop()
            self.updateTextView()
            #self.log = ""
            
    def logUpdate(self, text):
        self.log += text
        self.update()
        