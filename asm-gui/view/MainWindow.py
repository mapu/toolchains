# -*- coding: utf-8 -*-   
from PyQt4.QtGui import QMainWindow, QToolBar, QAction, QIcon, QCheckBox, QTextEdit, QFileDialog, QLineEdit
from PyQt4.QtCore import pyqtSignal, Qt, SIGNAL, pyqtSlot
from res import qrc_resources 
from view.MicrocodeTableWidget.MicrocodeTableWidget import MicrocodeTableWidget
from Utils import warning
from data.MicrocodeDB import MicrocodeDB

class MainWindow(QMainWindow):  
    def __init__(self, parent = None):  
        super(MainWindow, self).__init__(parent)   

	#create data base
	database = MicrocodeDB()
	database.createTable()
	
        self.register = ["KI12", "KI13", "KI14", "KI15"]
        self.setWindowTitle("Graphic Microcode Editor")
        self.resize(800, 600)
        self.microcodeTableWidget = MicrocodeTableWidget(self.register, database)
        self.setCentralWidget(self.microcodeTableWidget)
        self.createAction()
        self.createContextMenu()
        self.createToolBars()
        self.createMenus()
        self.microcodeTableWidget.itemRegStateSignal.connect(self.itemRegStateSlot)
        self.newFileName = "New File*"

    def createAction(self):
        self.newAction = QAction(self.tr('New'), self)
        self.newAction.setIcon(QIcon(":/new.png"))
        self.newAction.setShortcut("Ctrl+N")
        self.newAction.setStatusTip("Create a new Microcode table")
        self.connect(self.newAction, SIGNAL("triggered()"), self.newFile)

        self.openAction = QAction(QIcon(":/open.png"), self.tr("Open"), self)
        self.openAction.setShortcut("Ctrl+O")
        self.openAction.setStatusTip("Open an existing Microcode file")
        self.connect(self.openAction, SIGNAL("triggered()"), self.openFile)

        self.saveAction = QAction(QIcon(":/save.png"), self.tr("Save"), self)
        self.saveAction.setShortcut("Ctrl+S")
        self.saveAction.setStatusTip("Save the Microcode table to disk")
        self.connect(self.saveAction, SIGNAL("triggered()"), self.saveFile)
	self.saveAction.setEnabled(False)
	
	self.saveasAction = QAction(QIcon(":/saveas.png"), self.tr("Save As"), self)
	self.connect(self.saveasAction, SIGNAL("triggered()"), self.saveasFile)
	self.saveasAction.setEnabled(False)

        self.exitAction = QAction(self.tr("Exit"), self)
        self.exitAction.setShortcut("Ctrl+Q")
        self.exitAction.setStatusTip("Exit the application")
        self.connect(self.exitAction, SIGNAL("triggered()"), self.closeWindow)

        self.cutAction = QAction(QIcon(":/cut.png"), self.tr("Cut"), self)
        self.cutAction.setShortcut("Ctrl+X")
        self.cutAction.setStatusTip("Cut the current selection's contents to the clipboard")
        self.connect(self.cutAction, SIGNAL("triggered()"), self.microcodeTableWidget.cut)
	self.cutAction.setEnabled(False)
	
        self.copyAction = QAction(QIcon(":/copy.png"), self.tr("Copy"), self)
        self.copyAction.setShortcut("Ctrl+C")
        self.copyAction.setStatusTip("Copy the current selection's contents to the clipboard")
        self.connect(self.copyAction, SIGNAL("triggered()"), self.microcodeTableWidget.copy)
	self.copyAction.setEnabled(False)
	
        self.pasteAction = QAction(QIcon(":/paste.png"), self.tr("Paste"), self)
        self.pasteAction.setShortcut("Ctrl+V")
        self.pasteAction.setStatusTip("Paste the clipboard's contents into the current selection")
        self.connect(self.pasteAction, SIGNAL("triggered()"), self.microcodeTableWidget.paste)
	self.pasteAction.setEnabled(False)
	
        self.clearAction = QAction(self.tr("Clear"), self)
        self.clearAction.setShortcut("Del")
        self.clearAction.setStatusTip("Delete the current selection's contents")
        self.connect(self.clearAction, SIGNAL("triggered()"), self.microcodeTableWidget.delete)

        self.selectRowAction = QAction(self.tr("Row"), self)
        self.selectRowAction.setStatusTip("Select all the cells in the current row")
        self.connect(self.selectRowAction, SIGNAL("triggered()"), self.microcodeTableWidget.selectCurrentRow)

        self.selectColumnAction = QAction(self.tr("Column"), self)
        self.selectColumnAction.setStatusTip("select all the cells in the current column")
        self.connect(self.selectColumnAction, SIGNAL("triggered()"), self.microcodeTableWidget.selectCurrentColumn)

        self.selectAllAction = QAction(self.tr("All"), self)
        self.selectAllAction.setShortcut("Ctrl+A")
        self.selectAllAction.setStatusTip("Select all the cells in the Microcode table")
        self.connect(self.selectAllAction, SIGNAL("triggered()"), self.microcodeTableWidget.selectAll)

        self.insertCellAction = QAction(self.tr("Insert"), self)
        self.connect(self.insertCellAction, SIGNAL("triggered()"), self.microcodeTableWidget.insertDialogShow)

        self.deleteCellAction = QAction(self.tr("Delete"),self)
        self.connect(self.deleteCellAction, SIGNAL("triggered()"), self.microcodeTableWidget.deleteDialogShow)

    def createMenus(self):
        fileMenu = self.menuBar().addMenu("File")
        fileMenu.addAction(self.newAction)
        fileMenu.addAction(self.openAction)
        fileMenu.addAction(self.saveAction)
        fileMenu.addAction(self.saveasAction)
        fileMenu.addSeparator()
        fileMenu.addAction(self.exitAction)

        editMenu = self.menuBar().addMenu("Edit")
        editMenu.addAction(self.cutAction)
        editMenu.addAction(self.copyAction)
        editMenu.addAction(self.pasteAction)
        editMenu.addAction(self.clearAction)

        selectSubMenu = editMenu.addMenu("Select")
        selectSubMenu.addAction(self.selectRowAction)
        selectSubMenu.addAction(self.selectColumnAction)
        selectSubMenu.addAction(self.selectAllAction)

    def createContextMenu(self):
        self.microcodeTableWidget.addAction(self.cutAction)
        self.microcodeTableWidget.addAction(self.copyAction)
        self.microcodeTableWidget.addAction(self.pasteAction)
        self.microcodeTableWidget.addAction(self.clearAction)
        self.microcodeTableWidget.addAction(self.insertCellAction)
        self.microcodeTableWidget.addAction(self.deleteCellAction)
        self.microcodeTableWidget.setContextMenuPolicy(Qt.ActionsContextMenu)

    def createToolBars(self):
        fileToolBar = self.addToolBar("File")
        fileToolBar.addAction(self.newAction)
        fileToolBar.addAction(self.openAction)
        fileToolBar.addAction(self.saveAction)
        fileToolBar.addAction(self.saveasAction)

        editToolBar = self.addToolBar("Edit")
        editToolBar.addAction(self.cutAction)
        editToolBar.addAction(self.copyAction)
        editToolBar.addAction(self.pasteAction)

        registerToolBar = self.addToolBar("Register")
        self.register0Check = QCheckBox(self.register[0]) 
        self.register0Text = QLineEdit("0")
        self.register0Text.setFixedSize(40, 20)
        #self.register0Text.setEnabled(False)
        self.register1Check = QCheckBox(self.register[1]) 
        self.register1Text = QLineEdit("0")
        self.register1Text.setFixedSize(40, 20)
        #self.register1Text.setEnabled(False)
        self.register2Check = QCheckBox(self.register[2]) 
        self.register2Text = QLineEdit("0")
        self.register2Text.setFixedSize(40, 20)
        #self.register2Text.setEnabled(False)
        self.register3Check = QCheckBox(self.register[3])
        self.register3Text =QLineEdit("0")
        self.register3Text.setFixedSize(40, 20)
        #self.register3Text.setEnabled(False)
        self.registerCheck = []
        self.registerCheck.append(self.register0Check)
        self.registerCheck.append(self.register1Check)
        self.registerCheck.append(self.register2Check)
        self.registerCheck.append(self.register3Check)
        self.registerText = []
        self.registerText.append(self.register0Text)
        self.registerText.append(self.register1Text)
        self.registerText.append(self.register2Text)
        self.registerText.append(self.register3Text)
        self.connect(self.register0Check, SIGNAL("clicked(bool)"), self.registerCheckSlot)
        self.connect(self.register1Check, SIGNAL("clicked(bool)"), self.registerCheckSlot)
        self.connect(self.register2Check, SIGNAL("clicked(bool)"), self.registerCheckSlot)
        self.connect(self.register3Check, SIGNAL("clicked(bool)"), self.registerCheckSlot)
        registerToolBar.addWidget(self.register0Check)
        registerToolBar.addWidget(self.register0Text)
        registerToolBar.addWidget(self.register1Check)
        registerToolBar.addWidget(self.register1Text)
        registerToolBar.addWidget(self.register2Check)
        registerToolBar.addWidget(self.register2Text)
        registerToolBar.addWidget(self.register3Check)
        registerToolBar.addWidget(self.register3Text)

    @pyqtSlot()
    def newFile(self):
        self.microcodeTableWidget.loopBodyList = [[]for i in xrange(self.microcodeTableWidget.ColumnCount)]
        self.microcodeTableWidget.initTable()
        self.setWindowTitle(self.newFileName)
        self.saveAction.setEnabled(True)
        self.saveasAction.setEnabled(True)
        self.copyAction.setEnabled(True)
        self.cutAction.setEnabled(True)
        self.pasteAction.setEnabled(True)

    @pyqtSlot()
    def openFile(self):
	fileName = self.windowTitle()
        if fileName == self.newFileName:
            fileName = QFileDialog.getOpenFileName(self, self.tr("select file"), "/")
            if fileName != "":
	        self.setWindowTitle(fileName)
                self.microcodeTableWidget.initTable()
                self.microcodeTableWidget.openFile(fileName)
	    

    @pyqtSlot()
    def saveFile(self):
        fileName = QFileDialog.getSaveFileName(self, self.tr("Save File"), "untitled.png")
        if fileName != "":
            self.microcodeTableWidget.saveFile(fileName)
	    self.setWindowTitle(fileName)

    @pyqtSlot()
    def saveasFile(self):
        fileName = QFileDialog.getSaveFileName(self, self.tr("Save File"), "untitled.png")
        if fileName != "":
            self.microcodeTableWidget.saveFile(fileName)
	    self.setWindowTitle(fileName)      

    @pyqtSlot()
    def closeWindow(self):
        self.close()

    def closeEvent(self, event):
        self.closeWindow()

    @pyqtSlot(bool)	
    def registerCheckSlot(self, checkState):
        if self.sender() in self.registerCheck:
            idx = self.registerCheck.index(self.sender())
            if checkState == True:
                re = self.microcodeTableWidget.paintRect(idx, self.registerText[idx].text()) 
                if re == 0:
                    self.registerCheck[idx].setCheckState(Qt.Unchecked)
                    warning("Please select one column")
                    return
                if re == -1:
                    self.registerCheck[idx].setCheckState(Qt.Unchecked)
                    warning("Please select one or more cells") 
                    return
                if re == -2:
                    self.registerCheck[idx].setCheckState(Qt.Unchecked)
                    warning("Cannot set loop at the intersection of cells")   
                    return
                self.registerText[idx].setEnabled(True)   
            else:
                self.microcodeTableWidget.eraserRect(idx)
                #self.registerText[idx].setEnabled(False)

    @pyqtSlot(bool)
    def setAllCheckStatus(self, status):
        num = len(self.registerCheck)
        for i in xrange(0, num):
            self.registerCheck[i].setCheckState(status)
            self.registerText[i].clear()
            #self.registerText[i].setEnabled(False)

    @pyqtSlot(list, list)
    def itemRegStateSlot(self, regList, textList):
        self.setAllCheckStatus(Qt.Unchecked)
        num = len(regList)
        for i in xrange(0, num):
            self.registerCheck[int(regList[i])].setCheckState(Qt.Checked)
            self.registerText[int(regList[i])].setEnabled(True)
        num = len(textList)
        for i in xrange(num):
            self.registerText[i].setText(str(textList[i]))

                






