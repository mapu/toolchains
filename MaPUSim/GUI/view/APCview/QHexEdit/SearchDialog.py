#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from QHexEdit import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class SearchDialog(QDialog):
    def __init__(self,hexEdit,parent=None):
	super(SearchDialog,self).__init__(parent)

	self.setWindowTitle("Find")	
	self.resize(300,150)
  	self._hexEdit = hexEdit
	gbFind=QGroupBox()
	self.cbFindLabel=QLabel("Find")
	self.cbFindLabel.setFixedWidth(50)
	self.cbFind=QComboBox()
	self.cbFind.setEditable(True)
	findLay=QHBoxLayout()
	findLay.addWidget(self.cbFindLabel)
	findLay.addWidget(self.cbFind)
	gbFind.setLayout(findLay)

	#gbOptions=QGroupBox("Options")
	#self.cbBackwards=QCheckBox("Backwards")
	#self.cbPrompt=QCheckBox("Prompt on replace")
	#optionLay=QVBoxLayout()
	#optionLay.addWidget(self.cbBackwards)
	#optionLay.addWidget(self.cbPrompt)
	#gbOptions.setLayout(optionLay)

	self.pbFind=QPushButton("Find")
	self.pbFind.setFixedWidth(100)
	self.pbCancel=QPushButton("Close")
	self.pbCancel.setFixedWidth(100)
	buttonLay=QHBoxLayout()
	buttonLay.addWidget(self.pbFind)
	buttonLay.addWidget(self.pbCancel)

	mainLay=QVBoxLayout()
	mainLay.addWidget(gbFind)
	mainLay.addLayout(buttonLay)
	self.setLayout(mainLay)

	self.connect(self.pbFind,SIGNAL("clicked()"),self.on_pbFind_clicked)
	self.connect(self.pbCancel,SIGNAL("clicked()"),self.closeDialog)

    def findNext(self):
    	self._findBa=self.cbFind.currentText()
    	idx = -1
    	if self._findBa.length() > 0:
            idx = self._hexEdit.indexOf(self._findBa)
    	return idx

    def on_pbFind_clicked(self):
    	self.findNext()

    def closeDialog(self):
	self.close()

