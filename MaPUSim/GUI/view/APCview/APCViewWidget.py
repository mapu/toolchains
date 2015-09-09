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
from StageDialog import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  

class APCViewWidget(QWidget):  
    progressShowSignal=pyqtSignal(int,str,int)
    updateWidgetSignal=pyqtSignal(str)
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

	self.mainOpen=0

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

    def createThread(self,num,path):
	self.thread=Thread(self)
	self.thread.progressCall(self.callback)
	self.thread.num=num
	if self.mainOpen==0:
	    self.thread.path="m5out/"+path
	else:
	    self.thread.path=path
	self.thread.start()
	#thread.wait(3000)
	eventLoop=QEventLoop()
	self.connect(self.thread,SIGNAL("finished()"),eventLoop.quit)
	eventLoop.exec_()
	self.dataBase=self.thread.dataBase
	return self.dataBase

    def indexCallback(self):  
        self.currentValueSlot(self.slider.value())

    def simulatorDoneSlot(self,num,path): #num APE count
	self.updateWidgetSignal.emit("Create data base, and then update interface...")
	self.num=num
	#simulator exit normal,create data base
	#show dialog and show data base is building
	i=datetime.datetime.now()
        print ("start create data base %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	self.dataBaseDialog=DataBaseDialog()
	self.progressShowSignal.connect(self.dataBaseDialog.updateDataBaseDialog)
	self.dataBaseDialog.show()
	self.dataBase=self.createThread(num,path)
	i=datetime.datetime.now()
        print ("end create data base %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	i=datetime.datetime.now()
        print ("start update widget %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	self.thread.exit()
	del self.thread
	#read file content, get min and max time
	f=open(self.dataBase.filePath,"r")
        lines=f.readlines()
	f.close()
	if len(lines)==0:
	    self.dataBaseDialog.close()
	    QMessageBox.warning(self,"Warning","Trace file is empty!")
	    return
	#line=lines[0]
	#pos=line.index("[")
	#self.minTime=int(line[:pos])/1000
	self.minTime=0
	count=len(lines)
	line=lines[count-1]
	pos=line.find("[")
	self.maxTime=int(line[:pos])/1000
	self.updateWidgetSignal.emit("Data base has been created successfully, now update interface...")
	#update MPU and SPU stage dialog
	#APE0 MPU STAGE
	self.APE0Widget.MPUWidget.stageDialog=StageDialog()
	self.APE0Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE0dbFilePath,self.minTime,self.maxTime,"m") 
	#APE0 SPU STAGE
	self.APE0Widget.SPUWidget.stageDialog=StageDialog()
	self.APE0Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE0dbFilePath,self.minTime,self.maxTime,"s") 
	self.APE0Widget.MPUWidget.stageDialog.slider=self.slider 
	self.APE0Widget.SPUWidget.stageDialog.slider=self.slider 
	self.APE0Widget.MPUWidget.setButtonEnabled(True)
	self.APE0Widget.SPUWidget.stageButton.setEnabled(True)
	if self.num==2:
	    #APE1 MPU STAGE
	    self.APE1Widget.MPUWidget.stageDialog=StageDialog()
	    self.APE1Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE1 SPU STAGE
	    self.APE1Widget.SPUWidget.stageDialog=StageDialog()
	    self.APE1Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"s")
	    self.APE1Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.SPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.MPUWidget.setButtonEnabled(True)
	    self.APE1Widget.SPUWidget.stageButton.setEnabled(True)
	elif self.num==3:
	    #APE1 MPU STAGE
	    self.APE1Widget.MPUWidget.stageDialog=StageDialog()
	    self.APE1Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE1 SPU STAGE
	    self.APE1Widget.SPUWidget.stageDialog=StageDialog()
	    self.APE1Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE1Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.SPUWidget.stageDialog.slider=self.slider  
	    self.APE1Widget.MPUWidget.setButtonEnabled(True)
	    self.APE1Widget.SPUWidget.stageButton.setEnabled(True) 
	    #APE2 MPU STAGE
	    self.APE2Widget.MPUWidget.stageDialog=StageDialog()
	    self.APE2Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE2 SPU STAGE
	    self.APE2Widget.SPUWidget.stageDialog=StageDialog()
	    self.APE2Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE2Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.SPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.MPUWidget.setButtonEnabled(True)
	    self.APE2Widget.SPUWidget.stageButton.setEnabled(True) 
	elif self.num==4:
	    #APE1 MPU STAGE
	    self.APE1Widget.MPUWidget.stageDialog=StageDialog()
	    self.APE1Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE1 SPU STAGE
	    self.APE1Widget.SPUWidget.stageDialog=StageDialog()
	    self.APE1Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE1dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE1Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE1Widget.SPUWidget.stageDialog.slider=self.slider   
	    self.APE1Widget.MPUWidget.setButtonEnabled(True)
	    self.APE1Widget.SPUWidget.stageButton.setEnabled(True)
	    #APE2 MPU STAGE
	    self.APE2Widget.MPUWidget.stageDialog=StageDialog()
	    self.APE2Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE2 SPU STAGE
	    self.APE2Widget.SPUWidget.stageDialog=StageDialog()
	    self.APE2Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE2dbFilePath,self.minTime,self.maxTime,"s") 
	    self.APE2Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.SPUWidget.stageDialog.slider=self.slider 
	    self.APE2Widget.MPUWidget.setButtonEnabled(True)
	    self.APE2Widget.SPUWidget.stageButton.setEnabled(True)
	    #APE3 MPU STAGE
	    self.APE3Widget.MPUWidget.stageDialog=StageDialog()
	    self.APE3Widget.MPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE3dbFilePath,self.minTime,self.maxTime,"m") 
	    #APE3 SPU STAGE
	    self.APE3Widget.SPUWidget.stageDialog=StageDialog()
	    self.APE3Widget.SPUWidget.stageDialog.updatAPEData(self.dataBase,self.dataBase.APE3dbFilePath,self.minTime,self.maxTime,"s")
	    self.APE3Widget.MPUWidget.stageDialog.slider=self.slider 
	    self.APE3Widget.SPUWidget.stageDialog.slider=self.slider  
	    self.APE3Widget.MPUWidget.setButtonEnabled(True)
	    self.APE3Widget.SPUWidget.stageButton.setEnabled(True)
	#set the range
	self.slider.setEnabled(True)
	self.spinBox.setEnabled(True)
	#set slider min and max value
	self.slider.setRange(self.minTime,self.maxTime)
	self.spinBox.setRange(self.minTime,self.maxTime)
	#add slot for slider,if silder's value change, then response
	self.connect(self.slider,SIGNAL("valueChanged(int)"),self.currentValueSlot)
	self.connect(self.slider,SIGNAL("valueChanged(int)"),self.spinBox.setValue)
	self.connect(self.spinBox,SIGNAL("valueChanged(int)"),self.slider.setValue)
	self.APE0Widget.MPUWidget.indexCallback(self.indexCallback)
	self.APE1Widget.MPUWidget.indexCallback(self.indexCallback)
	self.APE2Widget.MPUWidget.indexCallback(self.indexCallback)
	self.APE3Widget.MPUWidget.indexCallback(self.indexCallback)
	self.updateWidgetSignal.emit("Data update successfully!")
	i=datetime.datetime.now()
        print ("end update widget %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))

    def currentValueSlot(self,time):
	curTime=self.slider.value()
	self.updateAPEWidget(curTime,self.APE0Widget,self.dataBase.APE0dbFilePath,self.dataBase.APE0timeFilePath)
	if self.num==2:
	    self.updateAPEWidget(curTime,self.APE1Widget,self.dataBase.APE1dbFilePath,self.dataBase.APE1timeFilePath)
	if self.num==3:
	    self.updateAPEWidget(curTime,self.APE1Widget,self.dataBase.APE1dbFilePath,self.dataBase.APE1timeFilePath)
	    self.updateAPEWidget(curTime,self.APE2Widget,self.dataBase.APE2dbFilePath,self.dataBase.APE2timeFilePath)
	if self.num==4:
	    self.updateAPEWidget(curTime,self.APE1Widget,self.dataBase.APE1dbFilePath,self.dataBase.APE1timeFilePath)
	    self.updateAPEWidget(curTime,self.APE2Widget,self.dataBase.APE2dbFilePath,self.dataBase.APE2timeFilePath)
	    self.updateAPEWidget(curTime,self.APE3Widget,self.dataBase.APE3dbFilePath,self.dataBase.APE3timeFilePath)

    def updateAPEWidget(self,curTime,APEWidget,APEdbFilePath,APEtimeFilePath):
	MPURList=[]
	SPURRList=[]
	SPURJList=[]
	APEWidget.MPUWidget.regModel.clear()
	APEWidget.MPUWidget.regFileTableInit()
        APEWidget.MPUWidget.buttonWidget.lineArray = [([0] * 14) for i in range(14)] 
	fetchall_sql="SELECT * FROM "+self.dataBase.timeTableName+" WHERE time = "+str(curTime)
	r=self.dataBase.fetchall(APEtimeFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		APEWidget.MPUWidget.updateMPUWidget(r[e])
		APEWidget.SPUWidget.updateSPUWidget(r[e])
	fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE time = "+str(curTime)
        r=self.dataBase.fetchall(APEdbFilePath,fetchall_sql)
	if r!=0:
	    for e in range(len(r)):
		if r[e][6]=="MPU Reg":
		    if r[e][5]=="R":
			MPURList.append(r[e][7])
		    else:
		        APEWidget.MPUWidget.updateMPURegWFlag(r[e])
		elif r[e][6]=="R Reg":
		    if r[e][5]=="R":
			SPURRList.append(r[e][7])
		    else:
		    	APEWidget.SPUWidget.updateSPURegWFlag(r[e])
		elif r[e][6]=="J Reg":
		    if r[e][5]=="R":
			SPURJList.append(r[e][7])
		    else:
		    	APEWidget.SPUWidget.updateSPURegWFlag(r[e])
	    for i in range(0,len(MPURList)):
		APEWidget.MPUWidget.updateMPURegRFlag(MPURList[i])
	    for i in range(0,len(SPURRList)):
		APEWidget.SPUWidget.updateSPURegRRFlag(SPURRList[i])
	    for i in range(0,len(SPURJList)):
		APEWidget.SPUWidget.updateSPURegRJFlag(SPURJList[i])

	floatDialogList=["nop"]*440
	fetchall_sql = "SELECT * FROM "+self.dataBase.snMTableName+" WHERE stage0 = "+str(curTime)+" or "+"stage1 = "+str(curTime)+" or "+"stage2 = "+str(curTime)+" or "+"stage3 = "+str(curTime)+" or "+"stage4 = "+str(curTime)+" or "+"stage5 = "+str(curTime)+" or "+"stage6 = "+str(curTime)+" or "+"stage7 = "+str(curTime)+" or "+"stage8 = "+str(curTime)+" or "+"stage9 = "+str(curTime)+" or "+"stage10 = "+str(curTime)+" or "+"stage11 = "+str(curTime)+" or "+"stage12 = "+str(curTime)+" or "+"stage13 = "+str(curTime)+" or "+"stage14 = "+str(curTime)+" or "+"stage15 = "+str(curTime)+" or "+"stage16 = "+str(curTime)+" or "+"stage17 = "+str(curTime)+" or "+"stage18 = "+str(curTime)+" or "+"stage19 = "+str(curTime)
        r=self.dataBase.fetchall(APEdbFilePath,fetchall_sql)
	if r!=0:
            if len(r) > 0:	
		for e in range(len(r)):
		    s=r[e][7]
		    pos=s.find(":")
		    s=s[(pos+1):]
		    if s[0]=="m":
			a="mrf"
		    else:
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
		
		    if r[e][7]!="nop":  #dis (memo:ialuadd   ) IALU.T1+T2->IMAC.T0  (memo:shu0idx   ) SHU0.T0 IND TSQ->M[0]  (memo:ialuadd   ) IALU.T1+T2->IMAC 
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
			    if pos!=-1:
			    	temp=temp+s[:pos]
			    else:
				pos=s.find("[")
				if pos!=-1:
				    temp=temp+"MRF"
				else:
				    pos=s.find(" ")
				    temp=temp+s
			    if temp=="DMBIU0": 
				APEWidget.MPUWidget.buttonWidget.lineArray[0][1]=1
			    elif temp=="BIU0DM":
				APEWidget.MPUWidget.buttonWidget.lineArray[1][0]=1
			    elif temp=="DMBIU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[0][2]=1
			    elif temp=="BIU1DM":
				APEWidget.MPUWidget.buttonWidget.lineArray[2][0]=1
			    elif temp=="DMBIU2":
				APEWidget.MPUWidget.buttonWidget.lineArray[0][3]=1
			    elif temp=="BIU2DM":
				APEWidget.MPUWidget.buttonWidget.lineArray[3][0]=1
			    elif temp=="BIU0SHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[1][4]=1
			    elif temp=="SHU0BIU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][1]=1
			    elif temp=="BIU1SHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[2][4]=1
			    elif temp=="SHU0BIU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][2]=1
			    elif temp=="BIU2SHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[3][4]=1
			    elif temp=="SHU0BIU2":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][3]=1
			    elif temp=="BIU0SHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[1][8]=1
			    elif temp=="SHU1BIU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[8][1]=1
			    elif temp=="BIU1SHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[2][8]=1
			    elif temp=="SHU1BIU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[8][2]=1
			    elif temp=="BIU2SHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[3][8]=1
			    elif temp=="SHU1BIU2":
				APEWidget.MPUWidget.buttonWidget.lineArray[8][3]=1
			    elif temp=="BIU0MRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[1][13]=1
			    elif temp=="MRFBIU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[13][1]=1
			    elif temp=="BIU1MRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[2][13]=1
			    elif temp=="MRFBIU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[13][2]=1
			    elif temp=="BIU2MRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[3][13]=1
			    elif temp=="MRFBIU2":
				APEWidget.MPUWidget.buttonWidget.lineArray[13][3]=1
			    elif temp=="SHU0IALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][9]=1
			    elif temp=="IALUSHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[9][4]=1
			    elif temp=="SHU0IMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][10]=1
			    elif temp=="IMACSHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[10][4]=1
			    elif temp=="SHU0FALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][11]=1
			    elif temp=="FALUSHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[11][4]=1
			    elif temp=="SHU0FMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][12]=1
			    elif temp=="FMACSHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[12][4]=1
			    elif temp=="R0IALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[5][9]=1
			    elif temp=="R1IALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[6][9]=1
			    elif temp=="R2IALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[7][9]=1
			    elif temp=="IALUMRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[9][13]=1
			    elif temp=="R0IMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[5][10]=1
			    elif temp=="R1IMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[6][10]=1
			    elif temp=="R2IMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[7][10]=1
			    elif temp=="IMACMRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[10][13]=1
			    elif temp=="R0FALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[5][11]=1
			    elif temp=="R1FALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[6][11]=1
			    elif temp=="R2FALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[7][11]=1
			    elif temp=="FALUMRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[11][13]=1
			    elif temp=="R0FMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[5][12]=1
			    elif temp=="R1FMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[6][12]=1
			    elif temp=="R2FMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[7][12]=1
			    elif temp=="FMACMRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[12][13]=1
			    elif temp=="SHU1IALU": 
				APEWidget.MPUWidget.buttonWidget.lineArray[8][9]=1
			    elif temp=="IALUSHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[9][8]=1
			    elif temp=="SHU1IMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[8][10]=1
			    elif temp=="IMACSHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[10][8]=1
			    elif temp=="SHU1FALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[8][11]=1
			    elif temp=="FALUSHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[11][8]=1
			    elif temp=="SHU1FMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[8][12]=1
			    elif temp=="FMACSHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[12][8]=1
			    elif temp=="IALUFALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[9][11]=1
			    elif temp=="FALUIALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[11][9]=1  #DM=0; BIU0=1; BIU1=2; BIU2=3; SHU0=4; MRF=13; SHU1=8; M0=5; M1=6; M2=7; IALU=9; IMAC=10; FALU=11; FMAC=12 
			    elif temp=="SHU0MRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[4][13]=1
			    elif temp=="R0SHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[13][4]=1
			    elif temp=="R1SHU0":
				APEWidget.MPUWidget.buttonWidget.lineArray[5][4]=1
			    elif temp=="R0SHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[13][8]=1
			    elif temp=="R1SHU1":
				APEWidget.MPUWidget.buttonWidget.lineArray[5][8]=1
			    elif temp=="SHU1MRF":
				APEWidget.MPUWidget.buttonWidget.lineArray[8][13]=1
			    elif temp=="IALUIMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[9][10]=1
			    elif temp=="IMACIALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[10][9]=1
			    elif temp=="IMACFALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[10][11]=1
			    elif temp=="FALUIMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[11][10]=1
			    elif temp=="FALUFMAC":
				APEWidget.MPUWidget.buttonWidget.lineArray[11][12]=1
			    elif temp=="FMACFALU":
				APEWidget.MPUWidget.buttonWidget.lineArray[12][11]=1
			    elif temp=="DMDM":
				APEWidget.MPUWidget.buttonWidget.DMButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="BIU0BIU0":
				APEWidget.MPUWidget.buttonWidget.BIU0Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="BIU1BIU1":
				APEWidget.MPUWidget.buttonWidget.BIU1Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="BIU2BIU2":
				APEWidget.MPUWidget.buttonWidget.BIU2Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="SHU0SHU0":
				APEWidget.MPUWidget.buttonWidget.SHU0Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="MRFMRF":
				APEWidget.MPUWidget.buttonWidget.MRFButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="SHU1SHU1":
				APEWidget.MPUWidget.buttonWidget.SHU1Button.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="IALUIALU":
				APEWidget.MPUWidget.buttonWidget.IALUButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="IMACIMAC":
				APEWidget.MPUWidget.buttonWidget.IMACButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="FALUFALU":
				APEWidget.MPUWidget.buttonWidget.FALUButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
			    elif temp=="FMACFMAC":
				APEWidget.MPUWidget.buttonWidget.FMACButton.setStyleSheet("QPushButton{border: 3px solid grey;border-radius:3px}")
	APEWidget.MPUWidget.buttonWidget.buttonLines()
	APEWidget.MPUWidget.buttonWidget.update()
	APEWidget.MPUWidget.updateMPUFloatDialog(floatDialogList)


