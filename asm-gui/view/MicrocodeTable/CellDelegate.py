# -*- coding: utf-8 -*-

from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class CellDelegate(QItemDelegate):
    floatDialogShowSignal = pyqtSignal(int, int, str)
    floatDialogCloseSignal = pyqtSignal()
    def __init__(self, parent = None):
        super(CellDelegate, self).__init__(parent)

    def createEditor(self, parent, option, index):
        self.lineEdit = QLineEdit(parent)
        self.tableRow = index.row()
        self.tableColumn = index.column()
        self.connect(self.lineEdit, SIGNAL("editingFinished()"), self.commitAndCloseEditor)
        self.connect(self.lineEdit, SIGNAL("textChanged(QString)"), self.textChangedSlot)
        return self.lineEdit

    def commitAndCloseEditor(self):
        self.floatDialogCloseSignal.emit()

    def textChangedSlot(self, text):
        self.floatDialogShowSignal.emit(self.tableRow, self.tableColumn, str(text))

    def lineEditClear(self):
        self.lineEdit.clear()

