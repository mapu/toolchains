# -*- coding: utf-8 -*-
from PyQt4.QtCore import SIGNAL, pyqtSlot, QModelIndex, pyqtSignal, Qt
from PyQt4.QtGui import QTableView, QAbstractItemView, QHeaderView,\
    QFont, QComboBox, QHBoxLayout, QVBoxLayout, QWidget
from data.StageTableModel import StageTableModel
from view.APCview.LegendWidget import LegendWidget
from view.APCview.SearchWidget import SearchWidget
from view.APCview.HeaderView import HeaderView
from view.APCview.CellDelegate import CellDelegate

class StageWidget(QWidget):
    updateTimePointSignal = pyqtSignal(int)
    
    def __init__(self, inst_table, stage_table, reg_table, parent = None):
        super(StageWidget, self).__init__(parent)

        #self.resize(1500, 800)
        #self.setMinimumSize(400, 600)
        #add delegate
        self.delegate = CellDelegate()

        self.tableModel = StageTableModel(stage_table, reg_table, self.delegate, self)

        self.tableView = QTableView()

        self.tableView.setItemDelegate(self.delegate)        

        self.tableView.setModel(self.tableModel)
        self.tableView.setFont(QFont("Monospace", 8))
        
        self.headerView = HeaderView(self.tableModel, Qt.Horizontal)
        self.headerView.setDefaultAlignment(Qt.AlignHCenter)
        self.tableView.setHorizontalHeader(self.headerView)

        self.tableView.setSelectionBehavior(QAbstractItemView.SelectColumns)
        self.tableView.setCornerButtonEnabled(False)
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
        
        self.pageCombo = QComboBox()
        self.pageCombo.setFixedSize(100, 25)
        self.connect(self.pageCombo, SIGNAL("currentIndexChanged(int)"),
                     self.tableModel.switchPageSlot)
        # blank = QLabel()
        # blank.setFixedSize(300, 25)
        self.upLayout = QHBoxLayout()
        self.upLayout.addWidget(self.pageCombo)
        # upLay.addWidget(blank)
        self.searchWidget = SearchWidget(self.tableView, self.tableModel)
        self.upLayout.addWidget(self.searchWidget)
        
        # blank1 = QLabel()
        # blank1.setFixedSize(500, 20)
        self.legendWidget = LegendWidget(self.delegate.rwColors)
        # midLayout.addWidget(blank1)
        
        self.mainLayout = QVBoxLayout()
        self.mainLayout.addLayout(self.upLayout)
        self.mainLayout.addWidget(self.legendWidget)
        self.mainLayout.addWidget(self.tableView)
        self.setLayout(self.mainLayout)
        
    @pyqtSlot(int)
    def updateTimePoint(self, index):
        column = index
        text = self.tableModel.hHeaderList[column]
        self.updateTimePointSignal.emit(int(text))

    @pyqtSlot(QModelIndex)
    def updateTimePoint(self, index):
        column = index.column()
        text = self.tableModel.hHeaderList[column]
        self.updateTimePointSignal.emit(int(text))

    @pyqtSlot(int)
    def scrollToStage(self, value):
        '''
        Synchronize the time axis and inst axis to keep the first stage of the
        first inst always showing at the first column
        '''
        self.tableView.horizontalScrollBar().setValue(self.tableModel.getStart(value))

    @pyqtSlot()        
    def updatePageListSlot(self):
        self.pageCombo.clear()
        self.pageCombo.addItems([self.tableModel.stageTable.pageList[i][1]
                                 for i in range(len(self.tableModel.stageTable.pageList))])

    @pyqtSlot()        
    def show(self):
        self.pageCombo.clear()
        self.pageCombo.addItems([self.tableModel.stageTable.pageList[i][1]
                                 for i in range(len(self.tableModel.stageTable.pageList))])
        return QWidget.show(self)

    def closeEvent(self, event):
        self.hide()
        event.ignore()


