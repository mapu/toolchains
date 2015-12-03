#!/usr/bin/env python
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
from view.MainWindow import*

app = QApplication(sys.argv)

pixmap=QPixmap(":/splash.jpg")
splash=QSplashScreen(pixmap)
splash.show()
splash.showMessage("Loading Module")
app.processEvents()

main = MainWindow(sys.argv)
main.show()
splash.finish(main)
app.exec_()


