from PyQt4.QtGui import QMessageBox

mainWindow = None

def initParent(parent):
    global mainWindow
    mainWindow = parent

def fatal(message, title = "Fatal error"):
    QMessageBox.critical(mainWindow, title, message, buttons = QMessageBox.Ok,
                         defaultButton = QMessageBox.NoButton);

def warning(message, title = "Warning"):
    QMessageBox.warning(mainWindow, title, message, buttons = QMessageBox.Ok,
                        defaultButton = QMessageBox.NoButton);
