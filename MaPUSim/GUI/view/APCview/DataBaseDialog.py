from PyQt4.QtGui import*
from PyQt4.QtCore import*
from Thread import*

class DataBaseDialog(QDialog):
    def __init__(self,parent=None):
	super(DataBaseDialog,self).__init__(parent)

	self.setFixedSize(300,100)
	self.setWindowTitle("Information")
	self.setAttribute (Qt.WA_DeleteOnClose);
	self.label=QLabel("Data base is building, please wait......")
	lay=QGridLayout()
	lay.addWidget(self.label,0,0)
	self.setLayout(lay) 
	#self.setModal(True)

    def createDataBase(self):
	thread=Thread()
	thread.start()
	eventLoop=QEventLoop()
	self.connect(thread,SIGNAL("finished()"),eventLoop.quit)
	eventLoop.exec_()
	self.close()
	self.dataBase=thread.dataBase
	return self.dataBase
  

