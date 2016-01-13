'''
Created on Dec 22, 2015

@author: wangl
'''

from PyQt4.QtCore import QObject
import re
import data

class InstTable(QObject):
    '''
    The base class of instruction tables
    '''


    def __init__(self, conn, parent = None):
        '''
        Constructor
        '''
        super(InstTable, self).__init__(parent)
        self.DBConn = conn
        self.Name = ""
        self.itemsid = ["id", "time", "sn", "sln", "pc", "dis"]
        self.itemstype = ["integer primary key autoincrement",
                          "integer", "integer", "integer", "integer",
                          "varchar(128)"]
        #=======================================================================
        # The insert template prepares the sql command for this table
        # this will save time for insert procedure
        #=======================================================================
        self.insert_template = ""
        self.key = ""
        self.srcdest_template = ""
        
    def createTable(self):
        '''
        Create the table
        '''
        sql_query = "CREATE TABLE " + self.Name + " ("
        sql_query += ", ".join(" ".join(x)
                               for x in zip(self.itemsid, self.itemstype))
        sql_query += ")"
        self.DBConn.execute(sql_query)
        
    def searchRecord(self, text, attributes):
        attribute_exps = ["%s LIKE '%%%s%%'" % (attr, text) for attr in attributes]
        query = self.search_template + ' OR '.join(attribute_exps)
        cur = self.DBConn.execute(query)
        return cur.fetchall()
        
    def clearTable(self):
        '''
        Drop the table first if it exists, and then create a new one
        '''
        sql_query = "DROP TABLE IF EXISTS " + self.Name
        self.DBConn.execute(sql_query)
        self.createTable()
            
    def traceAnalyze(self, lines):
        '''
        Analyze the trace in 'lines', and create the record if it is matched
        the pattern
        '''
        #=======================================================================
        # Note that [:] will make a copy of lines, which is nessecary 
        # because items are deleted during loop
        #=======================================================================
        text = ""
        for i in xrange(len(lines)):
            if self.key in lines[i]:
                text += lines[i]
                lines[i] = "" 
                #lines.remove(line)
        records = self.pattern.findall(text)
        if records != []:
            records = self.fixupRecord(records)
            self.DBConn.executemany(self.insert_template, records)
            self.DBConn.commit()
    
    def fixupRecord(self, records):
        return records
    
    def getInstSrcDest(self, time):
        sql_query = self.srcdest_template + str(time)
        cursor = self.DBConn.execute(sql_query)
        instList = cursor.fetchall()
        acc = ["IALU", "IMAC", "FALU", "FMAC"]
        retList = []
        for inst in instList:
            dest = self.destpattern.search(inst[1])
            if dest == None:
                continue
            dest = dest.groups()[0]
            src = inst[0].upper()
            if dest == src:
                continue
            if ((src in acc) and (not (dest in acc))):
                retList.extend([(src, "O"), ("O", dest)])
                continue
            if ((not (src in acc)) and (dest in acc)):
                path = self.pathpattern.search(inst[1]).groups()[0]
                retList.extend([(src, path), (path, dest)])
                continue
            if (("BIU" in src) and ("BIU" in dest)):
                retList.append(("DM", dest))
                continue
            retList.append((src, dest))
        return retList
                    

class SPUInstTable(InstTable):
    '''
    SPU instruction table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(SPUInstTable, self).__init__(conn, parent)
        self.numOfStages = len(data.StageNames.SPUStages)
        self.itemsid.extend(data.StageNames.SPUStages)
        self.itemstype.extend(["integer"] * self.numOfStages)
        self.Name = "APE%dSPUInstTable" % idx
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        
        self.key = "graduation_unit: [tid:0]"
        
        if idx == 0:
            self.pattern = re.compile(
                "(\d+)000\[\d+\]: system\.cpu[0]?\.graduation_unit: \[tid:0\]: "
                "\[sn:(\d+)\]: \[sln:(\d+)\]: (0x[0-9a-fA-F]+): "
                "(.*) :((?: \d+)+)", re.MULTILINE)
        else:
            self.pattern = re.compile(
                "(\d+)000\[\d+\]: system\.cpu" + str(idx) + "\.graduation_unit: \[tid:0\]: "
                "\[sn:(\d+)\]: \[sln:(\d+)\]: (0x[0-9a-fA-F]+): "
                "(.*) :((?: \d+)*)", re.MULTILINE)
        
        self.search_template = "SELECT * FROM " + self.Name + " WHERE "
    
    def fixupRecord(self, records):
        '''
        The stage times are stored in record[5] and seperated by ' ', which
        must be fixup by this method
        '''
        new_records = []
        for rec in records:
            rec = list(rec)
            stages = re.sub("(000 |000$)", " ", rec[5])
            stages = stages.split()
            rec.pop()
            rec.extend(stages)
            new_records.append(rec)
        return InstTable.fixupRecord(self, new_records)
        

class MPUInstTable(InstTable):
    '''
    MPU instruction table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(MPUInstTable, self).__init__(conn, parent)
        self.numOfStages = len(data.StageNames.MPUStages)
        self.itemsid.insert(-1, "src")
        self.itemsid.append("dest")
        self.itemsid.extend(data.StageNames.MPUStages)
        self.itemstype.insert(-1, "varchar(4)")
        self.itemstype.append("varchar(8)")
        self.itemstype.extend(["integer"] * self.numOfStages)
        self.Name = "APE%dMPUInstTable" % idx
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        
        self.key = "graduation_unit: [tid:1]"
        
        self.srcdest_template = "SELECT src, dest FROM " + self.Name + " WHERE time = "
        
        if idx == 0:
            self.pattern = re.compile(
                "(\d+)000\[\d+\]: system\.cpu[0]?\.graduation_unit: \[tid:1\]: "
                "\[sn:(\d+)\]: \[sln:(\d+)\]: (0x[0-9a-fA-F]+): \(memo:(\S{1,4})\S*\s*\) "
                "(.*->(.+)|.*) :((?: \d+)+)", re.MULTILINE)
        else:
            self.pattern = re.compile(
                "(\d+)000\[\d+\]: system\.cpu" + str(idx) + "\.graduation_unit: \[tid:1\]: "
                "\[sn:(\d+)\]: \[sln:(\d+)\]: (0x[0-9a-fA-F]+): \(memo:(\S{1,4})\S*\s*\) "
                "(.*->(.+)|.*) :((?: \d+)+)", re.MULTILINE)
        
        self.search_template = "SELECT * FROM " + self.Name + " WHERE "
        
        self.destpattern = re.compile("(M(?=\[\d+\])|DM|BIU[0-2]|SHU[01]|IALU|IMAC|FALU|FMAC)")
        self.pathpattern = re.compile("\((I[0-2])\)")
    
    def fixupRecord(self, records):
        '''
        The stage times are stored in record[6] and seperated by ' ', which
        must be fixup by this method
        '''
        new_records = []
        for rec in records:
            rec = list(rec)
            stages = re.sub("(000 |000$)", " ", rec[7])
            stages = stages.split()
            rec.pop()
            rec.extend(stages)
            new_records.append(rec)
        return InstTable.fixupRecord(self, new_records)
        
        