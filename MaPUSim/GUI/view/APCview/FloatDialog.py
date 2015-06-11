# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class FloatDialog(QDialog):
    def __init__(self,parent=None):
	super(FloatDialog,self).__init__(parent)

	self.flag=-1 #-1 close , 0 open
	self.setMaximumWidth(150)
	self.setMaximumHeight(120)
	self.stages=QListWidget()
	self.stages.setMaximumWidth(145)
	self.stages.setMaximumHeight(145)
	self.setAttribute(Qt.WA_DeleteOnClose)
	#self.setModal(False)
	self.setWindowFlags(Qt.WindowStaysOnTopHint)
	
	self.initDialog()

	stagesLay=QVBoxLayout()
	stagesLay.addWidget(self.stages)
	self.setLayout(stagesLay)
	
    def initDialog(self):
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))
	self.stages.addItem(self.tr(""))

    def closeEvent(self,event):
	self.flag=-1


