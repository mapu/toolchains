#!/usr/bin/env python
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
from view.MainWindow import*

app = QApplication(sys.argv)
main = MainWindow()
main.move((QApplication.desktop().width()-main.width())/2, (QApplication.desktop().height()-main.height())/2) 
main.show()

app.exec_()
