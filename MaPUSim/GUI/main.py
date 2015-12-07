#!/usr/bin/env python
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
import time
from view.MainWindow import*

app = QApplication(sys.argv)

pixmap=QPixmap(":/splash.jpg")
splash=QSplashScreen(pixmap)
splash.show()
splash.showMessage("Loading Module")
app.processEvents()
time.sleep(1)
main = MainWindow(sys.argv)
main.show()
main.move((QApplication.desktop().width()-main.width())/2,(QApplication.desktop().height()-main.height())/2) 
splash.finish(main)
app.exec_()


