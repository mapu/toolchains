# -*- coding: utf-8 -*-
from FloatDialog import FloatDialog
from PyQt4.QtCore import QPointF, SIGNAL, Qt, QRectF, QSizeF
from PyQt4.QtGui import QSizePolicy, QPainterPath, QButtonGroup, QWidget,\
    QPushButton, QGridLayout, QLabel, QColor, QPainter, QBrush, QPen, QPixmap
from QHexEdit.HexMainWindow import HexMainWindow
from nose.util import src

#QTextCodec.setCodecForTr(QTextCodec.codecForName("utf8"))

class MPUCoreWidget(QWidget):
    
    # The gridlayout size is 55 x 47
    
    # All components info
    components = [
        # ["name", [row, col, row span, col span], class]
        ["DM", [25, 0, 5, 5], QPushButton],
        ["BIU0", [10, 10, 5, 5], QPushButton],
        ["BIU1", [25, 10, 5, 5], QPushButton],
        ["BIU2", [40, 10, 5, 5], QPushButton],
        ["SHU0", [0, 20, 5, 5], QPushButton],
        ["SHU1", [50, 20, 5, 5], QPushButton],
        ["MRF", [15, 20, 25, 5], QPushButton],
        ["IALU", [10, 36, 5, 5], QPushButton],
        ["IMAC", [20, 36, 5, 5], QPushButton],
        ["FALU", [30, 36, 5, 5], QPushButton],
        ["FMAC", [40, 36, 5, 5], QPushButton],
        ["I0", [16, 30, 3, 1], QLabel],
        ["I1", [26, 30, 3, 1], QLabel],
        ["I2", [36, 30, 3, 1], QLabel],
        ["O", [26, 46, 3, 1], QLabel]
    ]
    
    # the in/out ports on components
    ports = [ #[outs, ins]
        [[(5.0, 5.0/3, 'R')], [(5.0, 10.0/3, 'L')]], # DM
        [[(0.0, 10.0/3, 'L'), (5.0, 5.0/3, 'R')], [(0.0, 5.0/3, 'R'), (5.0, 10.0/3, 'L')]], # BIU0
        [[(0.0, 10.0/3, 'L'), (5.0, 5.0/3, 'R')], [(0.0, 5.0/3, 'R'), (5.0, 10.0/3, 'L')]], # BIU1
        [[(0.0, 10.0/3, 'L'), (5.0, 5.0/3, 'R')], [(0.0, 5.0/3, 'R'), (5.0, 10.0/3, 'L')]], # BIU2
        [[(0.0, 10.0/3, 'L'), (5.0, 10.0/3, 'R'), (5.0/4, 5.0, 'D')],
         [(0.0, 5.0/3, 'R'), (5.0, 5.0/3, 'L'), (5.0/2, 5.0, 'U'), (15.0/4, 5.0, 'U')]], # SHU0
        [[(0.0, 10.0/3, 'L'), (5.0, 5.0/3, 'R'), (5.0/4, 0.0, 'U')],
         [(0.0, 5.0/3, 'R'), (5.0, 10.0/3, 'L'), (5.0/2, 0.0, 'D'), (15.0/4, 0.0, 'D')]], # SHU1
        [[(0.0, 10.0 + 10.0/3, 'L'), (5.0, 25.0/4, 'R'), (5.0, 50.0/4, 'R'),
          (5.0, 75.0/4, 'R'), (5.0/2, 0.0, 'U'), (15.0/4, 0.0, 'U'),
          (5.0/2, 25.0, 'D'), (15.0/4, 25.0, 'D')],
         [(0.0, 10.0 + 5.0/3, 'R'), (0.0, 5.0 + 5.0/3, 'R'), (0.0, 15.0 + 5.0/3, 'R'), 
          (5.0/4, 0.0, 'D'), (5.0/4, 25.0, 'U')]], # MRF
        [[(5.0, 5.0/4, 'R'), (5.0, 15.0/4, 'R'), (5.0/3, 5.0, 'D')],
         [(0.0, 5.0/2, 'R'), (5.0, 5.0/2, 'L'), (10.0/3, 5.0, 'U')]], # IALU
        [[(5.0, 5.0/2, 'R'), (10.0/3, 0.0, 'U'), (5.0/3, 5.0, 'D')],
         [(0.0, 5.0/2, 'R'), (5.0/3, 0.0, 'D'), (10.0/3, 5.0, 'U')]], # IMAC
        [[(5.0, 10.0/4, 'R'), (5.0, 15.0/4, 'R'), (5.0/3, 5.0, 'D'), (10.0/3, 0.0, 'U')],
         [(0.0, 5.0/2, 'R'), (5.0, 5.0/4, 'L'), (10.0/3, 5.0, 'U'), (5.0/3, 0.0, 'D')]], # FALU
        [[(5.0, 5.0/2, 'R'), (10.0/3, 0.0, 'U')],
         [(0.0, 5.0/2, 'R'), (5.0/3, 0.0, 'D')]], # FMAC
        [[(1.0, 3.0/2, 'R')], [(0, 3.0/2, 'R')]], # I0
        [[(1.0, 3.0/2, 'R')], [(0, 3.0/2, 'R')]], # I1
        [[(1.0, 3.0/2, 'R')], [(0, 3.0/2, 'R')]], # I2
        [[(1.0/2, 3.0, 'D'), (1.0/2, 0, 'U')], [(0, 3.0/2, 'R')]], # O
    ]
    
    #===========================================================================
    # Port to port connections
    #===========================================================================
    DMtoBIU0 = [QPointF(5.0, 25.0 + 5.0/3), QPointF(5.5, 25.0 + 5.0/3),
                QPointF(9.5, 10.0 + 5.0/3), QPointF(10.0, 10.0 + 5.0/3)]
    DMtoBIU1 = [QPointF(5.0, 25.0 + 5.0/3), QPointF(10.0, 25.0 + 5.0/3)]
    DMtoBIU2 = [QPointF(5.0, 25.0 + 5.0/3), QPointF(5.5, 25.0 + 5.0/3),
                QPointF(9.5, 40.0 + 5.0/3), QPointF(10.0, 40.0 + 5.0/3)]
    BIU0toDM = [QPointF(5.0, 25.0 + 10.0/3), QPointF(5.5, 25.0 + 10.0/3),
                QPointF(9.5, 10.0 + 10.0/3), QPointF(10.0, 10.0 + 10.0/3)]
    BIU1toDM = [QPointF(5.0, 25.0 + 10.0/3), QPointF(10.0, 25.0 + 10.0/3)]
    BIU2toDM = [QPointF(5.0, 25.0 + 10.0/3), QPointF(5.5, 25.0 + 10.0/3),
                QPointF(9.5, 40.0 + 10.0/3), QPointF(10.0, 40.0 + 10.0/3)]
    
    BIU0toSHU0 = [QPointF(15.0, 10.0 + 5.0/3), QPointF(18.75, 10.0 + 5.0/3),
                  QPointF(19.0, 9.5 + 5.0/3), QPointF(19.0, 1.0 + 5.0/3),
                  QPointF(19.5, 0.0 + 5.0/3), QPointF(20.0, 0.0 + 5.0/3)]
    BIU1toSHU0 = [QPointF(15.0, 25.0 + 5.0/3), QPointF(18.75, 25.0 + 5.0/3),
                  QPointF(19.0, 24.5 + 5.0/3), QPointF(19.0, 1.0 + 5.0/3),
                  QPointF(19.5, 0.0 + 5.0/3), QPointF(20.0, 0.0 + 5.0/3)]
    BIU2toSHU0 = [QPointF(15.0, 40.0 + 5.0/3), QPointF(18.75, 40.0 + 5.0/3),
                  QPointF(19.0, 39.5 + 5.0/3), QPointF(19.0, 1.0 + 5.0/3),
                  QPointF(19.5, 0.0 + 5.0/3), QPointF(20.0, 0.0 + 5.0/3)]
    SHU0toBIU0 = [QPointF(15.0, 10.0 + 10.0/3), QPointF(19.25, 10.0 + 10.0/3),
                  QPointF(19.5, 9.5 + 10.0/3), QPointF(19.5, 0.5 + 10.0/3),
                  QPointF(19.75, 0.0 + 10.0/3), QPointF(20.0, 0.0 + 10.0/3)]
    SHU0toBIU1 = [QPointF(15.0, 25.0 + 10.0/3), QPointF(19.25, 25.0 + 10.0/3),
                  QPointF(19.5, 24.5 + 10.0/3), QPointF(19.5, 0.5 + 10.0/3),
                  QPointF(19.75, 0.0 + 10.0/3), QPointF(20.0, 0.0 + 10.0/3)]
    SHU0toBIU2 = [QPointF(15.0, 40.0 + 10.0/3), QPointF(19.25, 40.0 + 10.0/3),
                  QPointF(19.5, 39.5 + 10.0/3), QPointF(19.5, 0.5 + 10.0/3),
                  QPointF(19.75, 0.0 + 10.0/3), QPointF(20.0, 0.0 + 10.0/3)]
    
    BIU0toSHU1 = [QPointF(15.0, 10.0 + 5.0/3), QPointF(18.25, 10.0 + 5.0/3),
                  QPointF(18.5, 10.5 + 5.0/3), QPointF(18.5, 49.5 + 5.0/3),
                  QPointF(18.75, 50.0 + 5.0/3), QPointF(20.0, 50.0 + 5.0/3)]
    BIU1toSHU1 = [QPointF(15.0, 25.0 + 5.0/3), QPointF(18.25, 25.0 + 5.0/3),
                  QPointF(18.5, 25.5 + 5.0/3), QPointF(18.5, 49.5 + 5.0/3),
                  QPointF(18.75, 50.0 + 5.0/3), QPointF(20.0, 50.0 + 5.0/3)]
    BIU2toSHU1 = [QPointF(15.0, 40.0 + 5.0/3), QPointF(18.25, 40.0 + 5.0/3),
                  QPointF(18.5, 40.5 + 5.0/3), QPointF(18.5, 49.5 + 5.0/3),
                  QPointF(18.75, 50.0 + 5.0/3), QPointF(20.0, 50.0 + 5.0/3)]
    SHU1toBIU0 = [QPointF(15.0, 10.0 + 10.0/3), QPointF(17.75, 10.0 + 10.0/3),
                  QPointF(18.0, 10.5 + 10.0/3), QPointF(18.0, 49.0 + 10.0/3),
                  QPointF(18.5, 50.0 + 10.0/3), QPointF(20.0, 50.0 + 10.0/3)]
    SHU1toBIU1 = [QPointF(15.0, 25.0 + 10.0/3), QPointF(17.75, 25.0 + 10.0/3),
                  QPointF(18.0, 25.5 + 10.0/3), QPointF(18.0, 49.0 + 10.0/3),
                  QPointF(18.5, 50.0 + 10.0/3), QPointF(20.0, 50.0 + 10.0/3)]
    SHU1toBIU2 = [QPointF(15.0, 40.0 + 10.0/3), QPointF(17.75, 40.0 + 10.0/3),
                  QPointF(18.0, 40.5 + 10.0/3), QPointF(18.0, 49.0 + 10.0/3),
                  QPointF(18.5, 50.0 + 10.0/3), QPointF(20.0, 50.0 + 10.0/3)]
    
    BIU0toM = [QPointF(15.0, 10.0 + 5.0/3), QPointF(18.25, 10.0 + 5.0/3),
               QPointF(18.5, 10.5 + 5.0/3), QPointF(18.5, 19.5 + 5.0/3),
               QPointF(18.75, 20.0 + 5.0/3), QPointF(20.0, 20.0 + 5.0/3)]
    BIU1toM = [QPointF(15.0, 25.0 + 5.0/3), QPointF(20.0, 25.0 + 5.0/3)]
    BIU2toM = [QPointF(15.0, 40.0 + 5.0/3), QPointF(18.75, 40.0 + 5.0/3),
               QPointF(19.0, 39.5 + 5.0/3), QPointF(19.0, 30.5 + 5.0/3),
               QPointF(19.25, 30.0 + 5.0/3), QPointF(20.0, 30.0 + 5.0/3)]
    
    MtoBIU0 = [QPointF(15.0, 10.0 + 10.0/3), QPointF(20.0, 25.0 + 10.0/3)]
    MtoBIU1 = [QPointF(15.0, 25.0 + 10.0/3), QPointF(20.0, 25.0 + 10.0/3)]
    MtoBIU2 = [QPointF(15.0, 40.0 + 10.0/3), QPointF(20.0, 25.0 + 10.0/3)]
    
    SHU0ToM = [QPointF(20.0 + 5.0/4, 5.0), QPointF(20.0 + 5.0/4, 15.0)]
    SHU1ToM = [QPointF(20.0 + 5.0/4, 50.0), QPointF(20.0 + 5.0/4, 40.0)]
    
    MR0toSHU0 = [QPointF(20.0 + 5.0/2, 5.0), QPointF(20.0 + 5.0/2, 15.0)]
    MR0toSHU1 = [QPointF(20.0 + 5.0/2, 50.0), QPointF(20.0 + 5.0/2, 40.0)]
    
    MR1toSHU0 = [QPointF(20.0 + 15.0/4, 5.0), QPointF(20.0 + 15.0/4, 15.0)]
    MR1toSHU1 = [QPointF(20.0 + 15.0/4, 50.0), QPointF(20.0 + 15.0/4, 40.0)]
    
    SHU0ToI0 = [QPointF(20.0, 0.0 + 10.0/3), QPointF(28.75, 0.0 + 10.0/3),
                QPointF(29.0, 0.5 + 10.0/3), QPointF(29.0, 15.5 + 3.0/2),
                QPointF(29.25, 16.0 + 3.0/2), QPointF(30.0, 16.0 + 3.0/2),]
    SHU0ToI1 = [QPointF(20.0, 0.0 + 10.0/3), QPointF(28.25, 0.0 + 10.0/3),
                QPointF(28.5, 0.5 + 10.0/3), QPointF(28.5, 25.5 + 3.0/2),
                QPointF(28.75, 26.0 + 3.0/2), QPointF(30.0, 26.0 + 3.0/2),]
    SHU0ToI2 = [QPointF(20.0, 0.0 + 10.0/3), QPointF(27.75, 0.0 + 10.0/3),
                QPointF(28.0, 0.5 + 10.0/3), QPointF(28.0, 35.5 + 3.0/2),
                QPointF(28.25, 36.0 + 3.0/2), QPointF(30.0, 36.0 + 3.0/2),]
    
    SHU1ToI0 = [QPointF(20.0, 50.0 + 5.0/3), QPointF(28.75, 50.0 + 5.0/3),
                QPointF(29.0, 49.5 + 5.0/3), QPointF(29.0, 16.5 + 3.0/2),
                QPointF(29.25, 16.0 + 3.0/2), QPointF(30.0, 16.0 + 3.0/2),]
    SHU1ToI1 = [QPointF(20.0, 50.0 + 5.0/3), QPointF(28.25, 50.0 + 5.0/3),
                QPointF(28.5, 49.5 + 5.0/3), QPointF(28.5, 26.5 + 3.0/2),
                QPointF(28.75, 26.0 + 3.0/2), QPointF(30.0, 26.0 + 3.0/2),]
    SHU1ToI2 = [QPointF(20.0, 50.0 + 5.0/3), QPointF(27.75, 50.0 + 5.0/3),
                QPointF(28.0, 49.5 + 5.0/3), QPointF(28.0, 36.5 + 3.0/2),
                QPointF(28.25, 36.0 + 3.0/2), QPointF(30.0, 36.0 + 3.0/2),]
    
    MR0toI0 = [QPointF(25.0, 15.0 + 25.0/4), QPointF(25.5, 15.0 + 25.0/4),
               QPointF(27.5, 16.0 + 3.0/2), QPointF(30.0, 16.0 + 3.0/2)]
    MR0toI1 = [QPointF(25.0, 15.0 + 25.0/4), QPointF(25.5, 15.0 + 25.0/4),
               QPointF(27.5, 26.0 + 3.0/2), QPointF(30.0, 26.0 + 3.0/2)]
    MR0toI2 = [QPointF(25.0, 15.0 + 25.0/4), QPointF(25.5, 15.0 + 25.0/4),
               QPointF(27.5, 36.0 + 3.0/2), QPointF(30.0, 36.0 + 3.0/2)]
    
    MR1toI0 = [QPointF(25.0, 15.0 + 25.0/2), QPointF(25.5, 15.0 + 25.0/2),
               QPointF(27.5, 16.0 + 3.0/2), QPointF(30.0, 16.0 + 3.0/2)]
    MR1toI1 = [QPointF(25.0, 15.0 + 25.0/2), QPointF(25.5, 15.0 + 25.0/2),
               QPointF(27.5, 26.0 + 3.0/2), QPointF(30.0, 26.0 + 3.0/2)]
    MR1toI2 = [QPointF(25.0, 15.0 + 25.0/2), QPointF(25.5, 15.0 + 25.0/2),
               QPointF(27.5, 36.0 + 3.0/2), QPointF(30.0, 36.0 + 3.0/2)]
    
    MR2toI0 = [QPointF(25.0, 15.0 + 75.0/4), QPointF(25.5, 15.0 + 75.0/4),
               QPointF(27.5, 16.0 + 3.0/2), QPointF(30.0, 16.0 + 3.0/2)]
    MR2toI1 = [QPointF(25.0, 15.0 + 75.0/4), QPointF(25.5, 15.0 + 75.0/4),
               QPointF(27.5, 26.0 + 3.0/2), QPointF(30.0, 26.0 + 3.0/2)]
    MR2toI2 = [QPointF(25.0, 15.0 + 75.0/4), QPointF(25.5, 15.0 + 75.0/4),
               QPointF(27.5, 36.0 + 3.0/2), QPointF(30.0, 36.0 + 3.0/2)]
    
    BIU0ToI0 = [QPointF(15.0, 10.0 + 5.0/3), QPointF(28.75, 10.0 + 5.0/3),
                QPointF(29.0, 10.5 + 5.0/3), QPointF(29.0, 15.5 + 3.0/2),
                QPointF(29.25, 16.0 + 3.0/2), QPointF(30, 16.0 + 3.0/2)]
    BIU0ToI1 = [QPointF(15.0, 10.0 + 5.0/3), QPointF(28.25, 10.0 + 5.0/3),
                QPointF(28.5, 10.5 + 5.0/3), QPointF(28.5, 25.5 + 3.0/2),
                QPointF(28.75, 26.0 + 3.0/2), QPointF(30, 26.0 + 3.0/2)]
    BIU0ToI2 = [QPointF(15.0, 10.0 + 5.0/3), QPointF(27.75, 10.0 + 5.0/3),
                QPointF(28.0, 10.5 + 5.0/3), QPointF(28.0, 35.5 + 3.0/2),
                QPointF(28.25, 36.0 + 3.0/2), QPointF(30, 36.0 + 3.0/2)]
    
    BIU1ToI0 = [QPointF(15.0, 25.0 + 5.0/3), QPointF(17.25, 25.0 + 5.0/3),
                QPointF(17.5, 25.5 + 5.0/3), QPointF(17.5, 44.5 + 5.0/3),
                QPointF(17.75, 45.0 + 5.0/3), QPointF(28.75, 45.0 + 5.0/3),
                QPointF(29.0, 44.5 + 5.0/3), QPointF(29.0, 16.5 + 3.0/2),
                QPointF(29.25, 16.0 + 3.0/2), QPointF(30, 16.0 + 3.0/2)]
    BIU1ToI1 = [QPointF(15.0, 25.0 + 5.0/3), QPointF(17.25, 25.0 + 5.0/3),
                QPointF(17.5, 25.5 + 5.0/3), QPointF(17.5, 44.5 + 5.0/3),
                QPointF(17.75, 45.0 + 5.0/3), QPointF(28.25, 45.0 + 5.0/3),
                QPointF(28.5, 44.5 + 5.0/3), QPointF(28.5, 26.5 + 3.0/2),
                QPointF(28.75, 26.0 + 3.0/2), QPointF(30, 26.0 + 3.0/2)]
    BIU1ToI2 = [QPointF(15.0, 25.0 + 5.0/3), QPointF(17.25, 25.0 + 5.0/3),
                QPointF(17.5, 25.5 + 5.0/3), QPointF(17.5, 44.5 + 5.0/3),
                QPointF(17.75, 45.0 + 5.0/3), QPointF(27.75, 45.0 + 5.0/3),
                QPointF(28.0, 44.5 + 5.0/3), QPointF(28.0, 36.5 + 3.0/2),
                QPointF(28.25, 36.0 + 3.0/2), QPointF(30, 36.0 + 3.0/2)]
    
    BIU2ToI0 = [QPointF(15.0, 40.0 + 5.0/3), QPointF(28.75, 40.0 + 5.0/3),
                QPointF(29.0, 39.5 + 5.0/3), QPointF(29.0, 16.5 + 3.0/2),
                QPointF(29.25, 16.0 + 3.0/2), QPointF(30, 16.0 + 3.0/2)]
    BIU2ToI1 = [QPointF(15.0, 40.0 + 5.0/3), QPointF(28.25, 40.0 + 5.0/3),
                QPointF(28.5, 39.5 + 5.0/3), QPointF(28.5, 26.5 + 3.0/2),
                QPointF(28.75, 26.0 + 3.0/2), QPointF(30, 26.0 + 3.0/2),]
    BIU2ToI2 = [QPointF(15.0, 40.0 + 5.0/3), QPointF(27.75, 40.0 + 5.0/3),
                QPointF(28.0, 39.5 + 5.0/3), QPointF(28.0, 36.5 + 3.0/2),
                QPointF(28.25, 36.0 + 3.0/2), QPointF(30, 36.0 + 3.0/2)]
    
    I0toIALU = [QPointF(31.0, 16.0 + 3.0/2), QPointF(31.5, 16.0 + 3.0/2),
                QPointF(35.5, 10.0 + 5.0/2), QPointF(36.0, 10.0 + 5.0/2)]
    I0toIMAC = [QPointF(31.0, 16.0 + 3.0/2), QPointF(31.5, 16.0 + 3.0/2),
                QPointF(35.5, 20.0 + 5.0/2), QPointF(36.0, 20.0 + 5.0/2)]
    I0toFALU = [QPointF(31.0, 16.0 + 3.0/2), QPointF(31.5, 16.0 + 3.0/2),
                QPointF(35.5, 30.0 + 5.0/2), QPointF(36.0, 30.0 + 5.0/2)]
    I0toFMAC = [QPointF(31.0, 16.0 + 3.0/2), QPointF(31.5, 16.0 + 3.0/2),
                QPointF(35.5, 40.0 + 5.0/2), QPointF(36.0, 40.0 + 5.0/2)]
    
    I1toIALU = [QPointF(31.0, 26.0 + 3.0/2), QPointF(31.5, 26.0 + 3.0/2),
                QPointF(35.5, 10.0 + 5.0/2), QPointF(36.0, 10.0 + 5.0/2)]
    I1toIMAC = [QPointF(31.0, 26.0 + 3.0/2), QPointF(31.5, 26.0 + 3.0/2),
                QPointF(35.5, 20.0 + 5.0/2), QPointF(36.0, 20.0 + 5.0/2)]
    I1toFALU = [QPointF(31.0, 26.0 + 3.0/2), QPointF(31.5, 26.0 + 3.0/2),
                QPointF(35.5, 30.0 + 5.0/2), QPointF(36.0, 30.0 + 5.0/2)]
    I1toFMAC = [QPointF(31.0, 26.0 + 3.0/2), QPointF(31.5, 26.0 + 3.0/2),
                QPointF(35.5, 40.0 + 5.0/2), QPointF(36.0, 40.0 + 5.0/2)]
    
    I2toIALU = [QPointF(31.0, 36.0 + 3.0/2), QPointF(31.5, 36.0 + 3.0/2),
                QPointF(35.5, 10.0 + 5.0/2), QPointF(36.0, 10.0 + 5.0/2)]
    I2toIMAC = [QPointF(31.0, 36.0 + 3.0/2), QPointF(31.5, 36.0 + 3.0/2),
                QPointF(35.5, 20.0 + 5.0/2), QPointF(36.0, 20.0 + 5.0/2)]
    I2toFALU = [QPointF(31.0, 36.0 + 3.0/2), QPointF(31.5, 36.0 + 3.0/2),
                QPointF(35.5, 30.0 + 5.0/2), QPointF(36.0, 30.0 + 5.0/2)]
    I2toFMAC = [QPointF(31.0, 36.0 + 3.0/2), QPointF(31.5, 36.0 + 3.0/2),
                QPointF(35.5, 40.0 + 5.0/2), QPointF(36.0, 40.0 + 5.0/2)]
    
    IALUToIMAC = [QPointF(36.0 + 5.0/3, 15.0), QPointF(36.0 + 5.0/3, 20.0)]
    IMACToIALU = [QPointF(36.0 + 10.0/3, 15.0), QPointF(36.0 + 10.0/3, 20.0)]
    
    IMACToFALU = [QPointF(36.0 + 5.0/3, 25.0), QPointF(36.0 + 5.0/3, 30.0)]
    FALUToIMAC = [QPointF(36.0 + 10.0/3, 25.0), QPointF(36.0 + 10.0/3, 30.0)]
    
    FALUToFMAC = [QPointF(36.0 + 5.0/3, 35.0), QPointF(36.0 + 5.0/3, 40.0)]
    FMACToFALU = [QPointF(36.0 + 10.0/3, 35.0), QPointF(36.0 + 10.0/3, 40.0)]
    
    IALUToFALU = [QPointF(41.0, 10.0 + 15.0/4), QPointF(41.75, 10.0 + 15.0/4),
                  QPointF(42.0, 10.5 + 15.0/4), QPointF(42.0, 29.5 + 5.0/4),
                  QPointF(41.75, 30.0 + 5.0/4), QPointF(41.0, 30.0 + 5.0/4)]
    FALUToIALU = [QPointF(41.0, 10.0 + 10.0/4), QPointF(42.25, 10.0 + 10.0/4),
                  QPointF(42.5, 10.5 + 10.0/4), QPointF(42.5, 29.5 + 10.0/4),
                  QPointF(42.0, 30.0 + 10.0/4), QPointF(41.0, 30.0 + 10.0/4)]
    
    IALUToO = [QPointF(41.0, 10.0 + 5.0/4), QPointF(43.0, 10.0 + 5.0/4),
               QPointF(45.5, 26.0 + 3.0/2), QPointF(46.0, 26.0 + 3.0/2)]
    IMACToO = [QPointF(41.0, 20.0 + 5.0/2), QPointF(43.0, 20.0 + 5.0/2),
               QPointF(45.5, 26.0 + 3.0/2), QPointF(46.0, 26.0 + 3.0/2)]
    FALUToO = [QPointF(41.0, 30.0 + 15.0/4), QPointF(43.0, 30.0 + 15.0/4),
               QPointF(45.5, 26.0 + 3.0/2), QPointF(46.0, 26.0 + 3.0/2)]
    FMACToO = [QPointF(41.0, 40.0 + 5.0/2), QPointF(43.0, 40.0 + 5.0/2),
               QPointF(45.5, 26.0 + 3.0/2), QPointF(46.0, 26.0 + 3.0/2)]
    
    OToSHU0 = [QPointF(20.0, 0.0 + 5.0/3), QPointF(45.75 + 1.0/2, 0.0 + 5.0/3),
               QPointF(46.0 + 1.0/2, 0.5 + 5.0/3), QPointF(46.0 + 1.0/2, 26.0)]
    OToSHU1 = [QPointF(20.0, 50.0 + 10.0/3), QPointF(45.75 + 1.0/2, 50.0 + 10.0/3),
               QPointF(46.0 + 1.0/2, 49.5 + 10.0/3), QPointF(46.0 + 1.0/2, 29.0)]
    
    OToBIU0 = [QPointF(15.0, 10.0 + 5.0/3), QPointF(16.75, 10.0 + 5.0/3),
               QPointF(17.0, 9.5 + 5.0/3), QPointF(17.0, 8.0),
               QPointF(17.25, 7.5), QPointF(45.75 + 1.0/2, 7.5),
               QPointF(46.0 + 1.0/2, 8.0), QPointF(46.0 + 1.0/2, 26.0)]
    OToBIU1 = [QPointF(15.0, 25.0 + 5.0/3), QPointF(16.75, 25.0 + 5.0/3),
               QPointF(17.0, 24.5 + 5.0/3), QPointF(17.0, 8.0),
               QPointF(17.25, 7.5), QPointF(45.75 + 1.0/2, 7.5),
               QPointF(46.0 + 1.0/2, 8.0), QPointF(46.0 + 1.0/2, 26.0)]
    OToBIU2 = [QPointF(15.0, 40.0 + 5.0/3), QPointF(16.75, 40.0 + 5.0/3),
               QPointF(17.0, 39.5 + 5.0/3), QPointF(17.0, 8.0),
               QPointF(17.25, 7.5), QPointF(45.75 + 1.0/2, 7.5),
               QPointF(46.0 + 1.0/2, 8.0), QPointF(46.0 + 1.0/2, 26.0)]   
    OToM = [QPointF(20.0 + 5.0/4, 15.0), QPointF(20.0 + 5.0/4, 8.0),
            QPointF(20.25 + 5.0/4, 7.5), QPointF(45.75 + 1.0/2, 7.5),
            QPointF(46.0 + 1.0/2, 8.0), QPointF(46.0 + 1.0/2, 26.0)]
    
    
    connections = {
        "DM": {"BIU0": DMtoBIU0, "BIU1": DMtoBIU1, "BIU2": DMtoBIU2, },
        "BIU0": {"DM": BIU0toDM, "SHU0": BIU0toSHU0, "SHU1": BIU0toSHU1, "M": BIU0toM,
                 "I0": BIU0ToI0, "I1": BIU0ToI1, "I2": BIU0ToI2,},
        "BIU1": {"DM": BIU1toDM, "SHU0": BIU1toSHU0, "SHU1": BIU1toSHU1, "M": BIU1toM,
                 "I0": BIU1ToI0, "I1": BIU1ToI1, "I2": BIU1ToI2,},
        "BIU2": {"DM": BIU2toDM, "SHU0": BIU2toSHU0, "SHU1": BIU2toSHU1, "M": BIU2toM},
        "SHU0": {"BIU0": SHU0toBIU0, "BIU1": SHU0toBIU1, "BIU2": SHU0toBIU2, "M": SHU0ToM,
                 "I0": SHU0ToI0, "I1": SHU0ToI1, "I2": SHU0ToI2,},
        "SHU1": {"BIU0": SHU1toBIU0, "BIU1": SHU1toBIU1, "BIU2": SHU1toBIU2, "M": SHU1ToM,
                 "I0": SHU1ToI0, "I1": SHU1ToI1, "I2": SHU1ToI2,},
        "MR3" : {"BIU0": MtoBIU0, "BIU1": MtoBIU1, "BIU2": MtoBIU2},
        "MR3K" : {"BIU0": MtoBIU0, "BIU1": MtoBIU1, "BIU2": MtoBIU2},
        "MR0": {"SHU0": MR0toSHU0, "SHU1": MR0toSHU1, "I0": MR0toI0, "I1": MR0toI1, "I2": MR0toI2, },
        "MR1": {"SHU0": MR1toSHU0, "SHU1": MR1toSHU1, "I0": MR1toI0, "I1": MR1toI1, "I2": MR1toI2, },
        "MR2": {"I0": MR2toI0, "I1": MR2toI1, "I2": MR2toI2, },
        "MR2W": {"I0": MR2toI0, "I1": MR2toI1, "I2": MR2toI2, },
        "IALU": {"IMAC": IALUToIMAC, "FALU": IALUToFALU, "O": IALUToO},
        "IMAC": {"IALU": IMACToIALU, "FALU": IMACToFALU, "O": IMACToO},
        "FALU": {"IMAC": FALUToIMAC, "FMAC": FALUToFMAC, "IALU": FALUToIALU, "O": FALUToO},
        "FMAC": {"FALU": FMACToFALU, "O": FMACToO},
        "I0": {"IALU": I0toIALU, "IMAC": I0toIMAC, "FALU": I0toFALU, "FMAC": I0toFMAC, },
        "I1": {"IALU": I1toIALU, "IMAC": I1toIMAC, "FALU": I1toFALU, "FMAC": I1toFMAC, },
        "I2": {"IALU": I2toIALU, "IMAC": I2toIMAC, "FALU": I2toFALU, "FMAC": I2toFMAC, },
        "O": {"SHU0": OToSHU0, "SHU1": OToSHU1, "BIU0": OToBIU0, "BIU1": OToBIU1, "BIU2": OToBIU2, 
              "M": OToM, }
    }
    
    def __init__(self, inst_table, parent = None):
        super(MPUCoreWidget, self).__init__(parent)        
        self.gridLayout = QGridLayout()
        
        self.comButtons = []
        
        self.instList = []
        self.instTable = inst_table
        
        sizePolicy = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum);
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)

        self.comGroup = QButtonGroup()
        
        for com in self.components:
            if com[2] is QPushButton:
                self.comButtons.append(com[2](self.tr(com[0])))
                self.comButtons[-1].setCheckable(True)
                self.comButtons[-1].setSizePolicy(sizePolicy)
                self.comButtons[-1].setStyleSheet('''QPushButton {
                        background: qradialgradient(cx:0, cy:0, radius: 1,
                                                    fx:0.5, fy:0.5, stop:0 white,
                                                    stop:1 lightblue);
                        border-style: solid;
                        border-width: 1px;
                        border-radius: 0px;
                        border-color: blue;
                        margin: 0px;
                        padding: 0px;
                    }
                    QPushButton:checked {
                        background: qradialgradient(cx:0, cy:0, radius: 1,
                                                    fx:0.5, fy:0.5, stop:0 lightblue,
                                                    stop:1 white);
                        border-style: solid;
                        border-width: 2px;
                        border-radius: 0px;
                        border-color: darkblue;
                        margin: 0px;
                        padding: 0px;
                    };
                ''')
                self.comGroup.addButton(self.comButtons[-1], len(self.comButtons) - 1)
                self.gridLayout.addWidget(self.comButtons[-1], *com[1])
            elif com[2] is QLabel:
                label = com[2](self.tr(com[0]))
                label.setAlignment(Qt.AlignCenter)
                label.setSizePolicy(sizePolicy)
                label.setStyleSheet('''
                    background: qradialgradient(cx:0, cy:0, radius: 1,
                                                fx:0.5, fy:0.5, stop:0 white,
                                                stop:1 lightblue);
                    border-style: solid;
                    border-width: 1px;
                    border-radius: 0px;
                    border-color: blue;
                    margin: 0px;
                    padding: 0px;
                ''')
                self.gridLayout.addWidget(label, *com[1])
        
        self.connect(self.comGroup, SIGNAL("buttonPressed(int)"), self.comBtnSlot)
        self.comGroup.setExclusive(False)
                
        #=======================================================================
        # Here constrains the gridlayout to help calulate the relative
        # coordinates which will used in drawing connections
        # 1. Set all rows as the same stretch 1
        # 2. Set all columes as the same stretch 1
        # 3. Set all spacing as 0
        #=======================================================================
        for i in xrange(self.gridLayout.columnCount()):
            self.gridLayout.setColumnStretch(i, 1)
        for i in xrange(self.gridLayout.rowCount()):
            self.gridLayout.setRowStretch(i, 1)
            
        self.gridLayout.setHorizontalSpacing(0)
        self.gridLayout.setVerticalSpacing(0)
        self.gridLayout.setMargin(0)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        
        self.inPortColor = QColor("red")
        self.outPortColor = QColor("green")
        self.setLayout(self.gridLayout)
        
        self.buttonDialogs = []
        self.buttonDialogs.append(HexMainWindow(self))
        self.buttonDialogs[0].setWindowTitle("Local memory")
        
        for i in xrange(1, len(self.comButtons) - 1):
            self.buttonDialogs.append(FloatDialog(self))
            self.buttonDialogs[-1].setWindowTitle("%s stages" %
                                                  self.components[i][0])
            self.buttonDialogs[-1].hide()
            
        self.hexFile = ""
        self.start = 0
        
        
    def comBtnSlot(self, idx):
        if self.comGroup.button(idx).isChecked():
            self.buttonDialogs[idx].hide()
        else:
            if idx == 0:
                self.buttonDialogs[idx].loadFile(self.hexFile, self.start)
            self.buttonDialogs[idx].show()
        
    def timeChangedSlot(self, time):
        self.instList = self.instTable.getInstSrcDest(time)

    def setHexFileStart(self, fileName, start):
        self.hexFile = fileName
        self.start = start
            
    def drawOnePort(self, x, y, direction, color):
        path = QPainterPath();
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
        self.paintBuffer = QPixmap(size)
        self.painter = QPainter(self.paintBuffer)
        self.painter.fillRect(QRectF(QPointF(0, 0), QSizeF(size)),
                              QBrush(QColor("white")))
        self.rowUnit = size.height() / 55.0
        self.colUnit = size.width() / 47.0
        self.edge = self.rowUnit / 0.8
        self.drawGrid()
        self.drawConnections()
        self.drawPorts()
        self.painter.end()
        
    def drawGrid(self):
        brush = QBrush(QColor("grey"))
        pen = QPen(brush, 1.0, Qt.DotLine)
        self.painter.setPen(pen)
        for i in xrange(47):
            self.painter.drawLine(QPointF(self.colUnit * i, 0),
                                  QPointF(self.colUnit * i, self.paintBuffer.height()))
        for i in xrange(55):
            self.painter.drawLine(QPointF(0, self.rowUnit * i),
                                  QPointF(self.paintBuffer.width(), self.rowUnit * i))
            
    def drawPorts(self):
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
        brush = QBrush(QColor("gray"))
        pen = QPen(brush, self.edge / 4, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin)
        self.painter.setPen(pen)
        for (src, end) in self.connections.iteritems():
            for (_, line) in end.iteritems():
                rline = line[:]
                for i in xrange(len(line)):
                    rline[i] = QPointF(line[i].x() * self.colUnit,
                                       line[i].y() * self.rowUnit)
                self.painter.drawPolyline(*rline)
        
    def resizeEvent(self, event):
        if event.oldSize() == event.size():
            return
        self.redraw(self.gridLayout.contentsRect().size())

    def paintEvent(self, event):
        qp = QPainter()
        qp.begin(self)
        qp.drawPixmap(self.gridLayout.contentsRect(), self.paintBuffer)
        brush = QBrush(QColor("yellow"))
        pen = QPen(brush, self.edge / 4, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin)
        qp.setPen(pen)
        for line in self.instList:
            rline = self.connections[line[0]][line[1]][:]
            for i in xrange(len(rline)):
                rline[i] = QPointF(rline[i].x() * self.colUnit,
                                   rline[i].y() * self.rowUnit)
            qp.drawPolyline(*rline)
        qp.end()


