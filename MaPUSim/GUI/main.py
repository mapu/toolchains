from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys
from view.MainWindow import*

app = QApplication(sys.argv)

main = MainWindow()
main.show()

app.exec_()


