# -*- coding =utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class ConfigViewWidget(QWidget):
    def __init__(self,parent=None):
	super(ConfigViewWidget,self).__init__(parent)
	
	self.textEdit=QTextEdit()
	self.pathEdit=QLineEdit()
	self.browseButton=QPushButton(self.tr("Browse"))
	self.browseButton.setFixedWidth(100)
	self.startButton=QPushButton(self.tr("Start"))
	self.startButton.setFixedWidth(100)
	self.stopButton=QPushButton(self.tr("Stop"))
	self.stopButton.setFixedWidth(100)
	browseLay=QHBoxLayout()
	browseLay.addWidget(self.pathEdit)
	browseLay.addWidget(self.browseButton)
	browseLay.setStretchFactor(self.pathEdit,4)
	browseLay.setStretchFactor(self.browseButton,1)
	buttonLay=QHBoxLayout()
	buttonLay.addWidget(self.startButton)
	buttonLay.addWidget(self.stopButton)
	mainLay=QVBoxLayout()
	mainLay.addWidget(self.textEdit)
	mainLay.addLayout(browseLay)
	mainLay.addLayout(buttonLay)
	self.setLayout(mainLay)
	self.connect(self.browseButton,SIGNAL("clicked()"),self.browseFile)
	self.connect(self.startButton,SIGNAL("clicked()"),self.startProcess)
	self.connect(self.stopButton,SIGNAL("clicked()"),self.stopProcess)

    def browseFile(self):
	path=QFileDialog.getExistingDirectory(self,self.tr("select dir"),"/",QFileDialog.ShowDirsOnly)
	self.pathEdit.setText(path)

    def startProcess(self):
	self.textEdit.append("start process")

    def stopProcess(self):
	self.textEdit.append("stop process")




	

