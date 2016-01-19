# -*- coding: utf-8 -*-
from PyQt4.QtGui import QWidget, QPushButton, QLabel, QButtonGroup, QSizePolicy,\
    QGridLayout, QColor, QPixmap, QBrush, QPen, QPainter, QPainterPath
from PyQt4.QtCore import Qt, QRectF, QPointF, QSizeF


#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class APCMutiCoreWidget(QWidget):
    # The gridlayout size is 9 x 9
    
    # All components info
    components = [
        # ["name", [row, col, row span, col span], class]
        ["ARM", [20, 0, 5, 5], QLabel],
        ["APE0", [10, 10, 5, 5], QPushButton],
        ["APE1", [30, 10, 5, 5], QPushButton],
        ["APE2", [30, 30, 5, 5], QPushButton],
        ["APE3", [10, 30, 5, 5], QPushButton],
        ["CDDR", [20, 40, 5, 5], QLabel],
        ["PDDR0", [0, 20, 5, 5], QLabel],
        ["PDDR1", [40, 20, 5, 5], QLabel]
    ]
    ports = [ #[outs, ins]

    ]
    connections = {
    }
    
    def __init__(self, parent = None):
        super(APCMutiCoreWidget, self).__init__(parent)

        #define left Widget
        self.APCSchemeLayout = QGridLayout() 
        
        self.APEButton = []
        self.APEButtonGroup = QButtonGroup()
        
        sizePolicy = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum);
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        
        for com in self.components:       
            if com[2] is QPushButton:
                self.APEButton.append(com[2](self.tr(com[0])))
                self.APEButton[-1].setSizePolicy(sizePolicy)
                self.APEButton[-1].setStyleSheet('''
                    background: qradialgradient(cx:0, cy:0, radius: 1,
                                                fx:0.5, fy:0.5, stop:0 white,
                                                stop:1 lightblue);
                    border-style: solid;
                    border-width: 1.5px;
                    border-radius: 0px;
                    border-color: steelblue;
                    margin: 0px;
                    padding: 0px;
                ''')
                self.APEButtonGroup.addButton(self.APEButton[-1], len(self.APEButton) - 1)
                self.APCSchemeLayout.addWidget(self.APEButton[-1], *com[1])
            elif com[2] is QLabel:
                label = com[2](self.tr(com[0]))
                label.setAlignment(Qt.AlignCenter)
                label.setSizePolicy(sizePolicy)
                label.setStyleSheet('''
                    background: qradialgradient(cx:0, cy:0, radius: 1,
                                                fx:0.5, fy:0.5, stop:0 white,
                                                stop:1 lightblue);
                    border-width: 0px;
                    margin: 0px;
                    padding: 0px;
                ''')
                self.APCSchemeLayout.addWidget(label, *com[1])

        for i in xrange(self.APCSchemeLayout.columnCount()):
            self.APCSchemeLayout.setColumnStretch(i, 1)
        for i in xrange(self.APCSchemeLayout.rowCount()):
            self.APCSchemeLayout.setRowStretch(i, 1)
            
        self.APCSchemeLayout.setHorizontalSpacing(0)
        self.APCSchemeLayout.setVerticalSpacing(0)
        self.APCSchemeLayout.setMargin(0)
        self.APCSchemeLayout.setContentsMargins(0, 0, 0, 0)

        self.APCSchemeLayout.setAlignment(Qt.AlignCenter)
        self.setLayout(self.APCSchemeLayout)
        
        self.painter = QPainter()
            
    def drawOnePort(self, x, y, direction, color):
        path = QPainterPath()
        if direction == 'R':
            sx = x + self.edge / 2
            sy = y
            ex0 = sx - self.edge
            ey0 = sy + self.edge / 2
            ex1 = ex0
            ey1 = ey0 - self.edge
        elif direction == 'L':
            sx = x - self.edge / 2
            sy = y
            ex0 = sx + self.edge
            ey0 = sy + self.edge / 2
            ex1 = ex0
            ey1 = ey0 - self.edge
        elif direction == 'U':
            sx = x
            sy = y - self.edge / 2
            ex0 = sx + self.edge / 2
            ey0 = sy + self.edge
            ex1 = ex0 - self.edge
            ey1 = ey0
        elif direction == 'D':
            sx = x
            sy = y + self.edge / 2
            ex0 = sx + self.edge / 2
            ey0 = sy - self.edge
            ex1 = ex0 - self.edge
            ey1 = ey0
            
        path.moveTo(sx, sy)
        path.lineTo(ex0, ey0)
        path.lineTo(ex1, ey1)
        path.lineTo(sx, sy)

        self.painter.setPen(Qt.SolidLine);
        self.painter.fillPath(path, QBrush(color));
    
    def redraw(self, size):
        '''
        Redraw the scheme due to resizing or time changing
        '''
        self.rowUnit = size.height() / 45.0
        self.colUnit = size.width() / 45.0
        self.edge = self.rowUnit / 0.8
        self.paintBuffer = [QPixmap(size), QPixmap(size), QPixmap(size)]
        self.painter.begin(self.paintBuffer[0])
        self.painter.fillRect(QRectF(QPointF(0, 0), QSizeF(size)),
                              QBrush(QColor("lightskyblue")))
        self.drawGrid()
        self.drawConnections()
        self.painter.end()
        self.paintBuffer[1].fill(Qt.transparent)
        self.painter.begin(self.paintBuffer[1])
        #self.drawActiveConnections()
        self.painter.end()
        
        self.paintBuffer[2].fill(Qt.transparent)
        self.painter.begin(self.paintBuffer[2])
        self.drawPorts()
        self.painter.end()
        
    def redrawActiveConnections(self):
        '''
        Redraw the buffer due to the changed active connections 
        '''
        self.paintBuffer[1].fill(Qt.transparent)
        self.painter.begin(self.paintBuffer[1])
        self.drawActiveConnections()
        self.painter.end()
        
    def drawGrid(self):
        '''
        Redraw the grid on the background
        '''
        brush = QBrush(QColor("lightgray"))
        pen = QPen(brush, 1.0, Qt.DotLine)
        self.painter.setPen(pen)
        for i in xrange(47):
            self.painter.drawLine(QPointF(self.colUnit * i, 0),
                                  QPointF(self.colUnit * i, self.paintBuffer[0].height()))
        for i in xrange(55):
            self.painter.drawLine(QPointF(0, self.rowUnit * i),
                                  QPointF(self.paintBuffer[0].width(), self.rowUnit * i))
            
    def drawPorts(self):
        '''
        Draw the ports on all components
        '''
        for i in xrange(len(self.ports)):
            for port in self.ports[i][0]:
                self.drawOnePort((self.components[i][1][1] + port[0]) * self.colUnit,
                                 (self.components[i][1][0] + port[1]) * self.rowUnit,
                                 port[2], self.outPortColor)
            for port in self.ports[i][1]:
                self.drawOnePort((self.components[i][1][1] + port[0]) * self.colUnit,
                                 (self.components[i][1][0] + port[1]) * self.rowUnit,
                                 port[2], self.inPortColor)
                
    def drawConnections(self):
        '''
        Draw all connections between components as inactive mode
        '''
        brush = QBrush(QColor("lightgray"))
        pen = QPen(brush, self.edge / 4, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin)
        self.painter.setPen(pen)
        for (_, end) in self.connections.iteritems():
            for (_, line) in end.iteritems():
                rline = line[:]
                for i in xrange(len(line)):
                    rline[i] = QPointF(line[i].x() * self.colUnit,
                                       line[i].y() * self.rowUnit)
                self.painter.drawPolyline(*rline)

    def drawActiveConnections(self):
        '''
        Draw the active connections between components
        '''
        brush = QBrush(QColor("aqua"))
        pen = QPen(brush, self.edge / 4, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin)
        self.painter.setPen(pen)
        for line in self.instList:
            rline = self.connections[line[0]][line[1]][:]
            for i in xrange(len(rline)):
                rline[i] = QPointF(rline[i].x() * self.colUnit,
                                   rline[i].y() * self.rowUnit)
            self.painter.drawPolyline(*rline)
        
    def resizeEvent(self, event):
        if event.oldSize() == event.size():
            return
        self.redraw(self.APCSchemeLayout.contentsRect().size())

    def paintEvent(self, event):
        qp = QPainter()
        qp.begin(self)
        qp.drawPixmap(self.APCSchemeLayout.contentsRect(), self.paintBuffer[0])
        qp.drawPixmap(self.APCSchemeLayout.contentsRect(), self.paintBuffer[1])
        qp.drawPixmap(self.APCSchemeLayout.contentsRect(), self.paintBuffer[2])
        qp.end()


