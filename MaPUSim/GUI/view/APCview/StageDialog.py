# -*- coding: utf-8 -*-
from PyQt4.QtCore import SIGNAL, pyqtSlot, QModelIndex, pyqtSignal, Qt
from PyQt4.QtGui import QDialog, QTableView, QAbstractItemView, QHeaderView,\
    QFont, QComboBox, QLabel, QHBoxLayout, QVBoxLayout
from data.StageTableModel import StageTableModel
from view.APCview.LegendWidget import LegendWidget
from view.APCview.SearchWidget import SearchWidget
from view.APCview.HeaderView import HeaderView

class StageDialog(QDialog):
    updateTimePointSignal = pyqtSignal(int)
    
    def __init__(self, inst_table, stage_table, reg_table, parent = None):
        super(StageDialog, self).__init__(parent)

        #self.resize(1500, 800)
        #self.setMinimumSize(400, 600)
        self.tableModel = StageTableModel(stage_table, reg_table, self)

        self.tableView = QTableView()
        self.tableView.setModel(self.tableModel)
        self.tableView.setFont(QFont("Monospace", 8))
        
        self.headerView = HeaderView(self.tableModel, Qt.Horizontal)
        self.headerView.setDefaultAlignment(Qt.AlignHCenter)
        self.tableView.setHorizontalHeader(self.headerView)

        self.tableView.setSelectionBehavior(QAbstractItemView.SelectColumns)
        self.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.tableView.verticalHeader().setResizeMode(QHeaderView.Fixed)
        self.tableView.verticalHeader().setHighlightSections(False)
        self.tableView.verticalHeader().setFont(QFont("Monospace", 10))
        self.rowHeight = 20
        self.tableView.verticalHeader().setDefaultSectionSize(self.rowHeight)
        self.columnWidth = 22
        self.tableView.horizontalHeader().setDefaultSectionSize(self.columnWidth)
        
        self.connect(self.tableView.horizontalHeader(),
                     SIGNAL("sectionClicked(int)"), self.updateTimePoint)
        self.connect(self.tableView, SIGNAL("clicked(QModelIndex)"),
                     self.updateTimePoint)
        self.connect(self.tableView.verticalScrollBar(),
                     SIGNAL("valueChanged(int)"), self.scrollToStage)
        
        self.slider = 0
        self.pageCombo = QComboBox()
        self.pageCombo.setFixedSize(100, 25)
        self.connect(self.pageCombo, SIGNAL("currentIndexChanged(int)"),
                     self.tableModel.switchPageSlot)
        blank = QLabel()
        blank.setFixedSize(300, 25)
        upLay = QHBoxLayout()
        upLay.addWidget(self.pageCombo)
        upLay.addWidget(blank)
        self.searchWidget = SearchWidget(["pc", "dis"], inst_table,
                                         self.tableView, self.tableModel)
        upLay.addWidget(self.searchWidget)
        
        blank1 = QLabel()
        blank1.setFixedSize(500, 20)
        self.legendWidget = LegendWidget()
        
        midLay = QHBoxLayout()
        midLay.addWidget(self.legendWidget)
        midLay.addWidget(blank1)
        
        mainLay = QVBoxLayout()
        mainLay.addLayout(upLay)
        mainLay.addLayout(midLay)
        mainLay.addWidget(self.tableView)
        self.setLayout(mainLay)
        
    @pyqtSlot(int)
    def updateTimePoint(self, index):
        column = index
        text = self.tableModel.hHeaderList[column]
        text = text.replace('\n', "")
        self.updateTimePointSignal.emit(int(text))

    @pyqtSlot(QModelIndex)
    def updateTimePoint(self, index):
        column = index.column()
        text = self.tableModel.hHeaderList[column]
        text = text.replace('\n', "")
        self.updateTimePointSignal.emit(int(text))

    @pyqtSlot(int)
    def scrollToStage(self, value):
        '''
        Synchronize the time axis and inst axis to keep the first stage of the
        first inst always showing at the first column
        '''
        self.tableView.horizontalScrollBar().setValue(self.tableModel.getStart(value))

    @pyqtSlot()        
    def show(self):
        self.pageCombo.clear()
        self.pageCombo.addItems([self.tableModel.stageTable.pageList[i][1]
                                 for i in range(len(self.tableModel.stageTable.pageList))])
        return QDialog.show(self)

    def closeEvent(self, event):
        self.hexEdit._cursorTimer.stop()
        self.optionsDialog.close()
        self.searchDialog.close()
        self.hide()
        event.ignore()


