# -*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from FloatDialog import*
from LineWidget import*
import sys
import math

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPULeftWidget(QWidget):
    def __init__(self,parent=None):
	super(MPULeftWidget,self).__init__(parent)        
        self.setFixedSize(1000,500)
        self.List=[1]*82
        self.ButLi = [([1] * 14) for i in range(14)]  
        self.DMButton=QPushButton(self.tr("DM"))
	self.DMButton.setFixedSize(140,50)
	self.DMButton.setEnabled(False)
	self.BIU0Button=QPushButton(self.tr("BIU0"))
	self.BIU0Button.setFixedSize(140,50)
	self.BIU1Button=QPushButton(self.tr("BIU1"))
	self.BIU1Button.setFixedSize(140,50)
	self.BIU2Button=QPushButton(self.tr("BIU2"))
	self.BIU2Button.setFixedSize(140,50)
	self.SHU0Button=QPushButton(self.tr("SHU0"))
	self.SHU0Button.setFixedSize(140,50)
	self.MRFButton=QPushButton(self.tr("M RF"))
	self.MRFButton.setFixedSize(140,150)
	self.SHU1Button=QPushButton(self.tr("SHU1"))
	self.SHU1Button.setFixedSize(140,50)
	self.IALUButton=QPushButton(self.tr("IALU"))
	self.IALUButton.setFixedSize(140,50)
	self.IMACButton=QPushButton(self.tr("IMAC"))
	self.IMACButton.setFixedSize(140,50)
	self.FALUButton=QPushButton(self.tr("FALU"))
	self.FALUButton.setFixedSize(140,50)
	self.FMACButton=QPushButton(self.tr("FMAC"))
	self.FMACButton.setFixedSize(140,50)
        
	self.blank0=QLabel()
	self.blank0.setFixedSize(140,25)
	self.blank1=QLabel()
	self.blank1.setFixedSize(140,25)
	self.blank2=QLabel()
	self.blank2.setFixedSize(140,25)
	self.blank3=QLabel()
	self.blank3.setFixedSize(140,25)
	self.blank4=QLabel()
	self.blank4.setFixedSize(140,25)
	self.blank5=QLabel()
	self.blank5.setFixedSize(140,25)
	self.blank6=QLabel()
	self.blank6.setFixedSize(140,25)           

	#layout left Widget        
	self.gridLay=QGridLayout()
	self.gridLay.addWidget(self.DMButton,7,0,2,1)
	self.gridLay.addWidget(self.blank0,0,1,2,1)
	self.gridLay.addWidget(self.blank1,3,2,2,1)
	self.gridLay.addWidget(self.BIU0Button,3,2,2,1)
	self.gridLay.addWidget(self.BIU1Button,7,2,2,1)
	self.gridLay.addWidget(self.BIU2Button,11,2,2,1)
	self.gridLay.addWidget(self.blank2,0,4,2,1)
	self.gridLay.addWidget(self.blank3,3,5,2,1)
	self.gridLay.addWidget(self.SHU0Button,0,5,2,1)
	self.gridLay.addWidget(self.SHU1Button,14,5,2,1)
	self.gridLay.addWidget(self.blank4,6,7,2,1)
	self.gridLay.addWidget(self.blank5,9,8,2,1)
	self.gridLay.addWidget(self.IALUButton,1,9,2,1)
	self.gridLay.addWidget(self.IMACButton,5,9,2,1)
	self.gridLay.addWidget(self.FALUButton,9,9,2,1)
	self.gridLay.addWidget(self.FMACButton,13,9,2,1)
	self.gridLay.addWidget(self.blank6,0,10,2,1)
        self.gridLay.addWidget(self.MRFButton,5,5,6,1)
	self.gridLay.setAlignment(Qt.AlignCenter)
	self.setLayout(self.gridLay)

	self.connect(self.DMButton,SIGNAL("clicked()"),self.DMButtonSlot)
	self.connect(self.BIU0Button,SIGNAL("clicked()"),self.BIU0ButtonSlot)
	self.connect(self.BIU1Button,SIGNAL("clicked()"),self.BIU1ButtonSlot)
	self.connect(self.BIU2Button,SIGNAL("clicked()"),self.BIU2ButtonSlot)
	self.connect(self.SHU0Button,SIGNAL("clicked()"),self.SHU0ButtonSlot)
	self.connect(self.SHU1Button,SIGNAL("clicked()"),self.SHU1ButtonSlot)
	self.connect(self.MRFButton,SIGNAL("clicked()"),self.MRFButtonSlot)
	self.connect(self.IALUButton,SIGNAL("clicked()"),self.IALUButtonSlot)
	self.connect(self.IMACButton,SIGNAL("clicked()"),self.IMACButtonSlot)
	self.connect(self.FALUButton,SIGNAL("clicked()"),self.FALUButtonSlot)
	self.connect(self.FMACButton,SIGNAL("clicked()"),self.FMACButtonSlot)

        #define float dialog
	self.DMButtonDialog=FloatDialog()
	self.DMButtonDialog.setWindowTitle(self.tr("DM Stages"))
	self.BIU0ButtonDialog=FloatDialog()
	self.BIU0ButtonDialog.setWindowTitle(self.tr("BIU0 Stages"))
	self.BIU1ButtonDialog=FloatDialog()
	self.BIU1ButtonDialog.setWindowTitle(self.tr("BIU1 Stages"))	
	self.BIU2ButtonDialog=FloatDialog()
	self.BIU2ButtonDialog.setWindowTitle(self.tr("BIU2 Stages"))	
	self.SHU0ButtonDialog=FloatDialog()
	self.SHU0ButtonDialog.setWindowTitle(self.tr("SHU0 Stages"))	
	self.SHU1ButtonDialog=FloatDialog()
	self.SHU1ButtonDialog.setWindowTitle(self.tr("SHU1 Stages"))
	self.MRFButtonDialog=FloatDialog()
	self.MRFButtonDialog.setWindowTitle(self.tr("MRF Stages"))	
	self.IALUButtonDialog=FloatDialog()
	self.IALUButtonDialog.setWindowTitle(self.tr("IALU Stages"))	
	self.IMACButtonDialog=FloatDialog()
	self.IMACButtonDialog.setWindowTitle(self.tr("IMAC Stages"))
	self.FALUButtonDialog=FloatDialog()
	self.FALUButtonDialog.setWindowTitle(self.tr("FALU Stages"))	
	self.FMACButtonDialog=FloatDialog()
	self.FMACButtonDialog.setWindowTitle(self.tr("FMAC Stages"))	

    #define slot function
    def DMButtonSlot(self):
	self.DMButtonDialog.flag=1
	self.DMButtonDialog.show()

    def BIU0ButtonSlot(self):
	self.BIU0ButtonDialog.flag=1
	self.BIU0ButtonDialog.show()

    def BIU1ButtonSlot(self):
	self.BIU1ButtonDialog.flag=1
	self.BIU1ButtonDialog.show()

    def BIU2ButtonSlot(self):	
	self.BIU2ButtonDialog.flag=1
	self.BIU2ButtonDialog.show()

    def SHU0ButtonSlot(self):	
	self.SHU0ButtonDialog.flag=1
	self.SHU0ButtonDialog.show()

    def SHU1ButtonSlot(self):	
	self.SHU1ButtonDialog.flag=1
	self.SHU1ButtonDialog.show()

    def MRFButtonSlot(self):	
	self.MRFButtonDialog.flag=1
	self.MRFButtonDialog.show()

    def IALUButtonSlot(self):
	self.IALUButtonDialog.flag=1
	self.IALUButtonDialog.show()

    def IMACButtonSlot(self):
	self.IMACButtonDialog.flag=1
	self.IMACButtonDialog.show()

    def FALUButtonSlot(self):
	self.FALUButtonDialog.flag=1
	self.FALUButtonDialog.show()

    def FMACButtonSlot(self):
	self.FMACButtonDialog.flag=1
	self.FMACButtonDialog.show()

    #define close all float dialogs
    def closeFloatDialogs(self):
	if self.DMButtonDialog.flag!=-1:
	    self.DMButtonDialog.close()
	if self.BIU0ButtonDialog.flag!=-1:
	    self.BIU0ButtonDialog.close()
	if self.BIU1ButtonDialog.flag!=-1:
	    self.BIU1ButtonDialog.close()
	if self.BIU2ButtonDialog.flag!=-1:
	    self.BIU2ButtonDialog.close()
	if self.SHU0ButtonDialog.flag!=-1:
	    self.SHU0ButtonDialog.close()
	if self.SHU1ButtonDialog.flag!=-1:
	    self.SHU1ButtonDialog.close()
	if self.MRFButtonDialog.flag!=-1:
	    self.MRFButtonDialog.close()
	if self.IALUButtonDialog.flag!=-1:
	    self.IALUButtonDialog.close()
	if self.IMACButtonDialog.flag!=-1:
	    self.IMACButtonDialog.close()
	if self.FALUButtonDialog.flag!=-1:
	    self.FALUButtonDialog.close()
	if self.FMACButtonDialog.flag!=-1:
	    self.FMACButtonDialog.close()
		 
    def paintLine(self,n,Llist):
        self.dn=n
        self.dlist=Llist
        self.qp=QPainter()
        self.qp.begin(self)
        self.pen=QPen(Qt.black,1.5,Qt.SolidLine)     
        self.qp.setPen(self.pen)      
        self.qp.translate(0,0)
        for i in range(0,2*n-3):
            if i%2==0:
               self.qp.drawLine(Llist[i],Llist[i+1],Llist[i+2],Llist[i+3])
        self.qp.end()
            
    def paintstaen(self,flag,Llist):
        self.Flag=flag
        self.dlist=Llist
        n=len(Llist)    
        a=2.5/math.sqrt(2)       
        if flag==0: 
            self.qp=QPainter()
            self.qp.begin(self)          
            self.brush=QBrush(Qt.black) 
            self.qp.setBrush(self.brush)
            self.qp.drawEllipse(Llist[0]-a,Llist[1]-a,5,5)
        elif flag==1:
            self.qp=QPainter()
            self.qp.begin(self) 
            self.pen=QPen(Qt.black,1,Qt.SolidLine)
            self.qp.setPen(self.pen)
            self.qp.drawEllipse(Llist[n-2]-a,Llist[n-1]-a,5,5)
        elif flag==-1:
            self.qp=QPainter()
            self.qp.begin(self) 
            self.pen=QPen(Qt.black,1,Qt.SolidLine)
            self.qp.setPen(self.pen)
            self.qp.drawEllipse(Llist[0]-a,Llist[1]-a,5,5)
        elif flag==2:
            self.qp=QPainter()
            self.qp.begin(self) 
            self.brush=QBrush(Qt.black) 
            self.qp.setBrush(self.brush)
            self.qp.drawEllipse(Llist[0]-a,Llist[1]-a,5,5)
            self.qp2=QPainter()
            self.qp2.begin(self) 
            self.pen2=QPen(Qt.black,1,Qt.SolidLine)
            self.qp2.setPen(self.pen)
            self.qp2.drawEllipse(Llist[n-2]-a,Llist[n-1]-a,5,5)     
        elif flag==-2:
            self.qp=QPainter()
            self.qp.begin(self) 
            self.brush=QBrush(Qt.black) 
            self.qp.setBrush(self.brush)
            self.qp.drawEllipse(Llist[n-2]-a,Llist[n-1]-a,5,5)
            self.qp2=QPainter()
            self.qp2.begin(self) 
            self.pen2=QPen(Qt.black,1,Qt.SolidLine)
            self.qp2.setPen(self.pen)
            self.qp2.drawEllipse(Llist[0]-a,Llist[1]-a,5,5)
                                         	            
    def paintEvent(self,event):                        
        #SHUO to BIU0
        x1BIU=370
        y1BIU=134
        width=17 
        X2BIU=435
        y2BIU=205
        B0S0X=445
        B0S0Y=61
        #B0 to S0
        if self.List[0]==1:
           L1B0S0=[x1BIU,y1BIU+width,x1BIU+50,y1BIU-20+width,x1BIU+50,y1BIU-10-40,X2BIU,B0S0Y]    
           self.paintLine(4,L1B0S0) 
           self.paintstaen(-2,L1B0S0)
        if self.List[1]==1:
           L2B0S0=[x1BIU,y1BIU,x1BIU+50,y1BIU-20,x1BIU+50,y1BIU-10-40,X2BIU,B0S0Y]
           self.paintLine(4,L2B0S0) 
           self.paintstaen(2,L2B0S0) 
        #B1 to s0     
        YB1S0=242  
        YBIU1toMRF=242    
        if self.List[2]==1:
           L1B1S0=[x1BIU,YBIU1toMRF+width,x1BIU+50,YB1S0-20,x1BIU+50,y1BIU-10-40,X2BIU,B0S0Y]
           self.paintLine(4,L1B1S0) 
           self.paintstaen(-2,L1B1S0) 
        if self.List[3]==1:   
           L2B1S0=[x1BIU,YBIU1toMRF,x1BIU+50,YB1S0-40,x1BIU+50,y1BIU-10-40,X2BIU,B0S0Y]
           self.paintLine(4,L2B1S0) 
           self.paintstaen(2,L2B1S0)  
        #B2 to s0
        YB2S0=360
        if self.List[4]==1:        
           L1B2S0=[x1BIU,YB2S0,x1BIU+50,YB2S0-25,x1BIU+50,y1BIU-10-40,X2BIU,B0S0Y]
           self.paintLine(4,L1B2S0) 
           self.paintstaen(-1,L1B2S0)       
        if self.List[5]==1:
           L2B2S0=[x1BIU,YB2S0-width,x1BIU+50,YB2S0-42,x1BIU+50,y1BIU-10-40,X2BIU,B0S0Y] 
           self.paintLine(4,L2B2S0) 
           self.paintstaen(0,L2B2S0) 
        #BIU0 to MRF                
        if self.List[6]==1:
           L1B0MRF=[x1BIU,y1BIU,X2BIU,YBIU1toMRF]
           self.paintLine(2,L1B0MRF) 
           self.paintstaen(2,L1B0MRF)  
        if self.List[7]==1:
           L2B0MRF=[x1BIU,y1BIU+17,X2BIU,YBIU1toMRF+width]
           self.paintLine(2,L2B0MRF) 
           self.paintstaen(-2,L2B0MRF)  
        #BIU1 to MRF           
        if self.List[8]==1: 
           L1B1MRF=[x1BIU,YBIU1toMRF,X2BIU,YBIU1toMRF]
           self.paintLine(2,L1B1MRF) 
           self.paintstaen(2,L1B1MRF) 
        if self.List[9]==1:
           L2B1MRF=[x1BIU,YBIU1toMRF+width,X2BIU,YBIU1toMRF+width]
           self.paintLine(2,L2B1MRF) 
           self.paintstaen(-2,L2B1MRF) 
        #BIU2 to MRF
        XBIU2toMRF=320 
        if self.List[10]==1:         
           L1B2MRF=[x1BIU,YB2S0-width,X2BIU,YBIU1toMRF]
           self.paintLine(2,L1B2MRF) 
           self.paintstaen(2,L1B2MRF) 
        if self.List[11]==1:
           L2B2MRF=[x1BIU,YB2S0,X2BIU,YBIU1toMRF+width]
           self.paintLine(2,L2B2MRF) 
           self.paintstaen(-2,L2B2MRF)            
        #SHU0 to MRF                                                                                                                                                                            
        X1SHU=493
        Y1SHU=100
        SHUtoMRFlength=50
        if self.List[12]==1:     
           S0MRF1=[X1SHU,Y1SHU,X1SHU,Y1SHU+SHUtoMRFlength] 
           self.paintLine(2,S0MRF1) 
           self.paintstaen(2,S0MRF1)   
        if self.List[13]==1:           
           S0MRF2=[X1SHU+47,Y1SHU,X1SHU+47,Y1SHU+SHUtoMRFlength] 
           self.paintLine(2,S0MRF2) 
           self.paintstaen(-2,S0MRF2)         
        #MRF to SHU1
        Y1MRFtoSHU1=340
        if self.List[14]==1:
           MRFS1=[X1SHU,Y1MRFtoSHU1,X1SHU,Y1MRFtoSHU1+SHUtoMRFlength]
           self.paintLine(2,MRFS1) 
           self.paintstaen(2,MRFS1)
        if self.List[15]==1:  
           MRFS12=[X1SHU+47,Y1MRFtoSHU1,X1SHU+47,Y1MRFtoSHU1+SHUtoMRFlength]
           self.paintLine(2,MRFS12) 
           self.paintstaen(-2,MRFS12)  
        #Point
        x1=686
        y1=142        
        #Point(x1,y1)
        #Point(x1,y1+108)
        #Point(x1,y1+2*108)        
        #point to SHU0
        Xbendso1=650
        ybendso1=84   
        X1MRF=590
        Y1MRF=256  
        XS0toP=67   
        if self.List[16]==1:
           Point1S0=[X1MRF,XS0toP,Xbendso1,ybendso1,Xbendso1,y1-10,x1,y1]
           self.paintLine(4,Point1S0) 
           self.paintstaen(2,Point1S0)         
        if self.List[17]==1:
           Point2S0=[X1MRF,XS0toP,Xbendso1,ybendso1,Xbendso1,y1+108-10,x1,y1+108]
           self.paintLine(4,Point2S0) 
           self.paintstaen(2,Point2S0)
        if self.List[18]==1: 
           Point3S0=[X1MRF,XS0toP,Xbendso1,ybendso1,Xbendso1,y1+2*108-10,x1,y1+2*108]
           self.paintLine(4,Point3S0)   
           self.paintstaen(2,Point3S0)
        #Point to MRF                   
        if self.List[19]==1: 
           Point1MRF2=[X1MRF,y1+108,x1,y1]
           self.paintLine(2,Point1MRF2)   
           self.paintstaen(2,Point1MRF2)
        if self.List[20]==1:
           Point2MRF2=[X1MRF,y1+108,x1,y1+108]
           self.paintLine(2,Point2MRF2) 
           self.paintstaen(2,Point2MRF2)
        if self.List[21]==1:
           Point3MRF2=[X1MRF,y1+108,x1,y1+2*108]
           self.paintLine(2,Point3MRF2) 
           self.paintstaen(2,Point3MRF2)
        if self.List[22]==1:     
           Point1MRF1=[X1MRF,y1+58,x1,y1]
           self.paintLine(2,Point1MRF1)
           self.paintstaen(2,Point1MRF1) 
        if self.List[23]==1: 
           Point2MRF1=[X1MRF,y1+58,x1,y1+108]
           self.paintLine(2,Point2MRF1) 
           self.paintstaen(2,Point2MRF1)
        if self.List[24]==1: 
           Point3MRF1=[X1MRF,y1+58,x1,y1+2*108]
           self.paintLine(2,Point3MRF1)
           self.paintstaen(2,Point3MRF1) 
        if self.List[25]==1:  
           Point1MRF3=[X1MRF,y1+158,x1,y1]
           self.paintLine(2,Point1MRF3)
           self.paintstaen(2,Point1MRF3) 
        if self.List[26]==1: 
           Point2MRF3=[X1MRF,y1+158,x1,y1+108]
           self.paintLine(2,Point2MRF3)
           self.paintstaen(2,Point2MRF3) 
        if self.List[27]==1: 
           Point3MRF3=[X1MRF,y1+158,x1,y1+2*108]
           self.paintLine(2,Point3MRF3)
           self.paintstaen(2,Point3MRF3)    
        #Point to SHU1
        if self.List[28]==1: 
           point1S1=[X1MRF,Y1MRF+184,X1MRF+15,Y1MRF+169,X1MRF+15,y1+20,x1,y1] 
           self.paintLine(4,point1S1) 
           self.paintstaen(2,point1S1)
        if self.List[29]==1:        
           point2S1=[X1MRF,Y1MRF+184,X1MRF+15,Y1MRF+169,X1MRF+15,y1+108+20,x1,y1+108]
           self.paintLine(4,point2S1)
           self.paintstaen(2,point2S1)
        if self.List[30]==1:  
           point3S1=[X1MRF,Y1MRF+184,X1MRF+15,Y1MRF+169,X1MRF+15,y1+2*108+20,x1,y1+2*108]
           self.paintLine(4,point3S1)
           self.paintstaen(2,point3S1)
        #SHU1 to BIU
        if self.List[31]==1:
           S1B21=[x1BIU,YB2S0-20,x1BIU+50-20,350+width,x1BIU+50-20,350+width+10+20+30,X2BIU,Y1MRF+183]
           self.paintLine(4,S1B21)
           self.paintstaen(2,S1B21)
        if self.List[32]==1:
           S1B22=[x1BIU,YB2S0,x1BIU+50-20,350+width+20,x1BIU+50-20,350+width+10+20+30,X2BIU,Y1MRF+183]
           self.paintLine(4,S1B22)
           self.paintstaen(-2,S1B22)
        if self.List[33]==1: 
           S1B11=[x1BIU,YBIU1toMRF,x1BIU+50-20,250+width,x1BIU+50-20,350+width+10+20+30,X2BIU,Y1MRF+183]
           self.paintLine(4,S1B11)
           self.paintstaen(2,S1B11)
        if self.List[34]==1:
           S1B12=[x1BIU,YBIU1toMRF+width,x1BIU+50-20,250+width+10+10,x1BIU+50-20,350+width+10+20+30,X2BIU,Y1MRF+183]
           self.paintLine(4,S1B12)
           self.paintstaen(-2,S1B12)
        if self.List[35]==1:
           S1B01=[x1BIU,y1BIU,x1BIU+30,y1BIU+30,x1BIU+30,350+width+60,X2BIU,Y1MRF+183]
           self.paintLine(4,S1B01)
           self.paintstaen(2,S1B01)
        if self.List[36]==1:
           S1B02=[x1BIU,y1BIU+17,x1BIU+30,y1BIU+30+17,x1BIU+30,350+width+60,X2BIU,Y1MRF+183]
           self.paintLine(4,S1B02)
           self.paintstaen(-2,S1B02)                          
        #points to LALU
        PXIALU=770
        P1YLALU=88
        if self.List[37]==1:
           P1IA=[x1,y1,PXIALU,P1YLALU]
           self.paintLine(2,P1IA)
           self.paintstaen(-2,P1IA)
        if self.List[38]==1: 
           P2IA=[x1,y1+108,PXIALU,P1YLALU]
           self.paintLine(2,P2IA)
           self.paintstaen(-2,P2IA)
        if self.List[39]==1:
           P3IA=[x1,y1+2*108,PXIALU,P1YLALU]
           self.paintLine(2,P3IA)
           self.paintstaen(-2,P3IA)
        #points to IMAC        
        P1YIMAC=196
        if self.List[40]==1:
           P1IM=[x1,y1,PXIALU,P1YIMAC]
           self.paintLine(2,P1IM)
           self.paintstaen(-2,P1IM)
        if self.List[41]==1:
           P2IM=[x1,y1+108,PXIALU,P1YIMAC]
           self.paintLine(2,P2IM)
           self.paintstaen(-2,P2IM)
        if self.List[42]==1: 
           P3IM=[x1,y1+2*108,PXIALU,P1YIMAC]
           self.paintLine(2,P3IM)
           self.paintstaen(-2,P3IM)   
        #point to FALU
        P1YFALU=304
        if self.List[43]==1:
           P1FA=[x1,y1,PXIALU,P1YFALU]  
           self.paintLine(2,P1FA)
           self.paintstaen(-2,P1FA)
        if self.List[44]==1: 
           P2FA=[x1,y1+108,PXIALU,P1YFALU]  
           self.paintLine(2,P2FA)
           self.paintstaen(-2,P2FA)
        if self.List[45]==1: 
           P3FA=[x1,y1+2*108,PXIALU,P1YFALU]  
           self.paintLine(2,P3FA)
           self.paintstaen(-2,P3FA)      
        #point to FMAC
        P1YFMAC=412
        if self.List[46]==1:
           P1FM=[x1,y1,PXIALU,P1YFMAC]
           self.paintLine(2,P1FM)
           self.paintstaen(-2,P1FM)
        if self.List[47]==1: 
           P2FM=[x1,y1+108,PXIALU,P1YFMAC]
           self.paintLine(2,P2FM)
           self.paintstaen(-2,P2FM)
        if self.List[48]==1:  
           P3FM=[x1,y1+2*108,PXIALU,P1YFMAC]
           self.paintLine(2,P3FM)
           self.paintstaen(-2,P3FM)    
        #LALU to IMAC
        XLAtoIM=825
        YLAtoIM=124
        LatoIMlen=30  
        if self.List[49]==1: 
           LAIM1=[XLAtoIM,YLAtoIM,XLAtoIM,YLAtoIM+LatoIMlen]
           self.paintLine(2,LAIM1)
           self.paintstaen(2,LAIM1)
        if self.List[50]==1:
           LAIM2=[XLAtoIM+47,YLAtoIM,XLAtoIM+47,YLAtoIM+LatoIMlen]
           self.paintLine(2,LAIM2)
           self.paintstaen(-2,LAIM2)         
        #IMAC to FALU
        if self.List[51]==1:
           IMFA1=[XLAtoIM,YLAtoIM+108,XLAtoIM,YLAtoIM+108+30]
           self.paintLine(2,IMFA1)
           self.paintstaen(2,IMFA1)
        if self.List[52]==1:
           IMFA2=[XLAtoIM+47,YLAtoIM+108,XLAtoIM+47,YLAtoIM+108+30]
           self.paintLine(2,IMFA2)
           self.paintstaen(-2,IMFA2)
        #FALU to FMAC
        if self.List[53]==1:
           FAFM1=[XLAtoIM,YLAtoIM+2*108,XLAtoIM,YLAtoIM+2*108+30]
           self.paintLine(2,FAFM1)
           self.paintstaen(2,FAFM1)
        if self.List[54]==1:
           FAFM2=[XLAtoIM+47,YLAtoIM+2*108,XLAtoIM+47,YLAtoIM+2*108+30]
           self.paintLine(2,FAFM2)
           self.paintstaen(-2,FAFM2)     
        #IA to FA
        Lefx=930
        Lefy=88
        if self.List[55]==1:
           IAFA1=[Lefx,Lefy,Lefx+15,Lefy+15,Lefx+15,Lefy+15+158,Lefx,Lefy+216]
           self.paintLine(4,IAFA1)
           self.paintstaen(-2,IAFA1)
        if self.List[56]==1:
           IAFA2=[Lefx+10,Lefy,Lefx+15+10,Lefy+15,Lefx+15+10,Lefy+15+158,Lefx+10,Lefy+216]
           self.paintLine(4,IAFA2)
           self.paintstaen(2,IAFA2)
        #left point 
        LePx=Lefx+35+30
        LePy=252
        #self.qp.drawPoint(LePx,LePy)
        XIA=925
        YIA=88
        if self.List[57]==1:
           LEPIA=[XIA,YIA,LePx,LePy]
           self.paintLine(2,LEPIA)
           self.paintstaen(0,LEPIA)
        if self.List[58]==1:
           LEPIM=[XIA,YIA+108,LePx,LePy]
           self.paintLine(2,LEPIM)
           self.paintstaen(0,LEPIM)
        if self.List [59]==1:
           LEPFA=[XIA,YIA+216,LePx,LePy]
           self.paintLine(2,LEPFA)
           self.paintstaen(0,LEPFA)
        if self.List[60]==1:        
           LEPFM=[XIA,YIA+324,LePx,LePy]
           self.paintLine(2,LEPFM)
           self.paintstaen(0,LEPFM)
        if self.List[61]==1:
        #right point to Bi
           LEPB2=[LePx,LePy,LePx,Y1MRF+184+20+20,x1BIU+50-20-10,Y1MRF+184+20+20,x1BIU+50-20-10,YB2S0+20,x1BIU,YB2S0]
           self.paintLine(5,LEPB2)
           self.paintstaen(1,LEPB2) 
        if self.List[62]==1:
           LEPB1=[LePx,LePy,LePx,Y1MRF+184+20+20,x1BIU+50-20-10,Y1MRF+184+20+20,x1BIU+50-20-10,YBIU1toMRF+width+20,x1BIU,YBIU1toMRF+width]
           self.paintLine(5,LEPB1)
           self.paintstaen(1,LEPB1) 
        if self.List[63]==1:
           LEPB0=[LePx,LePy,LePx,Y1MRF-184-20-40,x1BIU+40,Y1MRF-184-20-40,x1BIU+40,Y1MRF-154,x1BIU,y1BIU]
           self.paintLine(5,LEPB0)
           self.paintstaen(1,LEPB0) 
        if self.List[64]==1:
        #right point to shu1
           LEPS1=[LePx,LePy,LePx,Y1MRF+184+20+20,517,Y1MRF+184+20+20,517,Y1MRF+184+20+10]
           self.paintLine(4,LEPS1) 
           self.paintstaen(1,LEPS1)    
        #right point to MRF
        if self.List[65]==1:
           LEPMR=[LePx,LePy,LePx,Y1MRF-184-20-40,x1BIU+40,Y1MRF-184-20-40,x1BIU+40,YBIU1toMRF-35,X2BIU,YBIU1toMRF]
           self.paintLine(5,LEPMR)
           self.paintstaen(1,LEPMR)         
        #right point to shu0
        if self.List[66]==1:
           LEPS0=[LePx,LePy,LePx,Y1MRF-184-20-40,517,Y1MRF-184-20-40,517,Y1MRF-184-20-30]
           self.paintLine(4,LEPS0)  
           self.paintstaen(1,LEPS0)                          
        #DM to BIU0
           DMX=150
           DMY=242
           DtoBX=210
           DtoBY=134        
        if self.List[67]==1:
           DMBIU01=[DMX,DMY+width,DtoBX,DtoBY+width]
           self.paintLine(2,DMBIU01)
           self.paintstaen(-2,DMBIU01)
        if self.List[68]==1:
           DMBIU02=[DMX,DMY,DtoBX,DtoBY]
           self.paintLine(2,DMBIU02)
           self.paintstaen(2,DMBIU02)
        if self.List[69]==1:  
           #DM to BIU1
           DMBIU11=[DMX,DMY,DtoBX,DMY] 
           self.paintLine(2,DMBIU11)
           self.paintstaen(2,DMBIU11)
        if self.List[70]==1: 
           DMBIU12=[DMX,DMY+width,DtoBX,DMY+width] 
           self.paintLine(2,DMBIU12)
           self.paintstaen(-2,DMBIU12) 
        if self.List[71]==1:
             #DM to BIU2
           DMBIU21=[DMX,DMY,DtoBX,DtoBY+216]
           self.paintLine(2,DMBIU21)
           self.paintstaen(2,DMBIU21)  
        if self.List[72]==1: 
           DMBIU22=[DMX,DMY+width,DtoBX,DtoBY+216+width]
           self.paintLine(2,DMBIU22)
           self.paintstaen(-2,DMBIU22) 
        if self.List[73]==1:
           #BIU0 to Point1
           BIU0P1=[x1BIU,y1BIU,Xbendso1,y1BIU,x1,y1]
           self.paintLine(3,BIU0P1)
           self.paintstaen(0,BIU0P1) 
        if self.List[74]==1:
           #BIU0 to Point2
           BIU0P2=[x1BIU,y1BIU,Xbendso1,y1BIU,Xbendso1,y1+108-10,x1,y1+108]
           self.paintLine(4,BIU0P2)
           self.paintLine(0,BIU0P2)
        if self.List[75]==1:
           #BIU0 to Point3
           BIU0P3=[x1BIU,y1BIU,Xbendso1,y1BIU,Xbendso1,y1+2*108-10,x1,y1+2*108]
           self.paintLine(4,BIU0P3)
           self.paintLine(0,BIU0P3)
        if self.List[76]==1:
             #BIU1 to Point3
           BIU1P3=[x1BIU,YBIU1toMRF+width,X2BIU-10,YBIU1toMRF+width,X2BIU-10,YBIU1toMRF+width+80,X1MRF+15,YBIU1toMRF+width+80,x1,y1+2*108] 
           self.paintLine(5,BIU1P3)
           self.paintLine(0,BIU1P3)
        if self.List[77]==1:
           #BIU1 to Point2
           BIU1P2=[x1BIU,YBIU1toMRF+width,X2BIU-10,YBIU1toMRF+width,X2BIU-10,YBIU1toMRF+width+80,X1MRF+15,YBIU1toMRF+width+80,X1MRF+15,y1+108+20,x1,y1+108]
           self.paintLine(6,BIU1P2)
           self.paintLine(0,BIU1P2)
        if self.List[78]==1:
             #BIU1 to Point1
           BIU1P1=[x1BIU,YBIU1toMRF+width,X2BIU-10,YBIU1toMRF+width,X2BIU-10,YBIU1toMRF+width+80,X1MRF+15,YBIU1toMRF+width+80,X1MRF+15,y1+108+20,X1MRF+15,y1+20,x1,y1]
           self.paintLine(6,BIU1P1)
           self.paintLine(0,BIU1P1)
        if self.List[79]==1:
             #BIU2 to Point3
           BIU2P3=[x1BIU,YB2S0,X1MRF+15,YB2S0,x1,y1+2*108]
           self.paintLine(3,BIU2P3)
           self.paintLine(0,BIU2P3)
        if self.List[80]==1:
           #BIU2 to Point2
           BIU2P2=[x1BIU,YB2S0,X1MRF+15,YB2S0,X1MRF+15,y1+108+20,x1,y1+108]
           self.paintLine(4,BIU2P2)
           self.paintLine(0,BIU2P2)
        if self.List[81]==1:
           #BIU2 to Point1
           BIU2P1=[x1BIU,YB2S0,X1MRF+15,YB2S0,X1MRF+15,y1+20,x1,y1]
           self.paintLine(4,BIU2P1)
           self.paintLine(0,BIU2P1)


#Dm=0; BIU0=1; BIU1=2; BIU2=3; SHU0=4; MRF=13; SHU1=8; M0=5; M1=6; M2=7; IALU=9; IMAC=10; FALU=11; FMAC=12  
    def ButLine(self):
        if ButLi[0][1]==1:
           List[68]=1
        if ButLi[1][0]==1:
           List[67]=1
        if ButLi[0][2]==1:
           List[69]=1
        if ButLi[2][0]==1:
           List[70]=1
        if ButLi[0][3]==1:
           List[71]=1 
        if ButLi[3][0]==1:
           List[72]=1                   
        if ButLi[1][4]==1:
           List[1]=1
        if ButLi[4][1]==1:
           List[0]=1
        if ButLi[1][13]==1:
           List[6]=1 
        if ButLi[13][1]==1:
           List[7]=1
        if ButLi[1][8]==1:
           List[35]=1
        if ButLi[8][1]==1:
           List[36]=1        
        if ButLi[2][4]==1:
           List[3]=1
        if ButLi[4][2]==1:
           List[3]=1     
        if ButLi[2][13]==1:
           List[8]=1
        if ButLi[13][2]==1:
           List[9]=1
        if ButLi[2][8]==1:
           List[33]=1
        if ButLi[8][2]==1:
           List[34]=1
        if ButLi[3][4]==1:
           List[5]=1       
        if ButLi[4][3]==1:
           List[4]=1
        if ButLi[3][13]==1:
           List[10]=1
        if ButLi[13][3]==1:
           List[11]=1
        if ButLi[3][8]==1:
           List[31]=1
        if ButLi[8][3]==1:
           List[32]=1
        if ButLi[4][13]==1:
           List[12]=1 
        if ButLi[13][4]==1:
           List[13]=1 
        if ButLi[13][8]==1:
           List[14]=1
        if ButLi[8][13]==1:
           List[15]=1
 #value=1:cross P1;  value=2:cross P2; value=3:cross P3     
        if ButLi[4][9]==1:
           List[16]=1
           List[37]=1
        if ButLi[4][9]==2:
           List[17]=1
           List[38]=1
        if ButLi[4][9]==3:
           List[18]=1
           List[39]=1
        if ButLi[4][10]==1:
           List[16]=1
           List[40]=1
        if ButLi[4][10]==2:
           List[17]=1
           List[41]=1
        if ButLi[4][10]==3:
           List[18]=1
           List[42]=1
        if ButLi[4][11]==1:
           List[16]=1
           List[43]=1
        if ButLi[4][11]==2:
           List[17]=1
           List[44]=1
        if ButLi[4][11]==3:
           List[18]=1
           List[45]=1
        if ButLi[4][12]==1:
           List[16]=1
           List[46]=1
        if ButLi[4][12]==2:
           List[17]=1
           List[47]=1
        if ButLi[4][12]==3:
           List[18]=1
           List[48]=1
        if ButLi[5][9]==1:
           List[22]=1 
           LIst[37]=1
        if ButLi[5][9]==2:
           List[23]=1 
           LIst[38]=1
        if ButLi[5][9]==3:
           List[24]=1 
           LIst[39]=1
        if ButLi[5][10]==1:
           List[22]=1
           List[40]=1
        if ButLi[5][10]==2:
           List[23]=1
           List[41]=1
        if ButLi[5][10]==3:
           List[24]=1
           List[42]=1
        if ButLi[5][11]==1:
           List[22]=1
           List[43]=1
        if ButLi[5][11]==2:
           List[23]=1
           List[44]=1
        if ButLi[5][11]==3:
           List[24]=1
           List[45]=1
        if ButLi[5][12]==1:
           List[22]=1
           List[46]=1
        if ButLi[5][12]==2:
           List[23]=1
           List[47]=1
        if ButLi[5][12]==3:
           List[24]=1
           List[48]=1     
        if ButLi[6][9]==1:
           List[19]=1
           List[37]=1   
        if ButLi[6][9]==2:
           List[20]=1
           List[38]=1   
        if ButLi[6][9]==3:
           List[21]=1
           List[39]=1  
        if ButLi[6][10]==1:
           List[19]=1
           List[40]=1   
        if ButLi[6][10]==2:
           List[20]=1
           List[41]=1   
        if ButLi[6][10]==3:
           List[21]=1
           List[42]=1 
        if ButLi[6][11]==1:
           List[19]=1
           List[43]=1   
        if ButLi[6][11]==2:
           List[20]=1
           List[44]=1   
        if ButLi[6][11]==3:
           List[21]=1
           List[45]=1
        if ButLi[6][12]==1:
           List[19]=1
           List[46]=1   
        if ButLi[6][12]==2:
           List[20]=1
           List[47]=1   
        if ButLi[6][12]==3:
           List[21]=1
           List[48]=1      
        if ButLi[7][9]==1:
           List[25]=1
           List[37]=1   
        if ButLi[7][9]==2:
           List[26]=1
           List[38]=1   
        if ButLi[7][9]==3:
           List[27]=1
           List[39]=1  
        if ButLi[7][10]==1:
           List[15]=1
           List[40]=1   
        if ButLi[7][10]==2:
           List[26]=1
           List[41]=1   
        if ButLi[7][10]==3:
           List[27]=1
           List[42]=1 
        if ButLi[7][11]==1:
           List[25]=1
           List[43]=1   
        if ButLi[7][11]==2:
           List[26]=1
           List[44]=1   
        if ButLi[7][11]==3:
           List[27]=1
           List[45]=1
        if ButLi[7][12]==1:
           List[25]=1
           List[46]=1   
        if ButLi[7][12]==2:
           List[26]=1
           List[47]=1   
        if ButLi[7][12]==3:
           List[27]=1
           List[48]=1      
        if ButLi[8][9]==1:
           List[28]=1
           List[37]=1
        if ButLi[8][9]==2:
           List[29]=1
           List[38]=1
        if ButLi[4][9]==3:
           List[30]=1
           List[39]=1
        if ButLi[8][10]==1:
           List[28]=1
           List[40]=1
        if ButLi[8][10]==2:
           List[29]=1
           List[41]=1
        if ButLi[8][10]==3:
           List[30]=1
           List[42]=1
        if ButLi[8][11]==1:
           List[28]=1
           List[43]=1
        if ButLi[8][11]==2:
           List[29]=1
           List[44]=1
        if ButLi[8][11]==3:
           List[30]=1
           List[45]=1
        if ButLi[8][12]==1:
           List[28]=1
           List[46]=1
        if ButLi[8][12]==2:
           List[29]=1
           List[47]=1
        if ButLi[8][12]==3:
           List[30]=1
           List[48]=1              
        if ButLi[9][10]==1:
           List[49]=1
        if ButLi[10][9]==1:
           List[50]=1        
        if ButLi[10][11]==1:
           List[51]=1
        if ButLi[11][10]==1:
           List[52]=1
        if ButLi[11][12]==1:
           List[53]=1
        if ButLi[12][11]==1:
           List[54]=1
        if ButLi[9][11]==1:
           List[56]=1
        if ButLi[11][9]==1:
           List[55]=1 
        if ButLi[9][1]==1:
           List[57]=1
           List[63]=1
        if ButLi[9][2]==1:
           List[57]=1
           List[62]=1
        if ButLi[9][3]==1:
           List[57]=1
           List[61]=1
        if ButLi[9][4]==1:
           List[57]=1
           List[66]=1
        if ButLi[9][8]==1:
           List[57]=1
           List[64]=1
        if ButLi[9][13]==1:
           List[57]=1
           List[65]=1
        if ButLi[10][1]==1:
           List[58]=1
           List[63]=1
        if ButLi[10][2]==1:
           List[58]=1
           List[62]=1
        if ButLi[10][3]==1:
           List[58]=1
           List[61]=1
        if ButLi[10][4]==1:
           List[58]=1
           List[66]=1
        if ButLi[10][8]==1:
           List[58]=1
           List[64]=1
        if ButLi[10][13]==1:
           List[58]=1
           List[65]=1           
        if ButLi[11][1]==1:
           List[59]=1
           List[63]=1
        if ButLi[11][2]==1:
           List[59]=1
           List[62]=1
        if ButLi[11][3]==1:
           List[59]=1
           List[61]=1
        if ButLi[11][4]==1:
           List[59]=1
           List[66]=1
        if ButLi[11][8]==1:
           List[59]=1
           List[64]=1
        if ButLi[11][13]==1:
           List[59]=1
           List[65]=1             
        if ButLi[12][1]==1:
           List[60]=1
           List[63]=1
        if ButLi[12][2]==1:
           List[60]=1
           List[62]=1
        if ButLi[12][3]==1:
           List[60]=1
           List[61]=1
        if ButLi[12][4]==1:
           List[60]=1
           List[66]=1
        if ButLi[12][8]==1:
           List[60]=1
           List[64]=1
        if ButLi[12][13]==1:
           List[60]=1
           List[65]=1   

           BIU2P1=[x1BIU,YB2S0,X1MRF+15,YB2S0,X1MRF+15,y1+20,x1,y1]
           self.paintLine(4,BIU2P1)
           self.paintLine(0,BIU2P1)
        
