# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from EmbTerminal import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class UARTWidget(QMainWindow):
    def __init__(self,parent=None):
	super(UARTWidget,self).__init__(parent)

        centralWidget = QWidget()
        self.setCentralWidget(centralWidget)      
        widget = QWidget()
        widget.setMinimumSize(1000,280)

	self.embTerminal=EmbTerminal()
	layout=QVBoxLayout()
	layout.addWidget(self.embTerminal)

	fixedWidget=QWidget()
	fixedWidget.setFixedSize(1000,280)
	fixedWidget.setLayout(layout)

	fixedLay=QVBoxLayout()
	fixedLay.addWidget(fixedWidget)	
	widget.setLayout(fixedLay)
	#scroll
        scroll = QScrollArea()
        scroll.setWidget(widget)
        scroll.setAutoFillBackground(True)
        scroll.setWidgetResizable(True)
        vbox = QVBoxLayout()
        vbox.addWidget(scroll)  
        centralWidget.setLayout(vbox)

    def m5termProcessStart(self):
	self.embTerminal.startProcess()


