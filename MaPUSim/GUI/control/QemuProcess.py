'''
Created on Dec 21, 2015

@author: wangl
'''
from PyQt4.QtCore import QObject, QString, pyqtSignal, QProcess,\
    QCoreApplication
from PyQt4.QtGui import QMessageBox
from view.Utils import fatal
import view.Utils
import re

class ARMQemuProcess(QObject):
    '''
    Pretend as the QProcess for QEMU simulator
    '''

    signalUARTStart = pyqtSignal(QString, list)
    signalStarted = pyqtSignal()
    updateLog = pyqtSignal(QString)
    stateChanged = pyqtSignal(QProcess.ProcessState)

    def __init__(self, parent = None):
        '''
        Constructor
        '''
        super(ARMQemuProcess, self).__init__(parent)
        self.stderr = QString()
        self.qemu = None
        self.commkey = None
        self.shmem_pat = re.compile("(?<=Share memory key is )\d+")
        self.apcport_pat = re.compile(
            "(?<=Listening for system\.realview\.apc connection on port )\d+")
        self.gdbport = 1234
        
    def ReadStdOutput(self):
        '''
        Slot function for reading stdout of process
        '''
        return
        
    def ReadErrOutput(self):
        '''
        Slot function for reading stdout of process
        '''
        if self.qemu == None:
            fatal("Receive QEMU log from unknown process!", "Invalid signal")
            return
        new_output = QString(self.qemu.readAllStandardError().data())
        self.stderr += new_output
        self.updateLog.emit(new_output)
    
    def start(self, command, args):
        '''
        Start qemu process in a qterminal
        '''
        self.signalUARTStart.emit(command, args)
        
    def state(self):
        '''
        Perform QProcess state()
        '''
        if self.qemu == None:
            return QProcess.NotRunning
        return self.qemu.state()
        
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
            if ((shmemkey != None) and (apcport != None)):
                self.commkey = [int(shmemkey.group(0)),
                                int(apcport.group(0))]
            else:
                # wait for the ARM simulator starting 
                self.qemu.waitForReadyRead(100)
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
            self.qemu.terminate()
        else:
            return
        wait = 10
        while self.state() == QProcess.Running:
            self.qemu.waitForFinished(1000)
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
            self.qemu.kill()
        else:
            return
        wait = 10
        while self.state() == QProcess.Running:
            self.qemu.waitForFinished(1000)
            wait -= 1
            if wait == 0:
                ret = fatal(self.tr("Cannot kill ARM simulator. "
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
        self.stderr = ""
        
