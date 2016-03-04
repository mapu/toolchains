#!/usr/bin/env python
from PyQt4.QtCore import Qt
from PyQt4.QtGui import QApplication
from argparse import ArgumentParser
from control.Simulation import Simulation
from data.SimConfig import SimConfig
from view.MainWindow import MainWindow
from view.Splash import SplashScreen
from view.Utils import initParent
import sys
from data import StageNames, RegNames

app = QApplication(sys.argv)

splash = SplashScreen()
app.processEvents()
splash.show()
splash.showMessage("Loading global constants ...", Qt.AlignBottom | Qt.AlignLeft,
                   Qt.white)
StageNames.initStageName()
RegNames.initRegName()
splash.showMessage("Loading configuration ...", Qt.AlignBottom | Qt.AlignLeft,
                   Qt.white)
SimulationConfig = SimConfig(xmlfile = "config.xml")
splash.showMessage("Loading simulators ...", Qt.AlignBottom | Qt.AlignLeft,
                   Qt.white)
SimulationControl = Simulation(SimulationConfig);
splash.showMessage("Loading GUI elements ...", Qt.AlignBottom | Qt.AlignLeft,
                   Qt.white)
mainWindow = MainWindow(SimulationConfig, SimulationControl)
initParent(mainWindow)

splash.showMessage("Loading arguments ...", Qt.AlignBottom | Qt.AlignLeft,
                   Qt.white)
parser = ArgumentParser(description = "MaPU simulator GUI")
parser.add_argument("-t", "--trace", dest="tracefile",
                    help="Output trace to FILE", metavar="FILE")
parser.add_argument("-i", "--image", dest="image",
                    help="Use IMAGE to run simulation immediately after GUI start",
                    metavar="IMAGE")
SimulationArgs = parser.parse_args()
# Window size adaption
width = min(QApplication.desktop().width(), 1000)
height = min(QApplication.desktop().height(), 600)
mainWindow.resize(width, height)
pos_x = (QApplication.desktop().width() - mainWindow.width()) / 2
pos_y = (QApplication.desktop().height() - mainWindow.height()) / 2
mainWindow.move(pos_x, pos_y)

mainWindow.show()
# This is ugly code. the layout has to be disabled after mainwidow show up

# Determine what is going to do after GUI start up
if SimulationArgs.image != None:
    SimulationConfig.setConfig("flashimage", SimulationArgs.image)
    if SimulationArgs.tracefile != None:
        SimulationConfig.setConfig("flashimage", SimulationArgs.tracefile)    
    splash.showMessage("Starting simulation ...", Qt.AlignBottom | Qt.AlignLeft,
                       Qt.white)
    mainWindow.configWidget.startProcess()
elif SimulationArgs.tracefile != None:    
    splash.showMessage("Analyzing trace ...", Qt.AlignBottom | Qt.AlignLeft,
                       Qt.white)
# Keep splash screen for a certain period

splash.finish(mainWindow)
sys.exit(app.exec_())
