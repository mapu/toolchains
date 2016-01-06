#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from SearchDialog import*
from OptionsDialog import*
from QHexEdit import*
import res.qrc_resources 

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class HexMainWindow(QMainWindow):
    def __init__(self,parent=None):
	super(HexMainWindow,self).__init__(parent)
    	self.setAcceptDrops(True)
	self.file=QFile()
    	self.init()
    	self.setCurrentFile("QHexEdit")

    def dragEnterEvent(self,event):
    	if event.mimeData().hasUrls():
            event.accept()
            string=event.mimeData().urls().at(0).toLocalFile()
            self.statusBar().showMessage( self.tr("Drop File: ")+string,2000)

    def dropEvent(self,event):
    	if event.mimeData().hasUrls():
            string=event.mimeData().urls().at(0).toLocalFile()
            self.loadFile(string,self.start)
            event.accept()

    def dataChanged(self):
    	self.setWindowModified(self.hexEdit.isModified())

    def openFile(self):
      	fileName = QFileDialog.getOpenFileName(self)
    	if not fileName.isEmpty():
            self.loadFile(fileName,self.start)

    def saveAs(self):
   	fileName = QFileDialog.getSaveFileName(self, self.tr("Save As"),self.curFile)
    	if fileName.isEmpty():
            return False
    	return self.saveFile(fileName)

    def setAddress(self,address):
    	self.lbAddress.setText(QString("%1").arg(address, 1, 16))

    def setSize(self,size):
        self.lbSize.setText(QString("%1").arg(size))

    def showSearchDialog(self):
    	self.searchDialog.show()

    def init(self):
    	self.setAttribute(Qt.WA_DeleteOnClose)
    	self.optionsDialog=OptionsDialog()
    	self.isUntitled=True
    	self.hexEdit=QHexEdit()
    	self.setCentralWidget(self.hexEdit)
    	self.searchDialog=SearchDialog(self.hexEdit,self)
    	self.createActions()
    	self.createToolBars()
    	self.createStatusBar()

    	self.move(725,200)                                 
	self.resize(610,452)
    	self.hexEdit.setAddressArea(True)
    	self.hexEdit.setAsciiArea(True)
    	self.hexEdit.setHighlighting(True)

    	self.hexEdit.setHighlightingColor(QColor(0xff, 0xff, 0x99, 0xff))
    	self.hexEdit.setAddressAreaColor(self.palette().alternateBase().color())
    	self.hexEdit.setSelectionColor(self.palette().highlight().color())
    	self.hexEdit.setFont(QFont("Monospace", 10))
    	self.hexEdit.setAddressWidth(4)

    	self.setUnifiedTitleAndToolBarOnMac(True)
    	self.connect(self.hexEdit, SIGNAL("dataChanged()"),self.dataChanged)

    def createActions(self):
    	self.openAct=QAction(QIcon(":/open.png"),self.tr("&Open..."),self)
    	self.openAct.setShortcuts(QKeySequence.Open)
    	self.openAct.setStatusTip(self.tr("Open an existing file"))
    	self.connect(self.openAct,SIGNAL("triggered()"),self.openFile)

    	self.findAct=QAction(QIcon(":/find.png"),self.tr("&Find"),self)
    	self.findAct.setShortcuts(QKeySequence.Find)
    	self.findAct.setStatusTip(self.tr("Show the Dialog for finding and replacing"))
    	self.connect(self.findAct,SIGNAL("triggered()"),self.showSearchDialog)

    def createStatusBar(self):
    	#Address Label
    	self.lbAddressName=QLabel()
    	self.lbAddressName.setText(self.tr("Address:"))
    	self.statusBar().addPermanentWidget(self.lbAddressName)
    	self.lbAddress=QLabel("0")
    	self.lbAddress.setFrameShape(QFrame.Panel)
   	self.lbAddress.setFrameShadow(QFrame.Sunken)
    	self.lbAddress.setMinimumWidth(70)
    	self.statusBar().addPermanentWidget(self.lbAddress)
    	self.connect(self.hexEdit, SIGNAL("currentAddressChanged(int)"), self.setAddress)

    	# Size Label
    	self.lbSizeName=QLabel()
    	self.lbSizeName.setText(self.tr("Size:"))
    	self.statusBar().addPermanentWidget(self.lbSizeName)
    	self.lbSize=QLabel("0")
    	self.lbSize.setFrameShape(QFrame.Panel)
    	self.lbSize.setFrameShadow(QFrame.Sunken)
    	self.lbSize.setMinimumWidth(70)
    	self.statusBar().addPermanentWidget(self.lbSize)
    	self.connect(self.hexEdit,SIGNAL("currentSizeChanged(int)"),self.setSize)
	
    	self.statusBar().showMessage(self.tr("Ready"), 2000)

    def createToolBars(self):
    	self.fileToolBar=self.addToolBar(self.tr("File"))
    	self.fileToolBar.addAction(self.openAct)
    	self.editToolBar=self.addToolBar(self.tr("Edit"))
    	self.editToolBar.addAction(self.findAct)

    def loadFile(self,fileName,start):
    	self.file.setFileName(fileName)
    	if not self.hexEdit.setData(self.file,start):
            QMessageBox.warning(self,self.tr("QHexEdit"),self.tr("Cannot read file %1:\n%2.").arg(fileName).arg(self.file.errorString()))
            return
    	self.setCurrentFile(fileName)
    	self.statusBar().showMessage(self.tr("File loaded"), 2000)

    def setCurrentFile(self,fileName):
    	self.curFile = QFileInfo(fileName).canonicalFilePath()
	if fileName=="":
    	    self.isUntitled = True
	else:
    	    self.isUntitled = False	
    	self.setWindowModified(False)
    	if fileName=="":
            self.setWindowTitle("QHexEdit")
    	else:
            self.setWindowTitle(fileName + " - QHexEdit")

    def strippedName(self,fullFileName):
    	return QFileInfo(fullFileName).fileName()

    def closeEvent(self,event):
	self.hexEdit._cursorTimer.stop()
	self.optionsDialog.close()
	self.searchDialog.close()
	self.flag=-1



