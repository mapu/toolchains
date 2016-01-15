'''
Created on Dec 19, 2015

@author: wangl
'''
from PyQt4.QtCore import QProcess, SIGNAL, QString, pyqtSignal, QCoreApplication
from PyQt4.QtGui import QMessageBox
from time import sleep
from view.Utils import fatal
import view.Utils
import re

class Gem5Process(QProcess):
    '''
    Process for Gem5 simulator
    '''

    updateLog = pyqtSignal(QString)
    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(Gem5Process, self).__init__(parent)
        self.connect(self, SIGNAL("readyReadStandardOutput()"), self.ReadStdOutput)
        self.connect(self, SIGNAL("readyReadStandardError()"),
                     self.ReadErrOutput)
        self.stdout = QString()
        self.stderr = QString()
        
    def ReadStdOutput(self):
        '''
        Slot function for reading stdout of process
        '''
        new_output = QString(self.readAllStandardOutput().data())
        self.stdout += new_output
        self.updateLog.emit(new_output)
        
    def ReadErrOutput(self):
        '''
        Slot function for reading stdout of process
        '''
        new_output = QString(self.readAllStandardError().data())
        self.stderr += new_output
        self.updateLog.emit(new_output)


class ARMGem5Process(Gem5Process):
    '''
    Process for ARM Gem5 simulator
    '''

    signalUARTStart = pyqtSignal(int)
    
    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(ARMGem5Process, self).__init__(parent)
        self.connect(self, SIGNAL("finished(int, QProcess::ExitStatus)"), 
                     self.FinishProcess)
        self.commkey = None
        self.shmem_pat = re.compile("(?<=Share memory key is )\d+")
        self.apcport_pat = re.compile(
            "(?<=Listening for system\.realview\.apc connection on port )\d+")
        self.uart_pat = re.compile(
            "(?<=Listening for )(.*terminal.*)( connection on port )(\d+)")
        
    def FinishProcess(self, exitcode, exitstatus):
        '''
        Slot function for dealing with the process exit
        '''
        self.cleanup()
        if exitstatus == QProcess.CrashExit:
            fatal("ARM simulator exits abnormally!")
        
        
    def getCommKeys(self):
        '''
        Get share memory key and APC socket port from standard error
        '''
        retry = 100
        while self.commkey == None:
            str_stderr = unicode(self.stderr)
            shmemkey = self.shmem_pat.search(str_stderr)
            apcport = self.apcport_pat.search(str_stderr)
            uartport = self.uart_pat.search(str_stderr)
            if ((shmemkey != None) and (apcport != None) and (uartport != None)):
                self.commkey = [int(shmemkey.group(0)),
                                int(apcport.group(0)),
                                int(uartport.group(3))]
            else:
                # wait for the ARM simulator starting 
                self.waitForReadyRead(100)
                retry -= 1
                if retry == 0:
                    ret = QMessageBox.warning(
                        view.Utils.mainWindow, self.tr("ARM simulator has no output"),
                        self.tr("Cannot get proper standard error output from "
                           "ARM simulator. Do you want to keep on waiting?"),
                        buttons = QMessageBox.Yes | QMessageBox.No,
                        defaultButton = QMessageBox.No)
                    if ret == QMessageBox.Yes:
                        retry = 100
                    else:
                        break
        return self.commkey
    
    def tryTerminate(self):
        '''
        This method try to terminate the simulator. It will send SIGTERM firstly
        and then send SIGKILL in case of the failure of SIGTERM. If both are 
        failed, it will quit the entire application. 
        '''
        if self.state() == QProcess.Running:
            self.terminate()
            self.waitForFinished()
        else:
            return
        wait = 50
        while self.state() == QProcess.Running:
            self.waitForFinished(1000)
            wait -= 1
            if wait == 0:
                ret = QMessageBox.warning(view.Utils.mainWindow,
                                          self.tr("ARM simulator has no response"),
                                          self.tr("Cannot stop ARM simulator. "
                                             "Do you want to kill it?"),
                                          buttons = QMessageBox.Yes |
                                                    QMessageBox.No,
                                          defaultButton = QMessageBox.No)
                if ret == QMessageBox.No:
                    wait = 50
                else:
                    break
        if self.state() == QProcess.Running:
            self.kill()
        else:
            return
        wait = 50
        while self.state() == QProcess.Running:
            self.waitForFinished(1000)
            wait -= 1
            if wait == 0:
                fatal(self.tr("Cannot kill ARM simulator. "
                              "Please restart the application"))
                self.cleanup()
                QCoreApplication.exit(1)
                return
        
    def cleanup(self):
        '''
        This method will clean all output of the process executed last time
        and also clean the share memory between two simulators.
        '''
        if self.commkey != None:
            # unconditionally execute "ipcrm -m key"
            QProcess.execute("ipcrm", ["-M", ("%d" % self.commkey[0])])
            self.commkey = None
        else:
            # try get the key again
            str_stderr = unicode(self.stderr)
            shmemkey = self.shmem_pat.search(str_stderr)
            if shmemkey != None:
                QProcess.execute("ipcrm", ["-M", shmemkey.group(0)])
        self.stdout = ""
        self.stderr = ""


class APCGem5Process(Gem5Process):
    '''
    Process for APC Gem5 simulator
    '''


    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(APCGem5Process, self).__init__(parent)
        self.connect(self, SIGNAL("finished(int, QProcess::ExitStatus)"), 
                     self.FinishProcess)
        
    def FinishProcess(self, exitcode, exitstatus):
        '''
        Slot function for dealing with the process exit
        '''
        #from main import SimulationControl
        self.stdout = ""
        self.stderr = ""
        if exitstatus == QProcess.CrashExit:
            fatal("APC simulator exits abnormally!")
            #SimulationControl.ARMProcess.tryTerminate()
    
    def tryTerminate(self):
        '''
        This method try to terminate the simulator. It will send SIGTERM firstly
        and then send SIGKILL in case of the failure of SIGTERM. If both are 
        failed, it will quit the entire application. 
        '''
        if self.state() == QProcess.Running:
            self.terminate()
            self.waitForFinished()
        else:
            return
        wait = 50
        while self.state() == QProcess.Running:
            self.waitForFinished(1000)
            wait -= 1
            if wait == 0:
                ret = QMessageBox.warning(view.Utils.mainWindow,
                                          self.tr("APC simulator has no response"),
                                          self.tr("Cannot stop APC simulator. "
                                             "Do you want to kill it?"),
                                          buttons = QMessageBox.Yes |
                                                    QMessageBox.No,
                                          defaultButton = QMessageBox.No)
                if ret == QMessageBox.No:
                    wait = 50
                else:
                    break
        if self.state() == QProcess.Running:
            self.kill()
        else:
            return
        wait = 50
        while self.state() == QProcess.Running:
            sleep(1)
            wait -= 1
            if wait == 0:
                fatal(self.tr("Cannot kill ARM simulator. "
                              "Please restart the application"))
                self.cleanup()
                QCoreApplication.exit(1)
                return
            
