# -*- coding: utf-8 -*-   
from PyQt4.QtGui import*  
from PyQt4.QtCore import* 
import sys
sys.path.append("../..")
from control.DataBase import* 
from APCMutiCoreWidget import*
from APCSigCoreWidget import*
from DataBaseDialog import*
from APCStatusWidget import*
import datetime
from Thread import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class APCViewWidget(QWidget):  
    progressShowSignal=pyqtSignal(int,str,int)
    def __init__(self,parent=None):
	super(APCViewWidget,self).__init__(parent)

        self.slider = QSlider(Qt.Horizontal)
        self.slider.setTickPosition(QSlider.TicksBothSides)
	self.slider.setMaximumHeight(30)
	self.slider.setEnabled(False)    
	self.spinBox=QSpinBox()
	self.spinBox.setMaximumHeight(40)
	self.spinBox.setEnabled(False) 

        self.APCWidget=APCMutiCoreWidget(self)  #APC mutiple core view
        self.APE0Widget=APCSigCoreWidget()
        self.APE1Widget=APCSigCoreWidget()
        self.APE2Widget=APCSigCoreWidget()
        self.APE3Widget=APCSigCoreWidget() 
	self.statusWidget=APCStatusWidget()
     
	#define left tab
        self.leftTab=QTabWidget()
        self.leftTab.addTab(self.APCWidget,self.tr("APC"))
        self.leftTab.addTab(self.APE0Widget,self.tr("APE0"))         
        self.leftTab.addTab(self.APE1Widget,self.tr("APE1"))
        self.leftTab.addTab(self.APE2Widget,self.tr("APE2"))
        self.leftTab.addTab(self.APE3Widget,self.tr("APE3"))  
	self.leftTab.addTab(self.statusWidget,self.tr("Status"))

        self.connect(self.APCWidget.APE0Button,SIGNAL("clicked()"),self.slotAPE0Widget)
        self.connect(self.APCWidget.APE1Button,SIGNAL("clicked()"),self.slotAPE1Widget)
        self.connect(self.APCWidget.APE2Button,SIGNAL("clicked()"),self.slotAPE2Widget)
        self.connect(self.APCWidget.APE3Button,SIGNAL("clicked()"),self.slotAPE3Widget) 

	#lay out all widget  
	bottomLay=QHBoxLayout()
	bottomLay.addWidget(self.slider)
	bottomLay.addWidget(self.spinBox)
	bottomLay.setStretchFactor(self.slider,7)
	bottomLay.setStretchFactor(self.spinBox,1)    
        self.mainLayout=QVBoxLayout()
        self.mainLayout.addWidget(self.leftTab)
        self.mainLayout.addLayout(bottomLay)
        self.setLayout(self.mainLayout)  

 	#define slot function
    def slotAPE0Widget(self):
        self.leftTab.setCurrentWidget(self.APE0Widget)  

    def slotAPE1Widget(self):
        self.leftTab.setCurrentWidget(self.APE1Widget) 

    def slotAPE2Widget(self):
        self.leftTab.setCurrentWidget(self.APE2Widget) 

    def slotAPE3Widget(self):
        self.leftTab.setCurrentWidget(self.APE3Widget)

    def callback(self,value,string,maxValue):  
	self.progressShowSignal.emit(value,string,maxValue)	

    def createDataBase(self,num,path):
	thread=Thread()
	thread.progressCall(self.callback)
	thread.num=num
	thread.path="m5out/"+path
	thread.start()
	eventLoop=QEventLoop()
	self.connect(thread,SIGNAL("finished()"),eventLoop.quit)
	eventLoop.exec_()
	self.dataBase=thread.dataBase
	return self.dataBase

    def indexCallback(self):  
        self.currentValueSlot(self.slider.value())

    def simulatorDoneSlot(self,num,path): #num APE count
	self.num=num
	#simulator exit normal,create data base
	#show dialog and show data base is building
	i=datetime.datetime.now()
        print ("start create data base %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	self.dataBaseDialog=DataBaseDialog()
	self.progressShowSignal.connect(self.dataBaseDialog.updateDataBaseDialog)
	self.dataBaseDialog.show()
	self.dataBase=self.createDataBase(num,path)
	i=datetime.datetime.now()
        print ("end create data base %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	i=datetime.datetime.now()
        print ("start update widget %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	#set the range
	self.slider.setEnabled(True)
	self.spinBox.setEnabled(True)
	#read file content, get min and max time
	f=open(self.dataBase.filePath,"r")
        lines=f.readlines()
	f.close()
	if len(lines)==0:
	    return
	line=lines[0]
	pos=line.index("[")
	#self.minTime=int(line[:pos])/1000
	self.minTime=0
	count=len(lines)
	line=lines[count-1]
	pos=line.index("[")
	self.maxTime=int(line[:pos])/1000

	i=datetime.datetime.now()
        print ("start update MPU stage %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	#update MPU and SPU stage dialog
	#APE0 MPU STAGE
	self.APE0Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE0dbFilePath,self.minTime,self.maxTime,"m") 
	#APE0 SPU STAGE
	self.APE0Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE0dbFilePath,self.minTime,self.maxTime,"s") 
	self.APE0Widget.MPUWidget.stageDialog.slider=self.slider 
	self.APE0Widget.SPUWidget.stageDialog.slider=self.slider 
	self.APE0Widget.MPUWidget.setButtonEnabled(True)
	self.APE0Widget.SPUWidget.stageButton.setEnabled(True)
	i=datetime.datetime.now()
        print ("end update mpu stage %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	if self.num==2:
	    #APE1 MPU STAGE
	    self.APE1Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE1 SPU STAGE
	    self.APE1Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"s")
	    self.APE1Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.SPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.MPUWidget.setButtonEnabled(True)
	    self.APE1Widget.SPUWidget.stageButton.setEnabled(True)
	elif self.num==3:
	    #APE1 MPU STAGE
	    self.APE1Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE1 SPU STAGE
	    self.APE1Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE1Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.SPUWidget.stageDialog.slider=self.slider  
	    self.APE1Widget.MPUWidget.setButtonEnabled(True)
	    self.APE1Widget.SPUWidget.stageButton.setEnabled(True) 
	    #APE2 MPU STAGE
	    self.APE2Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE2 SPU STAGE
	    self.APE2Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE2Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.SPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.MPUWidget.setButtonEnabled(True)
	    self.APE2Widget.SPUWidget.stageButton.setEnabled(True) 
	elif self.num==4:
	    #APE1 MPU STAGE
	    self.APE1Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE1 SPU STAGE
	    self.APE1Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE1Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.SPUWidget.stageDialog.slider=self.slider   
	    self.APE1Widget.MPUWidget.setButtonEnabled(True)
	    self.APE1Widget.SPUWidget.stageButton.setEnabled(True)
	    #APE2 MPU STAGE
	    self.APE2Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE2 SPU STAGE
	    self.APE2Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE2Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.SPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.MPUWidget.setButtonEnabled(True)
	    self.APE2Widget.SPUWidget.stageButton.setEnabled(True)
	    #APE3 MPU STAGE
	    self.APE3Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE3dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE3 SPU STAGE
	    self.APE3Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE3dbFilePath,self.minTime,self.maxTime,"s")
	    self.APE3Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE3Widget.SPUWidget.stageDialog.slider=self.slider  
	    self.APE3Widget.MPUWidget.setButtonEnabled(True)
	    self.APE3Widget.SPUWidget.stageButton.setEnabled(True)
	i=datetime.datetime.now()
        print ("end update stage %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	#set slider min and max value
	self.slider.setRange(self.minTime,self.maxTime)
	self.spinBox.setRange(self.minTime,self.maxTime)
	#add slot for slider,if silder's value change, then response
	self.connect(self.slider,SIGNAL("valueChanged(int)"),self.currentValueSlot)
	self.connect(self.slider,SIGNAL("valueChanged(int)"),self.spinBox.setValue)
	self.connect(self.spinBox,SIGNAL("valueChanged(int)"),self.slider.setValue)
	self.dataBaseDialog.close()
	self.APE0Widget.MPUWidget.indexCallback(self.indexCallback)
	self.APE1Widget.MPUWidget.indexCallback(self.indexCallback)
	self.APE2Widget.MPUWidget.indexCallback(self.indexCallback)
	self.APE3Widget.MPUWidget.indexCallback(self.indexCallback)
	i=datetime.datetime.now()
        print ("end update widget %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))

    def currentValueSlot(self,time):
	curTime=self.slider.value()
	self.updateAPE0Widget(curTime)
	if self.num==2:
	    self.updateAPE1Widget(curTime)
	if self.num==3:
	    self.updateAPE1Widget(curTime)
	    self.updateAPE2Widget(curTime)
	if self.num==4:
	    self.updateAPE1Widget(curTime)
	    self.updateAPE2Widget(curTime)
	    self.updateAPE3Widget(curTime)

    def updateAPE0Widget(self,curTime):
	MPURList=[]
	SPURRList=[]
	SPURJList=[]
        self.APE0Widget.MPUWidget.buttonWidget.lineArray = [([0] * 14) for i in range(14)] 
	fetchall_sql="SELECT * FROM "+self.dataBase.timeTableName+" WHERE time = "+str(curTime)
	r=self.dataBase.fetchall(self.dataBase.APE0timeFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		self.APE0Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE0Widget.SPUWidget.updateSPUWidget(r[e])
	fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE time = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE0dbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		if r[e][6]=="MPU Reg":
		    if r[e][5]=="R":
			MPURList.append(r[e][7])
		    else:
		        self.APE0Widget.MPUWidget.updateMPURegWFlag(r[e])
		elif r[e][6]=="R Reg":
		    if r[e][5]=="R":
			SPURRList.append(r[e][7])
		    else:
		    	self.APE0Widget.SPUWidget.updateSPURegWFlag(r[e])
		elif r[e][6]=="J Reg":
		    if r[e][5]=="R":
			SPURJList.append(r[e][7])
		    else:
		    	self.APE0Widget.SPUWidget.updateSPURegWFlag(r[e])
	    for i in range(0,len(MPURList)):
		self.APE0Widget.MPUWidget.updateMPURegRFlag(MPURList[i])
	    for i in range(0,len(SPURRList)):
		self.APE0Widget.SPUWidget.updateSPURegRRFlag(SPURRList[i])
	    for i in range(0,len(SPURJList)):
		self.APE0Widget.SPUWidget.updateSPURegRJFlag(SPURJList[i])

	floatDialogList=["nop"]*440
	fetchall_sql = "SELECT * FROM "+self.dataBase.snMTableName+" WHERE stage0 = "+str(curTime)+" or "+"stage1 = "+str(curTime)+" or "+"stage2 = "+str(curTime)+" or "+"stage3 = "+str(curTime)+" or "+"stage4 = "+str(curTime)+" or "+"stage5 = "+str(curTime)+" or "+"stage6 = "+str(curTime)+" or "+"stage7 = "+str(curTime)+" or "+"stage8 = "+str(curTime)+" or "+"stage9 = "+str(curTime)+" or "+"stage10 = "+str(curTime)+" or "+"stage11 = "+str(curTime)+" or "+"stage12 = "+str(curTime)+" or "+"stage13 = "+str(curTime)+" or "+"stage14 = "+str(curTime)+" or "+"stage15 = "+str(curTime)+" or "+"stage16 = "+str(curTime)+" or "+"stage17 = "+str(curTime)+" or "+"stage18 = "+str(curTime)+" or "+"stage19 = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE0dbFilePath,fetchall_sql)
	if r!=0:
            if len(r) > 0:	
		for e in range(len(r)):
		    s=r[e][7]
		    pos=s.find(":")
		    s=s[(pos+1):]
		    a=s[:4]
		    stage="sn:"+str(r[e][4])
		    if r[e][4]!=-1:
			stage=stage+",sln:"+str(r[e][5])
		    if a=="dm":
			j=0
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2				
		    elif a=="biu0":
			j=40
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu1":
			j=80
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu2":
			j=120
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="shu0":
			j=160
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="mrf":
			j=200
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="shu1":
			j=240
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="ialu":
			j=280
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="imac":
			j=320
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="falu":
			j=360
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="fmac":
			j=400
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		
		    if r[e][7]!="nop":  #dis (memo:ialuadd   ) IALU.T1+T2->IMAC.T0  (memo:shu0idx   ) SHU0.T0 IND TSQ->M[0]  (memo:biu0ld    ) BIU0.DM(A++,K++)->M[I++]
			s=r[e][7]
			pos=s.find(")")
			if pos==-1:
			    continue
			s=s[(pos+2):]
			pos=s.find(".")
			temp=s[:pos]
			s=s[(pos+1):]  
			pos=s.find("->")
			if pos!=-1:
			    s=s[(pos+2):]
			    pos=s.find(".")
			    temp=temp+s[:pos]
			    if temp=="DMBIU0": 
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[0][1]=1
			    elif temp=="BIU0DM":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[1][0]=1
			    elif temp=="DMBIU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[0][2]=1
			    elif temp=="BIU1DM":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[2][0]=1
			    elif temp=="DMBIU2":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[0][3]=1
			    elif temp=="BIU2DM":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[3][0]=1
			    elif temp=="BIU0SHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[1][4]=1
			    elif temp=="SHU0BIU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][1]=1
			    elif temp=="BIU1SHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[2][4]=1
			    elif temp=="SHU0BIU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][2]=1
			    elif temp=="BIU2SHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[3][4]=1
			    elif temp=="SHU0BIU2":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][3]=1
			    elif temp=="BIU0SHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[1][8]=1
			    elif temp=="SHU1BIU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][1]=1
			    elif temp=="BIU1SHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[2][8]=1
			    elif temp=="SHU1BIU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][2]=1
			    elif temp=="BIU2SHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[3][8]=1
			    elif temp=="SHU1BIU2":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][3]=1
			    elif temp=="BIU0MRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[1][13]=1
			    elif temp=="MRFBIU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][1]=1
			    elif temp=="BIU1MRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[2][13]=1
			    elif temp=="MRFBIU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][2]=1
			    elif temp=="BIU2MRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[3][13]=1
			    elif temp=="MRFBIU2":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][3]=1
			    elif temp=="SHU0IALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][9]=1
			    elif temp=="IALUSHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[9][4]=1
			    elif temp=="SHU0IMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][10]=1
			    elif temp=="IMACSHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[10][4]=1
			    elif temp=="SHU0FALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][11]=1
			    elif temp=="FALUSHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[11][4]=1
			    elif temp=="SHU0FMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][12]=1
			    elif temp=="FMACSHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[12][4]=1
			    elif temp=="MRFIALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][9]=1
			    elif temp=="IALUMRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[9][13]=1
			    elif temp=="MRFIMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][10]=1
			    elif temp=="IMACMRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[10][13]=1
			    elif temp=="MRFFALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][11]=1
			    elif temp=="FALUMRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[11][13]=1
			    elif temp=="MRFFMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][12]=1
			    elif temp=="FMACMRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[12][13]=1
			    elif temp=="SHU1IALU": 
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][9]=1
			    elif temp=="IALUSHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[9][8]=1
			    elif temp=="SHU1IMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][10]=1
			    elif temp=="IMACSHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[10][8]=1
			    elif temp=="SHU1FALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][11]=1
			    elif temp=="FALUSHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[11][8]=1
			    elif temp=="SHU1FMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][12]=1
			    elif temp=="FMACSHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[12][8]=1
			    elif temp=="IALUFALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[9][11]=1
			    elif temp=="FALUIALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[11][9]=1
			    elif temp=="SHU0MRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[4][13]=1
			    elif temp=="MRFSHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][4]=1
			    elif temp=="MRFSHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[13][8]=1
			    elif temp=="SHU1MRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[8][13]=1
			    elif temp=="IALUIMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[9][10]=1
			    elif temp=="IMACIALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[10][9]=1
			    elif temp=="IMACFALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[10][11]=1
			    elif temp=="FALUIMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[11][10]=1
			    elif temp=="FALUFMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[11][12]=1
			    elif temp=="FMACFALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.lineArray[12][11]=1
			    elif temp=="DMDM":#DM=0; BIU0=1; BIU1=2; BIU2=3; SHU0=4; MRF=13; SHU1=8; M0=5; M1=6; M2=7; IALU=9; IMAC=10; FALU=11; FMAC=12 
				print temp 
				self.APE0Widget.MPUWidget.buttonWidget.DMButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="BIU0BIU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.BIU0Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="BIU1BIU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.BIU1Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="BIU2BIU2":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.BIU2Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="SHU0SHU0":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.SHU0Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="MRFMRF":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.MRFButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="SHU1SHU1":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.SHU1Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="IALUIALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.IALUButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="IMACIMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.IMACButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="FALUFALU":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.FALUButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="FMACFMAC":
				print temp
				self.APE0Widget.MPUWidget.buttonWidget.FMACButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
	self.APE0Widget.MPUWidget.buttonWidget.buttonLines()
	self.APE0Widget.MPUWidget.updateMPUFloatDialog(floatDialogList)

    def updateAPE1Widget(self,curTime):
	MPURList=[]
	SPURList=[]
	fetchall_sql="SELECT * FROM "+self.dataBase.timeTableName+" WHERE time = "+str(curTime)
	r=self.dataBase.fetchall(self.dataBase.APE1timeFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		self.APE1Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE1Widget.SPUWidget.updateSPUWidget(r[e])
	fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE time = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE1dbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		if r[e][6]=="MPU Reg":
		    if r[e][5]=="R":
			MPURList.append(r[e][7])
		    else:
		        self.APE1Widget.MPUWidget.updateMPURegWFlag(r[e])
		elif r[e][6]=="R Reg" or r[e][6]=="J Reg":
		    if r[e][5]=="R":
			SPURList.append(r[e][7])
		    else:
		    	self.APE1Widget.SPUWidget.updateSPURegWFlag(r[e])
	    for i in range(0,len(MPURList)):
		self.APE1Widget.MPUWidget.updateMPURegRFlag(MPURList[i])
	    for i in range(0,len(SPURList)):
		self.APE1Widget.SPUWidget.updateSPURegRFlag(SPURList[i])

	floatDialogList=["nop"]*440
	fetchall_sql = "SELECT * FROM "+self.dataBase.snMTableName+" WHERE stage0 = "+str(curTime)+" or "+"stage1 = "+str(curTime)+" or "+"stage2 = "+str(curTime)+" or "+"stage3 = "+str(curTime)+" or "+"stage4 = "+str(curTime)+" or "+"stage5 = "+str(curTime)+" or "+"stage6 = "+str(curTime)+" or "+"stage7 = "+str(curTime)+" or "+"stage8 = "+str(curTime)+" or "+"stage9 = "+str(curTime)+" or "+"stage10 = "+str(curTime)+" or "+"stage11 = "+str(curTime)+" or "+"stage12 = "+str(curTime)+" or "+"stage13 = "+str(curTime)+" or "+"stage14 = "+str(curTime)+" or "+"stage15 = "+str(curTime)+" or "+"stage16 = "+str(curTime)+" or "+"stage17 = "+str(curTime)+" or "+"stage18 = "+str(curTime)+" or "+"stage19 = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE1dbFilePath,fetchall_sql)
	if r!=0:
            if len(r) > 0:	
		for e in range(len(r)):
		    s=r[e][7]
		    pos=s.find(":")
		    s=s[(pos+1):]
		    a=s[:4]
		    stage="sn:"+str(r[e][4])
		    if r[e][4]!=-1:
			stage=stage+",sln:"+str(r[e][5])
		    if a=="dm":
			j=0
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2				
		    elif a=="biu0":
			j=40
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu1":
			j=80
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu2":
			j=120
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="shu0":
			j=160
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="mrf":
			j=200
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="shu1":
			j=240
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="ialu":
			j=280
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="imac":
			j=320
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="falu":
			j=360
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="fmac":
			j=400
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		
		    if r[e][7]!="nop":  #dis (memo:ialuadd   ) IALU.T1+T2->IMAC.T0  (memo:shu0idx   ) SHU0.T0 IND TSQ->M[0]
			s=r[e][7]
			pos=s.find(")")
			if pos==-1:
			    continue
			s=s[(pos+2):]
			pos=s.find(".")
			temp=s[:pos]
			s=s[(pos+1):]  
			pos=s.find("->")
			if pos!=-1:
			    s=s[(pos+2):]
			    pos=s.find(".")
			    temp=temp+s[:pos]
			    if temp=="DMBIU0":
				print "dmbiu0"
			    elif temp=="BIU0DM":
				print "biu0dm"
			    elif temp=="DMBIU1":
				print "dmbiu1"
			    elif temp=="BIU1DM":
				print "biu1dm"
			    elif temp=="DMBIU2":
				print "dmbiu2"
			    elif temp=="BIU2DM":
				print "biu2dm"
			    elif temp=="BIU0SHU0":
				print "biu0shu0"
			    elif temp=="SHU0BIU0":
				print "shu0biu0"
			    elif temp=="BIU1SHU0":
				print "biu0shu0"
			    elif temp=="SHU0BIU1":
				print "shu0biu1"
			    elif temp=="BIU2SHU0":
				print "biu2shu0"
			    elif temp=="SHU0BIU2":
				print "shu0biu2"
			    elif temp=="BIU0SHU1":
				print "biu0shu1"
			    elif temp=="SHU1BIU0":
				print "shu1biu0"
			    elif temp=="BIU1SHU1":
				print "biu1shu1"
			    elif temp=="SHU1BIU1":
				print "shu1biu1"
			    elif temp=="BIU2SHU1":
				print "biu2shu1"
			    elif temp=="SHU1BIU2":
				print "shu1biu2"
			    elif temp=="BIU0MRF":
				print "biu0mrf"
			    elif temp=="MRFBIU0":
				print "mrfbiu0"
			    elif temp=="BIU1MRF":
				print "biu1mrf"
			    elif temp=="MRFBIU1":
				print "mrfbiu1"
			    elif temp=="BIU2MRF":
				print "biu2mrf"
			    elif temp=="MRFBIU2":
				print "mrfbiu2"
			    elif temp=="SHU0IALU":
				print "shu0ialu"
			    elif temp=="IALUSHU0":
				print "ialushu0"
			    elif temp=="SHU0IMAC":
				print "shu0imac"
			    elif temp=="IMACSHU0":
				print "imacshu0"
			    elif temp=="SHU0FALU":
				print "shu0falu"
			    elif temp=="FALUSHU0":
				print "falushu0"
			    elif temp=="SHU0FMAC":
				print "shu0fmac"
			    elif temp=="FMACSHU0":
				print "fmacshu0"
			    elif temp=="MRFIALU":
				print "mrfialu"
			    elif temp=="IALUMRF":
				print "ialumrf"
			    elif temp=="MRFIMAC":
				print "mrfimac"
			    elif temp=="IMACMRF":
				print "imacmrf"
			    elif temp=="MRFFALU":
				print "mrffalu"
			    elif temp=="FALUMRF":
				print "falumrf"
			    elif temp=="MRFFMAC":
				print "mrffmac"
			    elif temp=="FMACMRF":
				print "fmacmrf"
			    elif temp=="SHU1IALU":
				print "shu1ialu"
			    elif temp=="IALUSHU1":
				print "ialushu1"
			    elif temp=="SHU1IMAC":
				print "shu1imac"
			    elif temp=="IMACSHU1":
				print "imacshu1"
			    elif temp=="SHU1FALU":
				print "shu1falu"
			    elif temp=="FALUSHU1":
				print "falushu1"
			    elif temp=="SHU1FMAC":
				print "shu1fmac"
			    elif temp=="FMACSHU1":
				print "fmacshu1"
			    elif temp=="IALUFALU":
				print "ialufalu"
			    elif temp=="FALUIALU":
				print "faluialu"
			    elif temp=="SHU0MRF":
				print "shu0mrf"
			    elif temp=="MRFSHU0":
				print "mrfshu0"
			    elif temp=="MRFSHU1":
				print "mrfshu1"
			    elif temp=="SHU1MRF":
				print "shu1mrf"
			    elif temp=="IALUIMAC":
				print "ialuimac"
			    elif temp=="IMACIALU":
				print "imacialu"
			    elif temp=="IMACFALU":
				print "imacfalu"
			    elif temp=="FALUIMAC":
				print "faluimac"
			    elif temp=="FALUFMAC":
				print "falufmac"
			    elif temp=="FMACFALU":
				print "fmacfalu"
	self.APE1Widget.MPUWidget.updateMPUFloatDialog(floatDialogList)

    def updateAPE2Widget(self,curTime):
	MPURList=[]
	SPURList=[]
	fetchall_sql="SELECT * FROM "+self.dataBase.timeTableName+" WHERE time = "+str(curTime)
	r=self.dataBase.fetchall(self.dataBase.APE2timeFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		self.APE2Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE2Widget.SPUWidget.updateSPUWidget(r[e])
	fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE time = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE2dbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		if r[e][6]=="MPU Reg":
		    if r[e][5]=="R":
			MPURList.append(r[e][7])
		    else:
		        self.APE2Widget.MPUWidget.updateMPURegWFlag(r[e])
		elif r[e][6]=="R Reg" or r[e][6]=="J Reg":
		    if r[e][5]=="R":
			SPURList.append(r[e][7])
		    else:
		    	self.APE2Widget.SPUWidget.updateSPURegWFlag(r[e])
	    for i in range(0,len(MPURList)):
		self.APE2Widget.MPUWidget.updateMPURegRFlag(MPURList[i])
	    for i in range(0,len(SPURList)):
		self.APE2Widget.SPUWidget.updateSPURegRFlag(SPURList[i])

	floatDialogList=["nop"]*440
	fetchall_sql = "SELECT * FROM "+self.dataBase.snMTableName+" WHERE stage0 = "+str(curTime)+" or "+"stage1 = "+str(curTime)+" or "+"stage2 = "+str(curTime)+" or "+"stage3 = "+str(curTime)+" or "+"stage4 = "+str(curTime)+" or "+"stage5 = "+str(curTime)+" or "+"stage6 = "+str(curTime)+" or "+"stage7 = "+str(curTime)+" or "+"stage8 = "+str(curTime)+" or "+"stage9 = "+str(curTime)+" or "+"stage10 = "+str(curTime)+" or "+"stage11 = "+str(curTime)+" or "+"stage12 = "+str(curTime)+" or "+"stage13 = "+str(curTime)+" or "+"stage14 = "+str(curTime)+" or "+"stage15 = "+str(curTime)+" or "+"stage16 = "+str(curTime)+" or "+"stage17 = "+str(curTime)+" or "+"stage18 = "+str(curTime)+" or "+"stage19 = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE2dbFilePath,fetchall_sql)
	if r!=0:
            if len(r) > 0:	
		for e in range(len(r)):
		    s=r[e][7]
		    pos=s.find(":")
		    s=s[(pos+1):]
		    a=s[:4]
		    stage="sn:"+str(r[e][4])
		    if r[e][4]!=-1:
			stage=stage+",sln:"+str(r[e][5])
		    if a=="dm":
			j=0
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2				
		    elif a=="biu0":
			j=40
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu1":
			j=80
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu2":
			j=120
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="shu0":
			j=160
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="mrf":
			j=200
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="shu1":
			j=240
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="ialu":
			j=280
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="imac":
			j=320
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="falu":
			j=360
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="fmac":
			j=400
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		
		    if r[e][7]!="nop":  #dis (memo:ialuadd   ) IALU.T1+T2->IMAC.T0  (memo:shu0idx   ) SHU0.T0 IND TSQ->M[0]
			s=r[e][7]
			pos=s.find(")")
			if pos==-1:
			    continue
			s=s[(pos+2):]
			pos=s.find(".")
			temp=s[:pos]
			s=s[(pos+1):]  
			pos=s.find("->")
			if pos!=-1:
			    s=s[(pos+2):]
			    pos=s.find(".")
			    temp=temp+s[:pos]
			    if temp=="DMBIU0":
				print "dmbiu0"
			    elif temp=="BIU0DM":
				print "biu0dm"
			    elif temp=="DMBIU1":
				print "dmbiu1"
			    elif temp=="BIU1DM":
				print "biu1dm"
			    elif temp=="DMBIU2":
				print "dmbiu2"
			    elif temp=="BIU2DM":
				print "biu2dm"
			    elif temp=="BIU0SHU0":
				print "biu0shu0"
			    elif temp=="SHU0BIU0":
				print "shu0biu0"
			    elif temp=="BIU1SHU0":
				print "biu0shu0"
			    elif temp=="SHU0BIU1":
				print "shu0biu1"
			    elif temp=="BIU2SHU0":
				print "biu2shu0"
			    elif temp=="SHU0BIU2":
				print "shu0biu2"
			    elif temp=="BIU0SHU1":
				print "biu0shu1"
			    elif temp=="SHU1BIU0":
				print "shu1biu0"
			    elif temp=="BIU1SHU1":
				print "biu1shu1"
			    elif temp=="SHU1BIU1":
				print "shu1biu1"
			    elif temp=="BIU2SHU1":
				print "biu2shu1"
			    elif temp=="SHU1BIU2":
				print "shu1biu2"
			    elif temp=="BIU0MRF":
				print "biu0mrf"
			    elif temp=="MRFBIU0":
				print "mrfbiu0"
			    elif temp=="BIU1MRF":
				print "biu1mrf"
			    elif temp=="MRFBIU1":
				print "mrfbiu1"
			    elif temp=="BIU2MRF":
				print "biu2mrf"
			    elif temp=="MRFBIU2":
				print "mrfbiu2"
			    elif temp=="SHU0IALU":
				print "shu0ialu"
			    elif temp=="IALUSHU0":
				print "ialushu0"
			    elif temp=="SHU0IMAC":
				print "shu0imac"
			    elif temp=="IMACSHU0":
				print "imacshu0"
			    elif temp=="SHU0FALU":
				print "shu0falu"
			    elif temp=="FALUSHU0":
				print "falushu0"
			    elif temp=="SHU0FMAC":
				print "shu0fmac"
			    elif temp=="FMACSHU0":
				print "fmacshu0"
			    elif temp=="MRFIALU":
				print "mrfialu"
			    elif temp=="IALUMRF":
				print "ialumrf"
			    elif temp=="MRFIMAC":
				print "mrfimac"
			    elif temp=="IMACMRF":
				print "imacmrf"
			    elif temp=="MRFFALU":
				print "mrffalu"
			    elif temp=="FALUMRF":
				print "falumrf"
			    elif temp=="MRFFMAC":
				print "mrffmac"
			    elif temp=="FMACMRF":
				print "fmacmrf"
			    elif temp=="SHU1IALU":
				print "shu1ialu"
			    elif temp=="IALUSHU1":
				print "ialushu1"
			    elif temp=="SHU1IMAC":
				print "shu1imac"
			    elif temp=="IMACSHU1":
				print "imacshu1"
			    elif temp=="SHU1FALU":
				print "shu1falu"
			    elif temp=="FALUSHU1":
				print "falushu1"
			    elif temp=="SHU1FMAC":
				print "shu1fmac"
			    elif temp=="FMACSHU1":
				print "fmacshu1"
			    elif temp=="IALUFALU":
				print "ialufalu"
			    elif temp=="FALUIALU":
				print "faluialu"
			    elif temp=="SHU0MRF":
				print "shu0mrf"
			    elif temp=="MRFSHU0":
				print "mrfshu0"
			    elif temp=="MRFSHU1":
				print "mrfshu1"
			    elif temp=="SHU1MRF":
				print "shu1mrf"
			    elif temp=="IALUIMAC":
				print "ialuimac"
			    elif temp=="IMACIALU":
				print "imacialu"
			    elif temp=="IMACFALU":
				print "imacfalu"
			    elif temp=="FALUIMAC":
				print "faluimac"
			    elif temp=="FALUFMAC":
				print "falufmac"
			    elif temp=="FMACFALU":
				print "fmacfalu"
	self.APE2Widget.MPUWidget.updateMPUFloatDialog(floatDialogList)

    def updateAPE3Widget(self,curTime):
	MPURList=[]
	SPURList=[]
	fetchall_sql="SELECT * FROM "+self.dataBase.timeTableName+" WHERE time = "+str(curTime)
	r=self.dataBase.fetchall(self.dataBase.APE3timeFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		self.APE3Widget.MPUWidget.updateMPUWidget(r[e])
		self.APE3Widget.SPUWidget.updateSPUWidget(r[e])
	fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE time = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE3dbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		if r[e][6]=="MPU Reg":
		    if r[e][5]=="R":
			MPURList.append(r[e][7])
		    else:
		        self.APE3Widget.MPUWidget.updateMPURegWFlag(r[e])
		elif r[e][6]=="R Reg" or r[e][6]=="J Reg":
		    if r[e][5]=="R":
			SPURList.append(r[e][7])
		    else:
		    	self.APE3Widget.SPUWidget.updateSPURegWFlag(r[e])
	    for i in range(0,len(MPURList)):
		self.APE3Widget.MPUWidget.updateMPURegRFlag(MPURList[i])
	    for i in range(0,len(SPURList)):
		self.APE3Widget.SPUWidget.updateSPURegRFlag(SPURList[i])

	floatDialogList=["nop"]*440
	fetchall_sql = "SELECT * FROM "+self.dataBase.snMTableName+" WHERE stage0 = "+str(curTime)+" or "+"stage1 = "+str(curTime)+" or "+"stage2 = "+str(curTime)+" or "+"stage3 = "+str(curTime)+" or "+"stage4 = "+str(curTime)+" or "+"stage5 = "+str(curTime)+" or "+"stage6 = "+str(curTime)+" or "+"stage7 = "+str(curTime)+" or "+"stage8 = "+str(curTime)+" or "+"stage9 = "+str(curTime)+" or "+"stage10 = "+str(curTime)+" or "+"stage11 = "+str(curTime)+" or "+"stage12 = "+str(curTime)+" or "+"stage13 = "+str(curTime)+" or "+"stage14 = "+str(curTime)+" or "+"stage15 = "+str(curTime)+" or "+"stage16 = "+str(curTime)+" or "+"stage17 = "+str(curTime)+" or "+"stage18 = "+str(curTime)+" or "+"stage19 = "+str(curTime)
        r=self.dataBase.fetchall(self.dataBase.APE3dbFilePath,fetchall_sql)
	if r!=0:
            if len(r) > 0:	
		for e in range(len(r)):
		    s=r[e][7]
		    pos=s.find(":")
		    s=s[(pos+1):]
		    a=s[:4]
		    stage="sn:"+str(r[e][4])
		    if r[e][4]!=-1:
			stage=stage+",sln:"+str(r[e][5])
		    if a=="dm":
			j=0
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2				
		    elif a=="biu0":
			j=40
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu1":
			j=80
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="biu2":
			j=120
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="shu0":
			j=160
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2
		    elif a=="mrf":
			j=200
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="shu1":
			j=240
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="ialu":
			j=280
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="imac":
			j=320
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="falu":
			j=360
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		    elif a=="fmac":
			j=400
			for i in range(0,20):
			    if r[e][9+i]==curTime:
			        floatDialogList[j]=stage
			        floatDialogList[j+1]=r[e][6]
			    j=j+2	
		
		    if r[e][7]!="nop":  #dis (memo:ialuadd   ) IALU.T1+T2->IMAC.T0  (memo:shu0idx   ) SHU0.T0 IND TSQ->M[0]
			s=r[e][7]
			pos=s.find(")")
			if pos==-1:
			    continue
			s=s[(pos+2):]
			pos=s.find(".")
			temp=s[:pos]
			s=s[(pos+1):]  
			pos=s.find("->")
			if pos!=-1:
			    s=s[(pos+2):]
			    pos=s.find(".")
			    temp=temp+s[:pos]
			    if temp=="DMBIU0":
				print "dmbiu0"
			    elif temp=="BIU0DM":
				print "biu0dm"
			    elif temp=="DMBIU1":
				print "dmbiu1"
			    elif temp=="BIU1DM":
				print "biu1dm"
			    elif temp=="DMBIU2":
				print "dmbiu2"
			    elif temp=="BIU2DM":
				print "biu2dm"
			    elif temp=="BIU0SHU0":
				print "biu0shu0"
			    elif temp=="SHU0BIU0":
				print "shu0biu0"
			    elif temp=="BIU1SHU0":
				print "biu0shu0"
			    elif temp=="SHU0BIU1":
				print "shu0biu1"
			    elif temp=="BIU2SHU0":
				print "biu2shu0"
			    elif temp=="SHU0BIU2":
				print "shu0biu2"
			    elif temp=="BIU0SHU1":
				print "biu0shu1"
			    elif temp=="SHU1BIU0":
				print "shu1biu0"
			    elif temp=="BIU1SHU1":
				print "biu1shu1"
			    elif temp=="SHU1BIU1":
				print "shu1biu1"
			    elif temp=="BIU2SHU1":
				print "biu2shu1"
			    elif temp=="SHU1BIU2":
				print "shu1biu2"
			    elif temp=="BIU0MRF":
				print "biu0mrf"
			    elif temp=="MRFBIU0":
				print "mrfbiu0"
			    elif temp=="BIU1MRF":
				print "biu1mrf"
			    elif temp=="MRFBIU1":
				print "mrfbiu1"
			    elif temp=="BIU2MRF":
				print "biu2mrf"
			    elif temp=="MRFBIU2":
				print "mrfbiu2"
			    elif temp=="SHU0IALU":
				print "shu0ialu"
			    elif temp=="IALUSHU0":
				print "ialushu0"
			    elif temp=="SHU0IMAC":
				print "shu0imac"
			    elif temp=="IMACSHU0":
				print "imacshu0"
			    elif temp=="SHU0FALU":
				print "shu0falu"
			    elif temp=="FALUSHU0":
				print "falushu0"
			    elif temp=="SHU0FMAC":
				print "shu0fmac"
			    elif temp=="FMACSHU0":
				print "fmacshu0"
			    elif temp=="MRFIALU":
				print "mrfialu"
			    elif temp=="IALUMRF":
				print "ialumrf"
			    elif temp=="MRFIMAC":
				print "mrfimac"
			    elif temp=="IMACMRF":
				print "imacmrf"
			    elif temp=="MRFFALU":
				print "mrffalu"
			    elif temp=="FALUMRF":
				print "falumrf"
			    elif temp=="MRFFMAC":
				print "mrffmac"
			    elif temp=="FMACMRF":
				print "fmacmrf"
			    elif temp=="SHU1IALU":
				print "shu1ialu"
			    elif temp=="IALUSHU1":
				print "ialushu1"
			    elif temp=="SHU1IMAC":
				print "shu1imac"
			    elif temp=="IMACSHU1":
				print "imacshu1"
			    elif temp=="SHU1FALU":
				print "shu1falu"
			    elif temp=="FALUSHU1":
				print "falushu1"
			    elif temp=="SHU1FMAC":
				print "shu1fmac"
			    elif temp=="FMACSHU1":
				print "fmacshu1"
			    elif temp=="IALUFALU":
				print "ialufalu"
			    elif temp=="FALUIALU":
				print "faluialu"
			    elif temp=="SHU0MRF":
				print "shu0mrf"
			    elif temp=="MRFSHU0":
				print "mrfshu0"
			    elif temp=="MRFSHU1":
				print "mrfshu1"
			    elif temp=="SHU1MRF":
				print "shu1mrf"
			    elif temp=="IALUIMAC":
				print "ialuimac"
			    elif temp=="IMACIALU":
				print "imacialu"
			    elif temp=="IMACFALU":
				print "imacfalu"
			    elif temp=="FALUIMAC":
				print "faluimac"
			    elif temp=="FALUFMAC":
				print "falufmac"
			    elif temp=="FMACFALU":
				print "fmacfalu"
	self.APE3Widget.MPUWidget.updateMPUFloatDialog(floatDialogList)

    
