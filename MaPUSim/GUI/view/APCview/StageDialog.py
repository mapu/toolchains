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
	self.tableView.verticalHeader().setHighlightSections(False)
	self.rowHeight=25
	self.tableView.verticalHeader().setDefaultSectionSize(self.rowHeight)
	gridLay=QHBoxLayout()
	gridLay.addWidget(self.tableView)
	self.pageValue=100
	self.tableView.setRowCount(self.pageValue+1)
	self.tableView.horizontalHeader().setDefaultSectionSize(25)
	self.verticalScroll=QScrollBar(Qt.Vertical)
	self.maxValue=1000
	self.minValue=0
	self.curValue=0
	self.verticalScroll.setRange(self.minValue,self.maxValue)
	self.connect(self.verticalScroll,SIGNAL("valueChanged(int)"),self.currentVValueSlot)
	self.horizontalScroll=QScrollBar(Qt.Horizontal)
	self.maxTime=100
	self.minTime=0
	self.pageTime=70
	self.curTime=0
	self.tableView.setColumnCount(self.pageTime+1)
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
	self.connect(self.tableView,SIGNAL("clicked(QModelIndex)"),self.updateDialogIndex)
	self.slider=0
	self.end=False
	self.vdown=0
	self.vend=0
	self.vend1=0
	self.vtop=0
	self.curTableValue=0
	self.connect(self.tableView.verticalScrollBar(),SIGNAL("valueChanged(int)"),self.tableScrollSlot)
	
    def tableScrollSlot(self,tableValue):
	if tableValue!=0 or tableValue!=self.pageValue-1:
	    if self.curTableValue<tableValue:
	        self.verticalScroll.setValue(self.curValue+10)
	    else:
	        if self.curValue>=10:
	            self.verticalScroll.setValue(self.curValue-10)	
	        else:
	            self.verticalScroll.setValue(0)	
	    self.curTableValue=tableValue	
	
    def updateDialog(self,column):
	text=self.tableView.horizontalHeaderItem(column).text()	
	self.slider.setValue(int(text))

    def updateDialogIndex(self,index):
	text=self.tableView.horizontalHeaderItem(index.column()).text()	
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
	self.row2=self.pageValue+1
	self.col1=self.minTime
	self.col2=self.pageTime+1
	self.verticalScroll.setRange(self.minValue,self.maxValue)
	self.horizontalScroll.setRange(self.minTime,self.maxTime)
	if self.pageValue>self.maxValue:
	    self.pageValue=self.maxValue
	if self.pageTime>self.maxTime:
	    self.pageTime=self.maxTime
	self.currentVValueSlot(0)

    def currentHValueSlot(self,time):
	if self.maxTime!=0 and self.maxValue!=0:
	    if time<=self.pageTime/2:
	        self.col1=self.minTime
	        self.col2=self.pageTime+1
		self.end=False	    
	    elif time>=self.maxTime-self.pageTime/2:
	        self.col1=self.maxTime-self.pageTime-1
	        self.col2=self.maxTime
	    else:
	        self.col1=time-self.pageTime/2
	        self.col2=time+self.pageTime/2+1
		self.end=False
	    self.curTime=time
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue+1)
	    self.tableView.setColumnCount(self.pageTime+1)
	    self.initAPEDialog(self.row1,self.row2,self.col1,self.col2,self.end)
	else:
	    self.tableView.clear()
	    self.tableView.setRowCount(self.pageValue+1)
	    self.tableView.setColumnCount(self.pageTime+1)
		
    def currentVValueSlot(self,value):
	print "scroll",value
	if value==0:
	    num=0
	    orientation=0
	    self.curValue=value
	    self.updateAPEDialog(self.col1,self.col2,num,orientation)
	elif value>self.curValue:
	    num=value-self.curValue
	    orientation=1
	    self.curValue=value
	    self.updateAPEDialog(self.col1,self.col2,num,orientation)
	else:
	    num=self.curValue-value
	    orientation=2
	    self.curValue=value
	    self.updateAPEDialog(self.col1,self.col2,num,orientation)

    def updateAPEDialog(self,c1,c2,num,orientation):
	if orientation==0:
	    stringList=QStringList()
	    stringList.clear()
	    for i in range(c1,c2):
	        stringList.append(str(i))
	    self.tableView.setHorizontalHeaderLabels(stringList)
	    self.vdown=0
	    self.row1=0
	    self.row2=self.pageValue
	    for k in range(self.row1,self.row2):
	        j=k-self.row1
	        stringList=self.snAll[k]
	        temp=-1 #forward column
	        self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	        for i in range(0,20):
		    if stringList[9+i]!=-1:
		        if stringList[9+i]>=c1 and stringList[9+i]<c2:
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
				        self.tableView.setItem(j,h,QTableWidgetItem("S"))
				        self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))					
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
	elif orientation==1:
	    if self.maxValue-self.curValue>=self.pageValue:	
	        self.vdown=self.curValue
	        for k in range(0,num):
	    	    self.tableView.removeRow(0)
	        self.tableView.setRowCount(self.pageValue+1)
		self.row1=self.curValue
		self.row2=self.curValue+self.pageValue-1
	        for k in range(0,num):
		    stringList=self.snAll[self.curValue+self.pageValue-num+k]
		    j=self.pageValue-num+k
	            temp=-1 #forward column
	            self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	            for i in range(0,20):
		        if stringList[9+i]!=-1:
		            if stringList[9+i]>=c1 and stringList[9+i]<c2:
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
				            self.tableView.setItem(j,h,QTableWidgetItem("S"))
				            self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))					
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
	    else:
		if 1:
		    if self.vend==0:
			self.vend=1
		        num=self.maxValue-self.vdown-self.pageValue
		        for k in range(0,num):
	    	            self.tableView.removeRow(0)    
	                self.tableView.setRowCount(self.pageValue+1)
			self.row2=self.vdown+self.pageValue+num-1
			self.row1=self.vdown+num-2
	                for k in range(0,num):
		            stringList=self.snAll[self.vdown+self.pageValue+k]
		            j=self.pageValue-num+k
	                    temp=-1 #forward column
	                    self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	                    for i in range(0,20):
		                if stringList[9+i]!=-1:
		                    if stringList[9+i]>=c1 and stringList[9+i]<c2:
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
				                    self.tableView.setItem(j,h,QTableWidgetItem("S"))
				                    self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))					
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
			self.vdown=self.curValue
		    else:
		        curNum=(self.height()-35)/self.rowHeight-1
			if self.maxValue-self.curValue>=curNum:
			    num=self.curValue-self.vdown
			else:
			    if self.vend1==0:
				num=curNum-self.maxValue+self.curValue+3
				self.vend1=1
			    else:
				num=0
				self.tableView.verticalScrollBar().setValue(self.pageValue-1)
			self.vdown=self.curValue
			self.row1=self.row1+num
			for i in range(0,num):
			    self.tableView.removeRow(0)    
		
	elif orientation==2:
	    curNum=(self.height()-35)/self.rowHeight-1
	    if self.curValue>curNum:
	        for k in range(0,num):
	    	    self.tableView.removeRow(self.pageValue-k-1)
	        self.tableView.setRowCount(self.pageValue+1)
		if self.curValue+num==self.maxValue:
		    self.curValue=self.curValue-1
	        for k in range(num,0,-1):
		    stringList=self.snAll[self.curValue+k]
		    j=0
		    self.tableView.insertRow(j)
	            temp=-1 #forward column
	            self.tableView.setVerticalHeaderItem(j,QTableWidgetItem(str(stringList[4])+":"+stringList[7]))
	            for i in range(0,20):
		        if stringList[9+i]!=-1:
		            if stringList[9+i]>=c1 and stringList[9+i]<c2:
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
				            self.tableView.setItem(j,h,QTableWidgetItem("S"))
				            self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))					
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
	    else:
	        self.tableView.clear()
	        self.tableView.setRowCount(self.pageValue+1)
	        self.tableView.setColumnCount(self.pageTime+1)
		self.updateAPEDialog(c1,c2,0,0)
		self.tableView.verticalScrollBar().setValue(0)
		
    def initAPEDialog(self,r1,r2,c1,c2,orientation):
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
				    self.tableView.setItem(j,h,QTableWidgetItem("S"))
				    self.tableView.item(j,h).setBackground(QBrush(QColor("gray")))					
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
	 	self.verticalScroll.setValue(self.verticalScroll.value()-10)
	    else:
	 	self.verticalScroll.setValue(self.verticalScroll.value()+10)	

    #def resizeEvent(self,event):
	#self.pageValue=(self.height()-35)/self.rowHeight-1
	#self.tableView.setRowCount(self.pageValue)
	#self.currentVValueSlot(self.curValue)



