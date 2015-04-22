# -*- coding: utf-8 -*-   
from PyQt4.QtCore import *  
from PyQt4.QtGui import *  
import sys  
from apcsource import apcdialog  
QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))  
  

           
app=QApplication(sys.argv)  
main=apcdialog()  
main.show()  
app.exec_()  
