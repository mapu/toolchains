'''
Created on Dec 23, 2015

@author: wangl
'''

from PyQt4.QtCore import QObject
import re

class RegTable(QObject):
    '''
    The base class of register operation tables
    '''


    def __init__(self, conn, parent = None):
        '''
        Constructor
        '''
        super(RegTable, self).__init__(parent)
        self.DBConn = conn
        self.Name = ""
        self.itemsid = ["id", "time", "sn", "op", "class", "no", "val"]
        self.itemstype = ["integer primary key autoincrement", "integer",
                          "integer", "varchar", "varchar(8)", "integer",
                          "varchar(256)"]
        #=======================================================================
        # The insert template prepares the sql command for this table
        # this will save time for insert procedure
        #=======================================================================
        self.insert_template = ""
        self.getop_template = ""
        self.key = ""
        
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
            
            
    def getOperations(self, time, condition):
        '''
        Get all registers' operations on 'time'
        '''
        sql_query = self.getop_template + str(time)
        if condition != "":
            sql_query += " AND " + condition
        cursor = self.DBConn.execute(sql_query)
        ret = cursor.fetchall()
        cursor.close()
        return ret
        

class SPURegTable(RegTable):
    '''
    SPU register operation table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(SPURegTable, self).__init__(conn, parent)
        self.Name = "APE%dSPURegTable" % idx
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        
        self.getop_template = "SELECT op, class, no FROM " + self.Name + " "
        self.getop_template += "WHERE time = "
        
        self.key = "ape" + str(idx) + ".srf:"
        
        self.pattern = re.compile(
            "(\d+)000: ape" + str(idx) + "\.srf: \[tid:0\]: "
            "\[sn:(\d+)\]: (\w) (\w+) Reg (\d+) :\s+(.+)", re.MULTILINE)
        

class MPURegTable(RegTable):
    '''
    MPU register operation table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(MPURegTable, self).__init__(conn, parent)
        self.Name = "APE%dMPURegTable" % idx
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        
        self.getop_template = "SELECT op, class, no FROM " + self.Name + " "
        self.getop_template += "WHERE time = "
        
        self.key = "ape" + str(idx) + ".mrf:"
        
        self.pattern = re.compile(
            "(\d+)000: ape" + str(idx) + "\.mrf: \[tid:1\]: "
            "\[sn:(\d+)\]\s?: (\w) (\w+) Reg (\d+) :\s+(.+)", re.MULTILINE)
            
            
