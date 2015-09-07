#-*- coding: utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class OptionsDialog(QWidget):
    def __init__(self,parent=None):
	super(OptionsDialog,self).__init__(parent)

	self.setWindowTitle("Options")
	self.resize(395,362)
	gbFlags=QGroupBox("Flags")
	self.cbAddressArea=QCheckBox("Address Area")
	self.cbAsciiArea=QCheckBox("Ascii Area")
	flagLay=QGridLayout()
	flagLay.addWidget(self.cbAddressArea,0,0)
	flagLay.addWidget(self.cbAsciiArea,0,1)	
	gbFlags.setLayout(flagLay)

	gbColors=QGroupBox("Fonts")
	self.pbWidgetFont=QPushButton("Widget Font")
	self.leWidgetFont=QLineEdit("01 23 45 67 89 ab cd ef")
	colorLay=QGridLayout()
	colorLay.addWidget(self.pbWidgetFont,0,0)
	colorLay.addWidget(self.leWidgetFont,0,1)
	gbColors.setLayout(colorLay)

	gbAddressAreaWidth=QGroupBox("Address Area")
	lbAddressAreaWidth=QLabel("Address Area Width")
	self.sbAddressAreaWidth=QSpinBox()
	self.sbAddressAreaWidth.setRange(1,6)
	self.sbAddressAreaWidth.setValue(4)
	addressLay=QGridLayout()
	addressLay.addWidget(lbAddressAreaWidth,0,0)
	addressLay.addWidget(self.sbAddressAreaWidth,0,1)
	gbAddressAreaWidth.setLayout(addressLay)

	self.okButton=QPushButton("OK")
	self.cancelButton=QPushButton("Cancel")
	self.connect(self.okButton,SIGNAL("clicked()"),self.okSlot)
	self.connect(self.cancelButton,SIGNAL("clicked()"),self.cancelSlot)
	buttonLay=QHBoxLayout()
	buttonLay.addWidget(self.okButton)
	buttonLay.addWidget(self.cancelButton)

	mainLay=QVBoxLayout()
	mainLay.addWidget(gbFlags)
	mainLay.addWidget(gbColors)
	mainLay.addWidget(gbAddressAreaWidth)
	mainLay.addLayout(buttonLay)
	self.setLayout(mainLay)

	self.connect(self.pbWidgetFont,SIGNAL("clicked()"),self.on_pbWidgetFont_clicked)

    def showWidget(self):
    	self.readSettings()
    	self.show()

    def okSlot(self):
    	self.close()

    def cancelSlot(self):
	self.close()

    def readSettings(self):
    	self.cbAddressArea.setChecked(True)
    	self.cbAsciiArea.setChecked(True)
    	self.leWidgetFont.setFont(QFont("Monospace", 10))
    	self.sbAddressAreaWidth.setValue(4)

    def on_pbWidgetFont_clicked(self):
    	ok=True
	result=QFontDialog.getFont(self.leWidgetFont.font(), self)
    	font = result[0]
	ok= result[1]
    	if ok:
            self.leWidgetFont.setFont(font)

