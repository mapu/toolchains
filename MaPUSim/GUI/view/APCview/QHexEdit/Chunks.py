#-*- coding utf-8 -*-
from PyQt4.QtGui import*
from PyQt4.QtCore import*

QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

NORMAL=0
HIGHLIGHTED=1
BUFFER_SIZE=0x10000
CHUNK_SIZE=0x1000
READ_CHUNK_MASK=0xfffffffffffff000
LONG_LONG_MAX=9223372036854775807


class Chunk():
    def __init__(self):	
	data=QByteArray()
    	dataChanged=QByteArray()
    	absPos=0

class Chunks():
    def __init__(self,ioDevice=None):
	self._chunks=[]
	if ioDevice==None:
    	    buf = QBuffer()
    	    self.setIODevice(buf)
	else:
    	    self.setIODevice(ioDevice)

    def setIODevice(self,ioDevice):
    	self._ioDevice = ioDevice
    	ok = self._ioDevice.open(QIODevice.ReadOnly)
    	if ok:   #Try to open IODevice
            self._size = self._ioDevice.size()
            self._ioDevice.close()
        else:               #Fallback is an empty buffer
            buf = QBuffer()
            self._ioDevice = buf
            self._size = 0
    	self._chunks=[]
    	self._pos = 0
    	return ok

    def data(self,pos=0,maxSize=-1,highlighted=0):
        ioDelta = 0
    	chunkIdx = 0
    	chunk=Chunk()
    	buffern=QByteArray()

    	#Do some checks and some arrangements
	if highlighted:
            highlighted.clear()

    	if pos >= self._size:
            return buffern

    	if maxSize < 0:
            maxSize = self._size
    	else:
            if (pos + maxSize) > self._size:
                maxSize = self._size - pos

    	self._ioDevice.open(QIODevice.ReadOnly)

   	while maxSize > 0:
            chunk.absPos = LONG_LONG_MAX
            chunksLoopOngoing = True
            while (chunkIdx < len(self._chunks)) and chunksLoopOngoing:
            	#In this section, we track changes before our required data and
            	#we take the editdet data, if availible. ioDelta is a difference
            	#counter to justify the read pointer to the original data, if
            	#data in between was deleted or inserted.
            	chunk = self._chunks[chunkIdx]
            	if chunk.absPos > pos:
                    chunksLoopOngoing = False
            	else:
                    chunkIdx += 1
                    chunkOfs = pos - chunk.absPos
                    if maxSize > len(chunk.data) - chunkOfs:
                    	count = len(chunk.data) - chunkOfs
                    	ioDelta += CHUNK_SIZE - len(chunk.data)
                    else:
                    	count = maxSize
                    if count > 0:
                    	buffern += QString(chunk.data).toLatin1().mid(chunkOfs, count)
                    	maxSize -= count
                    	pos += count
                    	if highlighted:
                            highlighted += chunk.dataChanged.mid(chunkOfs, count)

            if (maxSize > 0) and (pos < chunk.absPos):
            	#In this section, we read data from the original source. This only will
            	#happen, whe no copied data is available
            	if (chunk.absPos - pos) > maxSize:
                    byteCount = maxSize
            	else:
                    byteCount = chunk.absPos - pos
            	maxSize -= byteCount
            	self._ioDevice.seek(pos + ioDelta)
		readBuffer=QByteArray()
            	readBuffer = self._ioDevice.read(byteCount)
            	buffern += readBuffer
            	if highlighted:
                    highlighted += QByteArray(len(readBuffer), NORMAL)
            	pos += len(readBuffer)
	self._ioDevice.close()
     	return buffern

    def write(self,iODevice, pos=0, count=-1):
    	if count == -1:
            count = self._size
    	ok = iODevice.open(QIODevice.WriteOnly)
    	if ok:
            for idx in range(pos,count,BUFFER_SIZE): 
           	ba = self.data(idx, BUFFER_SIZE)
            	iODevice.write(ba)
            iODevice.close()
   	return ok

    def setDataChanged(self,pos, dataChanged):
    	if (pos < 0) or (pos >= _size):
            return
    	chunkIdx = self.getChunkIndex(pos)
    	posInBa = pos - self._chunks[chunkIdx].absPos
    	self._chunks[chunkIdx].dataChanged[posInBa] = char(dataChanged)

    def dataChanged(self,pos):
    	high=QByteArray()
	highlighted=[]
	highlighted.append(high)
    	self.data(pos, 1, highlighted)
    	return bool(highlighted.at(0))

    def indexOf(self,ba, fromn):
    	result = -1

    	for pos in range(fromn,self._size,BUFFER_SIZE): 
	    if  result < 0:
            	buffern = self.data(pos, BUFFER_SIZE + ba.size() - 1)
            	findPos = buffern.indexOf(ba)
            	if findPos >= 0:
            	    result = pos + findPos
    	return result

    def lastIndexOf(self,ba, fromn):
    	result = -1

    	for pos in range(fromn,self._size,-BUFFER_SIZE):  
	    if  result < 0:
            	sPos = pos - BUFFER_SIZE - ba.size() + 1
            	if sPos < 0:
                    sPos = 0
            	buffern = self.data(sPos, pos - sPos)
            	findPos = buffern.lastIndexOf(ba)
            	if findPos >= 0:
                    result = sPos + findPos
        return result

    def insert(self,pos, b):
    	if (pos < 0) or (pos > self._size):
            return False
    	if pos == self._size:
            chunkIdx = self.getChunkIndex(pos-1)
   	else:
            chunkIdx = self.getChunkIndex(pos)
    	posInBa = pos - self._chunks[chunkIdx].absPos
    	self._chunks[chunkIdx].data.insert(posInBa, b)
    	self._chunks[chunkIdx].dataChanged.insert(posInBa, "1")
    	for idx in range(chunkIdx+1,self._chunks.size()):
            self._chunks[idx].absPos += 1
    	self._size += 1
    	self._pos = pos
    	return true

    def overwrite(self,pos, b):
    	if (pos < 0) or (pos >= self._size):
            return False
    	chunkIdx = self.getChunkIndex(pos)
    	posInBa = pos - self._chunks[chunkIdx].absPos
    	QString(self._chunks[chunkIdx].data).toLatin1().remove(posInBa,1)
	QString(self._chunks[chunkIdx].data).toLatin1().insert(posInBa,b)
    	self._chunks[chunkIdx].dataChanged.remove(posInBa,1)
    	self._chunks[chunkIdx].dataChanged.insert(posInBa,"1")
   	self._pos = pos
    	return True

    def removeAt(self,pos):
    	if (pos < 0) or (pos >= _size):
            return False
    	chunkIdx = self.getChunkIndex(pos)
    	posInBa = pos - self._chunks[chunkIdx].absPos
    	self._chunks[chunkIdx].data.remove(posInBa, 1)
    	self._chunks[chunkIdx].dataChanged.remove(posInBa, 1)
    	for idx in range(chunkIdx+1,self._chunks.size()):
            self._chunks[idx].absPos -= 1
    	self._size -= 1
    	self._pos = pos
    	return true

    def __getitem__(self,pos):
    	return self.data(pos, 1)[0]

    def pos(self):
    	return self._pos

    def size(self):
    	return self._size

    def getChunkIndex(self,absPos):
    	#This routine checks, if there is already a copied chunk available. If os, it
    	#returns a reference to it. If there is no copied chunk available, original
    	#data will be copied into a new chunk.
    	foundIdx = -1
    	insertIdx = 0
    	ioDelta = 0

    	for idx in range(len(self._chunks)):
            chunk = self._chunks[idx]
            if (absPos >= chunk.absPos) and (absPos < (chunk.absPos + len(chunk.data))):
            	foundIdx = idx
            	break
            if absPos < chunk.absPos:
            	insertIdx = idx
            	break
            ioDelta += len(chunk.data) - CHUNK_SIZE
            insertIdx = idx + 1

    	if foundIdx == -1:
	    newChunk=Chunk()
            readAbsPos = absPos - ioDelta
            readPos = (readAbsPos & READ_CHUNK_MASK)
            self._ioDevice.open(QIODevice.ReadOnly)
            self._ioDevice.seek(readPos)
            newChunk.data = self._ioDevice.read(CHUNK_SIZE)
            self._ioDevice.close()
            newChunk.absPos = absPos - (readAbsPos - readPos)
            newChunk.dataChanged = QByteArray(len(newChunk.data), "0")
            self._chunks.insert(insertIdx, newChunk)
            foundIdx = insertIdx
        return foundIdx

    def chunkSize(self):
    	return self._chunks.size()



