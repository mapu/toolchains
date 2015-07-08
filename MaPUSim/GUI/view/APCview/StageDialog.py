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

	self.setMinimumSize(300,200)
	self.resize(900,500)
	self.tableView=QTableView()
	self.tableView.setSelectionBehavior(QAbstractItemView.SelectColumns)
	self.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.tableView.verticalHeader().setDefaultSectionSize(25)
	self.model=QStandardItemModel(self.tableView)
	self.tableView.setModel(self.model)
	#self.tableView.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
	self.tableView.horizontalHeader().setDefaultSectionSize(60)
	self.setModal(True)
	gridLay=QGridLayout()
	gridLay.addWidget(self.tableView,0,0)
	self.setLayout(gridLay)
	self.connect(self.tableView.horizontalHeader(),SIGNAL("sectionClicked(int)"),self.updateDialog)
	self.slider=0
	#self.connect(self.tableView.verticalScrollBar(),SIGNAL("valueChanged(int)"),self.verticalRangeChangedSlot)
	self.stageComboBox=QComboBox()
	self.stageComboBox.setFixedSize(100,40)
	gridLay.addWidget(self.stageComboBox,1,0)
	self.connect(self.stageComboBox,SIGNAL("currentIndexChanged(int)"),self.anotherPage)
	
    def updateDialog(self,column):
	self.slider.setValue(column)

    #def verticalRangeChangedSlot(self,value):
	#print value

    def anotherPage(self,num):
	self.updateAPEDialog(num*self.step,(num+1)*self.step,num)	
	self.updateAPEColor(num*self.step,(num+1)*self.step,num)

    def updatAPEData(self,dataBase,APEdbFilePath,minTime,maxTime,flag,step):
	self.dataBase=dataBase
	self.APEdbFilePath=APEdbFilePath
	self.minTime=minTime
	self.maxTime=maxTime
	self.flag=flag
	self.step=step

    def updateAPEDialog(self,row0,row1,num):
	self.model.clear()
	headerList=QStringList()
	headerList.clear()
	for k in range(self.minTime,self.maxTime+1):
	    headerList.append(str(k))
	self.model.setHorizontalHeaderLabels(headerList)
	fetchall_sql = "SELECT * FROM "+self.dataBase.snTableName+" WHERE spumpu = "+"'"+self.flag+"'"
	r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):	
		stringList=r[e]
		if stringList[7]!="nop":
		    temp=-1 #forward column
		    j=int(stringList[4])-1
		    if j>=row0 and j<row1:
			j=j-self.step*num
		        #print "row",j
		        if stringList[7]=='':
		    	    self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[4])))
		        else:
		            self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[4])+":"+stringList[7]))
		        for i in range(0,20):
		            if stringList[9+i]!=-1:
			        column=stringList[9+i]-self.minTime
			        self.model.setItem(j,column,QStandardItem(str(i-5)))	 
			        if i<5:
				    self.model.item(j,column).setBackground(QBrush(QColor("gray")))
			        else:
				    self.model.item(j,column).setBackground(QBrush(QColor(193,210,255)))
			        if temp!=-1:
				    item=self.model.item(j,temp)
				    for i in range(temp+1,column):
				        self.model.setItem(j,i,QStandardItem(item.text()))
				        self.model.item(j,i).setBackground(item.background())					
			        temp=column	
	    i=datetime.datetime.now()
            print ("end row %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))	

    def updateAPEColor(self,row0,row1,num):
	rowCount=self.model.rowCount()
	fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE spumpu = "+"'"+self.flag+"'"
	r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		stringList=r[e]
		if stringList[6]!="Misc Reg":
		    j=int(stringList[4])-1
		    if j>=row0 and j<row1:
			j=j-self.step*num
		        if j<=rowCount:
		            item=self.model.item(j,int(stringList[1])-self.minTime)
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
	    

