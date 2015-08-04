#!/usr/bin/env python
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
from view.MainWindow import*

app = QApplication(sys.argv)
main = MainWindow(sys.argv)
main.show()

app.exec_()


