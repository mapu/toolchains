'''
Created on Dec 22, 2015

@author: wangl
'''

from PyQt4.Qt import QObject
import data.RegNames
import re

class TimeTable(QObject):
    '''
    The base class of time tables
    '''


    def __init__(self, conn, parent = None):
        '''
        Constructor
        '''
        super(TimeTable, self).__init__(parent)
        self.DBConn = conn
        self.Name = ""
        self.regTableName = ""
        self.regOffset = {"MPU" : 0, "R" : 0, "J" : 32, "Misc" : 64}
        self.itemsid = ["time"]
        self.itemstype = ["integer"]
        #=======================================================================
        # The insert template prepares the sql command for this table
        # this will save time for insert procedure
        #=======================================================================
        self.insert_template = ""
        self.select_template = ""
        
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
        Analyze the trace in regTableName and build the timing table
        '''
        sql_query = "SELECT time, class, no, val FROM " + self.regTableName
        sql_query += " WHERE op = 'W' ORDER BY time ASC"
        cursor = self.DBConn.execute(sql_query)
        regWrittenList = cursor.fetchall()
        cursor.close()
        if len(regWrittenList) == 0:
            return
        
        records = []
        record = ['N/A'] * len(self.itemsid)
        record[0] = regWrittenList[0][0] #   The start point time
        for rec in regWrittenList:
            if rec[0] == record[0]:
                #  record[reg no + reg class offset + 1] = reg value------------
                record[rec[2] + self.regOffset[rec[1]] + 1] = rec[3]   # +1 is because that rec[0] is time
            else:
                records.append(list(record)) # [:] makes a copy of record
                record[0] = rec[0]
                record[rec[2] + self.regOffset[rec[1]] + 1] = rec[3]
        records.append(list(record))
        self.DBConn.executemany(self.insert_template, records)
        self.DBConn.commit()
            
    def getValue(self, time):
        '''
        Get all registers' values on 'time'
        '''
        sql_query = re.sub("curtime", str(time), self.select_template) 
        cursor = self.DBConn.execute(sql_query)
        ret = cursor.fetchall()
        cursor.close()
        return ret
            
        

class SPUTimeTable(TimeTable):
    '''
    SPU timing table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(SPUTimeTable, self).__init__(conn, parent)
        self.Name = "APE%dSPUTimeTable" % idx
        self.regTableName = re.sub("Time", "Reg", self.Name)
        
        self.itemsid.extend(data.RegNames.SPURegNames)
        self.itemstype.extend(["varchar(128)"] * len(data.RegNames.SPURegNames))
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid)
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * len(self.itemsid)) + ")"
        
        self.select_template = "SELECT * FROM " + self.Name + " "
        self.select_template += "WHERE time <= curtime ORDER BY (curtime - time) "
        self.select_template += "LIMIT 1"

class MPUTimeTable(TimeTable):
    '''
    MPU timing table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(MPUTimeTable, self).__init__(conn, parent)
        self.Name = "APE%dMPUTimeTable" % idx
        self.regTableName = re.sub("Time", "Reg", self.Name)
        
        self.itemsid.extend(data.RegNames.MPURegNames)
        self.itemstype.extend(["varchar(128)"] * len(data.RegNames.MPURegNames))
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid)
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * len(self.itemsid)) + ")"
        
        self.select_template = "SELECT * FROM " + self.Name + " "
        self.select_template += "WHERE time <= curtime ORDER BY (curtime - time) "
        self.select_template += "LIMIT 1"
        
        
