# -*- coding = utf-8 -*-
from PyQt4.QtCore import Qt, pyqtSignal, pyqtSlot
from PyQt4.QtGui import QDialog, QGroupBox, QRadioButton, QPushButton, QHBoxLayout, QVBoxLayout

class DeleteDialog(QDialog):
    deleteOperateSignal = pyqtSignal(int)
    def __init__(self, parent = None):
        super(DeleteDialog, self).__init__(parent)

        self.setWindowTitle("Delete")
        self.setFixedSize(250, 200)
        groupBox = QGroupBox()
        self.cellLeftRadio = QRadioButton("The active cell to the right")
        self.cellLeftRadio.setChecked(True)
        self.connect(self.cellLeftRadio, SIGNAL("clicked(bool)"), self.cellLeftSlot)
        self.radioIndex = 0
        self.cellUpRadio = QRadioButton("The active cell to the down")
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

    def cellLeftSlot(self, flag):
        if flag == True:
            self.radioIndex = 0

    def cellUpSlot(self, flag):
        if flag == True:
            self.radioIndex = 1

    def wholeRowsSlot(self, flag):
        if flag == True:
            self.radioIndex = 2

    def wholeColumnsSlot(self, flag):
        if flag == True:
            self.radioIndex = 3

    def OKSlot(self):
        self.deleteOperateSignal.emit(self.radioIndex)
        self.close()

    def cancelSlot(self):
        self.close()





