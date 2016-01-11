from PyQt4.QtGui import QProgressDialog
from PyQt4.QtCore import Qt

class ProgressDialog(QProgressDialog):
    def __init__(self, parent = None):
        super(ProgressDialog, self).__init__(parent)

        self.setFixedSize(300, 100)
        self.setWindowTitle(self.tr("Progress"))
        self.setAttribute (Qt.WA_DeleteOnClose);
        self.setLabelText(self.tr("Trace analyzing ..."))
        self.setCancelButton(None)

    def updateDataBaseDialog(self, value, string, maxValue):
        if maxValue != -1:
            self.setRange(value, maxValue)
        self.setValue(value)
        self.setLabelText(string)
  

