# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt, pyqtSignal, SIGNAL, pyqtSlot
from PyQt4.QtGui import QDialog, QGroupBox, QRadioButton, QVBoxLayout, QHBoxLayout, QPushButton

class InsertDialog(QDialog):
    insertOperateSignal = pyqtSignal(int)
    def __init__(self, parent = None):
        super(InsertDialog, self).__init__(parent)
	
        self.setWindowTitle("Insert")
        self.setFixedSize(255, 200)
        groupBox = QGroupBox()
        self.cellRightRadio = QRadioButton("The active cell to the right")
        self.connect(self.cellRightRadio, SIGNAL("clicked(bool)"), self.cellRightSlot)
        self.cellRightRadio.setChecked(True)
        self.radioIndex = 0
        self.cellDownRadio = QRadioButton("The active cell to the bottom")
        self.connect(self.cellDownRadio, SIGNAL("clicked(bool)"), self.cellDownSlot)
        self.wholeRowsRadio = QRadioButton("The whole rows")
        self.connect(self.wholeRowsRadio, SIGNAL("clicked(bool)"), self.wholeRowsSlot)
        self.wholeColumnsRadio = QRadioButton("The whole columns")
        self.connect(self.wholeColumnsRadio, SIGNAL("clicked(bool)"), self.wholeColumnsSlot)
        radioLay = QVBoxLayout()
        radioLay.addWidget(self.cellRightRadio)
        radioLay.addWidget(self.cellDownRadio)
        radioLay.addWidget(self.wholeRowsRadio)
        radioLay.addWidget(self.wholeColumnsRadio)
        groupBox.setLayout(radioLay)	

        self.OKButton = QPushButton("OK")
        self.connect(self.OKButton, SIGNAL("clicked()"), self.OKSlot)
        self.cancelButton = QPushButton("Cancel")
        self.connect(self.cancelButton, SIGNAL("clicked()"), self.cancelSlot)
        buttonLay = QHBoxLayout()
        buttonLay.addWidget(self.OKButton)
        buttonLay.addWidget(self.cancelButton)
	
        mainLay = QVBoxLayout(self)
        mainLay.addWidget(groupBox)
        mainLay.addLayout(buttonLay)

    @pyqtSlot(bool)
    def cellRightSlot(self, flag):
        if flag == True:
            self.radioIndex = 0

    @pyqtSlot(bool)
    def cellDownSlot(self, flag):
        if flag == True:
            self.radioIndex = 1

    @pyqtSlot(bool)
    def wholeRowsSlot(self, flag):
        if flag == True:
            self.radioIndex = 2

    @pyqtSlot(bool)
    def wholeColumnsSlot(self, flag):
        if flag == True:
            self.radioIndex = 3

    @pyqtSlot()
    def OKSlot(self):
        self.insertOperateSignal.emit(self.radioIndex)
        self.close()

    @pyqtSlot()
    def cancelSlot(self):
        self.close()


