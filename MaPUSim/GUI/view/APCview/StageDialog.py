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
	self.tableView.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents )
	self.setModal(True)
	gridLay=QGridLayout()
	gridLay.addWidget(self.tableView,0,0)
	self.setLayout(gridLay)
	self.connect(self.tableView.horizontalHeader(),SIGNAL("sectionClicked(int)"),self.updateDialog)
	self.slider=0

    def updateDialog(self,column):
	self.slider.setValue(column)

    def updateAPEDialog(self,dataBase,APEdbFilePath,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.flag=flag
	self.model.clear()
	j=0
	i=datetime.datetime.now()
        print ("start column %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	#headerList=QStringList()
	#headerList.clear()
	for i in range(minTime,maxTime+1):
	    self.model.setHorizontalHeaderItem(j,QStandardItem(str(i)))
	    j+=1
	    #headerList.append(str(i))
	self.model.setHorizontalHeaderLabels(headerList)
	i=datetime.datetime.now()
        print ("end column %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	fetchall_sql = "SELECT * FROM "+dataBase.snTableName+" WHERE spumpu = "+"'"+flag+"'"
	r=self.dataBase.fetchall(APEdbFilePath,fetchall_sql)
	if r!=0:
	    i=datetime.datetime.now()
            print ("start row %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    fetchall_sql = "SELECT * FROM "+dataBase.snTableName+" WHERE spumpu = "+"'"+flag+"'"
	    for e in range(len(r)):	
		stringList=r[e]
		if stringList[7]!="nop":
		    temp=-1 #forward column
		    j=int(stringList[4])-1
		    print "row",j
		    if stringList[7]=='':
			self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[4])))
		    else:
		        self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[4])+":"+stringList[7]))
		    for i in range(0,20):
		        if stringList[9+i]!=-1:
			    column=stringList[9+i]-minTime
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
		else:
		    self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[4])))
		    #self.tableView.hideRow(j)		
	    i=datetime.datetime.now()
            print ("end row %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))	

    def updateAPEColor(self,dataBase,APEdbFilePath,minTime,maxTime,flag):
	self.dataBase=dataBase
	rowCount=self.model.rowCount()
	fetchall_sql = "SELECT * FROM "+dataBase.regTableName+" WHERE spumpu = "+"'"+flag+"'"
	r=self.dataBase.fetchall(APEdbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		stringList=r[e]
		if int(stringList[4])<=rowCount:
		    item=self.model.item(int(stringList[4])-1,int(stringList[1])-minTime)
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
	    

