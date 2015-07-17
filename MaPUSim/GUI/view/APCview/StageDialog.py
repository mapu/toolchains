#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import datetime
import re
import sys
sys.path.append("../..")
from control.DataBase import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class StageDialog(QDialog):
    def __init__(self,parent=None):
	super(StageDialog,self).__init__(parent)

	self.setFixedSize(1500,770)
	self.openFlag=-1
	self.tableView=QTableWidget()
	self.tableView.setSelectionBehavior(QAbstractItemView.SelectColumns)
	self.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.tableView.horizontalHeader().setResizeMode(QHeaderView.Fixed)
	self.tableView.verticalHeader().setResizeMode(QHeaderView.Fixed)
	self.tableView.verticalHeader().setHighlightSections(False)
	self.rowHeight=23
	self.tableView.verticalHeader().setDefaultSectionSize(self.rowHeight)
	self.columnWidth=20
	self.tableView.horizontalHeader().setDefaultSectionSize(self.columnWidth)
	self.pageValue=30
	self.tableView.setRowCount(self.pageValue)
	self.tableView.horizontalHeader().setDefaultSectionSize(25)
	self.verticalScroll=QScrollBar(Qt.Vertical)
	self.maxValue=100
	self.minValue=0
	self.oldValue=0
	self.verticalScroll.setRange(self.minValue,self.maxValue)
	self.connect(self.verticalScroll,SIGNAL("valueChanged(int)"),self.currentVValueSlot)
	self.horizontalScroll=QScrollBar(Qt.Horizontal)
	self.maxTime=100
	self.minTime=0
	self.pageTime=60
	self.curTime=0
	self.tableView.setColumnCount(self.pageTime+1)
	self.horizontalScroll.setRange(self.minTime,self.maxTime)
	self.connect(self.horizontalScroll,SIGNAL("valueChanged(int)"),self.currentHValueSlot)
	self.tableView.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
	self.tableView.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
	self.connect(self.tableView.horizontalHeader(),SIGNAL("sectionClicked(int)"),self.updateDialog)
	self.connect(self.tableView,SIGNAL("clicked(QModelIndex)"),self.updateDialogIndex)
	self.slider=0
	self.headerHeight=38  #header text string length is 2,height
	self.headerLength=2  #header text string length
	self.blankHeight=42  #the edging height
	self.headerFlag=0  #1,code set widget size,not call resizeEvent;  0, drag widget call resizeEvent
	gridLay=QHBoxLayout()
	gridLay.addWidget(self.tableView)
	gridLay.addWidget(self.verticalScroll)
	mainLay=QVBoxLayout()
	mainLay.addLayout(gridLay)
	mainLay.addWidget(self.horizontalScroll)
	self.setLayout(mainLay)
	
    def updateDialog(self,column):
	text=self.tableView.horizontalHeaderItem(column).text()	
	text.replace(QString("\n"),QString(""))
	self.slider.setValue(int(text))

    def updateDialogIndex(self,index):
	text=self.tableView.horizontalHeaderItem(index.column()).text()	
	text.replace(QString("\n"),QString(""))
	self.slider.setValue(int(text))

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
	self.row2=self.pageValue-1
	self.col1=self.minTime
	self.col2=self.pageTime-1
	self.verticalScroll.setRange(self.minValue,self.maxValue)
	self.horizontalScroll.setRange(self.minTime,self.maxTime)

    def currentHValueSlot(self,time):
	num=0
	orientation=0
	c=self.col2
	if self.maxTime!=0 and self.maxValue!=0:
	    if time<=self.pageTime/2:
	        self.col1=self.minTime
	        self.col2=self.pageTime-1    
	    elif time>=self.maxTime-self.pageTime/2:
	        self.col1=self.maxTime-self.pageTime
	        self.col2=self.maxTime-1
	    else:
	        self.col1=time-self.pageTime/2
	        self.col2=time+self.pageTime/2-1
	    self.curTime=time
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue)
	    self.tableView.setColumnCount(self.pageTime+1)
	    hvFlag=1
	    string=str(self.col2)
	    self.headerFlag=1
	    if len(string)>len(str(c)):
		self.headerHeight=self.headerHeight+15*(len(string)-len(str(c)))
		self.tableView.setFixedSize(self.tableView.width(),self.tableView.height()+15*(len(string)-len(str(c))))
		self.setFixedSize(self.width(),self.height()+15*(len(string)-len(str(c))))
	    else:
		self.headerHeight=self.headerHeight-15*(len(str(c))-len(string))
		self.tableView.setFixedSize(self.tableView.width(),self.tableView.height()-15*(len(str(c))-len(string)))
		self.setFixedSize(self.width(),self.height()-15*(len(str(c))-len(string)))
	    self.updateStageValue(self.row1,self.row2,self.col1,self.col2,num,orientation,hvFlag)
	else:
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue)
	    self.tableView.setColumnCount(self.pageTime+1)
		
    def currentVValueSlot(self,value):
	headerHeight=self.tableView.horizontalHeader().height()
	num=0
	orientation=0
	if self.maxValue!=0 and self.maxTime!=0:
	    if value<=self.pageValue/2+10:
	        self.row1=self.minValue
	        self.row2=self.pageValue-1
	        self.oldValue=value
	        self.tableView.clear()
	        self.tableView.setRowCount(self.pageValue)
	        self.tableView.setColumnCount(self.pageTime+1)
	        self.updateStageValue(self.row1,self.row2,self.col1,self.col2,num,orientation)
	    elif value>=self.maxValue-self.pageValue/2-10:
	        self.row1=self.maxValue-self.pageValue
	        self.row2=self.maxValue-1
	        self.oldValue=value
	        self.tableView.clear()
	        self.tableView.setRowCount(self.pageValue)
	        self.tableView.setColumnCount(self.pageTime+1)
	        self.updateStageValue(self.row1,self.row2,self.col1,self.col2,num,orientation)
	    else:
	        self.row1=value-self.pageValue/2
	        self.row2=value+self.pageValue/2-1
		if value>self.oldValue:
		    orientation=1
		    num=value-self.oldValue
		else:
		    orientation=2
		    num=self.oldValue-value
	        self.oldValue=value
	        self.updateStageValue(self.row1,self.row2,self.col1,self.col2,num,orientation)		
	else:
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue)
	    self.tableView.setColumnCount(self.pageTime+1)

    def updateStageValue(self,r1,r2,c1,c2,num,orientation,hvFlag=0):
	if orientation==0:
	    stringList=QStringList()
	    stringList.clear()
	    for i in range(c1,c2+1):
	        pattern = re.compile('.{1,1}')
	        stringList.append('\n'.join(pattern.findall(str(i))))
	    self.tableView.setHorizontalHeaderLabels(stringList)
	    for k in range(r1,r2+1):
	        j=k-r1
	        stringList=self.snAll[k]
	        temp=-1 #forward column
	        self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	        for i in range(0,20):
		    if stringList[9+i]!=-1:
		        if stringList[9+i]>=c1 and stringList[9+i]<=c2:
		            column=stringList[9+i]-c1
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
			    else:
			        if i>0:
				    for h in range(0,column):
				        self.tableView.setItem(j,h,QTableWidgetItem(str(i-1-5)))
					if i-1<5:
				            self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))
					else:
					    self.tableView.item(j,column).setBackground(QBrush(QColor(193,210,255)))					
		            temp=column	
	        fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(stringList[4])
	        r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	        if r!=0:
	    	    for e in range(len(r)):
		        stringList=r[e]
		        if stringList[6]!="Misc Reg":
			    if int(stringList[1])>=c1 and int(stringList[1])<=c2:
			        column=int(stringList[1])-c1
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
	elif orientation==1:
	    for k in range(0,num):
	    	self.tableView.removeRow(0)
	    self.tableView.setRowCount(self.pageValue)
	    for k in range(0,num):
		stringList=self.snAll[r2+k]
		j=self.pageValue-num+k
	        temp=-1 #forward column
	        self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	        for i in range(0,20):
		    if stringList[9+i]!=-1:
		        if stringList[9+i]>=c1 and stringList[9+i]<=c2:
		            column=stringList[9+i]-c1
		            self.tableView.setItem(j,column,QTableWidgetItem(str(i-5)))	
		            if i<5:
			        self.tableView.item(j,column).setBackground(QBrush(QColor("gray")))
		            else:
			        self.tableView.item(j,column).setBackground(QBrush(QColor(193,210,255)))
		            if temp!=-1:
			        item=self.tableView.item(j,temp)
			        for s in range(temp+1,column):
			            self.tableView.setItem(j,s,QTableWidgetItem(item.text()))
			            self.tableView.item(j,s).setBackground(item.background())
			    else:
			        if i>0:
				    for h in range(0,column):
				        self.tableView.setItem(j,h,QTableWidgetItem("S"))
				        self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))					
		            temp=column	
	        fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(stringList[4])
	        r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	        if r!=0:
	    	    for e in range(len(r)):
		        stringList=r[e]
		        if stringList[6]!="Misc Reg":
			    if int(stringList[1])>=c1 and int(stringList[1])<=c2:
			        column=int(stringList[1])-c1
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
	else:
	    for k in range(0,num):
	    	self.tableView.removeRow(self.pageValue-k-1)
	    self.tableView.setRowCount(self.pageValue)
	    for k in range(0,num):
		stringList=self.snAll[r1-k]
		j=0
		self.tableView.insertRow(j)
	        temp=-1 #forward column
	        self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	        for i in range(0,20):
		    if stringList[9+i]!=-1:
		        if stringList[9+i]>=c1 and stringList[9+i]<=c2:
		            column=stringList[9+i]-c1
		            self.tableView.setItem(j,column,QTableWidgetItem(str(i-5)))	
		            if i<5:
			        self.tableView.item(j,column).setBackground(QBrush(QColor("gray")))
		            else:
			        self.tableView.item(j,column).setBackground(QBrush(QColor(193,210,255)))
		            if temp!=-1:
			        item=self.tableView.item(j,temp)
			        for s in range(temp+1,column):
			            self.tableView.setItem(j,s,QTableWidgetItem(item.text()))
			            self.tableView.item(j,s).setBackground(item.background())
			    else:
			        if i>0:
				    for h in range(0,column):
				        self.tableView.setItem(j,h,QTableWidgetItem("S"))
				        self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))					
		            temp=column	
	        fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE spumpu = "+"'"+self.flag+"'"+" and sn = "+str(stringList[4])
	        r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	        if r!=0:
	    	    for e in range(len(r)):
		        stringList=r[e]
		        if stringList[6]!="Misc Reg":
			    if int(stringList[1])>=c1 and int(stringList[1])<=c2:
			        column=int(stringList[1])-c1
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
	if hvFlag==0:
	    stringList=self.snAll[r1]
	    if stringList[9]>self.pageTime/2:
		time=stringList[9]+self.pageTime/3
	    else:
		time=stringList[9]
	    self.horizontalScroll.setValue(time)
    
    def wheelEvent(self,event):
     	if event.orientation()==Qt.Vertical:    
	    data=event.delta()
	    if data>0:
	 	self.verticalScroll.setValue(self.verticalScroll.value()-10)
	    else:
	 	self.verticalScroll.setValue(self.verticalScroll.value()+10)	

    def resizeEvent(self,event):
	if self.headerFlag==1:
	    self.headerFlag=0
	else:
	    self.pageValue=(self.height()-self.blankHeight-self.headerHeight)/self.rowHeight
	    self.tableView.setRowCount(self.pageValue)
	    self.currentVValueSlot(self.oldValue)

    def closeEvent(self,event):
	self.openFlag=-1


