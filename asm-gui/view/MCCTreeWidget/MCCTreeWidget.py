# -*- coding: utf-8 -*- 
from PyQt4.QtGui import QTreeWidget, QTreeWidgetItem
from PyQt4.QtCore import Qt, SIGNAL, pyqtSignal, pyqtSlot, QStringList, QXmlStreamReader, QFile, QIODevice, QString, qDebug
import re

class MCCTreeWidget(QTreeWidget):
    floatDialogShowSignal = pyqtSignal(int, int, list)
    def __init__(self, parent = None):
	super(MCCTreeWidget, self).__init__(parent)
	
	self.setColumnCount(1)
	self.setHeaderLabel("MCC Tree")
	self.xmlfile = QFile("config.xml")
	self.regList = ["M", "SHU", "BIU", "IALU", "FALU", "IMAC", "FMAC", "MSEQ"]
	self.topLevelItemList = []
	self.readXML()
	
    def readXML(self):
        '''
        Initialize tree from XML file
        '''
	if self.xmlfile.open(QIODevice.ReadOnly | QIODevice.Text) == False:
	    return
        xmlReader = QXmlStreamReader(self.xmlfile)
        if xmlReader.readNext() != QXmlStreamReader.StartDocument:
            qDebug("XML file has a broken header!")
            return
        if (xmlReader.readNext() != QXmlStreamReader.StartElement or
            xmlReader.name() != "MCCTree"):
            return
	for reg in self.regList:
            xmlReader.readNext()
            xmlReader.readNext()
            if str(xmlReader.name()) == reg:
	        ppitem = QTreeWidgetItem(QStringList() << reg)
	        self.addTopLevelItem(ppitem)	
	        self.topLevelItemList.append(ppitem)
	    else:
                qDebug("XML file has a broken reg beginning!")
                return		
            xmlReader.readNext()  
	    while not xmlReader.atEnd():
		if xmlReader.readNext() == QXmlStreamReader.StartElement:
		    element_name = xmlReader.name()
		    if xmlReader.readNext() == QXmlStreamReader.Characters:
			pitem = QTreeWidgetItem(QStringList() << str(element_name))
			ppitem.addChild(pitem)
			text = xmlReader.text().toString()
			textList = text.split(";")
			itemList = []
			for i in textList:
			    item = QTreeWidgetItem(QStringList() << i)
			    itemList.append(item)
			pitem.addChildren(itemList)
			if (xmlReader.readNext() != QXmlStreamReader.EndElement or
			    xmlReader.name() != element_name):
			    qDebug("XML file has broken element end!")
			    return
			else:
			    xmlReader.readNext()
		    elif xmlReader.tokenType() == QXmlStreamReader.EndElement:
			xmlReader.readNext()
		    else:
			qDebug("XML file has broken element content!")
			return
		elif (xmlReader.tokenType() == QXmlStreamReader.EndElement and
		      xmlReader.name() == reg):
		    break
		else:
		    qDebug("XML file has broken element beginning!")
		    return
        self.xmlfile.close()
        
    def searchMcc(self, row, column, text):
      	self.mark0 = [".", "*", "+", "^", "|", "(", ")", "[", "]"]
      	#self.mark1 = ["s", "m", "n", "p", "t"]
      	self.mark2 = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "0"]
      	self.mark3 = ["r", "R", "U", "u", "i", "I"]
      	#self.mark4 = [ "imm", "imm3", "imm4", "imm5", "imm10"]
	num = len(text)
	for k in range(num, 0,  -1):
	    if text[k - 1] in self.mark0:
	        text = text[: k - 1] + "\\" + text[k - 1: ]
	num = len(text)
	for i in range(num, 0, -1):
	    if text[i - 1] in self.mark2:
		if text[i - 2] not in self.mark3:
		    text = text[ : i - 1] + "\w+" + text[i : ]	
	text += ".*"
	text = "^" + text
	pattern = re.compile(str(text), re.IGNORECASE)
	self.result = []
	for ppitem in self.topLevelItemList:
	    pnum =  ppitem.childCount()
	    self.collapseItem(ppitem)
	    for i in range(pnum):
		pitem = ppitem.child(i)
		self.collapseItem(pitem)
		num = pitem.childCount()
		for j in range(num):
		    item = pitem.child(j)
		    string = item.text(0)
		    '''
		    textNum = len(string)
		    for k in range(textNum, 0, -1):
			if string[k - 1] in self.mark1:
			    string = string[ : k -1] + "0" + string[k : ]
			    print string
		    '''
		    #search
		    if pattern.search(string) != None: 
		        self.result.append(string)
			self.expandItem(ppitem)
		        self.expandItem(pitem)
	self.floatDialogShowSignal.emit(row, column, self.result)
	return self.result