'''
Created on Dec 17, 2015

@author: wangl
'''
from PyQt4.QtCore import QObject, QFile, QIODevice, QString
from PyQt4.QtCore import QXmlStreamReader, QXmlStreamWriter
from PyQt4.Qt import qDebug
from os import environ

class SimConfig(QObject):
    '''
    The configuration of the GUI
    '''


    def __init__(self, xmlfile = "config.xml", parent = None):
        '''
        Constructor
        '''
        super(SimConfig, self).__init__(parent)
        self.xmlfile = QFile(xmlfile)
        self.simconfig = {}
        #=======================================================================
        # Try to init from XML file first.
        # If failed, goto default initialization.
        #=======================================================================
        if self.xmlfile.open(QIODevice.ReadOnly | QIODevice.Text):
            if self.readXML():
                self.xmlfile.close()
                return
            self.xmlfile.close()
        #---Try the environment variable "MAPU_HOME" first----------------------
        if environ.get("MAPU_HOME") != None:
            self.simconfig["simulatorpath"] = environ["MAPU_HOME"] + "/simulator"
        else:
            self.simconfig["simulatorpath"] = ""
        self.simconfig["isFullsystem"] = "True"
        self.simconfig["ARMSimType"] = "QEMU"
        self.simconfig["fulltrace"] = "full.trace"
        self.simconfig["flashimage"] = "image.bin"
        self.simconfig["standalonetrace"] = "sa.trace"
        self.simconfig["numberofAPEs"] = "1"
        self.simconfig["APE0SPUimage"] = "app0.s.elf"
        self.simconfig["APE0MPUimage"] = "app0.m.elf"
        self.simconfig["APE1SPUimage"] = "app1.s.elf"
        self.simconfig["APE1MPUimage"] = "app1.m.elf"
        self.simconfig["APE2SPUimage"] = "app2.s.elf"
        self.simconfig["APE2MPUimage"] = "app2.m.elf"
        self.simconfig["APE3SPUimage"] = "app3.s.elf"
        self.simconfig["APE3MPUimage"] = "app3.m.elf"

    def readXML(self):
        '''
        Initialize configuration from XML file
        '''
        xmlReader = QXmlStreamReader(self.xmlfile)
        if xmlReader.readNext() != QXmlStreamReader.StartDocument:
            qDebug("XML file has a broken header!")
            return False
        
        if (xmlReader.readNext() != QXmlStreamReader.StartElement or
            xmlReader.name() != "SimulatorConfig"):
            return False
        xmlReader.readNext()
        while not xmlReader.atEnd():
            if xmlReader.readNext() == QXmlStreamReader.StartElement:
                element_name = xmlReader.name()
                if xmlReader.readNext() == QXmlStreamReader.Characters:
                    self.simconfig[str(element_name)] = xmlReader.text().toString()
                    if (xmlReader.readNext() != QXmlStreamReader.EndElement or
                        xmlReader.name() != element_name):
                        qDebug("XML file has broken element end!")
                        return False
                    else:
                        xmlReader.readNext()
                elif xmlReader.tokenType() == QXmlStreamReader.EndElement:
                    self.simconfig[str(element_name)] = ""
                    xmlReader.readNext()
                else:
                    qDebug("XML file has broken element content!")
                    return False
            elif (xmlReader.tokenType() == QXmlStreamReader.EndElement and
                  xmlReader.name() == "SimulatorConfig"):
                return True
            else:
                qDebug("XML file has broken element beginning!")
                return False
        return True
        #self.configControlWidget.fullCheckedSlot()
        #self.configControlWidget.APCCheckedSlot()

 
    def writeXML(self):
        '''
        Store current configuration into XML file
        '''
        if self.xmlfile.open(QIODevice.WriteOnly | QIODevice.Text):
            xmlWriter = QXmlStreamWriter(self.xmlfile)
            xmlWriter.setAutoFormatting(True)
            xmlWriter.writeStartDocument()
            xmlWriter.writeStartElement("SimulatorConfig")
            for key, value in self.simconfig.iteritems():
                xmlWriter.writeTextElement(QString(key), value)
            xmlWriter.writeEndElement()
            xmlWriter.writeEndDocument()
            self.xmlfile.close()
        else:
            qDebug("XML file cannot be written!")
            
        
    def getConfig(self, name):
        '''
        Initialize configuration from XML file
        '''
        if str(name) in self.simconfig:
            return self.simconfig[str(name)]
        else:
            return None
        
    def setConfig(self, name, value):
        '''
        Change a specific configuration item if it is existing, otherwise
        add a new item
        '''
        self.simconfig[str(name)] = value
