# -*- coding: utf-8 -*-

from PyQt4.QtGui import QLineEdit, QItemDelegate, QPainter, QPen, QPixmap, QStyleOptionViewItem, QColor, QLinearGradient
from PyQt4.QtCore import Qt, pyqtSignal, pyqtSlot, SIGNAL

class CellDelegate(QItemDelegate):
    floatDialogShowSignal = pyqtSignal(int, int, str)
    floatDialogCloseSignal = pyqtSignal()
    def __init__(self, parent = None):
        super(CellDelegate, self).__init__(parent)
        self.stages = [[]]

        green = QLinearGradient()
        green.setColorAt(0.0, Qt.green)
        green.setColorAt(0.2, QColor(113, 255, 147,))
        green.setColorAt(0.4, QColor(137, 255, 165,))
        green.setColorAt(0.6, QColor(167, 255, 188,))
        green.setColorAt(0.8, QColor(209, 255, 218,))
        green.setColorAt(1.0, QColor(229, 255, 235))

        red = QLinearGradient()
        red.setColorAt(0.0, QColor(216, 64, 3))
        red.setColorAt(0.2, QColor(252, 86, 20))
        red.setColorAt(0.4, QColor(253, 157, 119))
        red.setColorAt(0.6, QColor(254, 191, 186))
        red.setColorAt(0.8, QColor(254, 219, 216))
        red.setColorAt(1.0, QColor(255, 244, 243))

        yellow = QLinearGradient()
        yellow.setColorAt(0.0, QColor(244, 238, 4))
        yellow.setColorAt(0.2, QColor(255, 251, 33))
        yellow.setColorAt(0.4, QColor(255, 253, 153))
        yellow.setColorAt(0.6, QColor(251, 254, 172))
        yellow.setColorAt(0.8, QColor(251, 254, 210))
        yellow.setColorAt(1.0, QColor(253, 255, 229))

        grey = QLinearGradient()
        grey.setColorAt(0.0, QColor(194, 194, 194))
        grey.setColorAt(0.2, QColor(207, 207, 207))
        grey.setColorAt(0.4, QColor(230, 230, 230))
        grey.setColorAt(0.6, QColor(243, 243, 243))
        grey.setColorAt(0.8, QColor(248, 248, 248))
        grey.setColorAt(1.0, QColor(253, 253, 253))

        blue = QLinearGradient()
        blue.setColorAt(0.0, QColor(71, 124, 255))
        blue.setColorAt(0.2, QColor(121, 159, 255))  
        blue.setColorAt(0.4, QColor(189, 208, 255)) 
        blue.setColorAt(0.6, QColor(213, 255, 255))         
        blue.setColorAt(0.8, QColor(220, 229, 255))
        blue.setColorAt(1.0, QColor(235, 241, 255))

        self.rwColors = {"R" : green,
                         "W" : red,
                         "RW" : yellow,
                         "WR" : yellow,
                         "Fetch" : grey,
                         "Ex" : blue}

    def setStages(self, data):
        self.stages = data

    def paint(self, painter, option, index):
        #pixmap = QPixmap("/home/litt/apcgreen.png")       
        #painter.drawPixmap(option.rect, pixmap)
        if self.stages[index.row()][index.column()] != "":
            text = self.stages[index.row()][index.column()].split(".")
            if len(text) > 1:
                linearGradient = self.rwColors[text[1]]
            elif text[0] in ["FG", "FS", "FW", "FR", "DP"]:
                linearGradient = self.rwColors["Fetch"]
            else:
                linearGradient = self.rwColors["Ex"]

            myOption = QStyleOptionViewItem(option)
            linearGradient.setStart(myOption.rect.topLeft())
            linearGradient.setFinalStop(myOption.rect.bottomRight())
            painter.setBrush(linearGradient)
            painter.drawRect(option.rect)
            QItemDelegate.paint(self, painter, myOption, index)        

