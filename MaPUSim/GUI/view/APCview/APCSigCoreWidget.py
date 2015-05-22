# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from MPUViewWidget import*
from SPUViewWidget import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class APCSigCoreWidget(QWidget):
    def __init__(self,parent=None):
	super(APCSigCoreWidget,self).__init__(parent)

	APETab=QTabWidget()
	self.SPUWidget=SPUViewWidget()
	self.MPUWidget=MPUViewWidget()
	APETab.addTab(self.SPUWidget,self.tr("SPU"))
	APETab.addTab(self.MPUWidget,self.tr("MPU"))
	APETab.setTabPosition(QTabWidget.South)

	lay=QHBoxLayout()
	lay.addWidget(APETab)
	self.setLayout(lay)

