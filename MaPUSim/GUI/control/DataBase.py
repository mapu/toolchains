import sys
import os
import sqlite3
import copy
sys.path.append("..")
from data.TableType import*
import datetime

reload(sys)
sys.setdefaultencoding('utf-8')

class DataBase():
    def __init__(self):

	self.filePath="m5out/aaa.out"
	self.APE0dbFilePath="APE0table.db"
	self.APE0timeFilePath="APE0time.db"
	self.APE1dbFilePath="APE1table.db"
	self.APE1timeFilePath="APE1time.db"
	self.APE2dbFilePath="APE2table.db"
	self.APE2timeFilePath="APE2time.db"
	self.APE3dbFilePath="APE3table.db"
	self.APE3timeFilePath="APE3time.db"
	self.snTableName="sn"
	self.regTableName="reg"
	self.memTableName="mem"
	self.timeTableName="time"

    def createDatabase(self,num,path):
	self.filePath=path
        self.APE0dbConn = self.get_conn(self.APE0dbFilePath)
        self.APE0timeConn = self.get_conn(self.APE0timeFilePath)
	self.createAPE0Database()
	if num==2:
            self.APE1dbConn = self.get_conn(self.APE1dbFilePath)
            self.APE1timeConn = self.get_conn(self.APE1timeFilePath)
	    self.createAPE1Database()	
	elif num==3:
            self.APE1dbConn = self.get_conn(self.APE1dbFilePath)
            self.APE1timeConn = self.get_conn(self.APE1timeFilePath)
            self.APE2dbConn = self.get_conn(self.APE2dbFilePath)
            self.APE2timeConn = self.get_conn(self.APE2timeFilePath)
	    self.createAPE1Database()	 
	    self.createAPE2Database()
	elif num==4:
            self.APE1dbConn = self.get_conn(self.APE1dbFilePath)
            self.APE1timeConn = self.get_conn(self.APE1timeFilePath)
            self.APE2dbConn = self.get_conn(self.APE2dbFilePath)
            self.APE2timeConn = self.get_conn(self.APE2timeFilePath)
            self.APE3dbConn = self.get_conn(self.APE3dbFilePath)
            self.APE3timeConn = self.get_conn(self.APE3timeFilePath)
	    self.createAPE1Database()	 
	    self.createAPE2Database()
	    self.createAPE3Database()

    def createAPE0Database(self):
	self.memTableInit()
	self.APESnRegTableInit(self.APE0dbConn)
	i=datetime.datetime.now()
        print ("start ape0 time %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))
	self.APEtimeTableInit(self.APE0timeConn,self.APE0dbFilePath)
	i=datetime.datetime.now()
        print ("end ape0 time %s:%s:%s,%s" %(i.hour,i.minute,i.second,i.microsecond))

    def createAPE1Database(self):
	self.APESnRegTableInit(self.APE1dbConn)
	self.APEtimeTableInit(self.APE1timeConn,self.APE1dbFilePath)

    def createAPE2Database(self):
	self.APESnRegTableInit(self.APE2dbConn)
	self.APEtimeTableInit(self.APE2timeConn,self.APE2dbFilePath)

    def createAPE3Database(self):
	self.APESnRegTableInit(self.APE3dbConn)
	self.APEtimeTableInit(self.APE3timeConn,self.APE3dbFilePath)

    def get_conn(self,path):
        conn = sqlite3.connect(path)
        if os.path.exists(path) and os.path.isfile(path):
            return conn
        else:
            conn = None
            return sqlite3.connect(':memory:')

    def get_cursor(self,conn):
        if conn is not None:
            return conn.cursor()
        else:
            return self.get_conn('').cursor()

    def drop_table(self,conn,table):
        if table is not None and table != '':
            sql = 'DROP TABLE IF EXISTS ' + table
            cu = self.get_cursor(conn)
            cu.execute(sql)
            conn.commit()
            self.close_all(conn, cu)

    def create_table(self,conn,sql):
        if sql is not None and sql != '':
            cu = self.get_cursor(conn)
            cu.execute(sql)
            conn.commit()
            self.close_all(conn, cu)

    def close_all(self,conn,cu):
        try:
            if cu is not None:
                cu.close()
        finally:
            if cu is not None:
                cu.close()

    def save(self,conn,sql,data):
        if sql is not None and sql != '':
            if data is not None:
                cu = self.get_cursor(conn)
                cu.execute(sql+data)
                #self.close_all(conn, cu)

    def fetchall(self,table,sql):
	conn=self.get_conn(table)
        if sql is not None and sql != '':
            cu = self.get_cursor(conn)
            cu.execute(sql)
            r = cu.fetchall()
            if len(r) > 0:
                #for e in range(len(r)):
		    #print (r[e])
		return r
	    else:
		return 0
	else:
	    return 0

    def fetchone(self,conn,sql):
        if sql is not None and sql != '':
            #Do this instead
            cu = self.get_cursor(conn)
            cu.execute(sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    return r[e]
	    else:
	        return 0
        else:
            return 0

    def update(self,conn,sql):
        if sql is not None and sql != '':
            cu = self.get_cursor(conn)
            cu.execute(sql)
            self.close_all(conn, cu)

    def delete(self,conn,sql,data):
        if sql is not None and sql != '':
            if data is not None:
                cu = self.get_cursor(conn)
                for d in data:
                    cu.execute(sql, d)
                    conn.commit()
                #self.close_all(conn, cu)

    def snSplit(self,string,item):
#22000[11]: system.cpu.graduation_unit: [tid:0]: [sn:5]: [sln:5]: 0x000010: immtransr  r2, 67125504 : 2000 4000 6000 8000 16000 18000 20000 22000 0 0 0 0
#129000[64]: system.cpu.graduation_unit: [tid:1]: [sn:25]: [sln:25]: 0x000047: (memo:biu0ld    ) BIU0.DM(A++,K++)->M[I++] : 109000 110000 111000 112000 113000 114000 115000 116000 117000 118000 120000 122000 124000 125000 126000 127000 128000 129000
        s=string
        #get time
        pos=s.index("[")
        item.time=s[0:pos]
	item.time=str(int(item.time)/1000)
        s=s[pos:]
        #delete the content before cpu
        pos=s.index("cpu")
        s=s[pos:]
        #get cpu
        pos=s.index(".")
        if pos==3:
	    item.cpu="0"
        else:
    	    item.cpu=s[0:pos]
    	    item.cpu=item.cpu[3:pos]
        s=s[(pos+1):]
	#get spumpu
	pos=s.index("tid")
	s=s[(pos+4):]
	pos=s.index("]")
	tmp=s[:pos]
	if tmp=="0":
	    item.spumpu="'s'"
	else:
	    item.spumpu="'m'"
	#get sn
	pos=s.index("sn")
	s=s[(pos+3):]
	pos=s.index("]")
	item.sn=s[:pos]
	#get sln
	pos=s.index("sln")
	s=s[(pos+4):]
	pos=s.index("]")
	item.sln=s[:pos]
	#get pc
	pos=s.index(":")
	s=s[(pos+2):]
	pos=s.index(":")
	item.pc="'"+s[:pos]+"'"
	s=s[(pos+2):]	
	#get dis 
	pos=s.index(":")
	tmp=s[(pos+1):]
	if tmp.find(":")>=0:
	    pos=pos+1+tmp.index(":")
	item.dis=s[:(pos-1)]
	l=item.dis.find("->")
	item.dest="'"+item.dis[(l+2):]+"'"
	item.dis="'"+s[:(pos-1)]+"'"
	#get stage
	s=s[(pos+2):]
	pos=s.find("\n")
	s=s[:pos]
	stages=s.split(" ")
	tmp=0
	for i in range(0,len(stages)):
	    if int(stages[i])>=int(tmp):
		tmp=int(stages[i])	
	        item.stageList[i]=str(int(stages[i])/1000)
	    else:
		break
        return item

    def regSplit(self,string,item):
        s=string
        #get time
        pos=s.index("[")
        item.time=s[0:pos]
	item.time=str(int(item.time)/1000)
        s=s[pos:]
        #delete the content before cpu
        pos=s.index("cpu")
        s=s[pos:]
        #get cpu
        pos=s.index(".")
        if pos==3:
	    item.cpu="0"
        else:
    	    item.cpu=s[0:pos]
    	    item.cpu=item.cpu[3:pos]
        s=s[(pos+1):]

        if s.find("mpurf_manager")>=0:
	    #324000[162]: system.cpu.mpurf_manager: [tid:1]: [sn:47] : R MPU Reg 129 : 0x1 0xff 0xff
	    #114000[57]: system.cpu.mpurf_manager: [tid:1]: [sn:25] : W Misc Reg 35 : 0
	    pos=13
    	    s=s[(pos+2):] 
	    pos=s.index(":")
	    s=s[(pos+1):]
	    pos=s.index("]")
	    item.spumpu=s[0:pos]
	    if item.spumpu=="0":
		item.spumpu="'s'"
	    else:
		item.spumpu="'m'"
	    s=s[(pos+3):]
    	    #get sn
    	    pos=s.index(":")
    	    s=s[(pos+1):]
    	    pos=s.index("]")
    	    item.sn=s[0:pos]
    	    s=s[(pos+4):] 
	    pos=s.find(":")
	    temp=s[:(pos-1)]
	    slist=temp.split(" ")
	    item.op="'"+slist[0]+"'"
	    item.type="'"+slist[1]+" "+slist[2]+"'"
	    item.reg=slist[3]
	    s=s[(pos+2):]
	    pos=s.index("\n")
	    item.dis="'"+s[:pos-1]+"'"	
	elif s.find("regfile_manager")>=0:
	    #16000[8]: system.cpu.regfile_manager: [tid:0]: [sn:2]: W R Reg 1 : 0xffffff01.
	    #318000[159]: system.cpu.regfile_manager: [tid:0]: [sn:118] : W MPU Reg 131 : 0x0 0x0 0x0 0x0
	    pos=15
    	    s=s[(pos+2):] 
	    pos=s.index(":")
	    s=s[(pos+1):]
	    pos=s.index("]")
	    item.spumpu=s[0:pos]
	    if item.spumpu=="0":
		item.spumpu="'s'"
	    else:
		item.spumpu="'m'"
	    s=s[(pos+3):]
    	    #get sn
    	    pos=s.index(":")
    	    s=s[(pos+1):]
    	    pos=s.index("]")
    	    item.sn=s[0:pos]
	    if s.find("MPU")>=0:
    	        s=s[(pos+4):] 
	    else:
		s=s[(pos+3):] 
	    pos=s.find(":")
	    temp=s[:(pos-1)]
	    slist=temp.split(" ")
	    item.op="'"+slist[0]+"'"
	    item.type="'"+slist[1]+" "+slist[2]+"'"
	    item.reg=slist[3]
	    s=s[(pos+2):]
	    pos=s.index("\n")
	    item.dis="'"+s[:pos-1]+"'"	
        return item

    def memSplit(self,string,item):
	#186001[93]: system.physmem: Mem W 0x00000w : 0x77
        s=string
        #get time
        pos=s.index("[")
        item.time=s[0:pos]
	item.time=str(int(item.time)/1000)
        s=s[pos:]
	pos=s.index(":")
	s=s[(pos+1):]
	pos=s.index(":")
	s=s[(pos+6):]	
	item.type="'"+s[:1]+"'"
	s=s[2:]
	pos=s.find(":")
	item.addr=s[:(pos-1)]
	item.addr="'"+item.addr+"'"
	s=s[(pos+2):]
	pos=s.index("\n")
	s=s[:pos]
	item.dis="'"+s+"'"

        return item	

    def memTableInit(self):
        #drop table
        self.drop_table(self.APE0dbConn,self.memTableName)
	#create table
        create_table_sql="create table "+self.memTableName+"(id integer primary key autoincrement,time integer,type varchar(8),addr varchar(16),dis varchar(16))"
        self.create_table(self.APE0dbConn, create_table_sql)
        save_sql = "INSERT INTO "+self.memTableName+" (time,type,addr,dis) "
        #open out file and read 
        f=open(self.filePath,"r")
        lines=f.readlines()
        item=MemItem()
	item.__init__()
        for line in lines:
	    if line.find("physmem")>=0:
	        item=self.memSplit(line,item)
	        data="values ("+item.time+","+item.type+","+item.addr+","+item.dis+")"
	        self.save(self.APE0dbConn, save_sql, data)
	f.close()
	self.APE0dbConn.commit()

        #show table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.memTableName
            cu = self.get_cursor(self.APE0dbConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])

    def APESnRegTableInit(self,APEdbConn): 
        #drop table
        self.drop_table(APEdbConn,self.snTableName)
	#create table
        create_table_sql_sn="create table "+self.snTableName+"(id integer primary key autoincrement,time integer,cpu integer,spumpu varchar(8),sn integer,sln integer,pc varchar(8),dis varchar(128),dest varchar(8),stage0 integer,stage1 integer,stage2 integer,stage3 integer,stage4 integer,stage5 integer,stage6 integer,stage7 integer,stage8 integer,stage9 integer,stage10 integer,stage11 integer,stage12 integer,stage13 integer,stage14 integer,stage15 integer,stage16 integer,stage17 integer,stage18 integer,stage19 integer)"
        self.create_table(APEdbConn, create_table_sql_sn)
        save_sql_sn = "INSERT INTO "+self.snTableName+" (time,cpu,spumpu,sn,sln,pc,dis,dest,stage0,stage1,stage2,stage3,stage4,stage5,stage6,stage7,stage8,stage9,stage10,stage11,stage12,stage13,stage14,stage15,stage16,stage17,stage18,stage19) "

        #drop table
        self.drop_table(APEdbConn,self.regTableName)
	#create table
        create_table_sql_reg="create table "+self.regTableName+"(id integer primary key autoincrement,time integer,cpu integer,spumpu varchar(4),sn integer,op varchar(8),type varchar(8),reg integer,dis varchar(256))"
        self.create_table(APEdbConn, create_table_sql_reg)
        save_sql_reg = "INSERT INTO "+self.regTableName+" (time,cpu,spumpu,sn,op,type,reg,dis) "
        #open out file and read 
        f=open(self.filePath,"r")
        lines=f.readlines()
	item_sn=SnItem()
        item_reg=RegItem()
        #open out file and read 
        f=open(self.filePath,"r")
        lines=f.readlines()
        for line in lines:
	    if line.find("cpu.")>=0 or line.find("cpu0")>=0:
	        if line.find("graduation_unit")>=0:
		    item_sn.__init__()
	            item_sn=self.snSplit(line,item_sn)
	            data="values ("+item_sn.time+","+item_sn.cpu+","+item_sn.spumpu+","+item_sn.sn+","+item_sn.sln+","+item_sn.pc+","+item_sn.dis+","+item_sn.dest+","+item_sn.stageList[0]+","+item_sn.stageList[1]+","+item_sn.stageList[2]+","+item_sn.stageList[3]+","+item_sn.stageList[4]+","+item_sn.stageList[5]+","+item_sn.stageList[6]+","+item_sn.stageList[7]+","+item_sn.stageList[8]+","+item_sn.stageList[9]+","+item_sn.stageList[10]+","+item_sn.stageList[11]+","+item_sn.stageList[12]+","+item_sn.stageList[13]+","+item_sn.stageList[14]+","+item_sn.stageList[15]+","+item_sn.stageList[16]+","+item_sn.stageList[17]+","+item_sn.stageList[18]+","+item_sn.stageList[19]+")"
	            self.save(APEdbConn, save_sql_sn, data)
	    	elif line.find("mpurf_manager")>=0 or line.find("regfile_manager")>=0:
		    item_reg.__init__()
	            item_reg=self.regSplit(line,item_reg)
	            data="values ("+item_reg.time+","+item_reg.cpu+","+item_reg.spumpu+","+item_reg.sn+","+item_reg.op+","+item_reg.type+","+item_reg.reg+","+item_reg.dis+")"
	            self.save(APEdbConn, save_sql_reg, data)
	 	
	f.close()
	APEdbConn.commit()
        #show sn table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.snTableName
            cu = self.get_cursor(APEdbConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])
        #show reg table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.regTableName
            cu = self.get_cursor(APEdbConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])

    def APEtimeTableInit(self,APEtimeConn,APEdbFilePath):
	#drop table
	self.drop_table(APEtimeConn,self.timeTableName)
	#create table
	create_table_sql="create table "+self.timeTableName+"(time integer,M0 varchar(128),M1 varchar(128),M2 varchar(128),M3 varchar(128),M4 varchar(128),M5 varchar(128),M6 varchar(128),M7 varchar(128),M8 varchar(128),M9 varchar(128),M10 varchar(128),M11 varchar(128),M12 varchar(128),M13 varchar(128),M14 varchar(128),M15 varchar(128),M16 varchar(128),M17 varchar(128),M18 varchar(128),M19 varchar(128),M20 varchar(128),M21 varchar(128),M22 varchar(128),M23 varchar(128),M24 varchar(128),M25 varchar(128),M26 varchar(128),M27 varchar(128),M28 varchar(128),M29 varchar(128),M30 varchar(128),M31 varchar(128),M32 varchar(128),M33 varchar(128),M34 varchar(128),M35 varchar(128),M36 varchar(128),M37 varchar(128),M38 varchar(128),M39 varchar(128),M40 varchar(128),M41 varchar(128),M42 varchar(128),M43 varchar(128),M44 varchar(128),M45 varchar(128),M46 varchar(128),M47 varchar(128),M48 varchar(128),M49 varchar(128),M50 varchar(128),M51 varchar(128),M52 varchar(128),M53 varchar(128),M54 varchar(128),M55 varchar(128),M56 varchar(128),M57 varchar(128),M58 varchar(128),M59 varchar(128),M60 varchar(128),M61 varchar(128),M62 varchar(128),M63 varchar(128),M64 varchar(128),M65 varchar(128),M66 varchar(128),M67 varchar(128),M68 varchar(128),M69 varchar(128),M70 varchar(128),M71 varchar(128),M72 varchar(128),M73 varchar(128),M74 varchar(128),M75 varchar(128),M76 varchar(128),M77 varchar(128),M78 varchar(128),M79 varchar(128),M80 varchar(128),M81 varchar(128),M82 varchar(128),M83 varchar(128),M84 varchar(128),M85 varchar(128),M86 varchar(128),M87 varchar(128),M88 varchar(128),M89 varchar(128),M90 varchar(128),M91 varchar(128),M92 varchar(128),M93 varchar(128),M94 varchar(128),M95 varchar(128),M96 varchar(128),M97 varchar(128),M98 varchar(128),M99 varchar(128),M100 varchar(128),M101 varchar(128),M102 varchar(128),M103 varchar(128),M104 varchar(128),M105 varchar(128),M106 varchar(128),M107 varchar(128),M108 varchar(128),M109 varchar(128),M110 varchar(128),M111 varchar(128),M112 varchar(128),M113 varchar(128),M114 varchar(128),M115 varchar(128),M116 varchar(128),M117 varchar(128),M118 varchar(128),M119 varchar(128),M120 varchar(128),M121 varchar(128),M122 varchar(128),M123 varchar(128),M124 varchar(128),M125 varchar(128),M126 varchar(128),M127 varchar(128),SHU0T0 varchar(128),SHU0T1 varchar(128),SHU0T2 varchar(128),SHU0T3 varchar(128),SHU0T4 varchar(128),SHU0T5 varchar(128),SHU0T6 varchar(128),SHU0T7 varchar(128),SHU0T8 varchar(128),SHU0T9 varchar(128),SHU0T10 varchar(128),SHU0T11 varchar(128),SHU0T12 varchar(128),SHU0T13 varchar(128),SHU0T14 varchar(128),SHU0T15 varchar(128),SHU1T0 varchar(128),SHU1T1 varchar(128),SHU1T2 varchar(128),SHU1T3 varchar(128),SHU1T4 varchar(128),SHU1T5 varchar(128),SHU1T6 varchar(128),SHU1T7 varchar(128),SHU1T8 varchar(128),SHU1T9 varchar(128),SHU1T10 varchar(128),SHU1T11 varchar(128),SHU1T12 varchar(128),SHU1T13 varchar(128),SHU1T14 varchar(128),SHU1T15 varchar(128),IALUT0 varchar(128),IALUT1 varchar(128),IALUT2 varchar(128),IALUT3 varchar(128),IMACT0 varchar(128),IMACT1 varchar(128),IMACT2 varchar(128),IMACT3 varchar(128),FALUT0 varchar(128),FALUT1 varchar(128),FALUT2 varchar(128),FALUT3 varchar(128),FMACT0 varchar(128),FMACT1 varchar(128),FMACT2 varchar(128),FMACT3 varchar(128),IMRL varchar(128),IMRH varchar(128),FMR varchar(128),BIU0W varchar(128),BIU1W varchar(128),BIU2W varchar(128),DIVQU varchar(128),DIVER varchar(128),DIVCNT varchar(128),SVR0 varchar(128),SVR1 varchar(128),R0 varchar(128),R1 varchar(128),R2 varchar(128),R3 varchar(128),R4 varchar(128),R5 varchar(128),R6 varchar(128),R7 varchar(128),R8 varchar(128),R9 varchar(128),R10 varchar(128),R11 varchar(128),R12 varchar(128),R13 varchar(128),R14 varchar(128),R15 varchar(128),R16 varchar(128),R17 varchar(128),R18 varchar(128),R19 varchar(128),R20 varchar(128),R21 varchar(128),R22 varchar(128),R23 varchar(128),R24 varchar(128),R25 varchar(128),R26 varchar(128),R27 varchar(128),R28 varchar(128),R29 varchar(128),R30 varchar(128),R31 varchar(128),J0 varchar(128),J1 varchar(128),J2 varchar(128),J3 varchar(128),J4 varchar(128),J5 varchar(128),J6 varchar(128),J7 varchar(128),J8 varchar(128),J9 varchar(128),J10 varchar(128),J11 varchar(128),J12 varchar(128),J13 varchar(128),J14 varchar(128),J15 varchar(128),J16 varchar(128),J17 varchar(128),J18 varchar(128),J19 varchar(128),J20 varchar(128),J21 varchar(128),J22 varchar(128),J23 varchar(128),J24 varchar(128),J25 varchar(128),J26 varchar(128),J27 varchar(128),J28 varchar(128),J29 varchar(128),J30 varchar(128),J31 varchar(128))"
#,Carryflag varchar(128),Loopenable varchar(128),Loop0start varchar(128),Loop0end varchar(128),Loop0number varchar(128),Loop1start varchar(128),Loop1end varchar(128),Loop1number varchar(128),MCr0 varchar(128),MCr1 varchar(128),MCw0 varchar(128),MCw1 varchar(128),MCw2 varchar(128),MIr0 varchar(128),MIr1 varchar(128),MIw0 varchar(128),MIw1 varchar(128),MIw2 varchar(128),MSr0 varchar(128),MSr1 varchar(128),MSw0 varchar(128),MSw1 varchar(128),MSw2 varchar(128),KB0 varchar(128),KB1 varchar(128),KB2 varchar(128),KB3 varchar(128),KB4 varchar(128),KB5 varchar(128),KB6 varchar(128),KB7 varchar(128),KB8 varchar(128),KB9 varchar(128),KB10 varchar(128),KB11 varchar(128),KB12 varchar(128),KB13 varchar(128),KB14 varchar(128),KB15 varchar(128),KE0 varchar(128),KE1 varchar(128),KE2 varchar(128),KE3 varchar(128),KE4 varchar(128),KE5 varchar(128),KE6 varchar(128),KE7 varchar(128),KE8 varchar(128),KE9 varchar(128),KE10 varchar(128),KE11 varchar(128),KE12 varchar(128),KE13 varchar(128),KE14 varchar(128),KE15 varchar(128),KS0 varchar(128),KS1 varchar(128),KS2 varchar(128),KS3 varchar(128),KS4 varchar(128),KS5 varchar(128),KS6 varchar(128),KS7 varchar(128),KS8 varchar(128),KS9 varchar(128),KS10 varchar(128),KS11 varchar(128),KS12 varchar(128),KS13 varchar(128),KS14 varchar(128),KS15 varchar(128),KI0 varchar(128),KI1 varchar(128),KI2 varchar(128),KI3 varchar(128),KI4 varchar(128),KI5 varchar(128),KI6 varchar(128),KI7 varchar(128),KI8 varchar(128),KI9 varchar(128),KI10 varchar(128),KI11 varchar(128),KI12 varchar(128),KI13 varchar(128),KI14 varchar(128),KI15 varchar(128),KM0L0 varchar(128),KM0L1 varchar(128),KM0H0 varchar(128),KM0H1 varchar(128),KM1L0 varchar(128),KM1L1 varchar(128),KM1H0 varchar(128),KM1H1 varchar(128),KM2L0 varchar(128),KM2L1 varchar(128),KM2H0 varchar(128),KM2H1 varchar(128),KG0 varchar(128),KG1 varchar(128),KG2 varchar(128),KL0 varchar(128),KL1 varchar(128),KL2 varchar(128),KL3 varchar(128),KL4 varchar(128),KL5 varchar(128),KL6 varchar(128),KL7 varchar(128),KL8 varchar(128),KL9 varchar(128),KL10 varchar(128),KL11 varchar(128),KL12 varchar(128),KL13 varchar(128),KL14 varchar(128),KL15 varchar(128),KC0 varchar(128),KC1 varchar(128),KC2 varchar(128),KC3 varchar(128),KC4 varchar(128),KC5 varchar(128),KC6 varchar(128),KC7 varchar(128),KC8 varchar(128),KC9 varchar(128),KC10 varchar(128),KC11 varchar(128),KC12 varchar(128),KC13 varchar(128),KC14 varchar(128),KC15 varchar(128),MPUcondition varchar(128),MPULoopenable varchar(128),MPULoop0start varchar(128),MPULoop1start varchar(128),MPULoop2start varchar(128),MPULoop3start varchar(128),MPULoop0end varchar(128),MPULoop1end varchar(128),MPULoop2end varchar(128),MPULoop3end varchar(128),MPULoop0number varchar(128),MPULoop1number varchar(128),MPULoop2number varchar(128),MPULoop3number varchar(128))"
	self.create_table(APEtimeConn,create_table_sql)
	save_sql = "INSERT INTO "+self.timeTableName+" (time,M0,M1,M2,M3,M4,M5,M6,M7,M8,M9,M10,M11,M12,M13,M14,M15,M16,M17,M18,M19,M20,M21,M22,M23,M24,M25, M26,M27,M28,M29,M30,M31,M32,M33,M34,M35,M36,M37,M38,M39,M40,M41,M42,M43,M44,M45,M46,M47,M48,M49,M50,M51,M52,M53,M54,M55,M56,M57,M58,M59,M60,M61,M62,M63,M64,M65,M66,M67,M68,M69,M70,M71,M72,M73,M74,M75,M76, M77,M78,M79,M80,M81,M82,M83,M84,M85,M86,M87,M88,M89,M90,M91,M92,M93,M94,M95,M96,M97,M98,M99,M100,M101,M102,M103,M104,M105,M106,M107,M108,M109,M110,M111,M112,M113,M114,M115,M116,M117,M118,M119,M120,M121,M122, M123,M124,M125,M126,M127,SHU0T0,SHU0T1,SHU0T2,SHU0T3,SHU0T4,SHU0T5,SHU0T6,SHU0T7,SHU0T8,SHU0T9,SHU0T10,SHU0T11,SHU0T12,SHU0T13,SHU0T14,SHU0T15,SHU1T0,SHU1T1,SHU1T2,SHU1T3,SHU1T4,SHU1T5,SHU1T6,SHU1T7,SHU1T8, SHU1T9,SHU1T10,SHU1T11,SHU1T12,SHU1T13,SHU1T14,SHU1T15,IALUT0,IALUT1,IALUT2,IALUT3,IMACT0,IMACT1,IMACT2,IMACT3,FALUT0,FALUT1,FALUT2,FALUT3,FMACT0,FMACT1,FMACT2,FMACT3,IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU, DIVER,DIVCNT,SVR0,SVR1,R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16,R17,R18,R19,R20,R21,R22,R23,R24,R25,R26,R27,R28,R29,R30,R31,J0,J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18, J19,J20,J21,J22,J23,J24,J25,J26,J27,J28,J29,J30,J31)"
#,Carryflag,Loopenable,Loop0start,Loop0end,Loop0number,Loop1start,Loop1end, Loop1number,MC.r0,MC.r1,MC.w0,MC.w1,MC.w2,MI.r0,MI.r1,MI.w0,MI.w1,MI.w2,MS.r0,MS.r1,MS.w0,MS.w1,MS.w2,KB0,KB1,KB2,KB3,KB4,KB5,KB6,KB7,KB8,KB9,KB10,KB11,KB12,KB13,KB14,KB15,KE0,KE1,KE2,KE3,KE4,KE5,KE6,KE7, KE8,KE9,KE10,KE11,KE12,KE13,KE14,KE15,KS0,KS1,KS2,KS3,KS4,KS5,KS6,KS7,KS8,KS9,KS10,KS11,KS12,KS13,KS14,KS15,KI0,KI1,KI2,KI3,KI4,KI5,KI6,KI7,KI8,KI9,KI10,KI11,KI12,KI13,KI14,KI15,KM0L0,KM0L1,KM0H0,KM0H1,KM1L0, KM1L1,KM1H0,KM1H1,KM2L0,KM2L1,KM2H0,KM2H1,KG0,KG1,KG2,KL0,KL1,KL2,KL3,KL4,KL5,KL6,KL7,KL8,KL9,KL10,KL11,KL12,KL13,KL14,KL15,KC0,KC1,KC2,KC3,KC4,KC5,KC6,KC7,KC8,KC9,KC10,KC11,KC12,KC13,KC14,KC15,MPUcondition, MPULoopenable,MPULoop0start,MPULoop1start,MPULoop2start,MPULoop3start,MPULoop0end,MPULoop1end,MPULoop2end,MPULoop3end,MPULoop0number,MPULoop1number,MPULoop2number,MPULoop3number)"
	
	#read file content, get min and max time
	f=open(self.filePath,"r")
        lines=f.readlines()
	line=lines[0]
	pos=line.index("[")
	#self.minTime=int(line[:pos])/1000
	self.minTime=0
	count=len(lines)
	line=lines[count-1]
	pos=line.index("[")
	self.maxTime=int(line[:pos])/1000
	f.close()
	
	forwardRegList=["'nop'"]*251
	for i in range(self.minTime,self.maxTime+1):
            regList=["'nop'"]*252
	    #specialRegList=["'nop'"]*148
	    time=str(i)
	    fetchall_sql = "SELECT * FROM "+self.regTableName+" WHERE time = "+str(i)
            r=self.fetchall(APEdbFilePath,fetchall_sql)
	    if r!=0:	
	            for e in range(len(r)):
                	if r[e][5]=="W":
			    if r[e][6]=="MPU Reg":
			        regList[r[e][7]]="'"+r[e][8]+"'"
			    elif r[e][6]=="R Reg":
			        regList[187+r[e][7]]="'"+r[e][8]+"'"
			    elif r[e][6]=="J Reg":
			        regList[187+r[e][7]]="'"+r[e][8]+"'"
			    elif 0: #r[e][6]=="Misc Reg":
				if r[e][7]<=7:
				    specialRegList[r[e][7]]="'"+r[e][8]+"'"	
				elif r[e][7]>=16 and r[e][7]<=30:
				    specialRegList[r[e][7]-8]="'"+r[e][8]+"'"
				elif r[e][7]>=32 and r[e][7]<=107:
				    specialRegList[r[e][7]-9]="'"+r[e][8]+"'"
				elif r[e][7]==112:
				    specialRegList[r[e][7]-13]="'"+r[e][8]+"'"
				elif r[e][7]==116:
				    specialRegList[r[e][7]-16]="'"+r[e][8]+"'"
				elif r[e][7]==120:
				    specialRegList[r[e][7]-19]="'"+r[e][8]+"'"
				elif r[e][7]>=128 and r[e][7]<=173:
				    specialRegList[r[e][7]-26]="'"+r[e][8]+"'"

	    for i in range(0,251):
		if regList[i]=="'nop'":
		    if forwardRegList[i]!="'nop'":
			regList[i]=forwardRegList[i]
	    forwardRegList=copy.deepcopy(regList)

	    data="values ("+time+","+regList[0]+","+regList[1]+","+regList[2]+","+regList[3]+","+regList[4]+","+regList[5]+","+regList[6]+","+regList[7]+","+regList[8]+","+regList[9]+","+regList[10]+","+regList[11]+","+regList[12]+","+regList[13]+","+regList[14]+","+regList[15]+","+regList[16]+","+regList[17]+","+regList[18]+","+regList[19]+","+regList[20]+","+regList[21]+","+regList[22]+","+regList[23]+","+regList[24]+","+regList[25]+","+ regList[26]+","+regList[27]+","+regList[28]+","+regList[29]+","+regList[30]+","+regList[31]+","+regList[32]+","+regList[33]+","+regList[34]+","+regList[35]+","+regList[36]+","+regList[37]+","+regList[38]+","+regList[39]+","+regList[40]+","+regList[41]+","+regList[42]+","+regList[43]+","+regList[44]+","+regList[45]+","+regList[46]+","+regList[47]+","+regList[48]+","+regList[49]+","+regList[50]+","+regList[51]+","+regList[52]+","+regList[53]+","+regList[54]+","+regList[55]+","+regList[56]+","+regList[57]+","+regList[58]+","+regList[59]+","+regList[60]+","+regList[61]+","+regList[62]+","+regList[63]+","+regList[64]+","+regList[65]+","+regList[66]+","+regList[67]+","+regList[68]+","+regList[69]+","+regList[70]+","+regList[71]+","+regList[72]+","+regList[73]+","+regList[74]+","+regList[75]+","+regList[76]+","+ regList[77]+","+regList[78]+","+regList[79]+","+regList[80]+","+regList[81]+","+regList[82]+","+regList[83]+","+regList[84]+","+regList[85]+","+regList[86]+","+regList[87]+","+regList[88]+","+regList[89]+","+regList[90]+","+regList[91]+","+regList[92]+","+regList[93]+","+regList[94]+","+regList[95]+","+regList[96]+","+regList[97]+","+regList[98]+","+regList[99]+","+regList[100]+","+regList[101]+","+regList[102]+","+regList[103]+","+regList[104]+","+regList[105]+","+regList[106]+","+regList[107]+","+regList[108]+","+regList[109]+","+regList[110]+","+regList[111]+","+regList[112]+","+regList[113]+","+regList[114]+","+regList[115]+","+regList[116]+","+regList[117]+","+regList[118]+","+regList[119]+","+regList[120]+","+regList[121]+","+regList[122]+","+regList[123]+","+regList[124]+","+regList[125]+","+regList[126]+","+regList[127]+","+regList[128]+","+regList[129]+","+regList[130]+","+regList[131]+","+regList[132]+","+regList[133]+","+regList[134]+","+regList[135]+","+regList[136]+","+regList[137]+","+regList[138]+","+regList[139]+","+regList[140]+","+regList[141]+","+regList[142]+","+regList[143]+","+regList[144]+","+regList[145]+","+regList[146]+","+regList[147]+","+regList[148]+","+regList[149]+","+regList[150]+","+regList[151]+","+regList[152]+","+regList[153]+","+regList[154]+","+regList[155]+","+regList[156]+","+regList[157]+","+regList[158]+","+regList[159]+","+regList[160]+","+regList[161]+","+regList[162]+","+regList[163]+","+regList[164]+","+regList[165]+","+regList[166]+","+regList[167]+","+regList[168]+","+regList[169]+","+regList[170]+","+regList[171]+","+regList[172]+","+regList[173]+","+regList[174]+","+regList[175]+","+regList[176]+","+regList[177]+","+regList[178]+","+regList[179]+","+regList[180]+","+regList[181]+","+regList[182]+","+regList[183]+","+regList[184]+","+regList[185]+","+regList[186]+","+regList[187]+","+regList[188]+","+regList[189]+","+regList[190]+","+regList[191]+","+regList[192]+","+regList[193]+","+regList[194]+","+regList[195]+","+regList[196]+","+regList[197]+","+regList[198]+","+regList[199]+","+regList[200]+","+regList[201]+","+regList[202]+","+regList[203]+","+regList[204]+","+regList[205]+","+regList[206]+","+regList[207]+","+regList[208]+","+regList[209]+","+regList[210]+","+regList[211]+","+regList[212]+","+regList[213]+","+regList[214]+","+regList[215]+","+regList[216]+","+regList[217]+","+regList[218]+","+regList[219]+","+regList[220]+","+regList[221]+","+regList[222]+","+regList[223]+","+regList[224]+","+regList[225]+","+regList[226]+","+regList[227]+","+regList[228]+","+regList[229]+","+regList[230]+","+regList[231]+","+regList[232]+","+regList[233]+","+regList[234]+","+regList[235]+","+regList[236]+","+regList[237]+","+regList[238]+","+ regList[239]+","+regList[240]+","+regList[241]+","+regList[242]+","+regList[243]+","+regList[244]+","+regList[245]+","+regList[246]+","+regList[247]+","+regList[248]+","+regList[249]+","+regList[250]+")"
#+","+specialRegList[0]+","+specialRegList[1]+","+specialRegList[2]+","+specialRegList[3]+","+specialRegList[4]+","+specialRegList[5]+","+specialRegList[6]+","+specialRegList[7]+","+specialRegList[8]+","+specialRegList[9]+","+specialRegList[10]+","+specialRegList[11]+","+specialRegList[12]+","+specialRegList[13]+","+specialRegList[14]+","+specialRegList[15]+","+specialRegList[16]+","+specialRegList[17]+","+specialRegList[18]+","+specialRegList[19]+","+specialRegList[20]+","+specialRegList[21]+","+specialRegList[22]+","+specialRegList[23]+","+specialRegList[24]+","+specialRegList[25]+","+specialRegList[26]+","+specialRegList[27]+","+specialRegList[28]+","+specialRegList[29]+","+specialRegList[30]+","+specialRegList[31]+","+specialRegList[32]+","+specialRegList[33]+","+specialRegList[34]+","+specialRegList[35]+","+specialRegList[36]+","+specialRegList[37]+","+specialRegList[38]+","+specialRegList[39]+","+specialRegList[40]+","+specialRegList[41]+","+specialRegList[42]+","+specialRegList[43]+","+specialRegList[44]+","+specialRegList[45]+","+specialRegList[46]+","+specialRegList[47]+","+specialRegList[48]+","+specialRegList[49]+","+specialRegList[50]+","+specialRegList[51]+","+specialRegList[52]+","+specialRegList[53]+","+specialRegList[54]+","+specialRegList[55]+","+specialRegList[56]+","+specialRegList[57]+","+specialRegList[58]+","+specialRegList[59]+","+specialRegList[60]+","+specialRegList[61]+","+specialRegList[62]+","+specialRegList[63]+","+specialRegList[64]+","+specialRegList[65]+","+specialRegList[66]+","+specialRegList[67]+","+specialRegList[68]+","+specialRegList[69]+","+specialRegList[70]+","+specialRegList[71]+","+specialRegList[72]+","+specialRegList[73]+","+specialRegList[74]+","+specialRegList[75]+","+specialRegList[76]+","+specialRegList[77]+","+specialRegList[78]+","+specialRegList[79]+","+specialRegList[80]+","+specialRegList[81]+","+specialRegList[82]+","+specialRegList[83]+","+specialRegList[84]+","+specialRegList[85]+","+specialRegList[86]+","+specialRegList[87]+","+specialRegList[88]+","+specialRegList[89]+","+specialRegList[90]+","+specialRegList[91]+","+specialRegList[92]+","+specialRegList[93]+","+specialRegList[94]+","+specialRegList[95]+","+specialRegList[96]+","+specialRegList[97]+","+specialRegList[98]+","+specialRegList[99]+","+specialRegList[100]+","+specialRegList[101]+","+specialRegList[102]+","+specialRegList[103]+","+specialRegList[104]+","+specialRegList[105]+","+specialRegList[106]+","+specialRegList[107]+","+specialRegList[108]+","+specialRegList[109]+","+specialRegList[110]+","+specialRegList[111]+","+specialRegList[112]+","+specialRegList[113]+","+specialRegList[114]+","+specialRegList[115]+","+specialRegList[116]+","+specialRegList[117]+","+specialRegList[118]+","+specialRegList[119]+","+specialRegList[120]+","+specialRegList[121]+","+specialRegList[122]+","+specialRegList[123]+","+specialRegList[124]+","+specialRegList[125]+","+specialRegList[126]+","+specialRegList[127]+","+specialRegList[128]+","+specialRegList[129]+","+specialRegList[130]+","+specialRegList[131]+","+specialRegList[132]+","+specialRegList[133]+","+specialRegList[134]+","+specialRegList[135]+","+specialRegList[136]+","+specialRegList[137]+","+specialRegList[138]+","+specialRegList[139]+","+specialRegList[140]+","+specialRegList[141]+","+specialRegList[142]+","+specialRegList[143]+","+specialRegList[144]+","+specialRegList[145]+","+specialRegList[146]+","+specialRegList[147]+")"

	    self.save(APEtimeConn,save_sql,data)

	APEtimeConn.commit()
        #show table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.timeTableName
            cu = self.get_cursor(APEtimeConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])


