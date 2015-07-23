#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import datetime
import re
import sys
sys.path.append("../..")
from control.DataBase import*
from TableModel import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class StageDialog(QDialog):
    def __init__(self,parent=None):
	super(StageDialog,self).__init__(parent)

	self.resize(1500,800)
	self.setMinimumSize(100,600)
	self.openFlag=-1
	self.arrayData=[["" for col in range(1)] for row in range(1)]
	self.tableModel=TableModel(self.arrayData)
	self.tableView=QTableView()
	self.tableView.setModel(self.tableModel)
	self.tableView.setSelectionBehavior(QAbstractItemView.SelectColumns)
	self.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)
	#self.tableView.horizontalHeader().setResizeMode(QHeaderView.Fixed)
	#self.tableView.verticalHeader().setResizeMode(QHeaderView.Fixed)
	self.tableView.verticalHeader().setHighlightSections(False)
	self.rowHeight=23
	self.tableView.verticalHeader().setDefaultSectionSize(self.rowHeight)
	self.columnWidth=23
	self.tableView.horizontalHeader().setDefaultSectionSize(self.columnWidth)
	#self.tableView.resizeColumnsToContents()
	self.connect(self.tableView.horizontalHeader(),SIGNAL("sectionClicked(int)"),self.updateDialog)
	self.connect(self.tableView,SIGNAL("clicked(QModelIndex)"),self.updateDialogIndex)
	self.connect(self.tableView.verticalScrollBar(),SIGNAL("valueChanged(int)"),self.scrollToStage)
	self.slider=0
	blank=QLabel()
	blank.setFixedSize(500,25)
	searchLabel=QLabel("Key:")
	searchLabel.setFixedSize(30,25)
	self.searchEdit=QLineEdit()
	self.searchEdit.setFixedSize(200,25)
	self.searchButton=QPushButton("Search")
	self.searchButton.setFixedSize(100,25)
	self.connect(self.searchButton,SIGNAL("clicked()"),self.searchSlot)
	self.preButton=QPushButton("Previous")
	self.preButton.setEnabled(False)
	self.preButton.setFixedSize(100,25)
	self.connect(self.preButton,SIGNAL("clicked()"),self.previousSlot)
	self.nextButton=QPushButton("Next")
	self.nextButton.setEnabled(False)
	self.nextButton.setFixedSize(100,25)
	self.connect(self.nextButton,SIGNAL("clicked()"),self.nextSlot)
	upLay=QHBoxLayout()
	upLay.addWidget(blank)
	upLay.addWidget(searchLabel)
	upLay.addWidget(self.searchEdit)
	upLay.addWidget(self.searchButton)
	upLay.addWidget(self.preButton)
	upLay.addWidget(self.nextButton)
	mainLay=QVBoxLayout()
	mainLay.addLayout(upLay)
	mainLay.addWidget(self.tableView)
	self.setLayout(mainLay)

	self.searchList=[]
	self.searchValue=0

    def updateDialog(self,column):
	self.slider.setValue(int(column))

    def updateDialogIndex(self,index):
	self.slider.setValue(index.column())

    def updatAPEData(self,dataBase,APEdbFilePath,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.APEdbFilePath=APEdbFilePath
	self.minTime=minTime
	self.maxTime=maxTime
	self.flag=flag
	self.minValue=0
	self.tableModel.setDataBase(self.dataBase,self.APEdbFilePath,self.flag)
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
	if self.maxTime!=0 or self.maxValue!=0:
	    i=datetime.datetime.now()
            print ("start update table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    self.initTable()
	    i=datetime.datetime.now()
            print ("end reflushmodel table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))

    def initTable(self):
	horizontalHeaderList=range(self.maxTime+1)
	self.tableModel.setHorizontalHeader(horizontalHeaderList)
	i=datetime.datetime.now()
        print ("end table horizontalHeader %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	verticalHeaderList=['0']*self.maxValue
        self.arrayData=[["" for col in range(self.maxTime+1)] for row in range(self.maxValue)]
	i=datetime.datetime.now()
        print ("end init array data %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	for i in range(0,self.maxValue):
	    stringList=self.snAll[i]	   
	    verticalHeaderList[i]=str(stringList[4])+":"+stringList[6]+":"+stringList[7]
	    temp=-1
	    for j in range(0,20):
	        self.arrayData[i][stringList[9+j]]=str(j-5)
	        for k in range(temp+1,stringList[9+j]):
		    self.arrayData[i][k]=self.arrayData[i][temp]
	        temp=stringList[9+j]
	self.tableModel.setVerticalHeader(verticalHeaderList)
	i=datetime.datetime.now()
        print ("end table verticalHeader %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	self.tableModel.setModalDatas(self.arrayData)
	i=datetime.datetime.now()
        print ("end update table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
    	self.tableModel.refrushModel()
	self.scrollToStage(0)

    def scrollToStage(self,value):
	self.tableView.horizontalScrollBar().setValue(self.snAll[value][9])
 
    def searchSlot(self):
	self.tableView.setStyleSheet("QHeaderView.section{color: black;}")
	key=self.searchEdit.text()
	self.searchList=[]
	self.searchValue=0
	if key!="":
	    self.tableModel.key=key
	    if self.flag=="m":
	        order_sql="SELECT * FROM "+self.dataBase.snMTableName+" WHERE pc LIKE "+"'%"+str(key)+"%'"+" or dis LIKE "+"'%"+str(key)+"%'"
	    else:
	    	order_sql="SELECT * FROM "+self.dataBase.snSTableName+" WHERE pc LIKE "+"'%"+str(key)+"%'"+" or dis LIKE "+"'%"+str(key)+"%'"
	    r=self.dataBase.fetchall(self.APEdbFilePath,order_sql)
	    if r!=0:
		for e in range(len(r)):
	            self.searchList.append(self.snAll.index(r[e]))
		if len(self.searchList)>1:
		    self.nextButton.setEnabled(True)
		self.tableView.verticalScrollBar().setValue(self.searchList[0])
    	        self.tableModel.refrushModel()
	    else:
		QMessageBox.warning(self,"Warning","No information matching!")
	
    def previousSlot(self):
	if self.searchValue>0:
	    self.searchValue-=1
	    self.nextButton.setEnabled(True)
	    while self.searchValue>self.tableView.verticalScrollBar().value() and self.searchValue>0:
		self.searchValue-=1
	    self.tableView.verticalScrollBar().setValue(self.searchList[self.searchValue])
	    if self.searchValue==0:
		self.preButton.setEnabled(False)    

    def nextSlot(self):
	if self.searchValue<len(self.searchList)-1:
	    self.searchValue+=1
	    self.preButton.setEnabled(True)
	    while self.tableView.verticalScrollBar().value()<self.searchValue<self.tableView.verticalScrollBar().value()+20 and self.searchValue<len(self.searchList)-1:
		self.searchValue+=1
	    self.tableView.verticalScrollBar().setValue(self.searchList[self.searchValue])
	    if self.searchValue==len(self.searchList)-1:
		self.nextButton.setEnabled(False)

    def addRow(self,rowList):
	self.arrayData.append(rowList)
    	self.tableModel.refrushModel()

    def removeRow(self,row):
	del self.arrayData[row]
    	self.tableModel.refrushModel()

    def clear(self):
	self.arrayData=[]
    	self.tableModel.refrushModel()

    def rowCount(self):
  	return self.tableModel.rowCount(QModelIndex())

    def closeEvent(self,event):
	self.openFlag=-1


