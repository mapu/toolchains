#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
import sys

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class TableModel(QAbstractTableModel):
    def __init__(self, arraydata, parent=None,):
        QAbstractTableModel.__init__(self, parent,)
	self.horizontalHeaderList=[]
	self.verticalHeaderList=[]
        self.arraydata=arraydata

    def setDataBase(self,dataBase,APEdbFilePath,flag):
	self.dataBase=dataBase
	self.APEdbFilePath=APEdbFilePath
	self.flag=flag

    def setModalDatas(self,arraydata):
         self.arraydata=arraydata

    def setHorizontalHeader(self,horizontalHeaderList):
	self.horizontalHeaderList=horizontalHeaderList

    def setVerticalHeader(self,verticalHeaderList):
        self.verticalHeaderList=verticalHeaderList

    def rowCount(self, parent):
        return len(self.arraydata)

    def columnCount(self, parent):
        return len(self.arraydata[0])

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
        elif role == Qt.DisplayRole:
            return QVariant(self.arraydata[index.row()][index.column()])
        elif role == Qt.BackgroundRole:
	    if index.data().toString()!=" ":
		header=self.verticalHeaderList[index.row()]
		pos=header.find(":")
		header=header[:pos]
		read=0
		write=0
		fetchall_sql = "SELECT * FROM "+self.dataBase.regTableName+" WHERE time = "+str(index.column())+" and spumpu = "+"'"+self.flag+"'"+" and sn = "+header
	        r=self.dataBase.fetchall(self.APEdbFilePath,fetchall_sql)
	        if r!=0:
	    	    for e in range(len(r)):
		        stringList=r[e]
		        if stringList[6]!="Misc Reg":
		    	    if stringList[5]=="W":
				write=1
		    	    elif stringList[5]=="R":
				read=1
		    if read==1 and write==1:
			return QColor(255,153,18)
		    elif read==1:
			return QColor(0,255,0)
		    elif write==1:
			return QColor(255,0,0)
		    elif read==0 and write==0:
		        text=str(index.data().toString())
                        if int(text)<0:
                            return QColor("gray");
                        else:
                            return QColor(193,210,255);  		
		else:
		    text=str(index.data().toString())
                    if int(text)<0:
                        return QColor("gray");
                    else:
                        return QColor(193,210,255);   

    def headerData(self,section,orientation,role):
        if role==Qt.DisplayRole:
            if orientation==Qt.Horizontal:
                if len(self.horizontalHeaderList)>section:
                    return self.horizontalHeaderList[section]
                else:
                    return QVariant() 
            else:
                if len(self.verticalHeaderList)>section:
                    return self.verticalHeaderList[section]
                else:
                    return QVariant()
    	return QVariant()

    def refrushModel(self):
        self.beginResetModel()
        self.endResetModel()









