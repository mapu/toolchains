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
    	self.setCurrentFile("")
	self.flag=-1 #-1 close , 0 open

    def dragEnterEvent(self,event):
    	if event.mimeData().hasUrls():
            event.accept()
            string=event.mimeData().urls().at(0).toLocalFile()
            self.statusBar().showMessage( tr("Drop File: ")+string,2000)

    def dropEvent(self,event):
    	if event.mimeData().hasUrls():
            string=event.mimeData().urls().at(0).toLocalFile()
            self.loadFile(string)
            event.accept()

    def about(self):
   	QMessageBox.about(self, "About QHexEdit","The QHexEdit example is a short Demo of the QHexEdit Widget.")

    def dataChanged(self):
    	self.setWindowModified(self.hexEdit.isModified())

    def openFile(self):
      	fileName = QFileDialog.getOpenFileName(self)
    	if not fileName.isEmpty():
            self.loadFile(fileName)

    def findNext(self):
    	self.searchDialog.findNext()

    def save(self):
    	if self.isUntitled:
            return self.saveAs()
	else:
            return self.saveFile(self.curFile)

    def saveAs(self):
   	fileName = QFileDialog.getSaveFileName(self, self.tr("Save As"),self.curFile)
    	if fileName.isEmpty():
            return False
    	return self.saveFile(fileName)

    def saveSelectionToReadableFile(self):
    	fileName = QFileDialog.getSaveFileName(self, self.tr("Save To Readable File"))
    	if not fileName.isEmpty():
            filen=QFile(fileName)
            if not filen.open(QFile.WriteOnly or QFile.Text):
                QMessageBox.warning(self, self.tr("QHexEdit"),self.tr("Cannot write file %1:\n%2.").arg(fileName).arg(filen.errorString()))
                return
            QApplication.setOverrideCursor(Qt.WaitCursor)
            filen.write(self.hexEdit.selectionToReadableString().toLatin1())
            QApplication.restoreOverrideCursor()
            self.statusBar().showMessage(self.tr("File saved"), 2000)

    def saveToReadableFile(self):
    	fileName = QFileDialog.getSaveFileName(self, self.tr("Save To Readable File"))
    	if not fileName.isEmpty():
            filen=QFile(fileName)
            if not filen.open(QFile.WriteOnly or QFile.Text):
                QMessageBox.warning(self, self.tr("QHexEdit"),self.tr("Cannot write file %1:\n%2.").arg(fileName).arg(filen.errorString()))
                return
            QApplication.setOverrideCursor(Qt.WaitCursor)
            filen.write(self.hexEdit.selectionToReadableString().toLatin1())
            QApplication.restoreOverrideCursor()
            self.statusBar().showMessage(self.tr("File saved"), 2000)

    def setAddress(self,address):
    	self.lbAddress.setText(QString("%1").arg(address, 1, 16))

    def setSize(self,size):
        self.lbSize.setText(QString("%1").arg(size))

    def showOptionsDialog(self):
        self.optionsDialog.showWidget()

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
    	self.createMenus()
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

    	self.saveAct=QAction(QIcon(":/save.png"),self.tr("&Save"),self)
    	self.saveAct.setShortcuts(QKeySequence.Save)
    	self.saveAct.setStatusTip(self.tr("Save the document to disk"))
    	self.connect(self.saveAct,SIGNAL("triggered()"),self.save)

    	self.saveAsAct=QAction(self.tr("Save &As..."),self)
    	self.saveAsAct.setShortcuts(QKeySequence.SaveAs)
    	self.saveAsAct.setStatusTip(self.tr("Save the document under a new name"))
    	self.connect(self.saveAsAct,SIGNAL("triggered()"),self.saveAs)
    	#self.saveReadable=QAction(self.tr("Save &Readable..."),self)
    	#self.saveReadable.setStatusTip(self.tr("Save document in readable form"))
    	#self.connect(self.saveReadable,SIGNAL("triggered()"),self.saveToReadableFile)

    	self.exitAct=QAction(self.tr("E&xit"),self)
    	self.exitAct.setShortcuts(QKeySequence.Quit)
    	self.exitAct.setStatusTip(self.tr("Exit the application"))
    	self.connect(self.exitAct,SIGNAL("triggered()"),self.closeAllWindows)

    	#self.undoAct=QAction(QIcon(":/undo.png"),self.tr("&Undo"),self)
    	#self.undoAct.setShortcuts(QKeySequence.Undo)
    	#self.connect(self.undoAct,SIGNAL("triggered()"),self.hexEdit.undo)

    	#self.redoAct = QAction(QIcon(":/redo.png"),self.tr("&Redo"),self)
    	#self.redoAct.setShortcuts(QKeySequence.Redo)
    	#self.connect(self.redoAct,SIGNAL("triggered()"),self.hexEdit.redo)

    	#self.saveSelectionReadable=QAction(self.tr("&Save Selection Readable..."),self)
    	#self.saveSelectionReadable.setStatusTip(self.tr("Save selection in readable form"))
    	#self.connect(self.saveSelectionReadable,SIGNAL("triggered()"),self.saveSelectionToReadableFile)

    	self.aboutAct=QAction(self.tr("&About"),self)
    	self.aboutAct.setStatusTip(self.tr("Show the application's About box"))
    	self.connect(self.aboutAct,SIGNAL("triggered()"),self.about)

    	self.findAct=QAction(QIcon(":/find.png"),self.tr("&Find"),self)
    	self.findAct.setShortcuts(QKeySequence.Find)
    	self.findAct.setStatusTip(self.tr("Show the Dialog for finding and replacing"))
    	self.connect(self.findAct,SIGNAL("triggered()"),self.showSearchDialog)

    	self.findNextAct=QAction(self.tr("Find &next"),self)
    	self.findNextAct.setShortcuts(QKeySequence.FindNext)
    	self.findNextAct.setStatusTip(self.tr("Find next occurrence of the searched pattern"))
    	self.connect(self.findNextAct,SIGNAL("triggered()"),self.findNext)

    	#self.optionsAct=QAction(self.tr("&Options"),self)
    	#self.optionsAct.setStatusTip(self.tr("Show the Dialog to select applications options"))
    	#self.connect(self.optionsAct,SIGNAL("triggered()"),self.showOptionsDialog)

    def createMenus(self):
    	self.fileMenu=self.menuBar().addMenu(self.tr("&File"))
    	self.fileMenu.addAction(self.openAct)
    	self.fileMenu.addAction(self.saveAct)
    	self.fileMenu.addAction(self.saveAsAct)
    	#self.fileMenu.addAction(self.saveReadable)
   	self.fileMenu.addSeparator()
    	self.fileMenu.addAction(self.exitAct)

    	self.editMenu=self.menuBar().addMenu(self.tr("&Edit"))
    	#self.editMenu.addAction(self.undoAct)
    	#self.editMenu.addAction(self.redoAct)
    	#self.editMenu.addAction(self.saveSelectionReadable)
    	#self.editMenu.addSeparator()
    	self.editMenu.addAction(self.findAct)
    	self.editMenu.addAction(self.findNextAct)
    	self.editMenu.addSeparator()
    	#self.editMenu.addAction(self.optionsAct)

    	self.helpMenu=self.menuBar().addMenu(self.tr("&Help"))
    	self.helpMenu.addAction(self.aboutAct)

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
    	self.fileToolBar.addAction(self.saveAct)
    	self.editToolBar=self.addToolBar(self.tr("Edit"))
    	#self.editToolBar.addAction(self.undoAct)
    	#self.editToolBar.addAction(self.redoAct)
    	self.editToolBar.addAction(self.findAct)

    def loadFile(self,fileName):
    	self.file.setFileName(fileName)
    	if not self.hexEdit.setData(self.file):
            QMessageBox.warning(self,self.tr("QHexEdit"),self.tr("Cannot read file %1:\n%2.").arg(fileName).arg(self.file.errorString()))
            return
    	self.setCurrentFile(fileName)
    	self.statusBar().showMessage(self.tr("File loaded"), 2000)

    def saveFile(self,fileName):
    	tmpFileName = fileName + ".~tmp"

    	QApplication.setOverrideCursor(Qt.WaitCursor)
    	filen=QFile(tmpFileName)
    	ok = self.hexEdit.write(filen)
    	if QFile.exists(fileName):
            ok = QFile.remove(fileName)
    	if ok:
            filen.setFileName(tmpFileName)
            ok = filen.copy(fileName)
            if ok:
            	ok = QFile.remove(tmpFileName)
    	QApplication.restoreOverrideCursor()
    	if not ok:
            QMessageBox.warning(self,self.tr("QHexEdit"),self.tr("Cannot write file %1.").arg(fileName))
            return False
    	self.setCurrentFile(fileName)
    	self.statusBar().showMessage(self.tr("File saved"), 2000)
    	return True

    def setCurrentFile(self,fileName):
    	self.curFile = QFileInfo(fileName).canonicalFilePath()
	if fileName=="":
    	    self.isUntitled = True
	else:
    	    self.isUntitled = False	
    	self.setWindowModified(False)
    	if fileName=="":
            self.setWindowFilePath("QHexEdit")
    	else:
            self.setWindowFilePath(self.curFile + " - QHexEdit")

    def strippedName(self,fullFileName):
    	return QFileInfo(fullFileName).fileName()

    def closeAllWindows(self):
	self.hexEdit._cursorTimer.stop()
	self.optionsDialog.close()
	self.searchDialog.close()
	self.close()

    def closeEvent(self,event):
	self.hexEdit._cursorTimer.stop()
	self.optionsDialog.close()
	self.searchDialog.close()
	self.flag=-1


