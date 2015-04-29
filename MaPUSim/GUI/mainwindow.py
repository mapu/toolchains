# -*- coding: utf-8 -*-   
from PyQt4.QtGui import *  
from PyQt4.QtCore import *  
import sys  
import qrc_resource

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  
  
class MainWindow(QMainWindow):  
    def __init__(self,parent=None):  
        super(MainWindow,self).__init__(parent)         
        self.setWindowTitle('MaPU Simulator')                          
        self.createActions() 
        self.createMenus()  
        self.createToolBars()       
        self.setMinimumSize (800,500)
     
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
        self.statusBar().showMessage(self.tr("状态栏"))    
        
        tab1=QDialog()
        self.tab2=QDialog()
        self.tab3=QDialog()
        tab4=QDialog()
        tabwidge=QTabWidget()         
        tabwidge.addTab(tab1,self.tr("模拟器信息"))        
        tabwidge.addTab(self.tab2,self.tr("ARM视窗"))
        tabwidge.addTab(self.tab3,self.tr("APC视窗"))
        tabwidge.addTab(tab4,self.tr("配置与控制"))      
        self.setCentralWidget(tabwidge)
 
        tab1.label1=QLabel("SoC")     #the detail of simulator
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
        tab1.mainLayout.setSpacing(20)  
        tab1.mainLayout.addLayout(tab1.layout1)  
        tab1.mainLayout.addLayout(tab1.layout2) 
        tab1.setLayout(tab1.mainLayout)        
        self.connect(tab1.button1,SIGNAL("clicked()"),self.slotClear1)  
        self.connect(tab1.button2,SIGNAL("clicked()"),self.slotClear2)
        self.connect(tab1.button3,SIGNAL("clicked()"),self.slotClear3)

        self.tab3.Ltabapc=QDialog()      # the detail of APC
        self.tab3.Lape0=QDialog()
        self.tab3.Lape1=QDialog()
        self.tab3.Lape2=QDialog()
        self.tab3.Lape3=QDialog()      
        self.tab3.Ltab=QTabWidget()
        self.tab3.Ltab.addTab(self.tab3.Ltabapc,"APC")
        self.tab3.Ltab.addTab(self.tab3.Lape0,"APE0")         
        self.tab3.Ltab.addTab(self.tab3.Lape1,"APE1")
        self.tab3.Ltab.addTab(self.tab3.Lape2,"APE2")
        self.tab3.Ltab.addTab(self.tab3.Lape3,"APE3")          

        butt0=QPushButton("APE0")          
        butt1=QPushButton("APE1")        
        butt2=QPushButton("APE2")
        butt3=QPushButton("APE3")
        self.tab3.Ltabapc.apclayout=QGridLayout()
        self.tab3.Ltabapc.apclayout.setMargin(70)  
        self.tab3.Ltabapc.apclayout.setSpacing(50)  
        self.tab3.Ltabapc.apclayout.addWidget(butt0,0,0)          
        self.tab3.Ltabapc.apclayout.addWidget(butt3,0,1) 
        self.tab3.Ltabapc.apclayout.addWidget(butt1,1,0) 
        self.tab3.Ltabapc.apclayout.addWidget(butt2,1,1) 
        self.tab3.Ltabapc.apclayout.setAlignment(Qt.AlignCenter)
        self.tab3.Ltabapc.setLayout(self.tab3.Ltabapc.apclayout)
         
        self.tab3.connect(butt0,SIGNAL("clicked()"),self.slottab0)
        self.tab3.connect(butt1,SIGNAL("clicked()"),self.slottab1)
        self.tab3.connect(butt2,SIGNAL("clicked()"),self.slottab2)
        self.tab3.connect(butt3,SIGNAL("clicked()"),self.slottab3)                                      
        wDMA=QTableWidget()
        nDMA=QTableWidget()
        station=QDialog()
        self.tab3.Rtab=QTabWidget() 
        self.tab3.Rtab.addTab(wDMA,self.tr("外DMA"))
        self.tab3.Rtab.addTab(nDMA,self.tr("内DMA"))
        self.tab3.Rtab.addTab(station,self.tr("状态"))
        
        wDMA.setColumnCount(2)  
        wDMA.setRowCount(10)        
        witem0 = QTableWidgetItem(self.tr("通道0"))          
        witem0.setTextColor(QColor(200,111,100))  
        witem0.setFont(QFont("Helvetica"))         
        witem1 = QTableWidgetItem(self.tr("通道1"))          
        witem1.setTextColor(QColor(200,111,100))  
        witem1.setFont(QFont("Helvetica"))         
        wDMA.setItem(0,0,witem0)
        wDMA.setItem(1,0,QTableWidgetItem(self.tr("目标地址")))
        wDMA.setItem(2,0,QTableWidgetItem(self.tr("源地址")))
        wDMA.setItem(3,0,QTableWidgetItem(self.tr("X长度")))
        wDMA.setItem(4,0,QTableWidgetItem(self.tr("Y长度")))
        wDMA.setItem(5,0,witem1)
        wDMA.setItem(6,0,QTableWidgetItem(self.tr("目标地址")))
        wDMA.setItem(7,0,QTableWidgetItem(self.tr("源地址")))
        wDMA.setItem(8,0,QTableWidgetItem(self.tr("X长度")))
        wDMA.setItem(9,0,QTableWidgetItem(self.tr("Y长度")))
            
        nDMA.setColumnCount(2)  
        nDMA.setRowCount(10)  
        nitem0 = QTableWidgetItem(self.tr("通道0"))          
        nitem0.setTextColor(QColor(200,111,100))  
        nitem0.setFont(QFont("Helvetica"))         
        nitem1 = QTableWidgetItem(self.tr("通道1"))          
        nitem1.setTextColor(QColor(200,111,100))  
        nitem1.setFont(QFont("Helvetica"))       
        nDMA.setItem(0,0,nitem0)
        nDMA.setItem(1,0,QTableWidgetItem(self.tr("目标地址")))
        nDMA.setItem(2,0,QTableWidgetItem(self.tr("源地址")))
        nDMA.setItem(3,0,QTableWidgetItem(self.tr("X长度")))
        nDMA.setItem(4,0,QTableWidgetItem(self.tr("Y长度")))
        nDMA.setItem(5,0,nitem1)
        nDMA.setItem(6,0,QTableWidgetItem(self.tr("目标地址")))
        nDMA.setItem(7,0,QTableWidgetItem(self.tr("源地址")))
        nDMA.setItem(8,0,QTableWidgetItem(self.tr("X长度")))
        nDMA.setItem(9,0,QTableWidgetItem(self.tr("Y长度")))
          
        self.tab3.hlayout=QHBoxLayout()  
        self.tab3.hlayout.addWidget(self.tab3.Ltab)  
        self.tab3.hlayout.addWidget(self.tab3.Rtab) 
        self.tab3.hlayout.setStretchFactor(self.tab3.Ltab,2)  
        self.tab3.hlayout.setStretchFactor(self.tab3.Rtab,1)          
        
        self.tab3.slider = QSlider(Qt.Horizontal)
        self.tab3.slider.setTickPosition(QSlider.TicksBothSides)
        self.tab3.slider.setMinimum(0)
        self.tab3.slider.setMaximum(100)  
        self.tab3.slider.setValue(50)          
        self.tab3.mainLayout=QVBoxLayout()
        self.tab3.mainLayout.addLayout(self.tab3.hlayout)
        self.tab3.mainLayout.addWidget(self.tab3.slider)
        self.tab3.setLayout(self.tab3.mainLayout)    
          
        self.tab2.Uart0=QDialog()    #the detail of ARM
        self.tab2.Uart1=QDialog()
        self.tab2.Lcd=QDialog()
        self.tab2.uptabwidget=QTabWidget() 
        self.tab2.uptabwidget.addTab(self.tab2.Uart0,"UART0")
        self.tab2.uptabwidget.addTab(self.tab2.Uart1,"UART1")
        self.tab2.uptabwidget.addTab(self.tab2.Lcd,"LCD")        
        self.tab2.text=QTextEdit()
        self.tab2.text.setWordWrapMode(QTextOption.NoWrap)               
        self.tab2.Gdbbutt=QPushButton("GDB")
        self.tab2.Console=QPushButton("Console")
        self.tab2.bottom=QHBoxLayout()                 
        self.tab2.bottom.addWidget(self.tab2.Gdbbutt)
        self.tab2.bottom.addWidget(self.tab2.Console)
        self.tab2.bottom.addStretch()  
        self.tab2.vlayout=QVBoxLayout()
        self.tab2.vlayout.addWidget(self.tab2.uptabwidget)     
        self.tab2.vlayout.addWidget(self.tab2.text)
        self.tab2.vlayout.addLayout(self.tab2.bottom)
        self.tab2.setLayout(self.tab2.vlayout)     

    def slotClear1(self):
        self.text1.clear()

    def slotClear2(self):
        self.text2.clear()

    def slotClear3(self):
        self.text3.clear()

    def slottab0(self):
        self.tab3.Ltab.setCurrentWidget(self.tab3.Lape0)  

    def slottab1(self):
        self.tab3.Ltab.setCurrentWidget(self.tab3.Lape1) 

    def slottab2(self):
        self.tab3.Ltab.setCurrentWidget(self.tab3.Lape2) 

    def slottab3(self):
        self.tab3.Ltab.setCurrentWidget(self.tab3.Lape3)
                  
app=QApplication(sys.argv) 
main=MainWindow()  
main.show()  
app.exec_()         
  

