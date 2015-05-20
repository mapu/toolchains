# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class FloatDialog(QDialog):
    def __init__(self,parent=None):
	super(FloatDialog,self).__init__(parent)

	self.setMaximumWidth(150)
	self.setMaximumHeight(120)
	self.stages=QListWidget()
	self.stages.setMaximumWidth(145)
	self.stages.setMaximumHeight(145)
	self.setAttribute(Qt.WA_DeleteOnClose)
	#self.setModal(False)
	self.setWindowFlags(Qt.WindowStaysOnTopHint)
	self.stages.addItem(self.tr("sn:41,sln:3"))
	self.stages.addItem(self.tr("sn:55,sln:4"))
	self.stages.addItem(self.tr("sn:69,sln:5"))
	self.stages.item(0).setToolTip("sn:41,sln:3")
	self.stages.item(1).setToolTip("sn:55,sln:4")
	self.stages.item(2).setToolTip("sn:69,sln:5")
	stagesLay=QVBoxLayout()
	stagesLay.addWidget(self.stages)
	self.setLayout(stagesLay)
	



