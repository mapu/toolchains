# -*- coding: utf-8 -*-
from PyQt4.QtCore import Qt, SIGNAL, QFile, QString, QTextStream, QIODevice
from PyQt4.QtGui import QDialog, QTextEdit, QVBoxLayout, QTextCursor

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class FileWidget(QDialog):
    def __init__(self, path, parent = None):
        super(FileWidget, self).__init__(parent)
        self.resize(700, 800)
        self.setWindowTitle(path)
        self.textEdit = QTextEdit()
        self.textEdit.setReadOnly(True)
        lay = QVBoxLayout(self)
        lay.addWidget(self.textEdit)
        file1 = QFile(path)
        if not file1.open(QIODevice.ReadOnly | QIODevice.Text):
            return
	s = QTextStream(file1)
	while not s.atEnd():
	    line = s.readLine()
	    self.textEdit.append(line)
	file1.close()
	self.textEdit.moveCursor(QTextCursor.Start)