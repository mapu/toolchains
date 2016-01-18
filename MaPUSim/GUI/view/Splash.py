'''
Created on Dec 17, 2015

@author: wangl
'''

from PyQt4.QtGui import QSplashScreen, QPixmap

class SplashScreen(QSplashScreen):
    def __init__(self):
        super(SplashScreen, self).__init__(QPixmap(":/splash.jpg"))
