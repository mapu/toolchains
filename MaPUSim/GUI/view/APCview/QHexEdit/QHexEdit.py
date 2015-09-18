#-*- coding utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*
from Chunks import*
from UndoStack import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

HEXCHARS_IN_LINE = 47
BYTES_PER_LINE = 16

class QHexEdit(QAbstractScrollArea):
    #Contains the address, where the cursor is located.
    currentAddressChanged=pyqtSignal(int)
    #Contains the size of the data to edit.
    currentSizeChanged=pyqtSignal(int)
    #The signal is emitted every time, the data is changed. 
    dataChanged=pyqtSignal()
    #The signal is emitted every time, the overwrite mode is changed. 
    overwriteModeChanged=pyqtSignal(bool)
    def __init__(self,parent=None):
	super(QHexEdit,self).__init__(parent)
	self.start=0
	self.addressSearch=QString()
   	self._chunks = Chunks()
   	self._undoStack = UndoStack(self._chunks, self)
    	self.setFont(QFont("Monospace", 10))
   	self.setAddressAreaColor(self.palette().alternateBase().color())
    	self.setHighlightingColor(QColor(0xff, 0xff, 0x99, 0xff))
    	self.setSelectionColor(self.palette().highlight().color())
	self._cursorTimer=QTimer()
    	self.connect(self._cursorTimer, SIGNAL("timeout()"), self.updateCursor)
    	self.connect(self.verticalScrollBar(), SIGNAL("valueChanged(int)"), self.adjust)
    	self.connect(self._undoStack, SIGNAL("indexChanged(int)"), self.dataChangedPrivate)

   	self._cursorTimer.setInterval(500)
    	self._cursorTimer.start()
	self._addressArea=True
	self._asciiArea=True
	self._markedShown=QByteArray()
	self._cursorPosition=0
	self._cursorRect=QRect(0,0,0,0)
	self._overwriteMode=True
	self._lastEventSize=0

    	self.setAddressWidth(4)
    	self.setAddressArea(True)
    	self.setAsciiArea(True)
    	self.setOverwriteMode(True)
    	self.setHighlighting(True)
    	self.setReadOnly(False)

    	self.initDialog()

    def setAddressArea(self,addressArea):
    	self._addressArea = addressArea
    	self.adjust()
    	self.setCursorPosition(self._cursorPosition)
    	self.viewport().update()

    def addressArea(self):
    	return self._addressArea

    def setAddressAreaColor(self,color):
    	self._addressAreaColor = color
    	self.viewport().update()

    def addressAreaColor(self):
    	return self._addressAreaColor

    def setAddressOffset(self,addressOffset):
    	self._addressOffset = addressOffset
    	self.adjust()
    	self.setCursorPosition(self._cursorPosition)
    	self.viewport().update()

    def addressOffset(self):
    	return self._addressOffset

    def setAddressWidth(self,addressWidth):
    	self._addressWidth = addressWidth
    	self.adjust()
    	self.setCursorPosition(self._cursorPosition)
    	self.viewport().update()

    def addressWidth(self):
    	size = self._chunks.size()
    	n = 1
    	if size > 0x100000000:
	    n += 8
	    size /= 0x100000000
    	if size > 0x10000:
	    n += 4
	    size /= 0x10000
    	if size > 0x100:
	    n += 2
 	    size /= 0x100
    	if size > 0x10:
	    n += 1
	    size /= 0x10

    	if n > self._addressWidth:
            return n
    	else:
            return self._addressWidth

    def setAsciiArea(self,asciiArea):
    	self._asciiArea = asciiArea
    	self.viewport().update()

    def asciiArea(self):
    	return self._asciiArea

    def setCursorPosition(self,position):
    	self._blink = False
    	self.viewport().update(self._cursorRect)

    	#2. Check, if cursor in range?
    	if self._overwriteMode and (position > (self._chunks.size() * 2 - 1)):
            position = self._chunks.size() * 2 - 1
    	if (not self._overwriteMode) and (position > (self._chunks.size() * 2)):
            position = self._chunks.size() * 2
    	if position < 0:
            position = 0

    	#3. Calc new position of curser
    	self._cursorPosition = position
    	self._bPosCurrent = position / 2
    	self._pxCursorY = ((position/2 - self._bPosFirst) / BYTES_PER_LINE + 1) * self._pxCharHeight
    	x = (position % (2 * BYTES_PER_LINE))
    	self._pxCursorX = (((x / 2) * 3) + (x % 2)) * self._pxCharWidth + self._pxPosHexX

    	if self._overwriteMode:
            self._cursorRect = QRect(self._pxCursorX, self._pxCursorY + self._pxCursorWidth, self._pxCharWidth, self._pxCursorWidth)
    	else:
            self._cursorRect = QRect(self._pxCursorX, self._pxCursorY - self._pxCharHeight + 4, self._pxCursorWidth, self._pxCharHeight)

    	#4. Immiadately draw new cursor
    	self._blink = True
    	self.viewport().update(self._cursorRect)
    	self.currentAddressChanged.emit(self._bPosCurrent)

    def cursorPosition(self,pos=0):
    	#Calc cursorposition depending on a graphical position
	if pos==0:
	    return self._cursorPosition
	else:
    	    result = -1
    	    if (pos.x() >= self._pxPosHexX) and (pos.x() < (self._pxPosHexX + (1 + HEXCHARS_IN_LINE) * self._pxCharWidth)):
            	x = (pos.x() - self._pxPosHexX - self._pxCharWidth / 2) / self._pxCharWidth
            	x = (x / 3) * 2 + x % 3
           	y = ((pos.y() - 3) / self._pxCharHeight) * 2 * BYTES_PER_LINE
            	result = self._bPosFirst * 2 + x + y
    	    return result

    #def cursorPosition(self):
    	#return self._cursorPosition

    #def setData(self,ba):
    	#self._data = ba
    	#self._bData.setData(self._data)
    	#self.setData(self._bData)

    def setData(self,iODevice,start):
	self.addressSearch.clear()
    	ok = self._chunks.setIODevice(iODevice)
    	self.initDialog()
	self.start=start
    	self.dataChangedPrivate(0)
    	return ok

    def data(self):
    	return self._chunks.data(0, -1)

    def setHighlighting(self,highlighting):
    	self._highlighting = highlighting
    	self.viewport().update()

    def highlighting(self):
    	return self._highlighting

    def setHighlightingColor(self,color):
    	self._brushHighlighted = QBrush(color)
    	self._penHighlighted = QPen(self.viewport().palette().color(QPalette.WindowText))
    	self.viewport().update()

    def highlightingColor(self):
    	return self._brushHighlighted.color()

    def setOverwriteMode(self,overwriteMode):
    	self._overwriteMode = overwriteMode
    	self.overwriteModeChanged.emit(overwriteMode)

    def overwriteMode(self):
    	return self._overwriteMode

    def setSelectionColor(self,color):
    	self._brushSelection = QBrush(color)
    	self._penSelection = QPen(Qt.white)
    	self.viewport().update()

    def selectionColor(self):
    	return self._brushSelection.color()

    def isReadOnly(self):
   	return self._readOnly

    def setReadOnly(self,readOnly):
    	self._readOnly = readOnly

    def dataAt(self,pos,count):
    	return self._chunks.data(pos, count)

    def write(self,iODevice, pos=0, count=-1):
    	return self._chunks.write(iODevice, pos, count)

    def insert(self,index,ch):
    	self._undoStack.insert(index, ch)
    	self.refresh()

    def remove(self,index,lenn):
    	self._undoStack.removeAt(index, lenn)
    	self.refresh()

    def replace_ch(self,index, ch):
    	self._undoStack.overwrite_ch(index, ch)
    	self.refresh()

    def insert_ba(self,pos, ba):
    	self._undoStack.insert(pos, ba)
    	self.refresh()

    def replace(self, pos, lenn, ba):
    	self._undoStack.overwrite(pos, lenn, ba)
    	self.refresh()

    def ensureVisible(self):
    	if self._cursorPosition < self._bPosFirst * 2:
            self.verticalScrollBar().setValue((int)(self._cursorPosition / 2 / BYTES_PER_LINE))
    	if self._cursorPosition > ((self._bPosFirst + (self._rowsShown - 1)*BYTES_PER_LINE) * 2):
            self.verticalScrollBar().setValue((int)(self._cursorPosition / 2 / BYTES_PER_LINE) - self._rowsShown + 1)
    	self.viewport().update()

    def indexOf(self,ba):
	self.addressSearch=ba
	pos=int(str(ba),16)
	if pos%16==0:
            self.setCursorPosition(pos*2)
            self.ensureVisible()
    	    return pos
	else:
	    return -1	    
	
    def isModified(self):
        return self._modified

    def redo(self):
    	self._undoStack.redo()
    	self.setCursorPosition(self._chunks.pos()*2)
    	self.refresh()

    def selectionToReadableString(self):
    	ba = self._chunks.data(self.getSelectionBegin(), self.getSelectionEnd() - self.getSelectionBegin())
    	return self.toReadable(ba)

    def setFont(self,font):
    	#QWidget.setFont(font)
    	self._pxCharWidth = self.fontMetrics().width('2')
    	self._pxCharHeight = self.fontMetrics().height()
    	self._pxGapAdr = self._pxCharWidth / 2
    	self._pxGapAdrHex = self._pxCharWidth
    	self._pxGapHexAscii = 2 * self._pxCharWidth
    	self._pxCursorWidth = self._pxCharHeight / 7
    	self._pxSelectionSub = self._pxCharHeight / 5
    	self.viewport().update()

    def toReadableString(self):
    	ba = _chunks.data()
    	return self.toReadable(ba)

    def undo(self):
    	self._undoStack.undo()
    	self.setCursorPosition(self._chunks.pos()*2)
    	self.refresh()

    def keyPressEvent(self,event):
	#Cursor movements
    	if event.matches(QKeySequence.MoveToNextChar):
            self.setCursorPosition(self._cursorPosition + 1)
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToPreviousChar):
            self.setCursorPosition(self._cursorPosition - 1)
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToEndOfLine):
            self.setCursorPosition(self._cursorPosition | (2 * BYTES_PER_LINE -1))
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToStartOfLine):
            self.setCursorPosition(self._cursorPosition - (self._cursorPosition % (2 * BYTES_PER_LINE)))
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToPreviousLine):
            self.setCursorPosition(self._cursorPosition - (2 * BYTES_PER_LINE))
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToNextLine):
            self.setCursorPosition(self._cursorPosition + (2 * BYTES_PER_LINE))
            self.resetSelection(self._cursorPosition)
      	if event.matches(QKeySequence.MoveToNextPage):
            self.setCursorPosition(self._cursorPosition + (((self._rowsShown - 1) * 2 * BYTES_PER_LINE)))
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToPreviousPage):
            self.setCursorPosition(self._cursorPosition - (((self._rowsShown - 1) * 2 * BYTES_PER_LINE)))
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToEndOfDocument):
            self.setCursorPosition(self._chunks.size() * 2)
            self.resetSelection(self._cursorPosition)
    	if event.matches(QKeySequence.MoveToStartOfDocument):
            self.setCursorPosition(0)
            self.resetSelection(self._cursorPosition)

	# Select commands
     	if event.matches(QKeySequence.SelectAll):
            self.resetSelection(0)
            self.setSelection(2*self._chunks.size() + 1)
    	if event.matches(QKeySequence.SelectNextChar):
            pos = self._cursorPosition + 1
            self.setCursorPosition(pos)
            self.setSelection(pos)
        if event.matches(QKeySequence.SelectPreviousChar):
            pos = self._cursorPosition - 1
            self.setSelection(pos)
            self.setCursorPosition(pos)
        if event.matches(QKeySequence.SelectEndOfLine):
            pos = self._cursorPosition - (self._cursorPosition % (2 * BYTES_PER_LINE)) + (2 * BYTES_PER_LINE)
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	if event.matches(QKeySequence.SelectStartOfLine):
            pos = self._cursorPosition - (self._cursorPosition % (2 * BYTES_PER_LINE))
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	if event.matches(QKeySequence.SelectPreviousLine):
            pos = self._cursorPosition - (2 * BYTES_PER_LINE)
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	if event.matches(QKeySequence.SelectNextLine):
            pos = self._cursorPosition + (2 * BYTES_PER_LINE)
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	if event.matches(QKeySequence.SelectNextPage):
            pos = self._cursorPosition + (((self.viewport().height() / self._pxCharHeight) - 1) * 2 * BYTES_PER_LINE)
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	if event.matches(QKeySequence.SelectPreviousPage):
            pos = self._cursorPosition - (((self.viewport().height() / self._pxCharHeight) - 1) * 2 * BYTES_PER_LINE)
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	if event.matches(QKeySequence.SelectEndOfDocument):
            self.pos = _chunks.size() * 2
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	if event.matches(QKeySequence.SelectStartOfDocument):
            pos = 0
            self.setCursorPosition(pos)
            self.setSelection(pos)
    	#Copy 
    	if event.matches(QKeySequence.Copy):
            ba = self._chunks.data(self.getSelectionBegin(), self.getSelectionEnd() - self.getSelectionBegin()).toHex()
            for idx in range(32,ba.size(),33):
            	ba.insert(idx, "\n")
            clipboard = QApplication.clipboard()
            clipboard.setText(ba.data())

    	self.refresh()

    def mouseMoveEvent(self,event):
    	self._blink = False
    	self.viewport().update()
    	actPos = self.cursorPosition(event.pos())
    	if actPos >= 0:
            self.setCursorPosition(actPos)
            self.setSelection(actPos)

    def mousePressEvent(self,event):
    	self._blink = False
    	self.viewport().update()
    	cPos = self.cursorPosition(event.pos())
    	if cPos >= 0:
            self.resetSelection(cPos)
            self.setCursorPosition(cPos)

    def paintEvent(self,event):
    	painter=QPainter(self.viewport())
	apainter=QPainter(self.viewport())
	apainter.setFont(QFont("Monospace"))
    	if event.rect() != self._cursorRect:
            #process some useful calculations
            pxOfsX = self.horizontalScrollBar().value()
            pxPosStartY = self._pxCharHeight

            #draw some patterns if needed
            painter.fillRect(event.rect(), self.viewport().palette().color(QPalette.Base))
            if self._addressArea:
                 painter.fillRect(QRect(-pxOfsX, event.rect().top(), self._pxPosHexX - self._pxGapAdrHex/2 - pxOfsX, self.height()), self._addressAreaColor)
            if self._asciiArea:
                linePos = self._pxPosAsciiX - (self._pxGapHexAscii / 2)
                painter.setPen(Qt.gray)
                painter.drawLine(linePos - pxOfsX, event.rect().top(), linePos - pxOfsX, self.height())

            painter.setPen(self.viewport().palette().color(QPalette.WindowText))

            #paint address area
            if self._addressArea:
		pxPosY = self._pxCharHeight
                for row in range(0,(self._dataShown.size()/BYTES_PER_LINE)+1):
                    address = QString("%1").arg(self._bPosFirst + row*BYTES_PER_LINE + self._addressOffset, self._addrDigits, 16, QChar('0'))
		    if address==self.addressSearch:
			apainter.fillRect(self._pxPosAdrX - pxOfsX, pxPosY-self._pxCharHeight+4,address.size()*self._pxCharWidth,self._pxCharHeight,QColor(255,255,0));
                    apainter.drawText(self._pxPosAdrX - pxOfsX, pxPosY, address)
		    pxPosY +=self._pxCharHeight

            #paint hex and ascii area
            colStandard = QPen(self.viewport().palette().color(QPalette.WindowText))
            painter.setBackgroundMode(Qt.TransparentMode)
	    pxPosY = pxPosStartY 
            for row in range(0,self._rowsShown+1): 
                pxPosX = self._pxPosHexX  - pxOfsX
                pxPosAsciiX2 = self._pxPosAsciiX  - pxOfsX
                bPosLine = row * BYTES_PER_LINE
                for colIdx in range(0,BYTES_PER_LINE):
		    if (bPosLine + colIdx) < self._dataShown.size():
                    	c = self.viewport().palette().color(QPalette.Base)
                    	painter.setPen(colStandard)

                    	posBa = self._bPosFirst + bPosLine + colIdx
                    	if (self.getSelectionBegin() <= posBa) and (self.getSelectionEnd() > posBa):
                            c = self._brushSelection.color()
                            painter.setPen(self._penSelection)
                    	else:
                            if self._highlighting:
                            	if self._markedShown.at((int)(posBa - self._bPosFirst)):
                                    c = self._brushHighlighted.color()
                                    painter.setPen(self._penHighlighted)

                        #render hex value
                        r=QRect()
                        if colIdx == 0:
                            r.setRect(pxPosX, pxPosY - self._pxCharHeight + self._pxSelectionSub, 2*self._pxCharWidth, self._pxCharHeight)
                    	else:
                            r.setRect(pxPosX - self._pxCharWidth, pxPosY - self._pxCharHeight + self._pxSelectionSub, 3*self._pxCharWidth, self._pxCharHeight)
                    	painter.fillRect(r,QColor(255,255,255))
                    	hexn = self._hexDataShown.mid((bPosLine + colIdx) * 2, 2).data()
                    	painter.drawText(pxPosX, pxPosY, hexn)
                    	pxPosX += 3*self._pxCharWidth
                    	#render ascii value
                    	if self._asciiArea:
                            ch = self._dataShown.at(bPosLine + colIdx)
			    print "len",len(ch)
                            if (chr(ord(ch))< 0x20) or (chr(ord(ch))> 0x7e):
                            	ch = '.'
                            r.setRect(pxPosAsciiX2, pxPosY - self._pxCharHeight + self._pxSelectionSub, self._pxCharWidth, self._pxCharHeight)
                            painter.fillRect(r, QColor(255,255,255))
                            painter.drawText(pxPosAsciiX2, pxPosY, ch)
                            pxPosAsciiX2 += self._pxCharWidth
		pxPosY +=self._pxCharHeight
            painter.setBackgroundMode(Qt.TransparentMode)
            painter.setPen(self.viewport().palette().color(QPalette.WindowText))

    	#paint cursor
    	if self._blink and not self._readOnly and self.hasFocus():
            painter.fillRect(self._cursorRect, self.palette().color(QPalette.WindowText))
   	else:
            painter.drawText(self._pxCursorX, self._pxCursorY, self._hexDataShown.mid(self._cursorPosition - self._bPosFirst * 2, 1).data())

        #emit event, if size has changed
        if self._lastEventSize != self._chunks.size():
            self._lastEventSize = self._chunks.size()
            self.currentSizeChanged.emit(self._lastEventSize)

    def resizeEvent(self,event):
    	self.adjust()

    def resetSelection(self):
    	self._bSelectionBegin = self._bSelectionInit
    	self._bSelectionEnd = self._bSelectionInit

    def resetSelection(self,pos):
    	if pos < 0:
            pos = 0
    	pos = pos / 2
    	self._bSelectionInit = pos
    	self._bSelectionBegin = pos
    	self._bSelectionEnd = pos

    def setSelection(self,pos):
	if pos < 0:
            pos = 0
        pos = pos / 2
    	if pos >= self._bSelectionInit:
            self._bSelectionEnd = pos
            self._bSelectionBegin = self._bSelectionInit
    	else:
            self._bSelectionBegin = pos
            self._bSelectionEnd = self._bSelectionInit


    def getSelectionBegin(self):
    	return self._bSelectionBegin

    def getSelectionEnd(self):
    	return self._bSelectionEnd

    def initDialog(self):
    	self._undoStack.clear()
    	self.setAddressOffset(0)
    	self.resetSelection(0)
    	self.setCursorPosition(0)
    	self.verticalScrollBar().setValue(0)
    	self._modified = False

    def adjust(self):
    	#recalc Graphics
    	if self._addressArea:
            self._addrDigits = self.addressWidth()
            self._pxPosHexX = self._pxGapAdr + self._addrDigits*self._pxCharWidth + self._pxGapAdrHex
    	else:
            self._pxPosHexX = self._pxGapAdrHex
    	self._pxPosAdrX = self._pxGapAdr
    	self._pxPosAsciiX = self._pxPosHexX + HEXCHARS_IN_LINE * self._pxCharWidth + self._pxGapHexAscii

    	#set horizontalScrollBar()
    	pxWidth = self._pxPosAsciiX
    	if self._asciiArea:
            pxWidth += BYTES_PER_LINE*self._pxCharWidth
    	self.horizontalScrollBar().setRange(0, pxWidth - self.viewport().width())
    	self.horizontalScrollBar().setPageStep(self.viewport().width())

    	#set verticalScrollbar()
    	self._rowsShown = ((self.viewport().height()-4)/self._pxCharHeight)
    	lineCount = (int)(self._chunks.size() / BYTES_PER_LINE) + 1
    	self.verticalScrollBar().setRange(0, lineCount - self._rowsShown)
    	self.verticalScrollBar().setPageStep(self._rowsShown)

    	value = self.verticalScrollBar().value()
    	self._bPosFirst = value * BYTES_PER_LINE
    	self._bPosLast = self._bPosFirst + (self._rowsShown * BYTES_PER_LINE) - 1
    	if self._bPosLast >= self._chunks.size():
            self._bPosLast = self._chunks.size() - 1
    	self.readBuffers()
    	self.setCursorPosition(self._cursorPosition)

    def dataChangedPrivate(self,a):
	if self._undoStack.index() != 0:
    	    self._modified = True
	else:
	    self._modified = False
    	self.adjust()
    	self.dataChanged.emit()

    def refresh(self):
    	self.ensureVisible()
    	self.readBuffers()

    def readBuffers(self):
    	self._dataShown = self._chunks.data(self.start+self._bPosFirst, self._bPosLast - self._bPosFirst + BYTES_PER_LINE + 1, self._markedShown,self._bPosFirst)
    	self._hexDataShown = QByteArray(self._dataShown.toHex())

    def toReadable(self,ba):
    	result=QString()

    	for i in range(0,ba.size(),16):
            addrStr = QString("%1").arg(self._addressOffset + i, self.addressWidth(), 16, QChar('0'))
            hexStr=QString()
            ascStr=QString()
            for j in range(0,16):
            	if (i + j) < ba.size():
                    hexStr.append(" ").append(ba.mid(i+j, 1).toHex())
                    ch = ba[i + j]
                    if ((ch < 0x20) or (ch > 0x7e)):
                        ch = '.'
                    ascStr.append(QChar(ch))
            result += addrStr + " " + QString("%1").arg(hexStr, -48) + "  " + QString("%1").arg(ascStr, -17) + "\n"
    	return result

    def updateCursor(self):
    	if self._blink:
            self._blink = False
    	else:
            self._blink = True
    	self.viewport().update(self._cursorRect)



