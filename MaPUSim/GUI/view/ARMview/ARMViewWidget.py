# -*- coding: utf-8 -*-  
from PyQt4.QtGui import*  
from PyQt4.QtCore import* 
from UARTWidget import*
from ARMStatusWidget import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class ARMViewWidget(QWidget):  
    def __init__(self,parent=None):  
        super(ARMViewWidget,self).__init__(parent) 

        self.UART0Widget=UARTWidget()    
        self.UART1Widget=UARTWidget()
        self.LCDWidget=QWidget()
	self.statusWidget=ARMStatusWidget()

        self.upTabWidget=QTabWidget() 
        self.upTabWidget.addTab(self.UART0Widget,"UART0")
        self.upTabWidget.addTab(self.UART1Widget,"UART1")
        self.upTabWidget.addTab(self.LCDWidget,"LCD")  
	self.upTabWidget.addTab(self.statusWidget,"Status")      
            
        self.GDBWidget=QWidget()
	self.ConsoleWidget=QWidget()
	self.downTabWidget=QTabWidget()
	self.downTabWidget.setTabPosition(QTabWidget.South)
	self.downTabWidget.addTab(self.GDBWidget,"GDB")
	self.downTabWidget.addTab(self.ConsoleWidget,"Console")

        self.layout=QVBoxLayout()                 
        self.layout.addWidget(self.upTabWidget)
        self.layout.addWidget(self.downTabWidget)
        self.setLayout(self.layout)   
