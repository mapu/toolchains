#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import math

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class LineWidget(QWidget):
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,4,4,self.height)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,9,9,self.height+5)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2

	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-40.5)
	        qp.drawText(self.length-18,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,4)
		qp.rotate(self.angle+39.5)
	        qp.drawText(self.length-13,7,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-20)
	        qp.drawText(9,10,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-7,8)
	        qp.rotate(self.angle+47)
	        qp.drawText(0,0,"<")
		qp.translate(0,0)
	qp.end()

#0 rows 2 columns
class LineWidget02(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget02,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,4,4,self.height)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,9,9,self.height+5)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2

	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-29)
	        qp.drawText(self.length-18,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,4)
		qp.rotate(self.angle+39.5)
	        qp.drawText(self.length-13,7,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-20)
	        qp.drawText(9,13,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-7,8)
	        qp.rotate(self.angle+47)
	        qp.drawText(0,0,"<")
		qp.translate(0,0)
	qp.end()

#4 rows 2 columns
class LineWidget42(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget42,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,4,4,self.height)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,9,9,self.height+5)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-45)
	        qp.drawText(self.length-18,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,4)
		qp.rotate(self.angle+43.5)
	        qp.drawText(self.length-13,7,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-28)
	        qp.drawText(9,9,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-7,10)
	        qp.rotate(self.angle+47)
	        qp.drawText(2,0,"<")
		qp.translate(0,0)
	qp.end()

#6 rows 2 columns
class LineWidget62(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget62,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width+10,4,4,self.height-5)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,19,19,self.height-5)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-45)
	        qp.drawText(self.length-18,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width+10,4)
		qp.rotate(self.angle+1)
	        qp.drawText(self.length-10,7,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-28)
	        qp.drawText(9,9,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,19)
	        qp.rotate(self.angle+8)
	        qp.drawText(10,1,"<")
		qp.translate(0,0)
	qp.end()

#7 rows 2 columns
class LineWidget72(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget72,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(15,0,self.width+5,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width+10,4,4,self.height+10)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(7,0,self.width+5,self.height+5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,24,13,self.height+15)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle+19.5)
	        qp.drawText(self.length-26,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width+10,4)
		qp.rotate(self.angle-22.4)
	        qp.drawText(self.length+2,7,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle+10)
	        qp.drawText(4,2,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,24)
	        qp.rotate(self.angle-8)
	        qp.drawText(10,0,"<")
		qp.translate(0,0)
	qp.end()

#8 rows 2 columns
class LineWidget82(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget82,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,20,self.width,self.height+20)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,4,4,self.height)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,15,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(4,12,self.width,self.height+12)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,9,9,self.height+5)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2

	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle+40.3)
	        qp.drawText(self.length+87,12,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,4)
		qp.rotate(self.angle+39.5)
	        qp.drawText(self.length-13,7,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle+27)
	        qp.drawText(9,12,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-7,8)
	        qp.rotate(self.angle+47)
	        qp.drawText(0,0,"<")
		qp.translate(0,0)
	qp.end()

#9 rows 2 columns
class LineWidget92(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget92,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(15,0,self.width+5,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,0,4,self.height+10)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(7,0,self.width+5,self.height+5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,10,10,self.height+15)   #30,15 BUTTON WIDTH  10 BUTTON HEIGHT/2
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle+19.5)
	        qp.drawText(self.length-26,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,0)
		qp.rotate(self.angle-50.3)
	        qp.drawText(self.length,0,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle+10)
	        qp.drawText(4,2,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,10)
	        qp.rotate(self.angle-32)
	        qp.drawText(15,-4,"<")
		qp.translate(0,0)
	qp.end()

#10 rows 2 columns
class LineWidget102(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget102,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,20,self.width+5,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,10,4,self.height+20)   
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,30,self.width+5,self.height+10)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,0,15,self.height-10)   
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-12.5)
	        qp.drawText(self.length-30,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,0)
		qp.rotate(self.angle+9.6)
	        qp.drawText(self.length+5,0,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-2)
	        qp.drawText(27,14,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,0)
	        qp.rotate(self.angle)
	        qp.drawText(14,8,"<")
		qp.translate(0,0)
	qp.end()


#1 rows 5 columns
class LineWidget15(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget15,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,20,self.width+5,self.height)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,10,27,self.height+16)   
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,30,self.width+5,self.height+10)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width-25,5,4,self.height+15)   
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-12.5)
	        qp.drawText(self.length-30,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,0)
		qp.rotate(self.angle+5.3)
	        qp.drawText(self.length-35,0,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-2)
	        qp.drawText(27,14,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-25,0)
	        qp.rotate(self.angle-2)
	        qp.drawText(0,0,"<")
		qp.translate(0,0)
	qp.end()

#0 rows 5 columns
class LineWidget05(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget05,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,0,self.width-20,self.height-20)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,10,27,self.height+16)   
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(5,6,self.width-20,self.height-15)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width-25,5,4,self.height+15)   
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-12.5)
	        qp.drawText(self.length-30,7,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,0)
		qp.rotate(self.angle+5.3)
	        qp.drawText(self.length-35,0,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-2)
	        qp.drawText(5,8,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-25,0)
	        qp.rotate(self.angle-2)
	        qp.drawText(0,0,"<")
		qp.translate(0,0)
	qp.end()

#3 rows 5 columns
class LineWidget35(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget35,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,0,self.width,self.height-10)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,10,27,self.height+16)   
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,5,self.width,self.height-5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width-25,5,4,self.height+15)   
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-26.3)
	        qp.drawText(self.length-20,0,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,0)
		qp.rotate(self.angle+5.3)
	        qp.drawText(self.length-35,0,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-20)
	        qp.drawText(0,9,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-25,0)
	        qp.rotate(self.angle-2)
	        qp.drawText(0,0,"<")
		qp.translate(0,0)
	qp.end()

#4 rows 5 columns
class LineWidget45(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidget45,self).__init__(parent)
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
	if self.flag==True:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,7,self.width,self.height)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,0,self.width,self.height-10)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width,10,27,self.height+16)   
	else:
	    if self.angle==0 or self.angle==90:
		qp.drawLine(4,12,self.width,self.height+5)
	    elif self.angle>0 and self.angle<90 :
	        qp.drawLine(0,5,self.width,self.height-5)
	    elif self.angle>90 and self.angle<=180 :
		qp.drawLine(self.width-15,5,4,self.height+15)   
	
	if self.flag==True:
	    if self.angle==0:
		qp.rotate(self.angle)
	        qp.drawText(self.length-17,12,">")
	    elif self.angle==90:
	        qp.rotate(self.angle)
	        qp.drawText(self.length-5,0,">")		
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-26.3)
	        qp.drawText(self.length-20,0,">")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width,10)
		qp.rotate(self.angle+30)
	        qp.drawText(self.length-30,0,">")
		qp.translate(0,0)
	else:
	    if self.angle==0:	    
		qp.rotate(self.angle)
	        qp.drawText(4,17,"<")
	    elif self.angle==90:
		qp.rotate(self.angle)
	        qp.drawText(5,0,"<")				
	    elif self.angle>0 and self.angle<90 :
		qp.rotate(self.angle-20)
	        qp.drawText(0,9,"<")
	    elif self.angle>90 and self.angle<=180 :
		qp.translate(self.width-11,0)
	        qp.rotate(self.angle+25)
	        qp.drawText(0,0,"<")
		qp.translate(0,0)
	qp.end()

#7 rows 1 columns  zhe xian
class LineWidgetB71(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetB71,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(0,20,10,20)
	    qp.drawLine(10,20,10,self.height-20)
	    qp.drawLine(10,self.height-20,0,self.height-20)
	    qp.drawText(0,self.height-15,"<")
	else:
	    qp.drawLine(0,10,20,10)
	    qp.drawLine(20,10,20,self.height-10)
	    qp.drawLine(20,self.height-10,0,self.height-10)
	    qp.drawText(0,15,"<")

	qp.end()

#5 rows 5 columns  zhe xian i
class LineWidgetBi55(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBi55,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(0,5,self.width-28,5)
	else:
	    qp.drawLine(0,10,self.width-33,10)
	    qp.drawText(0,15,"<")

	qp.end()

#5 rows 5 columns  zhe xian j
class LineWidgetBj55(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBj55,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(50,5,50,self.height-20)
	    qp.drawLine(50,self.height-20,60,self.height-20)
	    qp.drawText(57,self.height-15,">")
	else:
	    qp.drawLine(45,10,45,self.height-15)
	    qp.drawLine(45,self.height-15,60,self.height-15)

	qp.end()

#8 rows 5 columns  zhe xian i
class LineWidgetBi85(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBi85,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(0,15,self.width-38,15)
	else:
	    qp.drawLine(0,20,self.width-43,20)
	    qp.drawText(0,25,"<")

	qp.end()

#8 rows 5 columns  zhe xian j
class LineWidgetBj85(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBj85,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(40,15,40,self.height-20)
	    qp.drawLine(40,self.height-20,60,self.height-20)
	    qp.drawText(57,self.height-15,">")
	else:
	    qp.drawLine(35,20,35,self.height-15)
	    qp.drawLine(35,self.height-15,60,self.height-15)

	qp.end()

#9 rows 5 columns  zhe xian i
class LineWidgetBi95(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBi95,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(0,self.height-20,10,self.height-20)
	    qp.drawLine(10,self.height-20,10,10)
	else:
	    qp.drawLine(0,self.height-15,15,self.height-15)
	    qp.drawLine(15,self.height-15,15,15)
	    qp.drawText(0,self.height-10,"<")

	qp.end()

#9 rows 5 columns  zhe xian j
class LineWidgetBj95(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBj95,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(10,10,self.width,10)
	    qp.drawText(self.width-18,15,">")
	else:
	    qp.drawLine(15,15,self.width,15)

	qp.end()

#6 rows 5 columns  zhe xian i
class LineWidgetBi65(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBi65,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(0,self.height-10,20,self.height-10)
	    qp.drawLine(20,self.height-10,20,10)
	else:
	    qp.drawLine(0,self.height-5,25,self.height-5)
	    qp.drawLine(25,self.height-5,25,15)
	    qp.drawText(0,self.height,"<")

	qp.end()

#6 rows 5 columns  zhe xian j
class LineWidgetBj65(QWidget):
    def __init__(self,l,w,h,a,f,parent=None):
	super(LineWidgetBj65,self).__init__(parent)
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
	if self.flag==True:
	    qp.drawLine(20,10,self.width,10)
	    qp.drawText(self.width-18,15,">")
	else:
	    qp.drawLine(25,15,self.width,15)

	qp.end()

