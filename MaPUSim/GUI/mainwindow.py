# -*- coding: utf-8 -*-   
from PyQt4.QtGui import *  
from PyQt4.QtCore import *  
import sys  
import qrc_resource  
  
QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  
  
class MainWindow(QMainWindow):  
    def __init__(self,parent=None):  
        super(MainWindow,self).__init__(parent)  
        self.setWindowTitle("QMainWindow")  
       
        #dialog=QDialog()
        

        
                       
        #layout=QHBoxLayout()  
        #layout.setMargin(5)  
        #layout.setSpacing(5)  
        #layout.addWidget( dialog.tabwidge)  
  
        #dialog.setLayout(layout)         
        #dialog.resize(300, 100)       
          
                        
        self.createActions() 
        self.createMenus()  
        self.createToolBars()            
    def createActions(self): 
        self.fileOpenAction=QAction(QIcon(":/fileopen.png"),self.tr("打开"),self)                                 
        self.fileNewAction=QAction(QIcon(":/filenew.png"),self.tr("新建"),self)                  
        self.fileSaveAction=QAction(QIcon(":/filesave.png"),self.tr("保存"),self)                               
        self.exitAction=QAction(QIcon(":/filequit.png"),self.tr("退出"),self)              
        self.cutAction=QAction(QIcon(":/editcut.png"),self.tr("剪切"),self)                  
        self.copyAction=QAction(QIcon(":/editcopy.png"),self.tr("复制"),self)    
        self.pasteAction=QAction(QIcon(":/editpaste.png"),self.tr("粘贴"),self)          
        self.aboutAction=QAction(self.tr("关于"),self)          
    def createMenus(self):  
        fileMenu=self.menuBar().addMenu(self.tr("文件"))                      
        editMenu=self.menuBar().addMenu(self.tr("编辑"))          
        aboutMenu=self.menuBar().addMenu(self.tr("帮助"))            
    def createToolBars(self):  
        fileToolBar=self.addToolBar("File")    
        fileToolBar.addAction(self.fileNewAction)  
        fileToolBar.addAction(self.fileOpenAction)  
        fileToolBar.addAction(self.fileSaveAction) 
        editTool=self.addToolBar("Edit")  
        editTool.addAction(self.copyAction)  
        editTool.addAction(self.cutAction)  
        editTool.addAction(self.pasteAction)
        
        self.setWindowTitle('MaPU Simulator')
        self.statusBar().showMessage(self.tr("状态栏"))    
        




        tab1=QDialog()
        tab2=QDialog()
        tab3=QDialog()
        tab4=QDialog()

        tabwidge=QTabWidget()         
        tabwidge.addTab(tab1,self.tr("模拟器信息"))        
        tabwidge.addTab(tab2,self.tr("ARM视窗"))
        tabwidge.addTab(tab3,self.tr("APC视窗"))
        tabwidge.addTab(tab4,self.tr("配置与控制"))      
        self.setCentralWidget(tabwidge)




        tab1.label1=QLabel("SoC")
        tab1.label2=QLabel("ARM")
        tab1.label3=QLabel("APC")
        tab1.button1=QPushButton(self.tr("刷新"))   
        tab1.button2=QPushButton(self.tr("刷新")) 
        tab1.button3=QPushButton(self.tr("刷新")) 
        self.text1=QTextEdit()
        self.text2=QTextEdit()
        self.text3=QTextEdit()
         
        tab1.layout1=QHBoxLayout() 
        tab1.layout1.setMargin(15)  
        
        tab1.layout1.addWidget(tab1.label1) 
        tab1.layout1.addWidget(tab1.button1)       
        tab1.layout1.addWidget(tab1.label2)  
        tab1.layout1.addWidget(tab1.button2)  
        tab1.layout1.addWidget(tab1.label3)  
        tab1.layout1.addWidget(tab1.button3)
  
        tab1.layout2=QHBoxLayout()  
        tab1.layout2.addWidget(self.text1)  
        tab1.layout2.addWidget(self.text2)  
        tab1.layout2.addWidget(self.text3)   
        
        tab1.mainLayout=QVBoxLayout()    
        tab1.mainLayout.setSpacing(10)  
        tab1.mainLayout.addLayout(tab1.layout1)  
        tab1.mainLayout.addLayout(tab1.layout2) 
        tab1.setLayout(tab1.mainLayout)        
        self.connect(tab1.button1,SIGNAL("clicked()"),self.slotClear1)  
        self.connect(tab1.button2,SIGNAL("clicked()"),self.slotClear2)
        self.connect(tab1.button3,SIGNAL("clicked()"),self.slotClear3)
    def slotClear1(self):
        self.text1.clear()
    def slotClear2(self):
        self.text2.clear()
    def slotClear3(self):
        self.text3.clear()



      
app=QApplication(sys.argv) 
main=MainWindow()  
main.show()  
app.exec_()  
