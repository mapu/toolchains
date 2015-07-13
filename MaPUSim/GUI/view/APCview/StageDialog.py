#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import datetime
import sys
sys.path.append("../..")
from control.DataBase import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class StageDialog(QDialog):
    def __init__(self,parent=None):
	super(StageDialog,self).__init__(parent)

	self.resize(1500,810)
	self.tableView=QTableWidget()
	self.tableView.setSelectionBehavior(QAbstractItemView.SelectColumns)
	self.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.rowHeight=25
	self.tableView.verticalHeader().setDefaultSectionSize(self.rowHeight)
	gridLay=QHBoxLayout()
	gridLay.addWidget(self.tableView)
	self.pageValue=30
	self.tableView.setRowCount(self.pageValue+1)
	self.tableView.hideRow(self.pageValue)
	self.tableView.horizontalHeader().setDefaultSectionSize(25)
	self.verticalScroll=QScrollBar(Qt.Vertical)
	self.maxValue=100
	self.minValue=0
	self.curValue=0
	self.verticalScroll.setRange(self.minValue,self.maxValue)
	self.connect(self.verticalScroll,SIGNAL("valueChanged(int)"),self.currentVValueSlot)
	self.horizontalScroll=QScrollBar(Qt.Horizontal)
	self.maxTime=100
	self.minTime=0
	self.pageTime=100
	self.curTime=0
	self.tableView.setColumnCount(self.pageTime)
	self.horizontalScroll.setRange(self.minTime,self.maxTime)
	self.connect(self.horizontalScroll,SIGNAL("valueChanged(int)"),self.currentHValueSlot)
	gridLay.addWidget(self.verticalScroll)
	mainLay=QVBoxLayout()
	mainLay.addLayout(gridLay)
	mainLay.addWidget(self.horizontalScroll)
	self.setLayout(mainLay)
	self.tableView.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
	self.tableView.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
	self.connect(self.tableView.horizontalHeader(),SIGNAL("sectionClicked(int)"),self.updateDialog)
	self.slider=0
	self.end=False
	#self.oldList=[0]*self.pageValue
	
    def updateDialog(self,column):
	self.slider.setValue(column)

    def updatAPEData(self,dataBase,APEdbFilePath,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.APEdbFilePath=APEdbFilePath
	self.minTime=minTime
	self.maxTime=maxTime
	self.flag=flag
	self.minValue=0
	if self.flag=="m":
	    order_sql = "SELECT * FROM "+self.dataBase.snMTableName+" order by sn asc"
	else:
	    order_sql = "SELECT * FROM "+self.dataBase.snSTableName+" order by sn asc"
	self.snAll=self.dataBase.fetchall(self.APEdbFilePath,order_sql)
	if self.snAll!=0:
	    self.maxValue=len(self.snAll)
	else:
	    self.maxValue=0
	    self.maxTime=0
	self.row1=self.minValue
	self.row2=self.pageValue+1
	self.col1=self.minTime
	self.col2=self.pageTime+1
	self.verticalScroll.setRange(self.minValue,self.maxValue)
	self.horizontalScroll.setRange(self.minTime,self.maxTime)

    def currentHValueSlot(self,time):
	if self.maxTime!=0 and self.maxValue!=0:
	    if time<=self.pageTime/2:
	        self.col1=self.minTime
	        self.col2=self.pageTime+1
		self.end=False
		#self.oldList=[]
		#for i in range(0,self.pageValue):
		    #item=self.tableView.item(i,time-self.curTime)
		    
	    elif time>=self.maxTime-self.pageTime/2:
	        self.col1=self.maxTime-self.pageTime-1
	        self.col2=self.maxTime
		#if self.end==False:
		    #self.oldList=[]
		#self.end=True	
	    else:
	        self.col1=time-self.pageTime/2
	        self.col2=time+self.pageTime/2+1
		self.end=False
	        #self.oldList=[]

	    self.curTime=time
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue+1)
	    self.tableView.hideRow(self.pageValue)
	    self.tableView.setColumnCount(self.pageTime+1)
	    self.updateAPEDialog(self.row1,self.row2,self.col1,self.col2,self.end)
	else:
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue+1)
	    self.tableView.hideRow(self.pageValue)
	    self.tableView.setColumnCount(self.pageTime)
		
    def currentVValueSlot(self,value):
	self.curValue=value
	if self.maxValue!=0 and self.maxTime!=0:
	    if value<=self.pageValue/2:
	        self.row1=self.minValue
	        self.row2=self.pageValue+1
	    elif value>=self.maxValue-self.pageValue/2:
	        self.row1=self.maxValue-self.pageValue-1
	        self.row2=self.maxValue
	    else:
	        self.row1=value-self.pageValue/2
	        self.row2=value+self.pageValue/2+1
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue+1)
	    self.tableView.hideRow(self.pageValue)
	    self.tableView.setColumnCount(self.pageTime+1)
	    self.updateAPEDialog(self.row1,self.row2,self.col1,self.col2)
	else:
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue+1)
	    self.tableView.hideRow(self.pageValue)
	    self.tableView.setColumnCount(self.pageTime)

    def updateAPEDialog(self,r1,r2,c1,c2,orientation=True):
	stringList=QStringList()
	stringList.clear()
	for i in range(c1,c2):
	    stringList.append(str(i))
	    self.tableView.setHorizontalHeaderLabels(stringList)
	for k in range(r1,r2):
	    j=k-r1
	    stringList=self.snAll[k]
	    temp=-1 #forward column
	    self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	    for i in range(0,20):
		if stringList[9+i]!=-1:
		    if stringList[9+i]>=c1 and stringList[9+i]<c2:
		        column=stringList[9+i]-c1
		        self.tableView.setItem(j,column,QTableWidgetItem(str(i-5)))	
			print j,column
		        if i<5:
			    self.tableView.item(j,column).setBackground(QBrush(QColor("gray")))
		        else:
			    self.tableView.item(j,column).setBackground(QBrush(QColor(193,210,255)))
		        if temp!=-1:
			    item=self.tableView.item(j,temp)
			    for i in range(temp+1,column):
			        self.tableView.setItem(j,i,QTableWidgetItem(item.text()))
			        self.tableView.item(j,i).setBackground(item.background())					
		        temp=column	
	    fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(stringList[4])
	    r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	    if r!=0:
	    	for e in range(len(r)):
		    stringList=r[e]
		    if stringList[6]!="Misc Reg":
			column=int(stringList[1])-c1
			if column>=c1 and column<c2:
		    	    item=self.tableView.item(j,column)
		    	    if item!=None:
		    	        if stringList[5]=="W":
		        	    if item.background()==QBrush(QColor(0,255,0)):
			    	        item.setBackground(QBrush(QColor(255,153,18)))
		        	    elif item.background()!=QBrush(QColor(255,153,18)):
			    	        item.setBackground(QBrush(QColor(255,0,0)))
		    	        elif stringList[5]=="R":
		        	    if item.background()==QBrush(QColor(255,0,0)):
			   	        item.setBackground(QBrush(QColor(255,153,18)))
		        	    elif item.background()!=QBrush(QColor(255,153,18)):
			    	        item.setBackground(QBrush(QColor(0,255,0)))
	    
    def wheelEvent(self,event):
     	if event.orientation()==Qt.Vertical:    
	    data=event.delta()
	    if data>0:
	 	self.verticalScroll.setValue(self.verticalScroll.value()-3)
	    else:
	 	self.verticalScroll.setValue(self.verticalScroll.value()+3)	

    def resizeEvent(self,event):
	self.pageValue=(self.height()-35)/self.rowHeight-1
	self.tableView.setRowCount(self.pageValue)
	self.currentVValueSlot(self.curValue)



