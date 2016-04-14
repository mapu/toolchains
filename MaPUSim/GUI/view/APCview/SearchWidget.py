'''
Created on Dec 24, 2015

@author: wangl
'''
from PyQt4.QtGui import QWidget, QLabel, QLineEdit, QPushButton, QHBoxLayout, QComboBox
from PyQt4.QtCore import SIGNAL, pyqtSignal, pyqtSlot
from view.Utils import warning

class SearchWidget(QWidget):
    '''
    Widget provide searching function with "Previous" and "Next" buttons
    '''
    pageChangedSignal = pyqtSignal(int)

    def __init__(self, view, model, parent = None):
        '''
        Constructor
        '''
        super(SearchWidget, self).__init__(parent)
        self.tableView = view
        self.tableModel = model
        
        self.searchLabel = QLabel(self.tr("Search text:"))
        self.searchLabel.setFixedSize(80, 25)
        self.searchEdit = QLineEdit()
        self.searchLabel.setBuddy(self.searchEdit)
        self.searchEdit.setFixedSize(200, 25)
        
        self.searchButton = QPushButton(self.tr("Search"))
        self.searchButton.setFixedSize(100, 25)
        self.connect(self.searchButton, SIGNAL("clicked()"), self.searchSlot)
        self.searchCombo = QComboBox()
        self.searchCombo.addItem("Global Search")
        self.searchCombo.addItem("Current Search")
        self.searchCombo.setFixedSize(120, 25)
        self.searchCombo.setCurrentIndex(0)
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
        self.searchLayout.addWidget(self.searchCombo)
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
        self.mode = self.searchCombo.currentIndex()
        self.searchResult = self.tableModel.searchHeader(key, self.mode)
        if len(self.searchResult) != 0:
            if self.mode == 0:
                self.pageChangedSignal.emit(0)
                if len(self.searchResult) > 1:
                    self.nextButton.setEnabled(True)
                if self.searchResult[0] < self.tableModel.idList[self.tableModel.currentPage]:  
                    self.tableView.verticalScrollBar().setValue(self.searchResult[0])
                else:
                    page = self.tableModel.currentPage
                    for i in xrange(page + 1, len(self.tableModel.idList)):
                        page += 1
                        if self.searchResult[0] < self.tableModel.idList[i]:
                            break       
                    self.pageChangedSignal.emit(page)
                    self.tableView.verticalScrollBar().setValue(self.tableModel.idList[self.tableModel.currentPage] - self.tableModel.idList[self.tableModel.currentPage - 1] - 1)
                self.currentResult = 0
                self.tableModel.refrushModel()
            else:
                if len(self.searchResult) > 1:
                    self.nextButton.setEnabled(True)
                self.tableView.verticalScrollBar().setValue(self.searchResult[0])
                self.currentResult = 0
                self.tableModel.refrushModel()
        else:
            warning("Cannot find \"%s\"!" % key)	    

    def previousSlot(self):
        if self.currentResult > 0:
            self.currentResult -= 1
            self.nextButton.setEnabled(True)
            if self.mode == 0:
                page = self.tableModel.currentPage
                if self.searchResult[self.currentResult] < self.tableModel.idList[page]:
                    if page == 0:
                        value = self.searchResult[self.currentResult]
                    else:
                        if self.searchResult[self.currentResult] >= self.tableModel.idList[page - 1]:
                            value = self.searchResult[self.currentResult] - self.tableModel.idList[page - 1]
                        else:
                            for i in xrange(self.tableModel.currentPage, 0, -1):
                                page -= 1
                                if page == 0:
                                    value = self.searchResult[self.currentResult]
                                    break
                                else:
                                    if self.searchResult[self.currentResult] >= self.tableModel.idList[page - 1]:
                                        value = self.searchResult[self.currentResult] - self.tableModel.idList[page - 1]
                                        break   
                            self.pageChangedSignal.emit(page)    
                    self.tableView.verticalScrollBar().setValue(value)   
            else:
		self.tableView.verticalScrollBar().setValue(self.searchResult[self.currentResult]) 
            if self.currentResult == 0:
                self.preButton.setEnabled(False)

    def nextSlot(self):
        if self.currentResult < len(self.searchResult) - 1:
            self.currentResult += 1
            self.preButton.setEnabled(True)
            if self.mode == 0:
                page = self.tableModel.currentPage
                if self.searchResult[self.currentResult] < self.tableModel.idList[page]:
                    if page == 0:
                        value = self.searchResult[self.currentResult]
                    else:
                        value = self.searchResult[self.currentResult] - self.tableModel.idList[page - 1]     
                else:
                    for i in xrange(self.tableModel.currentPage + 1, len(self.tableModel.idList)):
                        page += 1
                        if self.searchResult[self.currentResult] < self.tableModel.idList[i]:
                            break       
                    self.pageChangedSignal.emit(page)
                    value = self.searchResult[self.currentResult] - self.tableModel.idList[self.tableModel.currentPage - 1]
                self.tableView.verticalScrollBar().setValue(value)
            else:
		self.tableView.verticalScrollBar().setValue(self.searchResult[self.currentResult])
            if self.currentResult == len(self.searchResult) - 1:
                self.nextButton.setEnabled(False)
        
        
