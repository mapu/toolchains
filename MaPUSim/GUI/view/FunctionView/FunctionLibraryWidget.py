# -*- coding: utf-8 -*-
from PyQt4.QtCore import pyqtSignal, Qt, SIGNAL, QDir, pyqtSlot, QString, QDir, QFileInfo, QFile
from PyQt4.QtGui import QWidget, QHBoxLayout, QVBoxLayout, QLabel, QPushButton, QLineEdit, QTreeWidget, QTreeWidgetItem, QFileDialog, QMessageBox, QFileIconProvider
from FileWidget import FileWidget

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class FunctionLibraryWidget(QWidget):
    def __init__(self, parent = None):
        super(FunctionLibraryWidget, self).__init__(parent)
        
        dirWidget = QWidget(self)
        dirWidget.setFixedSize(600, 50)
        dirLabel = QLabel(self.tr("Function dir:"))
        self.browseButton = QPushButton(self.tr("Browse ..."))
        self.showButton = QPushButton(self.tr("Show"))
        self.dirEdit = QLineEdit()
        self.dirEdit.setReadOnly(True)
        self.dirEdit.setFixedWidth(300)
        lay = QHBoxLayout(dirWidget)
        lay.addWidget(dirLabel)
        lay.addWidget(self.dirEdit)
        lay.addWidget(self.browseButton)
        lay.addWidget(self.showButton)
        
        self.treeWidget = QTreeWidget(self)
        self.treeWidget.setColumnCount(1)
        self.treeWidget.setHeaderHidden(True)
        
        mainLay = QVBoxLayout(self)
        mainLay.addWidget(dirWidget)
        mainLay.addWidget(self.treeWidget)
        
        self.browseButton.clicked.connect(self.browserSlot)
        self.showButton.clicked.connect(self.expandParentDir)
        self.treeWidget.itemDoubleClicked.connect(self.showFunction)
      
    @pyqtSlot()
    def browserSlot(self):
        dir1 = QFileDialog.getExistingDirectory(self, self.tr("Open Directory"), ".")
        self.dirEdit.setText(dir1)
     
    @pyqtSlot()
    def expandParentDir(self):
        path = self.dirEdit.text()
        icon = QFileIconProvider()
        self.treeWidget.clear()
        dir1 = QDir(path)
        if not dir1.exists():
	    QMessageBox.warning(self, "warning", "The dir path is not existing!")
	    return
	dir1.setFilter(QDir.Dirs)
	list1 = dir1.entryInfoList()
	for i in range(0, len(list1)):
	    file1 = list1[i]
	    if file1.fileName() == "." or file1.fileName() == "..":
	        continue
	    if file1.isDir():
	        item = QTreeWidgetItem()
	        item.setText(0, file1.fileName())
	        item.setIcon(0, icon.icon(QFileIconProvider.Folder))
	        self.treeWidget.addTopLevelItem(item)
	        self.expandDir(item, file1.filePath())
     
    def expandDir(self, item, path):
        icon = QFileIconProvider()
        childDir = QDir(path)
        childDir.setFilter(QDir.Files | QDir.Dirs)
        childList = childDir.entryInfoList()
        for i in range(0, len(childList)):
	    childFile = childList[i]
	    if childFile.fileName() == "." or childFile.fileName() == "..":
	        continue
	    if childFile.isDir():
	        childItem = QTreeWidgetItem()
	        childItem.setText(0, childFile.fileName())
	        childItem.setIcon(0, icon.icon(QFileIconProvider.Folder))
	        item.addChild(childItem)
	        self.expandDir(childItem, childFile.filePath())
	    else:
	      	childItem = QTreeWidgetItem()
	        childItem.setText(0, childFile.fileName())
	        childItem.setIcon(0, icon.icon(QFileIconProvider.File))
	        item.addChild(childItem)
      
    @pyqtSlot(QTreeWidgetItem, int)
    def showFunction(self, item, column):
        text = self.dirEdit.text()
        path = item.text(column)
        parent = item.parent()
        while parent:
	    path.insert(0, "/")
	    path.insert(0, parent.text(column))
	    parent = parent.parent()
	text.append("/")
	path.insert(0, text)
	childFile = QFileInfo(path)
	if childFile.isDir():
	    return
	file1 = QFile(path)
	if file1.exists():
	    fileWidget = FileWidget(path, self)
	    fileWidget.exec_()
	else:
	    QMessageBox.warning(self, "warning", "The file is not existing!")
	    return
        
        