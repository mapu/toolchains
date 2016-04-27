# -*- coding = utf-8 -*-
from PyQt4.QtCore import Qt, pyqtSignal, SIGNAL, pyqtSlot
from PyQt4.QtGui import QDialog, QGroupBox, QRadioButton, QPushButton, QHBoxLayout, QVBoxLayout

class DeleteDialog(QDialog):
    deleteOperateSignal = pyqtSignal(int)
    def __init__(self, parent = None):
        super(DeleteDialog, self).__init__(parent)

        self.setWindowTitle("Delete")
        self.setFixedSize(255, 200)
        groupBox = QGroupBox()
        self.cellLeftRadio = QRadioButton("The right cell to the left")
        self.cellLeftRadio.setChecked(True)
        self.connect(self.cellLeftRadio, SIGNAL("clicked(bool)"), self.cellLeftSlot)
        self.radioIndex = 0
        self.cellUpRadio = QRadioButton("The bottom cell to the up")
        self.connect(self.cellUpRadio, SIGNAL("clicked(bool)"), self.cellUpSlot)
        self.wholeRowsRadio = QRadioButton("The whole rows")
        self.connect(self.wholeRowsRadio, SIGNAL("clicked(bool)"), self.wholeRowsSlot)
        self.wholeColumnsRadio = QRadioButton("The whole columns")
        self.connect(self.wholeColumnsRadio, SIGNAL("clicked(bool)"), self.wholeColumnsSlot)
        radioLay = QVBoxLayout()
        radioLay.addWidget(self.cellLeftRadio)
        radioLay.addWidget(self.cellUpRadio)
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
    def cellLeftSlot(self, flag):
        if flag == True:
            self.radioIndex = 0

    @pyqtSlot(bool)
    def cellUpSlot(self, flag):
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
        self.deleteOperateSignal.emit(self.radioIndex)
        self.close()

    @pyqtSlot()
    def cancelSlot(self):
        self.close()





