'''
Created on Dec 24, 2015

@author: wangl
'''
from PyQt4.QtGui import QWidget, QLabel, QLineEdit, QPushButton, QHBoxLayout
from PyQt4.QtCore import SIGNAL
from view.Utils import warning

class SearchWidget(QWidget):
    '''
    Widget provide searching function with "Previous" and "Next" buttons
    '''


    def __init__(self, attributes, table, parent = None):
        '''
        Constructor
        '''
        super(SearchWidget, self).__init__(parent)
        self.attributes = attributes
        self.table = table
        
        self.searchLabel = QLabel(self.tr("Search text:"))
        self.searchLabel.setFixedSize(30, 25)
        self.searchEdit = QLineEdit()
        self.searchLabel.setBuddy(self.searchEdit)
        self.searchEdit.setFixedSize(200, 25)
        
        self.searchButton = QPushButton(self.tr("Search"))
        self.searchButton.setFixedSize(100, 25)
        self.connect(self.searchButton, SIGNAL("clicked()"), self.searchSlot)
        self.preButton = QPushButton(self.tr("Previous"))
        self.preButton.setEnabled(False)
        self.preButton.setFixedSize(100, 25)
        self.connect(self.preButton, SIGNAL("clicked()"), self.previousSlot)
        self.nextButton = QPushButton(self.tr("Next"))
        self.nextButton.setEnabled(False)
        self.nextButton.setFixedSize(100, 25)
        self.connect(self.nextButton, SIGNAL("clicked()"), self.nextSlot)
        
        self.searchLayout = QHBoxLayout()
        
        self.searchLayout.addWidget(self.searchLabel)
        self.searchLayout.addWidget(self.searchEdit)
        self.searchLayout.addWidget(self.searchButton)
        self.searchLayout.addWidget(self.preButton)
        self.searchLayout.addWidget(self.nextButton)
        
        self.setLayout(self.searchLayout)
        
        self.searchResult = []
        self.currentResult = 0

    def searchSlot(self):
        self.preButton.setEnabled(False)
        self.nextButton.setEnabled(False)
        key = self.searchEdit.text()
        if key == "":
            return
        self.tableView.setStyleSheet("QHeaderView.section{color: black;}")
        self.tableModel.key = key
        self.searchResult = self.table.searchRecord(key, self.attributes)
        if len(self.searchResult) != 0:
            if len(self.searchResult) > 1:
                self.nextButton.setEnabled(True)
            self.tableView.verticalScrollBar().setValue(self.searchResult[0])
            self.currentResult = 0
            self.tableModel.refrushModel()
        else:
            warning("Cannot find %s!", key)

    def previousSlot(self):
        if self.currentResult > 0:
            self.currentResult -= 1
            self.nextButton.setEnabled(True)
            self.tableView.verticalScrollBar().setValue(self.searchList[self.currentResult])
            if self.searchValue == 0:
                self.preButton.setEnabled(False)

    def nextSlot(self):
        if self.currentResult < len(self.searchList) - 1:
            self.currentResult += 1
            self.preButton.setEnabled(True)
            self.tableView.verticalScrollBar().setValue(self.searchList[self.currentResult])
            if self.currentResult == len(self.searchList) - 1:
                self.nextButton.setEnabled(False)
        
        