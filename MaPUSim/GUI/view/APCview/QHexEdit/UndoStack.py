#-*- coding utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

class CCmd():
    insert = 1
    removeAt = 2
    overwrite = 3

class CharCommand(QUndoCommand):
    def __init__(self, chunks, cmd, charPos, newChar, parent = None):
        super(CharCommand, self).__init__(parent)
        self._chunks = chunks
        self._charPos = charPos
        self._newChar = newChar
        self._cmd = cmd

    def mergeWith(self, command):
        #nextCommand = static_cast<const CharCommand *>(command)
        nextCommand = CharCommand()
        nextCommand = command
        result = False

        if self._cmd != CCmd.removeAt:
            if nextCommand._cmd == CCmd.overwrite:
                if nextCommand._charPos == self._charPos:
                    self._newChar = nextCommand._newChar
                    result = True
        return result

    def undo(self):
        if self._cmd == CCmd.insert:
            self._chunks.removeAt(self._charPos)
        elif self._cmd == CCmd.overwrite:
            self._chunks.overwrite(self._charPos, self._oldChar)
            self._chunks.setDataChanged(self._charPos, self._wasChanged)
        elif self._cmd == CCmd.removeAt:
            self._chunks.insert(self._charPos, self._oldChar)
            self._chunks.setDataChanged(self._charPos, self._wasChanged)

    def redo(self):
        if self._cmd == CCmd.insert:
            self._chunks.insert(self._charPos, self._newChar)
        elif self._cmd == CCmd.overwrite:
            self._oldChar = self._chunks[self._charPos]
            self._wasChanged = self._chunks.dataChanged(self._charPos)
            self._chunks.overwrite(self._charPos, self._newChar)
        elif self._cmd == CCmd.removeAt:
            self._oldChar = self._chunks[self._charPos]
            self._wasChanged = self._chunks.dataChanged(self._charPos)
            self._chunks.removeAt(self._charPos)

class UndoStack(QUndoStack):
    def __init__(self, chunks, parent = None):
        super(UndoStack, self).__init__(parent)
        self._chunks = chunks
        self._parent = parent

    def insert_ch(self, pos, c):
        if (pos >= 0) and (pos <= self._chunks.size()):
            cc = CharCommand(self._chunks, CCmd.insert, pos, c)
            self.push(cc)

    def insert(self, pos, ba):
        if (pos >= 0) and (pos <= self._chunks.size()):
            txt = QString(self.tr("Inserting %1 bytes")).arg(ba.size())
            self.beginMacro(txt)
            for idx in range(ba.size()): 
                cc = CharCommand(self._chunks, CCmd.insert, pos + idx, ba.at(idx))
                self.push(cc)
            self.endMacro()

    def removeAt(self,pos, lenn):
        if (pos >= 0) and (pos < self._chunks.size()):
            if lenn == 1:
                cc = CharCommand(self._chunks, CCmd.removeAt, pos, char(0))
                self.push(cc)
            else:
                txt = QString(self.tr("Delete %1 chars")).arg(lenn)
                self.beginMacro(txt)
                for cnt in range(0, lenn):
                    cc = CharCommand(self._chunks, CCmd.removeAt, pos, "0")
                    self.push(cc)
                self.endMacro()

    def overwrite_ch(self, pos, c):
        if (pos >= 0) and (pos < self._chunks.size()):
            cc = CharCommand(self._chunks, CCmd.overwrite, pos, c)
            self.push(cc)

    def overwrite(self, pos, lenn, ba):
        if (pos >= 0) and (pos < self._chunks.size()):
            txt = QString(self.tr("Overwrite %1 chars")).arg(lenn)
            self.beginMacro(txt)
            self.removeAt(pos, lenn)
            self.insert(pos, ba)
            self.endMacro()


