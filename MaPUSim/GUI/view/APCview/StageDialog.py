#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
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
	self.tableView.setSelectionBehavior(QAbstractItemView.SelectRows)
	self.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)
	self.tableView.verticalHeader().setDefaultSectionSize(25)
	self.model=QStandardItemModel(self.tableView)
	self.tableView.setModel(self.model)
	self.tableView.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents )
	self.setModal(True)
	gridLay=QGridLayout()
	gridLay.addWidget(self.tableView,0,0)
	self.setLayout(gridLay)

    def updateAPE0Dialog(self,dataBase,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.model.clear()
	j=0
	for i in range(minTime,maxTime+1):
	    self.model.setHorizontalHeaderItem(j,QStandardItem(str(i)))
	    j+=1
	fetchall_sql = "SELECT * FROM "+dataBase.snTableName
	r=self.dataBase.fetchall(dataBase.APE0dbFilePath,fetchall_sql)
	j=0 #row
	if r!=0:
	    for e in range(len(r)):	
		stringList=r[e]
		if stringList[2]==flag:
		    if stringList[6]!="nop":
			temp=-1 #forward column
		        self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[3])+":"+stringList[6]))
		        for i in range(0,20):
		            if stringList[8+i]!=-1:
			        column=stringList[8+i]-minTime
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
		        j+=1
		    
    def updateAPE1Dialog(self,dataBase,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.model.clear()
	j=0
	for i in range(minTime,maxTime+1):
	    self.model.setHorizontalHeaderItem(j,QStandardItem(str(i)))
	    j+=1
	fetchall_sql = "SELECT * FROM "+dataBase.snTableName
	r=self.dataBase.fetchall(dataBase.APE1dbFilePath,fetchall_sql)
	j=0 #row
	if r!=0:
	    for e in range(len(r)):	
		stringList=r[e]
		if stringList[2]==flag:
		    if stringList[6]!="nop":
			temp=-1 #forward column
		        self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[3])+":"+stringList[6]))
		        for i in range(0,20):
		            if stringList[8+i]!=-1:
			        column=stringList[8+i]-minTime
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
		        j+=1

    def updateAPE2Dialog(self,dataBase,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.model.clear()
	j=0
	for i in range(minTime,maxTime+1):
	    self.model.setHorizontalHeaderItem(j,QStandardItem(str(i)))
	    j+=1
	fetchall_sql = "SELECT * FROM "+dataBase.snTableName
	r=self.dataBase.fetchall(dataBase.APE2dbFilePath,fetchall_sql)
	j=0 #row
	if r!=0:
	    for e in range(len(r)):	
		stringList=r[e]
		if stringList[2]==flag:
		    if stringList[6]!="nop":
			temp=-1 #forward column
		        self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[3])+":"+stringList[6]))
		        for i in range(0,20):
		            if stringList[8+i]!=-1:
			        column=stringList[8+i]-minTime
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
		        j+=1

    def updateAPE3Dialog(self,dataBase,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.model.clear()
	j=0
	for i in range(minTime,maxTime+1):
	    self.model.setHorizontalHeaderItem(j,QStandardItem(str(i)))
	    j+=1
	fetchall_sql = "SELECT * FROM "+dataBase.snTableName
	r=self.dataBase.fetchall(dataBase.APE3dbFilePath,fetchall_sql)
	j=0 #row
	if r!=0:
	    for e in range(len(r)):	
		stringList=r[e]
		if stringList[2]==flag:
		    if stringList[6]!="nop":
			temp=-1 #forward column
		        self.model.setVerticalHeaderItem(j,QStandardItem(str(stringList[3])+":"+stringList[6]))
		        for i in range(0,20):
		            if stringList[8+i]!=-1:
			        column=stringList[8+i]-minTime
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
		        j+=1
