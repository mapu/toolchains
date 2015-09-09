#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import datetime
import re
import sys
sys.path.append("../..")
from control.DataBase import*
from TableModel import*
import time

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class StageDialog(QDialog):
    def __init__(self,parent=None):
	super(StageDialog,self).__init__(parent)

	self.resize(1500,800)
	self.setMinimumSize(400,600)
	self.openFlag=-1
	self.arrayData=[["" for col in range(1)] for row in range(1)]
	self.subArray=[["" for col in range(1)] for row in range(1)]
	self.tableModel=TableModel(self.subArray)
	self.tableView=QTableView()
	self.tableView.setModel(self.tableModel)
	self.tableView.setSelectionBehavior(QAbstractItemView.SelectColumns)
	self.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)
	#self.tableView.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
	self.tableView.verticalHeader().setResizeMode(QHeaderView.Fixed)
	self.tableView.verticalHeader().setHighlightSections(False)
	self.tableView.verticalHeader().setFont(QFont("Monospace"))
	self.rowHeight=23
	self.tableView.verticalHeader().setDefaultSectionSize(self.rowHeight)
	self.columnWidth=32
	self.tableView.horizontalHeader().setDefaultSectionSize(self.columnWidth)
	self.connect(self.tableView.horizontalHeader(),SIGNAL("sectionClicked(int)"),self.updateDialog)
	self.connect(self.tableView,SIGNAL("clicked(QModelIndex)"),self.updateDialogIndex)
	self.connect(self.tableView.verticalScrollBar(),SIGNAL("valueChanged(int)"),self.scrollToStage)
	self.slider=0
	self.pageCombo=QComboBox()
	self.pageCombo.addItem("1 page")
	self.page=1
	self.pageCombo.setFixedSize(100,25)
	self.connect(self.pageCombo,SIGNAL("currentIndexChanged(int)"),self.currentIndexSlot)
	blank=QLabel()
	blank.setFixedSize(300,25)
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
	upLay.addWidget(self.pageCombo)
	upLay.addWidget(blank)
	upLay.addWidget(searchLabel)
	upLay.addWidget(self.searchEdit)
	upLay.addWidget(self.searchButton)
	upLay.addWidget(self.preButton)
	upLay.addWidget(self.nextButton)
	blank1=QLabel()
	blank1.setFixedSize(500,20)
	tipLabel=QLabel("Graphic symbol:")
	tipLabel.setFixedSize(130,25)
	redLabel=QLabel()
	redLabel.setFixedSize(50,20)
	redLabel.setStyleSheet("QLabel{background:rgb(255,0,0)}")
	redtip=QLabel("Write Reg")
	redtip.setFixedSize(80,25)	
	greenLabel=QLabel()
	greenLabel.setFixedSize(50,20)
	greenLabel.setStyleSheet("QLabel{background:rgb(0,255,0)}")
	greentip=QLabel("Read Reg")
	greentip.setFixedSize(80,25)
	yellowLabel=QLabel()
	yellowLabel.setFixedSize(50,20)
	yellowLabel.setStyleSheet("QLabel{background:rgb(255,153,18)}")
	yellowtip=QLabel("Read and Write Reg")
	yellowtip.setFixedSize(150,25)
	midLay=QHBoxLayout()
	midLay.addWidget(tipLabel)
	midLay.addWidget(redLabel)
	midLay.addWidget(redtip)
	midLay.addWidget(greenLabel)
	midLay.addWidget(greentip)
	midLay.addWidget(yellowLabel)
	midLay.addWidget(yellowtip)
	midLay.addWidget(blank1)
	mainLay=QVBoxLayout()
	mainLay.addLayout(upLay)
	mainLay.addLayout(midLay)
	mainLay.addWidget(self.tableView)
	self.setLayout(mainLay)
	self.snList=[]
	self.snList.append(0)
	self.searchList=[]
	self.searchValue=0

	self.stagetime=0
	self.regtime=0

    def currentIndexSlot(self,index):
	if len(self.snList)>1:
	    i=index
	    if (index+1)==self.page:
		a=self.maxValue
		self.subVerticalHeaderList=0
		self.sub=0
		self.subArray=0
	        self.minTime=int(self.snAll[self.snList[i]][9])
		self.subArray=[x[:] for x in [[""]*(self.maxTime+1-self.minTime)]*(a-self.snList[i])]
	        self.subVerticalHeaderList=['0']*(a-self.snList[i]+1)
	        self.sub=[x[:] for x in [[""]*(self.maxTime+1)]*(a-self.snList[i]+1)]
	        self.subHorizontalHeaderList=range(self.minTime,self.maxTime+1)
	        self.subVerticalHeaderList=self.verticalHeaderList[self.snList[i]:a]
	        for j in range(self.snList[i],a):
		    self.subArray[j-self.snList[i]]=self.arrayData[j][self.minTime:self.maxTime+1]
	        #self.subArray=self.arrayData[self.snList[i]:a]
	    else:
	        a=index+1
		self.subVerticalHeaderList=0
		self.minTime=int(self.snAll[self.snList[i]][9])
		self.subArray=[x[:] for x in [[""]*(self.maxTime+1-self.minTime)]*(self.snList[a]-self.snList[i]-1)]
	        self.subVerticalHeaderList=['0']*(self.snList[a]-self.snList[i]-1)
	        self.subArray=[x[:] for x in [[""]*(self.maxTime+1)]*(self.snList[a]-self.snList[i]-1)]
	        self.subVerticalHeaderList=self.verticalHeaderList[self.snList[i]:(self.snList[a]-1)]
		self.subHorizontalHeaderList=range(self.minTime,self.maxTime+1)
		for j in range(self.snList[i],self.snList[a]-1):
		     self.subArray[j-self.snList[i]]=self.arrayData[j][self.minTime:self.maxTime+1]
	        #self.subArray=self.arrayData[self.snList[i]:(self.snList[a]-1)]
	    self.tableModel.setHorizontalHeader(self.subHorizontalHeaderList)
	    self.tableModel.setVerticalHeader(self.subVerticalHeaderList)
	    self.tableModel.setModalDatas(self.subArray)
    	    self.tableModel.refrushModel()
	    self.scrollToStage(0)
	    self.tableView.verticalScrollBar().setValue(0)

    def updateDialog(self,column):
	text=self.subHorizontalHeaderList[column]
	self.slider.setValue(int(text))

    def updateDialogIndex(self,index):
	column=index.column()
	text=self.subHorizontalHeaderList[column]
	self.slider.setValue(int(text))

    def updatAPEData(self,dataBase,APEdbFilePath,minTime,maxTime,flag):
	self.dataBase=dataBase
	self.APEdbFilePath=APEdbFilePath
	self.minTime=minTime
	self.maxTime=maxTime
	self.flag=flag
	self.minValue=0
	if self.flag=="m":
	    self.mstage=["FG","FS","FW","FR","DP","RR","E1","E2","E3","E4","E5","E6","E7","E8","E9","E10","E11","E12","E13","E14"]
	    order_sql_sn = "SELECT * FROM "+self.dataBase.snMTableName+" order by sn asc"
	else:
	    self.sstage=["FG","FS","FW","DP","RR","E1","E2","E3","E4","E5","E6","E7","E8","E9","E10","E11","E12","E13","E14"]
	    order_sql_sn = "SELECT * FROM "+self.dataBase.snSTableName+" order by sn asc"
	self.snAll=self.dataBase.fetchall(self.APEdbFilePath,order_sql_sn)
	if self.snAll!=0:
	    self.maxValue=len(self.snAll)
	else:
	    self.maxValue=0
	    self.maxTime=0
	order_sql_reg = "SELECT * FROM "+self.dataBase.regTableName+" where spumpu = "+"'"+self.flag+"'"+" order by sn asc"
	self.regAll=self.dataBase.fetchall(self.APEdbFilePath,order_sql_reg)
	if self.regAll!=0:
	    self.regMax=len(self.regAll)
	    self.reg=0
	else:
	    self.regMax=0
	    self.reg=0
	if self.maxTime!=0 or self.maxValue!=0:
	    i=datetime.datetime.now()
            print ("start init table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    self.initTable()
	    i=datetime.datetime.now()
            print ("end init table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    i=datetime.datetime.now()
            print ("start refrush model %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
    	    self.tableModel.refrushModel()
	    i=datetime.datetime.now()
            print ("end refrush model %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    self.scrollToStage(0)

    def initTable(self):
	self.verticalHeaderList=['0']*self.maxValue
        #self.arrayData=[["" for col in range(self.maxTime+1)] for row in range(self.maxValue)]
	i=datetime.datetime.now()
        print ("start init array %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	self.arrayData=[x[:] for x in [[""]*(self.maxTime+1)]*self.maxValue]
	i=datetime.datetime.now()
        print ("end init array %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	if self.flag=="s":
	    i=datetime.datetime.now()
            print ("start set array data SPU %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    for i in range(0,self.maxValue):
	        stringList=self.snAll[i]	   
	        self.verticalHeaderList[i]=str(stringList[4])+":"+stringList[6]+":"+stringList[7]
	        temp=-1
	        for j in range(0,20):
		    string=stringList[9+j]
		    if string!=-1:
	                self.arrayData[i][string]=self.sstage[j]
	                for k in range(temp+1,string):
			    data=self.arrayData[i][temp]
		            self.arrayData[i][k]=data
	                temp=string
		    else:
			break
		dis=""
	        while self.reg<self.regMax and self.regAll[self.reg][4]==stringList[4]:
	            read=0
	            write=0
		    stringList=self.regAll[self.reg]
		    self.reg+=1
		    if stringList[6]!="Misc Reg":
		        if stringList[5]=="W":
		            write=1
		        elif stringList[5]=="R":
		            read=1
			dis=stringList[8]
	                if read==1:
		            if self.arrayData[i][stringList[1]].find("r")<0:
	                        self.arrayData[i][stringList[1]]+="r"
	                elif write==1:
 		            if self.arrayData[i][stringList[1]].find("w")<0:
	                        self.arrayData[i][stringList[1]]+="w"
	    	self.arrayData[i][stringList[1]]+=dis
	    i=datetime.datetime.now()
            print ("end set array data SPU %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	else:
	    i=datetime.datetime.now()
            print ("start set array data MPU %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    for i in range(0,self.maxValue):
	        stringList=self.snAll[i]	   
	        self.verticalHeaderList[i]=str(stringList[4])+":"+stringList[6]+":"+stringList[7]
	        temp=-1
	        for j in range(0,20):
		    string=stringList[9+j]
		    if string!=-1:
	                self.arrayData[i][string]=self.mstage[j]
	                for k in range(temp+1,string):
			    data=self.arrayData[i][temp]
		            self.arrayData[i][k]=data
	                temp=string
		    else:
			break
		dis=""	
	        while self.reg<self.regMax and self.regAll[self.reg][4]==stringList[4]:
	            read=0
	            write=0
		    stringList=self.regAll[self.reg]
		    self.reg+=1
		    if stringList[6]!="Misc Reg":
		        if stringList[5]=="W":
		            write=1
		        elif stringList[5]=="R":
		            read=1
			dis=stringList[8]
	                if read==1:
		            if self.arrayData[i][stringList[1]].find("r")<0:
	                        self.arrayData[i][stringList[1]]+="r"
	                elif write==1:
 		            if self.arrayData[i][stringList[1]].find("w")<0:
	                        self.arrayData[i][stringList[1]]+="w"
	    	self.arrayData[i][stringList[1]]+=dis	
	    i=datetime.datetime.now()
            print ("end set array data MPU %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	if self.flag=="s":
	    i=datetime.datetime.now()
            print ("start split SPU table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    order_sql="SELECT * FROM "+self.dataBase.snSTableName+" WHERE dis LIKE '%callimm%' or dis LIKE '%callmimmb%' or dis LIKE '%stop%'"+" order by sn asc"
	    r=self.dataBase.fetchall(self.APEdbFilePath,order_sql)
	    if r!=0:
		for e in range(len(r)): 
		    if r[e][7].find("stop")>=0:
			self.snList.append(self.snAll.index(r[e])+1)
		    else:
		        self.snList.append(self.snAll.index(r[e]))   
	        if r[e][7].find("stop")>=0:
	            self.snList.pop(len(self.snList)-1) 
	    i=datetime.datetime.now()
            print ("end split SPU table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	elif self.flag=="m":
	    i=datetime.datetime.now()
            print ("start split MPU table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    order_sql="SELECT * FROM "+self.dataBase.snMTableName+" WHERE dis LIKE '%stop%'"+" order by sn asc"
	    r=self.dataBase.fetchall(self.APEdbFilePath,order_sql)
	    if r!=0:
		for e in range(len(r)): 
		    self.snList.append(self.snAll.index(r[e])+1)
	        if r[e][7].find("stop")>=0:
	            self.snList.pop(len(self.snList)-1)
	    i=datetime.datetime.now()
            print ("end split MPU table %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	if len(self.snList)>1:
	    i=datetime.datetime.now()
            print ("start copy to subarray %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    for i in range(1,len(self.snList)):
		self.page+=1
	    	self.pageCombo.addItem(str(self.page)+" page")	
	    self.minTime=int(self.snAll[self.snList[0]][9])
	    self.subVerticalHeaderList=['0']*(self.snList[1]-1)
	    self.subArray=[x[:] for x in [[""]*(self.maxTime+1)]*(self.snList[1]-1)]
	    self.subHorizontalHeaderList=range(self.minTime,self.maxTime+1)
	    self.subVerticalHeaderList=self.verticalHeaderList[self.snList[0]:(self.snList[1]-1)]
	    for i in range(self.snList[0],self.snList[1]-1):
		self.subArray[i-self.snList[0]]=self.arrayData[i][self.minTime:self.maxTime+1]
	    i=datetime.datetime.now()
            print ("end copy to subarray %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    #self.subArray=self.arrayData[self.snList[0]:(self.snList[1]-1)]
	    i=datetime.datetime.now()
            print ("start set data %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    self.tableModel.setHorizontalHeader(self.subHorizontalHeaderList)
	    self.tableModel.setVerticalHeader(self.subVerticalHeaderList)
	    self.tableModel.setModalDatas(self.subArray)
	    i=datetime.datetime.now()
            print ("end set data %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	else:
	    i=datetime.datetime.now()
            print ("start set data %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	    self.minTime=self.snAll[0][9]
	    self.horizontalHeaderList=range(self.minTime,self.maxTime+1)
	    self.tableModel.setHorizontalHeader(self.horizontalHeaderList)
	    self.tableModel.setVerticalHeader(self.verticalHeaderList)
	    self.subHorizontalHeaderList=self.horizontalHeaderList
	    self.subVerticalHeaderList=self.verticalHeaderList
	    self.subArray=[x[:] for x in [[""]*(self.maxTime+1-self.minTime)]*(self.maxValue)]
	    for i in range(0,self.maxValue):
		self.subArray[i]=self.arrayData[i][self.minTime:self.maxTime+1]
	    self.tableModel.setModalDatas(self.subArray)
	    i=datetime.datetime.now()
            print ("end set data %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))

    def scrollToStage(self,value):
	index=self.pageCombo.currentIndex()
	self.tableModel.curValue=value
	exValue=self.snList[index]+value
	self.tableView.horizontalScrollBar().setValue(self.snAll[exValue][9]-self.minTime)
 
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

    def closeEvent(self,event):
	self.openFlag=-1

