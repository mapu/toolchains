# -*- coding: utf-8 -*-   
from PyQt4.QtGui import QMainWindow, QToolBar, QAction, QIcon, QCheckBox, QTextEdit, QFileDialog, QLineEdit
from PyQt4.QtCore import pyqtSignal, Qt, SIGNAL, pyqtSlot
from res import qrc_resources 
from view.MicrocodeTable.MicrocodeTable import MicrocodeTable
from Utils import warning


class MainWindow(QMainWindow):  
    def __init__(self, parent = None):  
        super(MainWindow, self).__init__(parent)   

        self.register = ["KI12", "KI13", "KI14", "KI15"]
        self.setWindowTitle("Graphic Microcode Editor")
        self.resize(800, 600)
        self.microcodeTable = MicrocodeTable(self.register)
        self.setCentralWidget(self.microcodeTable)
        self.createAction()
        self.createContextMenu()
        self.createToolBars()
        self.createMenus()
        self.microcodeTable.itemRegStateSignal.connect(self.itemRegStateSlot)

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

        self.exitAction = QAction(self.tr("Exit"), self)
        self.exitAction.setShortcut("Ctrl+Q")
        self.exitAction.setStatusTip("Exit the application")
        self.connect(self.exitAction, SIGNAL("triggered()"), self.closeWindow)

        self.cutAction = QAction(QIcon(":/cut.png"), self.tr("Cut"), self)
        self.cutAction.setShortcut("Ctrl+X")
        self.cutAction.setStatusTip("Cut the current selection's contents to the clipboard")
        self.connect(self.cutAction, SIGNAL("triggered()"), self.microcodeTable.cut)

        self.copyAction = QAction(QIcon(":/copy.png"), self.tr("Copy"), self)
        self.copyAction.setShortcut("Ctrl+C")
        self.copyAction.setStatusTip("Copy the current selection's contents to the clipboard")
        self.connect(self.copyAction, SIGNAL("triggered()"), self.microcodeTable.copy)

        self.pasteAction = QAction(QIcon(":/paste.png"), self.tr("Paste"), self)
        self.pasteAction.setShortcut("Ctrl+V")
        self.pasteAction.setStatusTip("Paste the clipboard's contents into the current selection")
        self.connect(self.pasteAction, SIGNAL("triggered()"), self.microcodeTable.paste)

        self.deleteAction = QAction(self.tr("Delete"), self)
        self.deleteAction.setShortcut("Del")
        self.deleteAction.setStatusTip("Delete the current selection's contents")
        self.connect(self.deleteAction, SIGNAL("triggered()"), self.microcodeTable.delete)

        self.selectRowAction = QAction(self.tr("Row"), self)
        self.selectRowAction.setStatusTip("Select all the cells in the current row")
        self.connect(self.selectRowAction, SIGNAL("triggered()"), self.microcodeTable.selectCurrentRow)

        self.selectColumnAction = QAction(self.tr("Column"), self)
        self.selectColumnAction.setStatusTip("select all the cells in the current column")
        self.connect(self.selectColumnAction, SIGNAL("triggered()"), self.microcodeTable.selectCurrentColumn)

        self.selectAllAction = QAction(self.tr("All"), self)
        self.selectAllAction.setShortcut("Ctrl+A")
        self.selectAllAction.setStatusTip("Select all the cells in the Microcode table")
        self.connect(self.selectAllAction, SIGNAL("triggered()"), self.microcodeTable.selectAll)

        self.insertCellAction = QAction(self.tr("Insert"), self)
        self.connect(self.insertCellAction, SIGNAL("triggered()"), self.microcodeTable.insertDialogShow)

        self.deleteCellAction = QAction(self.tr("Delete"),self)
        self.connect(self.deleteCellAction, SIGNAL("triggered()"), self.microcodeTable.deleteDialogShow)

    def createMenus(self):
        fileMenu = self.menuBar().addMenu("File")
        fileMenu.addAction(self.newAction)
        fileMenu.addAction(self.openAction)
        fileMenu.addAction(self.saveAction)
        fileMenu.addSeparator()
        fileMenu.addAction(self.exitAction)

        editMenu = self.menuBar().addMenu("Edit")
        editMenu.addAction(self.cutAction)
        editMenu.addAction(self.copyAction)
        editMenu.addAction(self.pasteAction)
        editMenu.addAction(self.deleteAction)

        selectSubMenu = editMenu.addMenu("Select")
        selectSubMenu.addAction(self.selectRowAction)
        selectSubMenu.addAction(self.selectColumnAction)
        selectSubMenu.addAction(self.selectAllAction)

    def createContextMenu(self):
        self.microcodeTable.addAction(self.cutAction)
        self.microcodeTable.addAction(self.copyAction)
        self.microcodeTable.addAction(self.pasteAction)
        self.microcodeTable.addAction(self.insertCellAction)
        self.microcodeTable.addAction(self.deleteCellAction)
        self.microcodeTable.setContextMenuPolicy(Qt.ActionsContextMenu)

    def createToolBars(self):
        fileToolBar = self.addToolBar("File")
        fileToolBar.addAction(self.newAction)
        fileToolBar.addAction(self.openAction)
        fileToolBar.addAction(self.saveAction)

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
        self.connect(self.register0Check, SIGNAL("clicked(bool)"), self.register0Slot)
        self.connect(self.register1Check, SIGNAL("clicked(bool)"), self.register1Slot)
        self.connect(self.register2Check, SIGNAL("clicked(bool)"), self.register2Slot)
        self.connect(self.register3Check, SIGNAL("clicked(bool)"), self.register3Slot)
        registerToolBar.addWidget(self.register0Check)
        registerToolBar.addWidget(self.register0Text)
        registerToolBar.addWidget(self.register1Check)
        registerToolBar.addWidget(self.register1Text)
        registerToolBar.addWidget(self.register2Check)
        registerToolBar.addWidget(self.register2Text)
        registerToolBar.addWidget(self.register3Check)
        registerToolBar.addWidget(self.register3Text)

    def newFile(self):
        self.microcodeTable.loopBodyList = [[]for i in xrange(self.microcodeTable.ColumnCount)]
        self.microcodeTable.initTable()

    def openFile(self):
        fileName = QFileDialog.getOpenFileName(self, self.tr("select file"), "/")
        if fileName != "":
            self.microcodeTable.initTable()
            self.microcodeTable.openFile(fileName)

    def saveFile(self):
        fileName = QFileDialog.getSaveFileName(self, self.tr("Save File"), "untitled.png")
        if fileName != "":
            self.microcodeTable.saveFile(fileName)

    def closeWindow(self):
        self.close()

    def closeEvent(self, event):
        self.closeWindow()
	
    def register0Slot(self, checkState):
        if checkState == True:
            re = self.microcodeTable.paintRect(0, self.register0Text.text()) 
            if re == 0:
                self.register0Check.setCheckState(Qt.Unchecked)
                warning("Please select one column")
                return
            if re == -1:
                self.register0Check.setCheckState(Qt.Unchecked)
                warning("Please select one or more cells") 
                return
            if re == -2:
                self.register0Check.setCheckState(Qt.Unchecked)
                warning("Cannot set loop at the intersection of cells")   
                return
            self.register0Text.setEnabled(True)   
        else:
            self.microcodeTable.eraserRect(0)
            #self.register0Text.setEnabled(False)

    def register1Slot(self, checkState):
        if checkState == True:
            re = self.microcodeTable.paintRect(1, self.register1Text.text())   
            if re == 0:
                self.register1Check.setCheckState(Qt.Unchecked)   
                warning("Please select one column")  
                return
            if re == -1:
                self.register1Check.setCheckState(Qt.Unchecked)
                warning("Please select one or more cells") 
                return 
            if re == -2:
                self.register1Check.setCheckState(Qt.Unchecked)
                warning("Cannot set loop at the intersection of cells")  
                return
            self.register1Text.setEnabled(True)
        else:
            self.microcodeTable.eraserRect(1) 
            #self.register1Text.setEnabled(False)

    def register2Slot(self, checkState):
        if checkState == True:
            re = self.microcodeTable.paintRect(2, self.register2Text.text())   
            if re == 0:
                self.register2Check.setCheckState(Qt.Unchecked)   
                warning("Please select one column") 
                return 
            if re == -1:
                self.register2Check.setCheckState(Qt.Unchecked)
                warning("Please select one or more cells")  
                return
            if re == -2:
                self.register2Check.setCheckState(Qt.Unchecked)
                warning("Cannot set loop at the intersection of cells")  
                return
            self.register2Text.setEnabled(True)
        else:
            self.microcodeTable.eraserRect(2) 
            #self.register2Text.setEnabled(False)

    def register3Slot(self, checkState):
        if checkState == True:
            re = self.microcodeTable.paintRect(3, self.register3Text.text())   
            if re == 0:
                self.register3Check.setCheckState(Qt.Unchecked)   
                warning("Please select one column")  
                return
            if re == -1:
                self.register3Check.setCheckState(Qt.Unchecked)
                warning("Please select one or more cells")  
                return
            if re == -2:
                self.register3Check.setCheckState(Qt.Unchecked)
                warning("Cannot set loop at the intersection of cells")
                return
            self.register3Text.setEnabled(True)  
        else:
            self.microcodeTable.eraserRect(3) 
            #self.register3Text.setEnabled(False)

    def setAllCheckStatus(self, status):
        num = len(self.registerCheck)
        for i in xrange(0, num):
            self.registerCheck[i].setCheckState(status)
            self.registerText[i].clear()
            #self.registerText[i].setEnabled(False)

    def itemRegStateSlot(self, regList, textList):
        self.setAllCheckStatus(Qt.Unchecked)
        num = len(regList)
        for i in xrange(0, num):
            self.registerCheck[int(regList[i])].setCheckState(Qt.Checked)
            self.registerText[int(regList[i])].setEnabled(True)
        num = len(textList)
        for i in xrange(num):
            self.registerText[i].setText(str(textList[i]))

                






