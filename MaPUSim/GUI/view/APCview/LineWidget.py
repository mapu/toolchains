#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import math

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class LineWidget(QDialog):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget,self).__init__(parent)
	self.length=l
	self.width=w
	self.height=h
	self.angle=a
	self.flag=f
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
	if 1:
	    if self.flag==True:
		if self.angle>=0 and self.angle<=90 :
	            qp.drawLine(4,7,self.width,self.height)
		elif self.angle>90 and self.angle<=180 :
		    qp.drawLine(self.width,4,4,self.height)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	    else:
		if self.angle>=0 and self.angle<=90 :
	            qp.drawLine(4,12,self.width,self.height+5)
		elif self.angle>90 and self.angle<=180 :
		    qp.drawLine(self.width,9,9,self.height+5)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	
	    if self.angle==0:
		if self.flag==True:
		    qp.rotate(self.angle)
	            qp.drawText(self.length-17,12,">")
		else:
		    qp.rotate(self.angle)
	            qp.drawText(4,17,"<")

	    elif self.angle==90:
		if self.flag==True:
		    qp.rotate(self.angle)
	            qp.drawText(self.length-5,0,">")
		else:
		    qp.rotate(self.angle)
	            qp.drawText(5,0,"<")				
	    else:
	        if self.flag==True:
		    if self.angle>0 and self.angle<90 :
			qp.rotate(self.angle-49)
	            	qp.drawText(self.length-18,7,">")
		    elif self.angle>90 and self.angle<=180 :
			qp.translate(self.width,4)
			qp.rotate(self.angle+47)
	                qp.drawText(self.length-13,7,">")
			qp.translate(0,0)
	        else:
		    if self.angle>0 and self.angle<90 :
		        qp.rotate(self.angle-20)
	                qp.drawText(6,9,"<")
		    elif self.angle>90 and self.angle<=180 :
			qp.translate(self.width-9,8)
			qp.rotate(self.angle+47)
	                qp.drawText(0,0,"<")
			qp.translate(0,0)

	else:
	    qp.drawLine(0,4,self.length,4)
	    qp.drawText(self.length-4,8,">")
	qp.end()


class LineWidget21(QDialog):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget21,self).__init__(parent)
	self.length=l
	self.width=w
	self.height=h
	self.angle=a
	self.flag=f
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
	if 1:
	    if self.flag==True:
		if self.angle>=0 and self.angle<=90 :
	            qp.drawLine(4,7,self.width,self.height)
		elif self.angle>90 and self.angle<=180 :
		    qp.drawLine(self.width,4,4,self.height)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	    else:
		if self.angle>=0 and self.angle<=90 :
	            qp.drawLine(4,12,self.width,self.height+5)
		elif self.angle>90 and self.angle<=180 :
		    qp.drawLine(self.width,14,14,self.height+8)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	
	    if self.angle==0:
		if self.flag==True:
		    qp.rotate(self.angle)
	            qp.drawText(self.length-17,12,">")
		else:
		    qp.rotate(self.angle)
	            qp.drawText(4,17,"<")

	    elif self.angle==90:
		if self.flag==True:
		    qp.rotate(self.angle)
	            qp.drawText(self.length-5,0,">")
		else:
		    qp.rotate(self.angle)
	            qp.drawText(5,0,"<")				
	    else:
	        if self.flag==True:
		    if self.angle>0 and self.angle<90 :
			qp.rotate(self.angle-49)
	            	qp.drawText(self.length-18,7,">")
		    elif self.angle>90 and self.angle<=180 :
			qp.translate(self.width,4)
			qp.rotate(self.angle+47)
	                qp.drawText(self.length-13,7,">")
			qp.translate(0,0)
	        else:
		    if self.angle>0 and self.angle<90 :
		        qp.rotate(self.angle-20)
	                qp.drawText(6,9,"<")
		    elif self.angle>90 and self.angle<=180 :
			qp.translate(self.width-11,13)
			qp.rotate(self.angle+35)
	                qp.drawText(0,0,"<")
			qp.translate(0,0)

	else:
	    qp.drawLine(0,4,self.length,4)
	    qp.drawText(self.length-4,8,">")
	qp.end()

