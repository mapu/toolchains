'''
Created on Dec 17, 2015

@author: wangl
'''

from PyQt4.QtCore import Qt
from PyQt4.QtGui import QSplashScreen, QPixmap
import time

class SplashScreen(QSplashScreen):
    def __init__(self):
        super(SplashScreen, self).__init__(QPixmap(":/splash.jpg"),
                                           Qt.WindowStaysOnTopHint)

    def fadein(self):
        #self.setWindowOpacity(0)
        self.show()
        t = 0
        while t <= 50:
            newOpacity = self.windowOpacity() + 0.02
            if newOpacity > 1:
                break

            #self.setWindowOpacity(newOpacity)
            t += 1
            time.sleep(0.01)

    def fadeout(self, window):
        t = 0
        while t <= 50:
            newOpacity = self.windowOpacity() - 0.02
            if newOpacity < 0:
                break

            self.setWindowOpacity(newOpacity)
            t += 1
            time.sleep(0.01)
        self.finish(window)