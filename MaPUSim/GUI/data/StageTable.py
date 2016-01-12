'''
Created on Dec 24, 2015

@author: wangl
'''

from PyQt4.Qt import QObject
from data.InstTable import SPUInstTable, MPUInstTable
import data.StageNames
import re
import sys

class StageTable(QObject):
    '''
    The base class of stage tables
    '''


    def __init__(self, conn, parent = None):
        '''
        Constructor
        '''
        super(StageTable, self).__init__(parent)
        self.DBConn = conn
        self.Name = ""
        self.instTableName = ""
        self.regTableName = ""
        self.itemsid = ["id", "sn", "start", "end", "stages"]
        self.itemstype = ["integer primary key autoincrement", "integer",
                          "integer", "integer", "varchar(200)"]
        
        self.startIdx = self.itemsid.index("start")
        self.endIdx = self.itemsid.index("end")
        self.stagesIdx = self.itemsid.index("stages")
        
        self.stageCycle = 0
        self.stages = []
        self.stageStartIdx = 0
        self.stageLastIdx = 0
        self.snIdx = 1
        self.slnIdx = 2
        self.disIdx = 3
        self.pcIdx = 4
        
        #=======================================================================
        # The insert template prepares the sql command for this table
        # this will save time for insert procedure
        #=======================================================================
        self.insert_template = ""
        
        self.pageList = []
        self.startPoint = sys.maxint
        self.endPoint = 0
        
    def createTable(self):
        '''
        Create the table
        '''
        sql_query = "CREATE TABLE " + self.Name + "("
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
            
    def traceAnalyze(self, lines):
        '''
        Analyze the trace in instTableName and regTableName,
        and build the stage table
        '''
        sql_query = "SELECT * FROM " + self.instTableName
        sql_query += " ORDER BY sn ASC"
        cursor = self.DBConn.execute(sql_query)
        instList = cursor.fetchall()
        
        if len(instList) == 0:
            return
            
        sql_query = "SELECT time, sn, op, class FROM " + self.regTableName
        sql_query += " ORDER BY sn ASC"
        cursor.execute(sql_query)
        regList = cursor.fetchall()
        
        records = []
        reg_pointer = 0
        reg_end = len(regList)
        for inst in instList:
            # start from the first stage
            idx = 1
            laststage = inst[self.stageStartIdx]
            # Update the time zone lower boundary
            if laststage < self.startPoint:
                self.startPoint = laststage
            rwFlags = {}
            reg = reg_pointer
            # DO NOT use "for reg in range(reg_end)" here
            # Building the range list consumes a lot of time
            for reg in xrange(reg_pointer, reg_end):
                # The record is formed as "reg = [time, sn, op, class]"
                if (regList[reg][1] != inst[self.snIdx]):
                    reg_pointer = reg
                    break
                if regList[reg][3] != "Misc":
                    # ignore Misc currently
                    if regList[reg][0] in rwFlags:
                        rwFlags[regList[reg][0]] += regList[reg][2]
                    else:
                        rwFlags[regList[reg][0]] = regList[reg][2]
            if laststage in rwFlags:
                stages = ["%s.%s" % (self.stages[0],
                                     "".join(set(rwFlags[laststage])))]
            else:
                stages = ["%s" % self.stages[0]]
            for stage in inst[self.stageStartIdx + 1: self.stageLastIdx + 1]:
                if stage == 0:
                    break;
                stages.extend(["%s" % self.stages[idx - 1]] * (stage - laststage - 1))
                if stage in rwFlags:
                    stages.append("%s.%s" % (self.stages[idx],
                                             "".join(set(rwFlags[stage]))))
                else:
                    stages.append("%s" % self.stages[idx])
                laststage = stage
                idx += 1
            # Update the time zone upper boundary
            if laststage > self.endPoint:
                self.endPoint = laststage
            records.append([inst[self.snIdx], inst[self.stageStartIdx],
                            laststage, ",".join(stages)])
            
        self.DBConn.executemany(self.insert_template, records)
        self.DBConn.commit()
        
        # Determine how to split the stage table -------------------------------
        # No splitting ---------------------------------------------------------
        if len(instList) <= 0x4000:
            self.pageList = [(instList[-1][self.snIdx], self.tr("Entire"))]
            return
        # Firstly divide the table by native program 'stop's -------------------
        sql_query = "SELECT sn, dis FROM " + self.instTableName + ""
        sql_query += " WHERE dis LIKE '%callimm%' OR dis LIKE '%callmimmb%' OR dis LIKE '%stop%'"
        sql_query += " ORDER BY sn ASC"
        cursor.execute(sql_query)
        self.pageList = cursor.fetchall()
        # Divide the blocks that are still larger than 16384 (0x4000) ----------
        if ((len(self.pageList) == 0) or
            (self.pageList[-1][0] != instList[-1][self.snIdx])):
            self.PageList.append((instList[-1][self.snIdx],
                                  "%s" % instList[-1][self.disIdx]))
            
        prePage = (instList[0][self.snIdx], instList[0][self.disIdx])
        for page in self.pageList[:]:
            points = (page[0] - prePage[0]) >> 14
            for i in xrange(1, points):
                new_split = (prePage[0] + 0x4000 * i,
                             "%s(sn:0x%x+0x%x)" % (prePage[1], prePage[0], i << 14))
                self.pageList.insert(self.pageList.index(page), new_split)
            prePage = page
        cursor.close()
            
    def getTablePage(self, idx):
        '''
        Get a page of stage table
        '''
        if idx >= len(self.pageList):
            return [[""]]
        if idx == 0:
            sp = 0
        else:
            sp = self.pageList[idx - 1][0] + 1
        ep = self.pageList[idx][0]
        
        sql_query = "SELECT * FROM " + self.Name + " WHERE sn >= " + str(sp)
        sql_query += " AND sn <= " + str(ep) + " ORDER BY sn ASC"
        cursor = self.DBConn.execute(sql_query)
        self.instList = cursor.fetchall()
            
        sql_query = "SELECT pc, dis FROM " + self.instTableName
        sql_query += " WHERE sn >= " + str(sp) + " AND sn <= " + str(ep)
        sql_query += " ORDER BY sn ASC"
        cursor.execute(sql_query)
        
        minTime = self.instList[0][self.startIdx]
        maxTime = max([self.instList[i][self.endIdx] for i in range(len(self.instList))])
        
        expandedTable = []
        for inst in self.instList:
            rela_start = inst[self.startIdx] - minTime
            rela_end = inst[self.endIdx] - minTime
            record = [""] * rela_start
            record.extend(inst[self.stagesIdx].split(","))
            record.extend([""] * (maxTime - rela_end))
            expandedTable.append(record)
        
        vHeader = []
        headerList = cursor.fetchall()
        for inst in headerList:
            vHeader.append(str(inst[0] + ": " + inst[1]))
        
        cursor.close()
        return (minTime, maxTime, vHeader, expandedTable)
    
    def getStart(self, row):
        return self.instList[row][self.startIdx]
        
        
class SPUStageTable(StageTable):
    '''
    SPU stage table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(SPUStageTable, self).__init__(conn, parent)
        self.Name = "APE%dSPUStageTable" % idx
        self.instTableName = re.sub("Stage", "Inst", self.Name)
        self.regTableName = re.sub("Stage", "Reg", self.Name)
        
        self.stageCycle = 2
        
        self.stages = data.StageNames.SPUStages
        
        itemsid = SPUInstTable(idx, conn, parent).itemsid
        
        self.stageStartIdx = itemsid.index(data.StageNames.SPUStages[0])
        self.stageLastIdx = itemsid.index(data.StageNames.SPUStages[-1])
        
        self.snIdx = itemsid.index("sn")
        self.slnIdx = itemsid.index("sln")
        self.disIdx = itemsid.index("dis")
        self.pcIdx = itemsid.index("pc")
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        

class MPUStageTable(StageTable):
    '''
    MPU stage table
    '''


    def __init__(self, idx, conn, parent = None):
        '''
        Constructor
        '''
        super(MPUStageTable, self).__init__(conn, parent)
        self.Name = "APE%dMPUStageTable" % idx
        self.instTableName = re.sub("Stage", "Inst", self.Name)
        self.regTableName = re.sub("Stage", "Reg", self.Name)
        
        self.stageCycle = 1
        
        self.stages = data.StageNames.MPUStages
        
        itemsid = MPUInstTable(idx, conn, parent).itemsid
        
        self.stageStartIdx = itemsid.index(data.StageNames.MPUStages[0])
        self.stageLastIdx = itemsid.index(data.StageNames.MPUStages[-1])
        
        self.snIdx = itemsid.index("sn")
        self.slnIdx = itemsid.index("sln")
        self.disIdx = itemsid.index("dis")
        self.pcIdx = itemsid.index("pc")
        
        self.insert_template = "INSERT INTO " + self.Name + " ("
        self.insert_template += ", ".join(self.itemsid[1:])
        self.insert_template += ") VALUES("
        self.insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
   