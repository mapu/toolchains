#! /usr/bin/python
import sys
from PyQt4.QtGui import *
from PyQt4.QtCore import *

from view.mainwindow import MainWindow

# This is GUI of MaPU simulator

app = QApplication(sys.argv) 
main = MainWindow()  
main.show()  
app.exec_()

