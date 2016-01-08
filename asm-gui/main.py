#!/usr/bin/env python
from PyQt4.QtGui import QApplication
from PyQt4.QtCore import Qt
import sys
from view.MainWindow import MainWindow
from view.Utils import initParent

app = QApplication(sys.argv)
main = MainWindow()
initParent(main)
main.move((QApplication.desktop().width()-main.width())/2, (QApplication.desktop().height()-main.height())/2) 
main.show()

app.exec_()
