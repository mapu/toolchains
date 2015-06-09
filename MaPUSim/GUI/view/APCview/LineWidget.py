#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

#lineType:
    #HorizonFarward=0
    #HorizonReserve=1
    #VerticalForward=2
    #VerticalReserve=3
    #HorizonFarwarddown=4
    #HorizonReservedown=5
    #HorizonFarwardup=6
    #HorizonReserveup=7
    #IALU-FALU=8,9

class LineWidget(QWidget):
    def __init__(self,x1,y1,x2,y2,lineType,l1=0,l2=0,parent=None):
	super(LineWidget,self).__init__(parent)

	self.dx1=x1
	self.dy1=y1
	self.dx2=x2
	self.dy2=y2
	self.lineType=lineType
	self.dl1=l1
	self.dl2=l2
	self.setWindowFlags(Qt.FramelessWindowHint)
	self.setAttribute(Qt.WA_TranslucentBackground)
	#self.setStyleSheet("background-color: rgb(170, 0, 255);");

	#redefine paintEvent()
    def paintEvent(self,event):
	qp=QPainter()
	qp.begin(self)
	pen=QPen(Qt.black,1,Qt.SolidLine)
	qp.setPen(pen)
	qp.translate(0,0)
	if self.lineType==0:
	    qp.drawLine(self.dx1,self.dy1,self.dx1+self.dl1,self.dy1)
	    qp.drawLine(self.dx1+self.dl1,self.dy1,self.dx1+self.dl2,self.dy2)
	    qp.drawLine(self.dx1+self.dl2,self.dy2,self.dx2,self.dy2)
	    qp.drawText(self.dx2-7,self.dy2+5,">")
	elif self.lineType==1:
	    qp.drawLine(self.dx1,self.dy1,self.dx1+self.dl1,self.dy1)
	    qp.drawLine(self.dx1+self.dl1,self.dy1,self.dx1+self.dl2,self.dy2)
	    qp.drawLine(self.dx1+self.dl2,self.dy2,self.dx2,self.dy2)
	    qp.drawText(self.dx1,self.dy1+5,"<")	
	elif self.lineType==2:
	    qp.drawLine(self.dx1,self.dy1,self.dx2,self.dy2)
	    qp.translate(self.dx1-4,self.dy1)
	    qp.rotate(90)
	    qp.drawText(self.dy2-13,0,">")
	elif self.lineType==3:
	    qp.drawLine(self.dx1,self.dy1,self.dx2,self.dy2)
	    qp.translate(self.dx1-4,self.dy1)
	    qp.rotate(90)
	    qp.drawText(0,0,"<")
	elif self.lineType==4:
	    l0=(self.dx2-2*self.dl1)/2
	    if self.dy1 > self.dy2:
	    	h0=(self.dy1-self.dy2)-2*l0
	    else:
		h0=(self.dy2-self.dy1)-2*l0
	    qp.drawLine(self.dx1,self.dy1,self.dx1+self.dl1,self.dy1)
	    qp.drawLine(self.dx1+self.dl1,self.dy1,self.dx1+self.dl1+l0,self.dy1+l0)
	    qp.drawLine(self.dx1+self.dl1+l0,self.dy1+l0,self.dx1+self.dl1+l0,self.dy1+l0+h0)
	    qp.drawLine(self.dx1+self.dl1+l0,self.dy1+l0+h0,self.dx1+self.dl1+2*l0,self.dy1+2*l0+h0)
	    qp.drawLine(self.dx1+self.dl1+2*l0,self.dy1+2*l0+h0,self.dx2,self.dy2)
	    qp.drawText(self.dx2-7,self.dy2+5,">")
	elif self.lineType==5:
	    l0=(self.dx2-2*self.dl1)/2
	    if self.dy1 > self.dy2:
	    	h0=(self.dy1-self.dy2)-2*l0
	    else:
		h0=(self.dy2-self.dy1)-2*l0
	    qp.drawLine(self.dx1,self.dy1,self.dx1+self.dl1,self.dy1)
	    qp.drawLine(self.dx1+self.dl1,self.dy1,self.dx1+self.dl1+l0-3,self.dy1+l0-3)
	    qp.drawLine(self.dx1+self.dl1+l0-3,self.dy1+l0-3,self.dx1+self.dl1+l0-3,self.dy1+l0+h0-3)
	    qp.drawLine(self.dx1+self.dl1+l0-3,self.dy1+l0+h0-3,self.dx1+self.dl1+2*l0,self.dy1+2*l0+h0)
	    qp.drawLine(self.dx1+self.dl1+2*l0,self.dy1+2*l0+h0,self.dx2,self.dy2)
	    qp.drawText(self.dx1,self.dy1+5,"<")
	elif self.lineType==6:
	    l0=(self.dx2-2*self.dl1)/2
	    if self.dy1 > self.dy2:
	    	h0=(self.dy1-self.dy2)-2*l0
	    else:
		h0=(self.dy2-self.dy1)-2*l0
	    qp.drawLine(self.dx1,self.dy1,self.dx1+self.dl1,self.dy1)
	    qp.drawLine(self.dx1+self.dl1,self.dy1,self.dx1+self.dl1+l0,self.dy1-l0)
	    qp.drawLine(self.dx1+self.dl1+l0,self.dy1-l0,self.dx1+self.dl1+l0,self.dy1-l0-h0)
	    qp.drawLine(self.dx1+self.dl1+l0,self.dy1-l0-h0,self.dx1+self.dl1+2*l0,self.dy1-2*l0-h0)
	    qp.drawLine(self.dx1+self.dl1+2*l0,self.dy1-2*l0-h0,self.dx2,self.dy2)
	    qp.drawText(self.dx2-7,self.dy2+5,">")
	elif self.lineType==7:
	    l0=(self.dx2-2*self.dl1)/2
	    if self.dy1 > self.dy2:
	    	h0=(self.dy1-self.dy2)-2*l0
	    else:
		h0=(self.dy2-self.dy1)-2*l0
	    qp.drawLine(self.dx1,self.dy1,self.dx1+self.dl1,self.dy1)
	    qp.drawLine(self.dx1+self.dl1,self.dy1,self.dx1+self.dl1+l0+3,self.dy1-l0-3)
	    qp.drawLine(self.dx1+self.dl1+l0+3,self.dy1-l0-3,self.dx1+self.dl1+l0+3,self.dy1-l0-h0-3)
	    qp.drawLine(self.dx1+self.dl1+l0+3,self.dy1-l0-h0-3,self.dx1+self.dl1+2*l0,self.dy1-2*l0-h0)
	    qp.drawLine(self.dx1+self.dl1+2*l0,self.dy1-2*l0-h0,self.dx2,self.dy2)
	    qp.drawText(self.dx1,self.dy1+5,"<")
	elif self.lineType==8:
	    qp.drawLine(self.dx1,self.dy1,self.dx1+10,self.dy1)
	    qp.drawLine(self.dx1+10,self.dy1,self.dx1+20,self.dy1+10)
	    qp.drawLine(self.dx1+20,self.dy1+10,self.dx1+20,self.dy2-10)
	    qp.drawLine(self.dx1+20,self.dy2-10,self.dx1+10,self.dy2)
	    qp.drawLine(self.dx1+10,self.dy2,self.dx2,self.dy2)
	    qp.drawText(self.dx2,self.dy2+5,"<")
	elif self.lineType==9:
	    qp.drawLine(self.dx1,self.dy1,self.dx1+10,self.dy1)
	    qp.drawLine(self.dx1+10,self.dy1,self.dx1+20+5,self.dy1+10+5)
	    qp.drawLine(self.dx1+20+5,self.dy1+10+5,self.dx1+20+5,self.dy2-10-5)
	    qp.drawLine(self.dx1+20+5,self.dy2-10-5,self.dx1+10,self.dy2)
	    qp.drawLine(self.dx1+10,self.dy2,self.dx2,self.dy2)
	    qp.drawText(self.dx1,self.dy1+5,"<")
	
	qp.end()

