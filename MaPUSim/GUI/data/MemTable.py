'''
Created on Jan 15, 2016

@author: wangl
'''
from PyQt4.QtCore import QObject
import re

class MemTable(QObject):
    '''
    The base class of memory access tables
    '''


    def __init__(self, conn, parent = None):
        '''
        Constructor
        '''
        super(MemTable, self).__init__(parent)
        self.DBConn = conn
        self.Name = ""
        self.key = ""
        self.itemsid = ["id", "time", "sn", "op", "value", "addr"]
        self.itemstype = ["integer primary key autoincrement",
                          "integer", "integer", "varchar(2)"]
        
        
        #=======================================================================
        # The insert template prepares the sql command for this table
        # this will save time for insert procedure
        #=======================================================================
        self.insert_template = ""
        
    def createTable(self):
        '''
        Create the table
        '''
        sql_query = "CREATE TABLE " + self.Name + " ("
        sql_query += ", ".join(" ".join(x)
                               for x in zip(self.itemsid, self.itemstype))
        sql_query += ")"
        self.DBConn.execute(sql_query)
        
    def clearTable(self):
        '''
        Drop the table first if it exists, and then create a new one
        '''
        sql_query = "DROP TABLE IF EXISTS " + self.Name
        self.DBConn.execute(sql_query)
        self.createTable()
            
    def traceAnalyze(self, lines_wrapper):
        '''
        Analyze the trace in 'lines', and create the record if it is matched
        the pattern
        '''
        if len(lines_wrapper[0]) == 0:
            return
        text = ""
        lines = lines_wrapper[0]
        rest = []
        for i in xrange(len(lines)):
            if self.key in lines[i]:
                text += lines[i]
            else:
                rest.append(lines[i]) 
        records = self.pattern.findall(text)
        if records != []:
            self.DBConn.executemany(self.insert_template, records)
            self.DBConn.commit()
        lines_wrapper[0] = rest
        
class SPUMemTable(MemTable):
    '''
    SPU instruction table
    '''

    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(SPUMemTable, self).__init__(conn, parent)
        self.itemstype.append("integer")
        self.itemstype.append("integer")
        self.Name = "APE%dSPUMemTable" % idx
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        
        self.key = "ape" + str(idx) + ".mem: [tid:0]:"
        self.pattern = re.compile(
            "(\d+)000: ape" + str(idx) + "\.mem: \[tid:0\]: "
            "\[sn:(\d+)\]: (\w+) Mem : (0x[0-9a-fA-F]+) @A=(0x[0-9a-fA-F]+)",
            re.MULTILINE)
        
class MPUMemTable(MemTable):
    '''
    SPU instruction table
    '''

    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(MPUMemTable, self).__init__(conn, parent)
        self.itemstype.append("varchar(128)")
        self.itemstype.append("integer")
        self.Name = "APE%dSPUMemTable" % idx
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        
        self.key = "ape" + str(idx) + ".mem: [tid:1]:"
        self.pattern = re.compile(
            "(\d+)000: ape" + str(idx) + "\.mem: \[tid:1\]: "
            "\[sn:(\d+)\]: (\w+) Mem : (0x[0-9a-fA-F]+ {64})@A=(0x[0-9a-fA-F]+)",
            re.MULTILINE)
            