# -*- coding: utf-8 -*-   
from PyQt4.QtGui import *  
from PyQt4.QtCore import *  
import sys  
  
QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  
  
class MainWidget(QDialog):  
      def __init__(self,parent=None):  
        super(MainWidget,self).__init__(parent)          
        label1=QLabel("SoC")
        label2=QLabel("ARM")
        label3=QLabel("APC")
        button1=QPushButton(self.tr("刷新"))   
        button2=QPushButton(self.tr("刷新")) 
        button3=QPushButton(self.tr("刷新")) 
        self.text1=QTextEdit()
        self.text2=QTextEdit()
        self.text3=QTextEdit()
         
        layout1=QHBoxLayout() 
        layout1.setMargin(15)  
        
        layout1.addWidget(label1) 
        layout1.addWidget(button1)       
        layout1.addWidget(label2)  
        layout1.addWidget(button2)  
        layout1.addWidget(label3)  
        layout1.addWidget(button3)
  
        layout2=QHBoxLayout()  
        layout2.addWidget(self.text1)  
        layout2.addWidget(self.text2)  
        layout2.addWidget(self.text3)   
        
        mainLayout=QVBoxLayout()    
        mainLayout.setSpacing(10)  
        mainLayout.addLayout(layout1)  
        mainLayout.addLayout(layout2) 
        self.setLayout(mainLayout)        
        self.connect(button1,SIGNAL("clicked()"),self.slotClear1)  
        self.connect(button2,SIGNAL("clicked()"),self.slotClear2)
        self.connect(button3,SIGNAL("clicked()"),self.slotClear3)
      def slotClear1(self):
         self.text1.clear()
      def slotClear2(self):
         self.text2.clear()
      def slotClear3(self):
         self.text3.clear()

app=QApplication(sys.argv)  
main=MainWidget()  
main.show()  
app.exec_()  
