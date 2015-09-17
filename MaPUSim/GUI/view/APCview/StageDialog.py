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
import sqlite3

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class StageDialog(QDialog):
    def __init__(self,parent=None):
	super(StageDialog,self).__init__(parent)

	self.resize(1500,800)
	self.setMinimumSize(400,600)
	self.openFlag=-1
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

	self.dbFile="StageArray.db"
	self.SPUTable="sputable"
	self.MPUTable="mputable"
	self.conn=self.getConn(self.dbFile)
	self.maxCol=2000  #data base can save max column
	self.tableNum=0   #the maxTime need number of data base table

    def currentIndexSlot(self,index):
	if len(self.snList)>1:
	    i=index
	    if (index+1)==self.page:
		a=self.maxValue
		del self.subHorizontalHeaderList
		self.subHorizontalHeaderList=0
		del self.subVerticalHeaderList
		self.subVerticalHeaderList=0
		del self.subArray
		self.subArray=0
	        self.subVerticalHeaderList=['']*(a-self.snList[i]+1)
	        self.subVerticalHeaderList=self.verticalHeaderList[self.snList[i]:a]
	        if self.flag=="s":
	            searchSql="SELECT * FROM "+self.SPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(a+1)
	        else:
		    searchSql="SELECT * FROM "+self.MPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(a+1)
	        r=self.fetchall(self.dbFile,searchSql)
		if r!=0:
	            self.minTime=r[0][1]
		    curMaxTime=self.maxTime
		    if self.flag=="s":
		        fetchSql = "SELECT end FROM "+self.SPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(a+1)
		    else:
		        fetchSql = "SELECT end FROM "+self.MPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(a+1)
        	    f=self.fetchall(self.dbFile,fetchSql)
		    if f!=0:
		        endList=[]	
	       	        for e in range(len(f)):
			    endList.append(f[e][0])
		        endList.sort()
		        endList.reverse()
		        curMaxTime=endList[0]
		        del endList
	            self.subArray=[["" for col in range(curMaxTime+1-self.minTime)] for row in range(a-self.snList[i])]
		    self.subHorizontalHeaderList=range(self.minTime,curMaxTime+1)
	            for i in range(0,a-self.snList[i]):
		        start=r[i][1]-self.minTime
		        end=r[i][3]+1-self.minTime
		        dataList=r[i][2].split(",")
	                self.subArray[i][start:end]=dataList
	    else:
	        a=index+1
		del self.subHorizontalHeaderList
		self.subHorizontalHeaderList=0
		del self.subVerticalHeaderList
		self.subVerticalHeaderList=0
		del self.subArray
		self.subArray=0
	        self.subVerticalHeaderList=['']*(self.snList[a]-self.snList[i])
	        self.subVerticalHeaderList=self.verticalHeaderList[self.snList[i]:(self.snList[a])]
	        if self.flag=="s":
	            searchSql="SELECT * FROM "+self.SPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(self.snList[a]+1)
	        else:
		    searchSql="SELECT * FROM "+self.MPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(self.snList[a]+1)
	        r=self.fetchall(self.dbFile,searchSql)
		if r!=0:
	            self.minTime=r[0][1]
		    curMaxTime=self.maxTime
		    if self.flag=="s":
		        fetchSql = "SELECT end FROM "+self.SPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(self.snList[a]+1)
		    else:
		        fetchSql = "SELECT end FROM "+self.MPUTable+" WHERE id>="+str(self.snList[i]+1)+" and id<"+str(self.snList[a]+1)
        	    f=self.fetchall(self.dbFile,fetchSql)
		    if f!=0:
		        endList=[]	
	       	        for e in range(len(f)):
			    endList.append(f[e][0])
		        endList.sort()
		        endList.reverse()
		        curMaxTime=endList[0]
		        del endList
		    self.subArray=[x[:] for x in [[""]*(curMaxTime+1-self.minTime)]*(self.snList[a]-self.snList[i])]
		    self.subHorizontalHeaderList=range(self.minTime,curMaxTime+1)
	            for i in range(0,self.snList[a]-self.snList[i]):
		        start=r[i][1]-self.minTime
		        end=r[i][3]+1-self.minTime
		        dataList=r[i][2].split(",")
	                self.subArray[i][start:end]=dataList
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
	self.colTypeString=""
	self.colString=""
	self.colStringLast=""
	self.colTypeStringLast=""
	#sqlite3_limit(self.dbFile,SQLITE_LIMIT_COLUMN,maxTime+2) 

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
	    h=datetime.datetime.now()
            print ("start init table %s:%s:%s,%s" %(h.hour,h.minute,h.second,h.microsecond))
	    self.initTable()
    	    self.tableModel.refrushModel()
	    self.scrollToStage(0)
	    h=datetime.datetime.now()
            print ("end init table %s:%s:%s,%s" %(h.hour,h.minute,h.second,h.microsecond))

    def initTable(self):
	self.verticalHeaderList=['0']*self.maxValue
        arrayData=[""]*(self.maxTime+1)
	if self.flag=="s":
	    self.initDataBaseTable(self.dbFile,self.SPUTable)
	    for i in range(0,self.maxValue):
	        stringList=self.snAll[i]	   
	        self.verticalHeaderList[i]=str(stringList[4])+":"+stringList[6]+":"+stringList[7]
	        temp=-1
		startTime=stringList[9]
		endTime=self.maxTime
	        for j in range(0,20):
		    string=stringList[9+j]
		    if string!=-1:
	                arrayData[string]=self.sstage[j]
			if temp!=-1:
			    if string-(temp+1)==1:
			        s=arrayData[temp]
			        arrayData[temp+1]=s
			    elif string>(temp+1):
			        s=arrayData[temp]
			        arrayData[(temp+1):string]=[s]*(stringList[9+j]-temp-1)
	                temp=string
		    else:
			endTime=stringList[9+j-1]
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
		            if arrayData[stringList[1]].find("r")<0:
	                        arrayData[stringList[1]]+="r"
	                elif write==1:
 		            if arrayData[stringList[1]].find("w")<0:
	                        arrayData[stringList[1]]+="w"
	    	arrayData[stringList[1]]+=dis
		data=""
		for k in range(startTime,endTime):
		    data+=(arrayData[k]+",")
		data+=arrayData[endTime]
		self.insertDataTable(self.dbFile,self.SPUTable,startTime,data,endTime)
	else:
	    self.initDataBaseTable(self.dbFile,self.MPUTable)
	    for i in range(0,self.maxValue):
	        stringList=self.snAll[i]	   
	        self.verticalHeaderList[i]=str(stringList[4])+":"+stringList[6]+":"+stringList[7]
	        temp=-1
		startTime=stringList[9]
		endTime=self.maxTime
	        for j in range(0,20):
		    string=stringList[9+j]
		    if string!=-1:
	                arrayData[string]=self.mstage[j]
			if temp!=-1:
			    if string-(temp+1)==1:
			        s=arrayData[temp]
			        arrayData[temp+1]=s
			    elif string>(temp+1):
			        s=arrayData[temp]
			        arrayData[(temp+1):string]=[s]*(stringList[9+j]-temp-1)
	                temp=string
		    else:
			endTime=stringList[9+j-1]
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
		            if arrayData[stringList[1]].find("r")<0:
	                        arrayData[stringList[1]]+="r"
	                elif write==1:
 		            if arrayData[stringList[1]].find("w")<0:
	                        arrayData[stringList[1]]+="w"
	    	arrayData[stringList[1]]+=dis	
		data=""
		for k in range(startTime,endTime):
		    data+=(arrayData[k]+",")
		data+=arrayData[endTime]
		self.insertDataTable(self.dbFile,self.MPUTable,startTime,data,endTime)

	self.conn.commit()
	if self.flag=="s":
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
	elif self.flag=="m":
	    order_sql="SELECT * FROM "+self.dataBase.snMTableName+" WHERE dis LIKE '%stop%'"+" order by sn asc"
	    r=self.dataBase.fetchall(self.APEdbFilePath,order_sql)
	    if r!=0:
		for e in range(len(r)): 
		    self.snList.append(self.snAll.index(r[e])+1)
	        if r[e][7].find("stop")>=0:
	            self.snList.pop(len(self.snList)-1)

	del self.regAll
	self.regAll=0
	del arrayData
	arrayData=0
	splitRow=10000
	markList=[]
	for j in range(1,len(self.snList)):
	    if self.snList[j]-self.snList[j-1]>splitRow:
		num=(self.snList[j]-self.snList[j-1])/splitRow
		splitString=[]
		for k in range(0,num):
		    splitString.append(self.snList[j-1]+splitRow*(k+1))
		markList.append(splitString)
		markList.append(j)
    	markList.reverse()
	for j in range(0,len(markList),2):
	    for k in range(len(markList[j+1]),0,-1):
	        self.snList.insert(markList[j],markList[j+1][k-1])
	if len(self.snList)>1:
	    text=self.verticalHeaderList[self.snList[0]]
	    pos=text.find(":")
	    text=text[:pos]
	    self.pageCombo.setItemText(0,"sn:"+text)	
	    for i in range(1,len(self.snList)):
		self.page+=1
	    	text=self.verticalHeaderList[self.snList[i]]
	    	pos=text.find(":")
	    	text=text[:pos]
	    	self.pageCombo.addItem("sn:"+text)
	    self.subVerticalHeaderList=['']*(self.snList[1])
	    self.subVerticalHeaderList=self.verticalHeaderList[self.snList[0]:(self.snList[1])]
	    if self.flag=="s":
	        searchSql="SELECT * FROM "+self.SPUTable+" WHERE id>=1 and id<"+str(self.snList[1]+1)
	    else:
		searchSql="SELECT * FROM "+self.MPUTable+" WHERE id>=1 and id<"+str(self.snList[1]+1)
	    r=self.fetchall(self.dbFile,searchSql)
	    if r!=0:
	    	self.minTime=r[0][1]
		curMaxTime=self.maxTime
		if self.flag=="s":
		    fetchSql = "SELECT end FROM "+self.SPUTable+" WHERE id>=1 and id<"+str(self.snList[1]+1)
		else:
		    fetchSql = "SELECT end FROM "+self.MPUTable+" WHERE id>=1 and id<"+str(self.snList[1]+1)
        	f=self.fetchall(self.dbFile,fetchSql)
		if f!=0:
		    endList=[]	
	       	    for e in range(len(f)):
			endList.append(f[e][0])
		    endList.sort()
		    endList.reverse()
		    curMaxTime=endList[0]
		    del endList
	        self.subArray=[["" for col in range(curMaxTime+1-self.minTime)] for row in range(self.snList[1])]
		self.subHorizontalHeaderList=range(self.minTime,curMaxTime+1)
	        for i in range(self.snList[0],self.snList[1]):
		    start=r[i][1]-self.minTime
		    end=r[i][3]+1-self.minTime
		    dataList=r[i][2].split(",")
	            self.subArray[i][start:end]=dataList
	else:
	    text=self.verticalHeaderList[self.snList[0]]
	    pos=text.find(":")
	    text=text[:pos]
	    self.pageCombo.setItemText(0,"sn:"+text)	
	    self.subVerticalHeaderList=self.verticalHeaderList
	    if self.flag=="s":
	        searchSql="SELECT * FROM "+self.SPUTable
	    else:
		searchSql="SELECT * FROM "+self.MPUTable
	    r=self.fetchall(self.dbFile,searchSql)
	    if r!=0:
	    	self.minTime=r[0][1]
		curMaxTime=self.maxTime
		if self.flag=="s":
		    fetchSql = "SELECT end FROM "+self.SPUTable+" WHERE id>=1 and id<"+str(self.maxValue+1)
		else:
		    fetchSql = "SELECT end FROM "+self.MPUTable+" WHERE id>=1 and id<"+str(self.maxValue+1)
        	f=self.fetchall(self.dbFile,fetchSql)
		if f!=0:
		    endList=[]	
	       	    for e in range(len(f)):
			endList.append(f[e][0])
		    endList.sort()
		    endList.reverse()
		    curMaxTime=endList[0]
		    del endList
	        self.subArray=[x[:] for x in [[""]*(curMaxTime+1-self.minTime)]*(self.maxValue)]
	        self.horizontalHeaderList=range(self.minTime,curMaxTime+1)
	        self.subHorizontalHeaderList=self.horizontalHeaderList	    
	        for i in range(0,self.maxValue):
		    start=r[i][1]-self.minTime
		    end=r[i][3]+1-self.minTime
		    dataList=r[i][2].split(",")
	            self.subArray[i][start:end]=dataList
	self.tableModel.setVerticalHeader(self.subVerticalHeaderList)
	self.tableModel.setHorizontalHeader(self.subHorizontalHeaderList)
	self.tableModel.setModalDatas(self.subArray)

    def initDataBaseTable(self,filePath,tableName):
	self.dropTable(self.conn,tableName)
	createTableSql="create table "+tableName+" (id integer primary key autoincrement, start integer, data verchar(8), end integer)"
	self.createTable(self.conn,createTableSql)
	self.saveSql="INSERT INTO "+tableName+" (start, data, end)"

    def insertDataTable(self,filePath,tableName,start,array,end):
	data="values ("
	data+=(str(start)+","+"'"+array+"'"+","+str(end)+")")
	self.save(self.conn, self.saveSql, data)

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

    def getConn(self,path):
        conn = sqlite3.connect(path)
        if os.path.exists(path) and os.path.isfile(path):
            return conn
        else:
            conn = None
            return sqlite3.connect(':memory:')

    def getCursor(self,conn):
        if conn is not None:
            return conn.cursor()
        else:
            return self.getConn('').cursor()

    def dropTable(self,conn,table):
        if table is not None and table != '':
            sql = 'DROP TABLE IF EXISTS ' + table
            cu = self.getCursor(conn)
            cu.execute(sql)
            conn.commit()
            self.closeAll(conn, cu)

    def createTable(self,conn,sql):
        if sql is not None and sql != '':
            cu = self.getCursor(conn)
            cu.execute(sql)
            conn.commit()
            self.closeAll(conn, cu)

    def closeAll(self,conn,cu):
        try:
            if cu is not None:
                cu.close()
        finally:
            if cu is not None:
                cu.close()

    def save(self,conn,sql,data):
        if sql is not None and sql != '':
            if data is not None:
                cu = self.getCursor(conn)
                cu.execute(sql+data)
                #self.close_all(conn, cu)

    def fetchall(self,filename,sql):
	conn=self.getConn(filename)
        if sql is not None and sql != '':
            cu = self.getCursor(conn)
            cu.execute(sql)
            r = cu.fetchall()
            if len(r) > 0:
                #for e in range(len(r)):
		    #print (r[e])
		return r
	    else:
		return 0
	else:
	    return 0


