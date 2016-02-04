# -*- coding: utf-8 -*-   
from PyQt4.QtGui import QDialog, QRegExpValidator, QPushButton, QHBoxLayout, QVBoxLayout, QLineEdit
from PyQt4.QtCore import pyqtSignal, pyqtSlot, SIGNAL, Qt, QRegExp
from view.Utils import warning

class SetFSMNameWidget(QDialog):  
    setFSMNameSignal = pyqtSignal(int, str)
    def __init__(self, column, parent = None):  
        super(SetFSMNameWidget, self).__init__(parent)   

        self.lineEdit = QLineEdit()
        regx = QRegExp("[a-zA-Z]+[0-9]+$")
        validator = QRegExpValidator(regx, self.lineEdit)
        self.lineEdit.setValidator(validator)

        self.OKButton = QPushButton("OK")
        self.cancelButton = QPushButton("Cancel")	

        lay = QHBoxLayout()
        lay.addWidget(self.OKButton)
        lay.addWidget(self.cancelButton)
        mainLay = QVBoxLayout()
        mainLay.addWidget(self.lineEdit)
        mainLay.addLayout(lay)

        self.setLayout(mainLay)
        self.column = column

        self.connect(self.OKButton, SIGNAL("clicked()"), self.OKButtonSlot)
        self.connect(self.cancelButton, SIGNAL("clicked()"), self.cancelButtonSlot)

    def OKButtonSlot(self):
        text = self.lineEdit.text()
        if text != "":
            self.close()
            self.setFSMNameSignal.emit(self.column, text)
        else:
            warning("Please set FSM name")
	
    def cancelButtonSlot(self):
        self.close()

