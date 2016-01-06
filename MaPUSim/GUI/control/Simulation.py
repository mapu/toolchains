'''
Created on Dec 17, 2015

@author: wangl
'''
from PyQt4.QtCore import QObject, QProcess, SIGNAL
from control.Gem5Process import ARMGem5Process, APCGem5Process
from control.QemuProcess import ARMQemuProcess
from view.Utils import fatal, warning
import os
import shutil
from data.SimDB import SimDB


class Simulation(QObject):
    '''
    This module controls the entire simulation process 
    '''


    def __init__(self, config):
        '''
        Constructor
        '''
        super(Simulation, self).__init__()
        self.APCProcess = APCGem5Process()
        self.ARMGem5Process = ARMGem5Process()
        self.ARMQemuProcess = ARMQemuProcess()
        self.config = config
        if self.config.getConfig("ARMSimType") == "QEMU":
            self.ARMProcess = self.ARMQemuProcess
        else:
            self.ARMProcess = self.ARMGem5Process
        self.simDB = SimDB(self)
        self.connect(self.APCProcess, SIGNAL("finished(int, QProcess::ExitStatus)"), 
                     self.analyzeTrace)
    
    def start(self):
        '''
        Start the simulation
        '''
        path = self.config.getConfig("simulatorpath")
                                 
        APCSimulatorFile = path + "/apc/gem5.opt"
        if not os.path.isfile(APCSimulatorFile):
            fatal(self.tr("Cannot find APC simulator %s!" % APCSimulatorFile), 
                  self.tr("Failed to launch the simulation"))
            return False
                                 
        if self.config.getConfig("isFullsystem") == "True":
            image = self.config.getConfig("flashimage")
            if not os.path.isfile(image):
                    fatal(self.tr("Cannot find flash image %s!" % ARMSimulatorFile),
                          self.tr("Failed to launch the simulation"))
                    return False
            if self.config.getConfig("ARMSimType") == "QEMU":
                ARMSimulatorFile = path + "/arm/bin/qemu-system-arm"
                if not os.path.isfile(ARMSimulatorFile):
                    fatal(self.tr("Cannot find ARM simulator %s!" % ARMSimulatorFile),
                          self.tr("Failed to launch the simulation"))
                    return False
                args = ["-M", "mapu", "-m", "512", "-pflash", image,
                        "-serial", "stdio", ""]
                self.ARMProcess.start(ARMSimulatorFile, args)
                
            elif self.config.getConfig("ARMSimType") == "GEM5":
                ARMSimulatorFile = path + "/arm/gem5.opt"
                if not os.path.isfile(ARMSimulatorFile):
                    fatal(self.tr("Cannot find ARM simulator %s!" % ARMSimulatorFile),
                          self.tr("Failed to launch the simulation"))
                    return False
                if not os.path.isfile(path + "/arm/system/fs.py"):
                    fatal(self.tr("Cannot find simulator file %s!" %
                             path + "/arm/system/fs.py"),
                          self.tr("Failed to launch the simulation"))
                    return False
                sim_command = ARMSimulatorFile + " "
                sim_command += path + "/arm/system/fs.py --bare-metal "
                sim_command += "--machine-type=MaPU_Board"
                if not os.path.exists("./images"):
                    os.makedirs("./images")
                shutil.copy2(image, "./images/image.bin")
                self.ARMProcess.start(sim_command)
                if not self.ARMProcess.waitForStarted():
                    return False
            else:
                fatal(self.tr("Unknown ARM simulator type!"),
                      self.tr("Failed to launch the simulation"))
                return False
            # Prepare to run APC simulator
            tracefile = self.config.getConfig("fulltrace")
            if ((tracefile == "") or (tracefile == None)):
                tracefile = "full.trace"
                self.config.setConfig("fulltrace", tracefile)
                warning(self.tr("Will use default trace file name %s" % tracefile),
                        self.tr("Trace file is not set"))
            sim_command = APCSimulatorFile + " "
            sim_command += "--debug-flags=MapuGUI,MapuMem "
            sim_command += "--trace-file=" + tracefile + " "
            if not os.path.isfile(path + "/apc/system/ms.py"):
                fatal(self.tr("Cannot find simulator file %s!" %
                         path + "/apc/system/ms.py"),
                      self.tr("Failed to launch the simulation"))
                return False
            
            commkeys = self.ARMProcess.getCommKeys()
            print commkeys
            if commkeys == None:
                self.ARMProcess.tryTerminate()
                fatal(self.tr("Cannot get communication keys from ARM simulator"),
                      self.tr("Failed to launch the simulation"))
                return False
            if self.config.getConfig("ARMSimType") == "GEM5":
                self.ARMProcess.signalUARTStart.emit(commkeys[2])
            sim_command += path + ("/apc/system/ms.py -c %d -k %d -n 4" %
                                   (commkeys[1], commkeys[0]))
        else: # Standalone mode
            tracefile = self.config.getConfig("standalonetrace")
            if ((tracefile == "") or (tracefile == None)):
                tracefile = "sa.trace"
                self.config.setConfig("standalonetrace", tracefile)
                warning(self.tr("Will use default trace file name %s" % tracefile), 
                        self.tr("Trace file is not set"))
            sim_command = APCSimulatorFile + " "
            sim_command += "--debug-flags=MapuGUI,MapuMem "
            sim_command += "--trace-file=" + tracefile + " "
            if not os.path.isfile(path + "/apc/system/se.py"):
                fatal(self.tr("Cannot find simulator file %s!" %
                         path + "/apc/system/se.py"),
                      self.tr("Failed to launch the simulation"))
                return False
            sim_command += path + "/apc/system/se.py "
            try:
                ape_num = int(self.config.getConfig("numberofAPEs"))
                if ((ape_num < 1) or (ape_num > 4)):
                    raise ValueError("Number is out of range.")
            except ValueError:
                self.config.setConfig("numberofAPEs", "1")
                warning(self.tr("Will use default number: 1"),
                        self.tr("APE number is invalid"))
            if ape_num > 1:
                sim_command += "-n %d " % ape_num
            
            sim_command += '-c "'
            delimitor = ""
            for i in range(0, ape_num - 1):
                sim_command += delimitor
                delimitor = ";"
                image = self.config.getConfig("APE%dSPUimage" % i)
                if not os.path.isfile(image):
                    fatal(self.tr("Cannot find APE%dSPU executable image %s!" %
                             i, image),
                          self.tr("Failed to launch the simulation"))
                    return False
                sim_command += image
                image = self.config.getConfig("APE%dMPUimage" % i)
                if image == "":
                    continue
                if not os.path.isfile(image):
                    fatal(self.tr("Cannot find APE%dMPU executable image %s!" %
                             i, image),
                          self.tr("Failed to launch the simulation"))
                    return False
                sim_command += "," + image
            sim_command += '"'
        # Run APC simulator
        self.APCProcess.start(sim_command)
        if not self.APCProcess.waitForStarted():
            fatal(self.tr("APC simulator is not able to execute"),
                  self.tr("Failed to launch the simulation"))
            self.ARMProcess.tryTerminate()
            return False
        return True
            
    
    def stop(self):
        '''
        Terminate the simulation
        '''
        self.ARMProcess.tryTerminate()
        self.APCProcess.tryTerminate()
        
    def ARMSimulatorSwitch(self):
        '''
        Switch ARM simulator between QEMU and Gem5
        '''
        if self.config.getConfig("ARMSimType") == "QEMU":
            self.ARMProcess = self.ARMQemuProcess
        else:
            self.ARMProcess = self.ARMGem5Process
    
    def bindARMQemuProcess(self, process):
        self.ARMQemuProcess.qemu = process
        self.connect(process, SIGNAL("readyReadStandardOutput()"),
                     self.ARMQemuProcess.ReadStdOutput)
        self.connect(process, SIGNAL("readyReadStandardError()"),
                     self.ARMQemuProcess.ReadErrOutput)
        self.connect(process, SIGNAL("finished(int, QProcess::ExitStatus)"), 
                     self.ARMQemuProcess.FinishProcess)
        self.connect(process, SIGNAL("stateChanged(int)"), 
                     self.ARMQemuProcess.stateChanged)
        
    def analyzeTrace(self, exitcode, exitstatus):
        if self.config.getConfig("isFullsystem") == "True":
            tracefile = self.config.getConfig("fulltrace")
            numofcores = 4
        else:
            tracefile = self.config.getConfig("standalonetrace")
            numofcores = int(self.config.getConfig("numberofAPEs"))
        if os.path.exists("m5out/" + tracefile):
            tracefile = "m5out/" + tracefile
            self.simDB.analyzeTraceFile(numofcores, tracefile)
        
