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
	stringList=QStringList()
	stringList.clear()
	for i in range(0,self.pageValue):
	    stringList.append(str(i))
	self.tableView.setVerticalHeaderLabels(stringList)
	self.scroll=QScrollBar(Qt.Vertical)
	self.maxValue=100
	self.minValue=0
	self.scroll.setRange(self.minValue,self.maxValue)
	gridLay.addWidget(self.scroll)
	self.connect(self.scroll,SIGNAL("valueChanged(int)"),self.currentValueSlot)
	self.setLayout(gridLay)
	#self.tableView.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
	self.curValue=0
	self.connect(self.tableView.horizontalHeader(),SIGNAL("sectionClicked(int)"),self.updateDialog)
	self.slider=0
	self.minTime=0
	self.maxTime=100
	self.tableView.setColumnCount(self.maxTime-self.minTime+1)
	self.time=0
	
    def updateDialog(self,column):
	self.slider.setValue(column)

    def updatAPEData(self,dataBase,APEdbFilePath,minTime,maxTime,flag,minValue,maxValue):
	self.dataBase=dataBase
	self.APEdbFilePath=APEdbFilePath
	self.minTime=minTime
	self.maxTime=maxTime
	self.flag=flag
	self.minValue=minValue
	self.maxValue=maxValue
	self.scroll.setRange(self.minValue,self.maxValue)

    def currentValueSlot(self,value):
	self.curValue=value
	self.tableView.clear()
	self.tableView.setRowCount(self.pageValue+1)
	self.tableView.hideRow(self.pageValue)
	self.tableView.setColumnCount(self.maxTime-self.minTime+1)
	stringList=QStringList()
	stringList.clear()
	for i in range(self.minTime,self.maxTime+1):
	    stringList.append(str(i))
	self.tableView.setHorizontalHeaderLabels(stringList)
	if value>self.pageValue/2:
	    r1=value-self.pageValue/2
	    r2=value+self.pageValue/2
	    if self.pageValue%2==1:
		r2=r2+1
	else:
	    r1=0
	    r2=self.pageValue
	if value>self.maxValue-self.pageValue/2:
	    r1=self.maxValue-self.pageValue
	    r2=self.maxValue
	self.updateAPEDialog(r1,r2)

    def updateAPEDialog(self,r1,r2):
	blankList=[]
	for k in range(r1,r2):
	    fetchall_sql = "SELECT * FROM "+self.dataBase.snTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(k+1)
	    r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	    if r!=0:
		j=k-r1
	        for e in range(len(r)):	
		    stringList=r[e]
		    if stringList[7]!="nop":
		        temp=-1 #forward column
		        if stringList[7]=='':
		    	    self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])))
		        else:
		            self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
		        for i in range(0,20):
		            if stringList[9+i]!=-1:
			        column=stringList[9+i]-self.minTime
			        self.tableView.setItem(j,column,QTableWidgetItem(str(i-5)))	
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
		fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(k+1)
		r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
		if r!=0:
	    	    for e in range(len(r)):
			stringList=r[e]
			if stringList[6]!="Misc Reg":
		    	    item=self.tableView.item(j,int(stringList[1])-self.minTime)
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
	    else:
	 	blankList.append(k-r1)	
	if len(blankList)>0:
	    for i in range(0,len(blankList)):
		self.tableView.removeRow(blankList[i])
		self.insertAPEDialog(r1-i-self.time)

    def insertAPEDialog(self,k):
	fetchall_sql = "SELECT * FROM "+self.dataBase.snTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(k)
	r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	if r!=0:
	    j=0
	    for e in range(len(r)):	
		stringList=r[e]
		if stringList[7]!="nop":
		    temp=-1 #forward column
		    self.tableView.insertRow(j)
		    if stringList[7]=='':
		    	self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])))
		    else:
		        self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
		    for i in range(0,20):
		        if stringList[9+i]!=-1:
			    column=stringList[9+i]-self.minTime
			    self.tableView.setItem(j,column,QTableWidgetItem(str(i-5)))	
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
	    fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(k)
	    r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	    if r!=0:
	    	for e in range(len(r)):
		    stringList=r[e]
		    if stringList[6]!="Misc Reg":
		    	item=self.tableView.item(j,int(stringList[1])-self.minTime)
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
	else:
	    self.time=self.time+1
	    self.insertAPEDialog(k-1)
	    
    def wheelEvent(self,event):
     	if event.orientation()==Qt.Vertical:    
	    data=event.delta()
	    if data>0:
	 	self.scroll.setValue(self.scroll.value()-3)
	    else:
	 	self.scroll.setValue(self.scroll.value()+3)	

    def resizeEvent(self,event):
	self.pageValue=(self.height()-35)/self.rowHeight-1
	self.tableView.setRowCount(self.pageValue)
	self.currentValueSlot(self.curValue)



