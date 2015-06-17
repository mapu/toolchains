import sys
import os
import sqlite3
import copy
sys.path.append("..")
from data.TableType import*

reload(sys)
sys.setdefaultencoding('utf-8')

class DataBase():
    def __init__(self):
	self.filePath="m5out/aaa.out"
	#self.filePath="/home/litt/aaa.out"
	self.dbFilePath="table.db"
	self.timeFilePath="time.db"
	self.snTableName="sn"
	self.regTableName="reg"
	self.timeTableName="time"

        self.dbConn = self.get_conn(self.dbFilePath)
        self.timeConn = self.get_conn(self.timeFilePath)

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
                self.close_all(conn, cu)

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
                self.close_all(conn, cu)

    def snSplit(self,string,item):
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
        if s.find(".")==3:
	    #0[0]: system.cpu.spu.stage0: [sn:1]  
	    pos=3
     	    item.spumpu=s[0:pos] 
    	    if item.spumpu=="spu":
	        item.spumpu="'s'"
    	    else:
 	        item.spumpu="'m'"
	    #get stage
	    s=s[(pos+1):]
	    pos=s.index(":")
	    item.num=s[5:pos]
	    item.stageList[int(item.num)]=item.time
	    s=s[(pos+2):]
    	    #get sn
    	    pos=s.index(":")
    	    s=s[(pos+1):]
    	    pos=s.index("]")
    	    item.sn=s[0:pos]
    	    s=s[(pos+1):] 

	elif s.find("im_port")>=0:
	    #326000[163]: system.cpu.mim_port: [sn:105] : [sln:105] : 181 : (memo:ialuadd   ) IALU.T1+T2->IMAC.T0
	    if s.find("mim_port")>=0:
	        pos=8
	        item.spumpu="'m'"
     	    	a=s[0:pos] #min_port
    	    	s=s[(pos+2):]	
    	    	#get sn
    	    	pos=s.index(":")
    	    	s=s[(pos+1):]
    	    	pos=s.index("]")
    	    	item.sn=s[0:pos]
    	    	s=s[(pos+1):] 
    	    	s=s[3:]
	    	s=s[5:]
	    	pos=s.index("]")
	    	item.sln=s[0:pos]
	    	s=s[(pos+4):]#181 : (memo:ialuadd   ) IALU.T1+T2->IMAC.T0
		pos=s.index(":")
		item.pc="'"+s[:pos-1]+"'"
		s=s[(pos+2):]
		pos=s.index("\n")
		s=s[:pos]
	    	item.dis="'"+s+"'" #(memo:ialuadd   ) IALU.T1+T2->IMAC.T0
		pos=s.find("->")
		if pos>=0:
		    s=s[(pos+2):]
		    item.dest="'"+s+"'" #IMAC.T0
	    else:
	        #4000[2]: system.cpu.im_port: [sn:2] : [sln:2] : 4 : immtransr  r1, -255 // 8000[4]: system.cpu.im_port: [sn:10] : [sln:8] : 24 : 
	        pos=7
	    	item.spumpu="'s'"
     	    	a=s[0:pos] #im_port
    	    	s=s[(pos+2):]	
    	    	#get sn
    	    	pos=s.index(":")
    	    	s=s[(pos+1):]
    	    	pos=s.index("]")
    	    	item.sn=s[0:pos]
    	    	s=s[(pos+1):] 
    	    	s=s[3:]
	    	s=s[5:]
	    	pos=s.index("]")
	    	item.sln=s[0:pos]
	    	s=s[(pos+4):]#4 : immtransr  r1, -255
		pos=s.index(":")
		item.pc="'"+s[:(pos-1)]+"'"
		s=s[(pos+2):]
		if len(s)!=1:
	    	    pos=s.index("\n")
	    	    s=s[:pos]
	    	    item.dis="'"+s+"'"
		else:
		    item.dis="'nop'"

        return item

    def snTableInit(self):
        #drop table
        self.drop_table(self.dbConn,self.snTableName)
	#create table
        create_table_sql="create table "+self.snTableName+"(id integer primary key autoincrement,cpu integer,spumpu varchar(8),sn integer,sln integer,pc varchar(8),dis varchar(128),dest varchar(8),stage0 integer,stage1 integer,stage2 integer,stage3 integer,stage4 integer,stage5 integer,stage6 integer,stage7 integer,stage8 integer,stage9 integer,stage10 integer,stage11 integer,stage12 integer,stage13 integer,stage14 integer,stage15 integer,stage16 integer,stage17 integer,stage18 integer,stage19 integer)"
        self.create_table(self.dbConn, create_table_sql)

        save_sql = "INSERT INTO "+self.snTableName+" (cpu,spumpu,sn,sln,pc,dis,dest,stage0,stage1,stage2,stage3,stage4,stage5,stage6,stage7,stage8,stage9,stage10,stage11,stage12,stage13,stage14,stage15,stage16,stage17,stage18,stage19) "

	item=SnItem()
        #open out file and read 
        f=open(self.filePath,"r")
        lines=f.readlines()
	i=0
        for line in lines:
	    if line.find("mpurf_manager")<0:
		item.__init__()
	        item=self.snSplit(line,item)
    	        fetchone_sql = "SELECT * FROM "+self.snTableName+" WHERE spumpu = "+item.spumpu+" and "+"sn = "+item.sn
                cu = self.get_cursor(self.dbConn)
                cu.execute(fetchone_sql)
                r = cu.fetchall()
                if len(r) > 0:
		    #update 
		    if line.find("stage")>=0:
		        stage="stage"+item.num
			if r[0][8+int(item.num)]!=-1:
			    if r[0][8+int(item.num)]>int(item.stageList[int(item.num)]):
			        time=item.stageList[int(item.num)]
			    else:
			        time=str(r[0][8+int(item.num)])
			else:
			    time=item.stageList[int(item.num)]
		        update_sql = "UPDATE "+self.snTableName+" SET "+stage+" = "+time+" WHERE spumpu = "+item.spumpu+" and "+"sn = "+item.sn
	    	        self.update(self.dbConn, update_sql)
		    else:
		        update_sql = "UPDATE "+self.snTableName+" SET sln = "+item.sln+" , pc = "+item.pc+" , dis = "+item.dis+" , dest = "+item.dest+" WHERE spumpu = "+item.spumpu+" and "+"sn = "+item.sn
	    	        self.update(self.dbConn, update_sql)		    	
	        else:
	            data="values ("+item.cpu+","+item.spumpu+","+item.sn+","+item.sln+","+item.pc+","+item.dis+","+item.dest+","+item.stageList[0]+","+item.stageList[1]+","+item.stageList[2]+","+item.stageList[3]+","+item.stageList[4]+","+item.stageList[5]+","+item.stageList[6]+","+item.stageList[7]+","+item.stageList[8]+","+item.stageList[9]+","+item.stageList[10]+","+item.stageList[11]+","+item.stageList[12]+","+item.stageList[13]+","+item.stageList[14]+","+item.stageList[15]+","+item.stageList[16]+","+item.stageList[17]+","+item.stageList[18]+","+item.stageList[19]+")"
	            self.save(self.dbConn, save_sql, data)
	f.close()
	self.dbConn.commit()
        #show table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.snTableName
            cu = self.get_cursor(self.dbConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])

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
    	    s=s[(pos+1):] 
    	    s=s[3:]
	    item.op="'"+s[:1]+"'"
	    s=s[2:]
	    item.type="'"+s[:7]+"'"
	    s=s[8:]
	    pos=s.index(":")
	    item.reg=s[:(pos-1)]
	    s=s[(pos+2):]
	    pos=s.index("\n")
	    item.dis="'"+s[:pos-1]+"'"	
	elif s.find("regfile_manager")>=0:
	    #16000[8]: system.cpu.regfile_manager: [tid:0]: [sn:2]: W R Reg 1 : 0xffffff01.
	    #318000[159]: system.cpu.regfile_manager: [tid:0]: [sn:118]: W MPU Reg 131 : 0x0 0x0 0x0 0x0
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
    	    s=s[(pos+1):] 
    	    s=s[2:]
	    item.op="'"+s[:1]+"'" #W MPU Reg 131 : 0x0 0x0 0x0 0x0
	    s=s[2:]   #MPU Reg 131 : 0x0 0x0 0x0 0x0 //  R Reg 1 : 0xffffff01.
	    temp=s
	    pos=temp.index(" ")
	    temp=temp[(pos+1):]
	    pos1=temp.index(" ")
	    pos=pos+pos1+1
	    item.type="'"+s[:pos]+"'"
	    s=s[(pos+1):]
	    pos=s.index(":")
	    item.reg=s[:(pos-1)]
	    s=s[(pos+2):]
	    pos=s.index("\n")
	    item.dis="'"+s[:pos-1]+"'"	
        return item

    def regTableInit(self):
        #drop table
        self.drop_table(self.dbConn,self.regTableName)
	#create table
        create_table_sql="create table "+self.regTableName+"(id integer primary key autoincrement,time integer,cpu integer,spumpu varchar(4),sn integer,op varchar(8),type varchar(8),reg integer,dis varchar(256))"
        self.create_table(self.dbConn, create_table_sql)

        save_sql = "INSERT INTO "+self.regTableName+" (time,cpu,spumpu,sn,op,type,reg,dis) "

        #open out file and read 
        f=open(self.filePath,"r")
        lines=f.readlines()
        item=RegItem()
	item.__init__()
        for line in lines:
	    if line.find("mpurf_manager")>=0 or line.find("regfile_manager")>=0:
	        item=self.regSplit(line,item)
		if item.op!="'R'":
	            data="values ("+item.time+","+item.cpu+","+item.spumpu+","+item.sn+","+item.op+","+item.type+","+item.reg+","+item.dis+")"
	            self.save(self.dbConn, save_sql, data)
	f.close()
	self.dbConn.commit()

        #show table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.regTableName
            cu = self.get_cursor(self.dbConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])

    def timeTableInit(self):
	#drop table
	self.drop_table(self.timeConn,self.timeTableName)
	#create table
	create_table_sql="create table "+self.timeTableName+"(time integer,DMBIU0 integer,DMBIU0Value varchar(16),BIU0DM integer,BIU0DMValue varchar(16),DMBIU1 integer,DMBIU1Value varchar(16),BIU1DM integer,BIU1DMValue varchar(16),DMBIU2 integer,DMBIU2Value varchar(16),BIU2DM integer,BIU2DMValue varchar(16),BIU0SHU0 integer,BIU0SHU0Value varchar(16),SHU0BIU0 integer,SHU0BIU0Value varchar(16),BIU1SHU0 integer,BIU1SHU0Value varchar(16),SHU0BIU1 integer,SHU0BIU1Value varchar(16),BIU2SHU0 integer,BIU2SHU0Value varchar(16),SHU0BIU2 integer,SHU0BIU2Value varchar(16),BIU0SHU1 integer,BIU0SHU1Value varchar(16), SHU1BIU0 integer,SHU1BIU0Value varchar(16),BIU1SHU1 integer,BIU1SHU1Value varchar(16),SHU1BIU1 integer,SHU1BIU1Value varchar(16),BIU2SHU1 integer,BIU2SHU1Value varchar(16),SHU1BIU2 integer,SHU1BIU2Value varchar(16),BIU0MRF integer,BIU0MRFValue varchar(16),MRFBIU0 integer,MRFBIU0Value varchar(16),BIU1MRF integer,BIU1MRFValue varchar(16),MRFBIU1 integer,MRFBIU1Value varchar(16),BIU2MRF integer,BIU2MRFValue varchar(16),MRFBIU2 integer,MRFBIU2Value varchar(16),SHU0IALU integer,SHU0IALUValue varchar(16),IALUSHU0 integer,IALUSHU0Value varchar(16),SHU0IMAC integer,SHU0IMACValue varchar(16),IMACSHU0 integer,IMACSHU0Value varchar(16),SHU0FALU integer,SHU0FALUValue varchar(16),FALUSHU0 integer,FALUSHU0Value varchar(16),SHU0FMAC integer,SHU0FMACValue varchar(16),FMACSHU0 integer,FMACSHU0Value varchar(16), MRFIALU integer,MRFIALUValue varchar(16),IALUMRF integer,IALUMRFValue varchar(16),MRFIMAC integer,MRFIMACValue varchar(16),IMACMRF integer,IMACMRFValue varchar(16),MRFFALU integer,MRFFALUValue varchar(16), FALUMRF integer,FALUMRFValue varchar(16),MRFFMAC integer,MRFFMACValue varchar(16),FMACMRF integer,FMACMRFValue varchar(16),SHU1IALU integer,SHU1IALUValue varchar(16),IALUSHU1 integer,IALUSHU1Value varchar(16),SHU1IMAC integer,SHU1IMACValue varchar(16),IMACSHU1 integer,IMACSHU1Value varchar(16),SHU1FALU integer,SHU1FALUValue varchar(16),FALUSHU1 integer,FALUSHU1Value varchar(16),SHU1FMAC integer,SHU1FMACValue varchar(16),FMACSHU1 integer,FMACSHU1Value varchar(16),IALUFALU integer,IALUFALUValue varchar(16),FALUIALU integer,FALUIALUValue varchar(16),SHU0MRF integer,SHU0MRFValue varchar(16),MRFSHU0 integer,MRFSHU0Value varchar(16),MRFSHU1 integer,MRFSHU1Value varchar(16),SHU1MRF integer,SHU1MRFValue varchar(16),IALUIMAC integer,IALUIMACValue varchar(16),IMACIALU integer,IMACIALUValue varchar(16),IMACFALU integer,IMACFALUValue varchar(16),FALUIMAC integer,FALUIMACValue varchar(16),FALUFMAC integer,FALUFMACValue varchar(16),FMACFALU integer,FMACFALUValue varchar(16),M0 varchar(128),M1 varchar(128),M2 varchar(128),M3 varchar(128),M4 varchar(128),M5 varchar(128),M6 varchar(128),M7 varchar(128),M8 varchar(128),M9 varchar(128),M10 varchar(128),M11 varchar(128),M12 varchar(128),M13 varchar(128),M14 varchar(128),M15 varchar(128),M16 varchar(128),M17 varchar(128),M18 varchar(128),M19 varchar(128),M20 varchar(128),M21 varchar(128),M22 varchar(128),M23 varchar(128),M24 varchar(128),M25 varchar(128),M26 varchar(128),M27 varchar(128),M28 varchar(128),M29 varchar(128),M30 varchar(128),M31 varchar(128),M32 varchar(128),M33 varchar(128),M34 varchar(128),M35 varchar(128),M36 varchar(128),M37 varchar(128),M38 varchar(128),M39 varchar(128),M40 varchar(128),M41 varchar(128),M42 varchar(128),M43 varchar(128),M44 varchar(128),M45 varchar(128),M46 varchar(128),M47 varchar(128),M48 varchar(128),M49 varchar(128),M50 varchar(128),M51 varchar(128),M52 varchar(128),M53 varchar(128),M54 varchar(128),M55 varchar(128),M56 varchar(128),M57 varchar(128),M58 varchar(128),M59 varchar(128),M60 varchar(128),M61 varchar(128),M62 varchar(128),M63 varchar(128),M64 varchar(128),M65 varchar(128),M66 varchar(128),M67 varchar(128),M68 varchar(128),M69 varchar(128),M70 varchar(128),M71 varchar(128),M72 varchar(128),M73 varchar(128),M74 varchar(128),M75 varchar(128),M76 varchar(128),M77 varchar(128),M78 varchar(128),M79 varchar(128),M80 varchar(128),M81 varchar(128),M82 varchar(128),M83 varchar(128),M84 varchar(128),M85 varchar(128),M86 varchar(128),M87 varchar(128),M88 varchar(128),M89 varchar(128),M90 varchar(128),M91 varchar(128),M92 varchar(128),M93 varchar(128),M94 varchar(128),M95 varchar(128),M96 varchar(128),M97 varchar(128),M98 varchar(128),M99 varchar(128),M100 varchar(128),M101 varchar(128),M102 varchar(128),M103 varchar(128),M104 varchar(128),M105 varchar(128),M106 varchar(128),M107 varchar(128),M108 varchar(128),M109 varchar(128),M110 varchar(128),M111 varchar(128),M112 varchar(128),M113 varchar(128),M114 varchar(128),M115 varchar(128),M116 varchar(128),M117 varchar(128),M118 varchar(128),M119 varchar(128),M120 varchar(128),M121 varchar(128),M122 varchar(128),M123 varchar(128),M124 varchar(128),M125 varchar(128),M126 varchar(128),M127 varchar(128),SHU0T0 varchar(128),SHU0T1 varchar(128),SHU0T2 varchar(128),SHU0T3 varchar(128),SHU0T4 varchar(128),SHU0T5 varchar(128),SHU0T6 varchar(128),SHU0T7 varchar(128),SHU0T8 varchar(128),SHU0T9 varchar(128),SHU0T10 varchar(128),SHU0T11 varchar(128),SHU0T12 varchar(128),SHU0T13 varchar(128),SHU0T14 varchar(128),SHU0T15 varchar(128),SHU1T0 varchar(128),SHU1T1 varchar(128),SHU1T2 varchar(128),SHU1T3 varchar(128),SHU1T4 varchar(128),SHU1T5 varchar(128),SHU1T6 varchar(128),SHU1T7 varchar(128),SHU1T8 varchar(128),SHU1T9 varchar(128),SHU1T10 varchar(128),SHU1T11 varchar(128),SHU1T12 varchar(128),SHU1T13 varchar(128),SHU1T14 varchar(128),SHU1T15 varchar(128),IALUT0 varchar(128),IALUT1 varchar(128),IALUT2 varchar(128),IALUT3 varchar(128),IMACT0 varchar(128),IMACT1 varchar(128),IMACT2 varchar(128),IMACT3 varchar(128),FALUT0 varchar(128),FALUT1 varchar(128),FALUT2 varchar(128),FALUT3 varchar(128),FMACT0 varchar(128),FMACT1 varchar(128),FMACT2 varchar(128),FMACT3 varchar(128),IMRL varchar(128),IMRH varchar(128),FMR varchar(128),BIU0W varchar(128),BIU1W varchar(128),BIU2W varchar(128),DIVQU varchar(128),DIVER varchar(128),DIVCNT varchar(128),SVR0 varchar(128),SVR1 varchar(128),R0 varchar(128),R1 varchar(128),R2 varchar(128),R3 varchar(128),R4 varchar(128),R5 varchar(128),R6 varchar(128),R7 varchar(128),R8 varchar(128),R9 varchar(128),R10 varchar(128),R11 varchar(128),R12 varchar(128),R13 varchar(128),R14 varchar(128),R15 varchar(128),R16 varchar(128),R17 varchar(128),R18 varchar(128),R19 varchar(128),R20 varchar(128),R21 varchar(128),R22 varchar(128),R23 varchar(128),R24 varchar(128),R25 varchar(128),R26 varchar(128),R27 varchar(128),R28 varchar(128),R29 varchar(128),R30 varchar(128),R31 varchar(128),J0 varchar(128),J1 varchar(128),J2 varchar(128),J3 varchar(128),J4 varchar(128),J5 varchar(128),J6 varchar(128),J7 varchar(128),J8 varchar(128),J9 varchar(128),J10 varchar(128),J11 varchar(128),J12 varchar(128),J13 varchar(128),J14 varchar(128),J15 varchar(128),J16 varchar(128),J17 varchar(128),J18 varchar(128),J19 varchar(128),J20 varchar(128),J21 varchar(128),J22 varchar(128),J23 varchar(128),J24 varchar(128),J25 varchar(128),J26 varchar(128),J27 varchar(128),J28 varchar(128),J29 varchar(128),J30 varchar(128),J31 varchar(128),DM0 varchar(16),DM0D varchar(16),DM1 varchar(16),DM1D varchar(16),DM2 varchar(16),DM2D varchar(16),DM3 varchar(16),DM3D varchar(16),DM4 varchar(16),DM4D varchar(16),DM5 varchar(16),DM5D varchar(16),DM6 varchar(16),DM6D varchar(16),DM7 varchar(16),DM7D varchar(16),DM8 varchar(16),DM8D varchar(16),DM9 varchar(16),DM9D varchar(16),DM10 varchar(16),DM10D varchar(16),DM11 varchar(16),DM11D varchar(16),DM12 varchar(16),DM12D varchar(16),DM13 varchar(16),DM13D varchar(16),DM14 varchar(16),DM14D varchar(16),DM15 varchar(16),DM15D varchar(16), DM16 varchar(16),DM16D varchar(16),DM17 varchar(16),DM17D varchar(16),DM18 varchar(16),DM18D varchar(16),DM19 varchar(16),DM19D varchar(16),BIU00 varchar(16),BIU00D varchar(16),BIU01 varchar(16),BIU01D varchar(16),BIU02 varchar(16),BIU02D varchar(16),BIU03 varchar(16),BIU03D varchar(16),BIU04 varchar(16),BIU04D varchar(16),BIU05 varchar(16),BIU05D varchar(16),BIU06 varchar(16),BIU06D varchar(16),BIU07 varchar(16),BIU07D varchar(16),BIU08 varchar(16),BIU08D varchar(16),BIU09 varchar(16),BIU09D varchar(16),BIU010 varchar(16),BIU010D varchar(16),BIU011 varchar(16),BIU011D varchar(16), BIU012 varchar(16),BIU012D varchar(16),BIU013 varchar(16),BIU013D varchar(16),BIU014 varchar(16),BIU014D varchar(16),BIU015 varchar(16),BIU015D varchar(16),BIU016 varchar(16),BIU016D varchar(16),BIU017 varchar(16),BIU017D varchar(16),BIU018 varchar(16),BIU018D varchar(16),BIU019 varchar(16),BIU019D varchar(16),BIU10 varchar(16),BIU10D varchar(16),BIU11 varchar(16),BIU11D varchar(16),BIU12 varchar(16),BIU12D varchar(16),BIU13 varchar(16),BIU13D varchar(16),BIU14 varchar(16),BIU14D varchar(16),BIU15 varchar(16),BIU15D varchar(16),BIU16 varchar(16), BIU16D varchar(16),BIU17 varchar(16),BIU17D varchar(16),BIU18 varchar(16),BIU18D varchar(16),BIU19 varchar(16),BIU19D varchar(16),BIU110 varchar(16),BIU110D varchar(16),BIU111 varchar(16),BIU111D varchar(16),BIU112 varchar(16),BIU112D varchar(16),BIU113 varchar(16),BIU113D varchar(16),BIU114 varchar(16),BIU114D varchar(16),BIU115 varchar(16),BIU115D varchar(16),BIU116 varchar(16),BIU116D varchar(16),BIU117 varchar(16),BIU117D varchar(16),BIU118 varchar(16),BIU118D varchar(16),BIU119 varchar(16),BIU119D varchar(16),BIU20 varchar(16),BIU20D varchar(16),BIU21 varchar(16),BIU21D varchar(16),BIU22 varchar(16),BIU22D varchar(16),BIU23 varchar(16),BIU23D varchar(16),BIU24 varchar(16),BIU24D varchar(16),BIU25 varchar(16),BIU25D varchar(16),BIU26 varchar(16),BIU26D varchar(16),BIU27 varchar(16),BIU27D varchar(16),BIU28 varchar(16),BIU28D varchar(16),BIU29 varchar(16),BIU29D varchar(16),BIU210 varchar(16),BIU210D varchar(16),BIU211 varchar(16),BIU211D varchar(16),BIU212 varchar(16),BIU212D varchar(16),BIU213 varchar(16),BIU213D varchar(16),BIU214 varchar(16),BIU214D varchar(16),BIU215 varchar(16),BIU215D varchar(16), BIU216 varchar(16),BIU216D varchar(16),BIU217 varchar(16),BIU217D varchar(16),BIU218 varchar(16),BIU218D varchar(16),BIU219 varchar(16),BIU219D varchar(16),SHU00 varchar(16),SHU00D varchar(16),SHU01 varchar(16),SHU01D varchar(16),SHU02 varchar(16),SHU02D varchar(16),SHU03 varchar(16),SHU03D varchar(16),SHU04 varchar(16),SHU04D varchar(16),SHU05 varchar(16),SHU05D varchar(16),SHU06 varchar(16),SHU06D varchar(16),SHU07 varchar(16),SHU07D varchar(16),SHU08 varchar(16),SHU08D varchar(16),SHU09 varchar(16),SHU09D varchar(16),SHU010 varchar(16),SHU010D varchar(16), SHU011 varchar(16),SHU011D varchar(16),SHU012 varchar(16),SHU012D varchar(16),SHU013 varchar(16),SHU013D varchar(16),SHU014 varchar(16),SHU014D varchar(16),SHU015 varchar(16),SHU015D varchar(16),SHU016 varchar(16),SHU016D varchar(16),SHU017 varchar(16),SHU017D varchar(16),SHU018 varchar(16),SHU018D varchar(16),SHU019 varchar(16),SHU019D varchar(16),MRF0 varchar(16),MRF0D varchar(16),MRF1 varchar(16),MRF1D varchar(16),MRF2 varchar(16),MRF2D varchar(16),MRF3 varchar(16),MRF3D varchar(16),MRF4 varchar(16),MRF4D varchar(16),MRF5 varchar(16),MRF5D varchar(16),MRF6 varchar(16), MRF6D varchar(16),MRF7 varchar(16),MRF7D varchar(16),MRF8 varchar(16),MRF8D varchar(16),MRF9 varchar(16),MRF9D varchar(16),MRF10 varchar(16),MRF10D varchar(16),MRF11 varchar(16),MRF11D varchar(16),MRF12 varchar(16),MRF12D varchar(16),MRF13 varchar(16),MRF13D varchar(16),MRF14 varchar(16),MRF14D varchar(16),MRF15 varchar(16),MRF15D varchar(16),MRF16 varchar(16),MRF16D varchar(16),MRF17 varchar(16),MRF17D varchar(16),MRF18 varchar(16),MRF18D varchar(16),MRF19 varchar(16),MRF19D varchar(16),SHU10 varchar(16),SHU10D varchar(16),SHU11 varchar(16),SHU11D varchar(16),SHU12 varchar(16),SHU12D varchar(16), SHU13 varchar(16),SHU13D varchar(16),SHU14 varchar(16),SHU14D varchar(16),SHU15 varchar(16),SHU15D varchar(16),SHU16 varchar(16),SHU16D varchar(16),SHU17 varchar(16),SHU17D varchar(16),SHU18 varchar(16),SHU18D varchar(16),SHU19 varchar(16),SHU19D varchar(16),SHU110 varchar(16),SHU110D varchar(16),SHU111 varchar(16),SHU111D varchar(16),SHU112 varchar(16),SHU112D varchar(16),SHU113 varchar(16),SHU113D varchar(16),SHU114 varchar(16),SHU114D varchar(16),SHU115 varchar(16),SHU115D varchar(16),SHU116 varchar(16),SHU116D varchar(16),SHU117 varchar(16), SHU117D varchar(16),SHU118 varchar(16),SHU118D varchar(16),SHU119 varchar(16),SHU119D varchar(16),IALU0 varchar(16),IALU0D varchar(16),IALU1 varchar(16),IALU1D varchar(16),IALU2 varchar(16),IALU2D varchar(16),IALU3 varchar(16),IALU3D varchar(16),IALU4 varchar(16),IALU4D varchar(16),IALU5 varchar(16),IALU5D varchar(16),IALU6 varchar(16),IALU6D varchar(16),IALU7 varchar(16),IALU7D varchar(16),IALU8 varchar(16),IALU8D varchar(16),IALU9 varchar(16),IALU9D varchar(16),IALU10 varchar(16),IALU10D varchar(16),IALU11 varchar(16),IALU11D varchar(16),IALU12 varchar(16), IALU12D varchar(16),IALU13 varchar(16),IALU13D varchar(16),IALU14 varchar(16),IALU14D varchar(16),IALU15 varchar(16),IALU15D varchar(16),IALU16 varchar(16),IALU16D varchar(16),IALU17 varchar(16),IALU17D varchar(16),IALU18 varchar(16),IALU18D varchar(16),IALU19 varchar(16),IALU19D varchar(16),IMAC0 varchar(16),IMAC0D varchar(16),IMAC1 varchar(16),IMAC1D varchar(16),IMAC2 varchar(16),IMAC2D varchar(16),IMAC3 varchar(16),IMAC3D varchar(16),IMAC4 varchar(16),IMAC4D varchar(16),IMAC5 varchar(16),IMAC5D varchar(16),IMAC6 varchar(16),IMAC6D varchar(16), IMAC7 varchar(16),IMAC7D varchar(16),IMAC8 varchar(16),IMAC8D varchar(16),IMAC9 varchar(16),IMAC9D varchar(16),IMAC10 varchar(16),IMAC10D varchar(16),IMAC11 varchar(16),IMAC11D varchar(16),IMAC12 varchar(16),IMAC12D varchar(16),IMAC13 varchar(16),IMAC13D varchar(16),IMAC14 varchar(16),IMAC14D varchar(16),IMAC15 varchar(16),IMAC15D varchar(16),IMAC16 varchar(16),IMAC16D varchar(16),IMAC17 varchar(16),IMAC17D varchar(16),IMAC18 varchar(16),IMAC18D varchar(16),IMAC19 varchar(16),IMAC19D varchar(16),FALU0 varchar(16),FALU0D varchar(16),FALU1 varchar(16),FALU1D varchar(16),FALU2 varchar(16),FALU2D varchar(16),FALU3 varchar(16),FALU3D varchar(16),FALU4 varchar(16),FALU4D varchar(16),FALU5 varchar(16),FALU5D varchar(16),FALU6 varchar(16),FALU6D varchar(16),FALU7 varchar(16),FALU7D varchar(16),FALU8 varchar(16),FALU8D varchar(16),FALU9 varchar(16),FALU9D varchar(16),FALU10 varchar(16),FALU10D varchar(16),FALU11 varchar(16),FALU11D varchar(16),FALU12 varchar(16),FALU12D varchar(16),FALU13 varchar(16),FALU13D varchar(16),FALU14 varchar(16),FALU14D varchar(16),FALU15 varchar(16),FALU15D varchar(16),FALU16 varchar(16), FALU16D varchar(16),FALU17 varchar(16),FALU17D varchar(16),FALU18 varchar(16),FALU18D varchar(16),FALU19 varchar(16),FALU19D varchar(16),FMAC0 varchar(16),FMAC0D varchar(16),FMAC1 varchar(16),FMAC1D varchar(16),FMAC2 varchar(16),FMAC2D varchar(16),FMAC3 varchar(16),FMAC3D varchar(16),FMAC4 varchar(16),FMAC4D varchar(16),FMAC5 varchar(16),FMAC5D varchar(16),FMAC6 varchar(16),FMAC6D varchar(16),FMAC7 varchar(16),FMAC7D varchar(16),FMAC8 varchar(16),FMAC8D varchar(16),FMAC9 varchar(16),FMAC9D varchar(16),FMAC10 varchar(16),FMAC10D varchar(16),FMAC11 varchar(16), FMAC11D varchar(16),FMAC12 varchar(16),FMAC12D varchar(16),FMAC13 varchar(16),FMAC13D varchar(16),FMAC14 varchar(16),FMAC14D varchar(16),FMAC15 varchar(16),FMAC15D varchar(16),FMAC16 varchar(16),FMAC16D varchar(16),FMAC17 varchar(16),FMAC17D varchar(16),FMAC18 varchar(16),FMAC18D varchar(16),FMAC19 varchar(16),FMAC19D varchar(16))"
	self.create_table(self.timeConn,create_table_sql)
	save_sql = "INSERT INTO "+self.timeTableName+" (time,DMBIU0,DMBIU0Value,BIU0DM,BIU0DMValue,DMBIU1,DMBIU1Value,BIU1DM,BIU1DMValue,DMBIU2,DMBIU2Value,BIU2DM,BIU2DMValue,BIU0SHU0,BIU0SHU0Value,SHU0BIU0,SHU0BIU0Value,BIU1SHU0,BIU1SHU0Value,SHU0BIU1, SHU0BIU1Value,BIU2SHU0,BIU2SHU0Value,SHU0BIU2,SHU0BIU2Value,BIU0SHU1,BIU0SHU1Value,SHU1BIU0,SHU1BIU0Value,BIU1SHU1,BIU1SHU1Value,SHU1BIU1,SHU1BIU1Value,BIU2SHU1,BIU2SHU1Value,SHU1BIU2,SHU1BIU2Value, BIU0MRF,BIU0MRFValue,MRFBIU0,MRFBIU0Value,BIU1MRF,BIU1MRFValue,MRFBIU1,MRFBIU1Value,BIU2MRF,BIU2MRFValue,MRFBIU2,MRFBIU2Value,SHU0IALU,SHU0IALUValue,IALUSHU0,IALUSHU0Value,SHU0IMAC,SHU0IMACValue,IMACSHU0, IMACSHU0Value,SHU0FALU,SHU0FALUValue,FALUSHU0,FALUSHU0Value,SHU0FMAC,SHU0FMACValue,FMACSHU0,FMACSHU0Value,MRFIALU,MRFIALUValue,IALUMRF,IALUMRFValue,MRFIMAC,MRFIMACValue,IMACMRF,IMACMRFValue,MRFFALU, MRFFALUValue,FALUMRF,FALUMRFValue,MRFFMAC,MRFFMACValue,FMACMRF,FMACMRFValue,SHU1IALU,SHU1IALUValue,IALUSHU1,IALUSHU1Value,SHU1IMAC,SHU1IMACValue,IMACSHU1,IMACSHU1Value,SHU1FALU,SHU1FALUValue,FALUSHU1, FALUSHU1Value,SHU1FMAC,SHU1FMACValue,FMACSHU1,FMACSHU1Value,IALUFALU,IALUFALUValue,FALUIALU,FALUIALUValue,SHU0MRF,SHU0MRFValue,MRFSHU0,MRFSHU0Value,MRFSHU1,MRFSHU1Value,SHU1MRF,SHU1MRFValue,IALUIMAC, IALUIMACValue,IMACIALU,IMACIALUValue,IMACFALU,IMACFALUValue,FALUIMAC,FALUIMACValue,FALUFMAC,FALUFMACValue,FMACFALU,FMACFALUValue,M0,M1,M2,M3,M4,M5,M6,M7,M8,M9,M10,M11,M12,M13,M14,M15,M16,M17,M18,M19,M20,M21,M22,M23,M24,M25, M26,M27,M28,M29,M30,M31,M32,M33,M34,M35,M36,M37,M38,M39,M40,M41,M42,M43,M44,M45,M46,M47,M48,M49,M50,M51,M52,M53,M54,M55,M56,M57,M58,M59,M60,M61,M62,M63,M64,M65,M66,M67,M68,M69,M70,M71,M72,M73,M74,M75,M76, M77,M78,M79,M80,M81,M82,M83,M84,M85,M86,M87,M88,M89,M90,M91,M92,M93,M94,M95,M96,M97,M98,M99,M100,M101,M102,M103,M104,M105,M106,M107,M108,M109,M110,M111,M112,M113,M114,M115,M116,M117,M118,M119,M120,M121,M122, M123,M124,M125,M126,M127,SHU0T0,SHU0T1,SHU0T2,SHU0T3,SHU0T4,SHU0T5,SHU0T6,SHU0T7,SHU0T8,SHU0T9,SHU0T10,SHU0T11,SHU0T12,SHU0T13,SHU0T14,SHU0T15,SHU1T0,SHU1T1,SHU1T2,SHU1T3,SHU1T4,SHU1T5,SHU1T6,SHU1T7,SHU1T8, SHU1T9,SHU1T10,SHU1T11,SHU1T12,SHU1T13,SHU1T14,SHU1T15,IALUT0,IALUT1,IALUT2,IALUT3,IMACT0,IMACT1,IMACT2,IMACT3,FALUT0,FALUT1,FALUT2,FALUT3,FMACT0,FMACT1,FMACT2,FMACT3,IMRL,IMRH,FMR,BIU0W,BIU1W,BIU2W,DIVQU, DIVER,DIVCNT,SVR0,SVR1,R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16,R17,R18,R19,R20,R21,R22,R23,R24,R25,R26,R27,R28,R29,R30,R31,J0,J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18, J19,J20,J21,J22,J23,J24,J25,J26,J27,J28,J29,J30,J31,DM0,DM0D,DM1,DM1D,DM2,DM2D,DM3,DM3D,DM4,DM4D,DM5,DM5D,DM6,DM6D,DM7,DM7D,DM8,DM8D,DM9,DM9D,DM10,DM10D,DM11,DM11D,DM12,DM12D,DM13,DM13D,DM14,DM14D,DM15,DM15D, DM16,DM16D,DM17,DM17D,DM18,DM18D,DM19,DM19D,BIU00,BIU00D,BIU01,BIU01D,BIU02,BIU02D,BIU03,BIU03D,BIU04,BIU04D,BIU05,BIU05D,BIU06,BIU06D,BIU07,BIU07D,BIU08,BIU08D,BIU09,BIU09D,BIU010,BIU010D,BIU011,BIU011D, BIU012,BIU012D,BIU013,BIU013D,BIU014,BIU014D,BIU015,BIU015D,BIU016,BIU016D,BIU017,BIU017D,BIU018,BIU018D,BIU019,BIU019D,BIU10,BIU10D,BIU11,BIU11D,BIU12,BIU12D,BIU13,BIU13D,BIU14,BIU14D,BIU15,BIU15D,BIU16, BIU16D,BIU17,BIU17D,BIU18,BIU18D,BIU19,BIU19D,BIU110,BIU110D,BIU111,BIU111D,BIU112,BIU112D,BIU113,BIU113D,BIU114,BIU114D,BIU115,BIU115D,BIU116,BIU116D,BIU117,BIU117D,BIU118,BIU118D,BIU119,BIU119D,BIU20,BIU20D, BIU21,BIU21D,BIU22,BIU22D,BIU23,BIU23D,BIU24,BIU24D,BIU25,BIU25D,BIU26,BIU26D,BIU27,BIU27D,BIU28,BIU28D,BIU29,BIU29D,BIU210,BIU210D,BIU211,BIU211D,BIU212,BIU212D,BIU213,BIU213D,BIU214,BIU214D,BIU215,BIU215D, BIU216,BIU216D,BIU217,BIU217D,BIU218,BIU218D,BIU219,BIU219D,SHU00,SHU00D,SHU01,SHU01D,SHU02,SHU02D,SHU03,SHU03D,SHU04,SHU04D,SHU05,SHU05D,SHU06,SHU06D,SHU07,SHU07D,SHU08,SHU08D,SHU09,SHU09D,SHU010,SHU010D, SHU011,SHU011D,SHU012,SHU012D,SHU013,SHU013D,SHU014,SHU014D,SHU015,SHU015D,SHU016,SHU016D,SHU017,SHU017D,SHU018,SHU018D,SHU019,SHU019D,MRF0,MRF0D,MRF1,MRF1D,MRF2,MRF2D,MRF3,MRF3D,MRF4,MRF4D,MRF5,MRF5D,MRF6, MRF6D,MRF7,MRF7D,MRF8,MRF8D,MRF9,MRF9D,MRF10,MRF10D,MRF11,MRF11D,MRF12,MRF12D,MRF13,MRF13D,MRF14,MRF14D,MRF15,MRF15D,MRF16,MRF16D,MRF17,MRF17D,MRF18,MRF18D,MRF19,MRF19D,SHU10,SHU10D,SHU11,SHU11D,SHU12,SHU12D, SHU13,SHU13D,SHU14,SHU14D,SHU15,SHU15D,SHU16,SHU16D,SHU17,SHU17D,SHU18,SHU18D,SHU19,SHU19D,SHU110,SHU110D,SHU111,SHU111D,SHU112,SHU112D,SHU113,SHU113D,SHU114,SHU114D,SHU115,SHU115D,SHU116,SHU116D,SHU117, SHU117D,SHU118,SHU118D,SHU119,SHU119D,IALU0,IALU0D,IALU1,IALU1D,IALU2,IALU2D,IALU3,IALU3D,IALU4,IALU4D,IALU5,IALU5D,IALU6,IALU6D,IALU7,IALU7D,IALU8,IALU8D,IALU9,IALU9D,IALU10,IALU10D,IALU11,IALU11D,IALU12, IALU12D,IALU13,IALU13D,IALU14,IALU14D,IALU15,IALU15D,IALU16,IALU16D,IALU17,IALU17D,IALU18,IALU18D,IALU19,IALU19D,IMAC0,IMAC0D,IMAC1,IMAC1D,IMAC2,IMAC2D,IMAC3,IMAC3D,IMAC4,IMAC4D,IMAC5,IMAC5D,IMAC6,IMAC6D, IMAC7,IMAC7D,IMAC8,IMAC8D,IMAC9,IMAC9D,IMAC10,IMAC10D,IMAC11,IMAC11D,IMAC12,IMAC12D,IMAC13,IMAC13D,IMAC14,IMAC14D,IMAC15,IMAC15D,IMAC16,IMAC16D,IMAC17,IMAC17D,IMAC18,IMAC18D,IMAC19,IMAC19D,FALU0,FALU0D,FALU1, FALU1D,FALU2,FALU2D,FALU3,FALU3D,FALU4,FALU4D,FALU5,FALU5D,FALU6,FALU6D,FALU7,FALU7D,FALU8,FALU8D,FALU9,FALU9D,FALU10,FALU10D,FALU11,FALU11D,FALU12,FALU12D,FALU13,FALU13D,FALU14,FALU14D,FALU15,FALU15D,FALU16, FALU16D,FALU17,FALU17D,FALU18,FALU18D,FALU19,FALU19D,FMAC0,FMAC0D,FMAC1,FMAC1D,FMAC2,FMAC2D,FMAC3,FMAC3D,FMAC4,FMAC4D,FMAC5,FMAC5D,FMAC6,FMAC6D,FMAC7,FMAC7D,FMAC8,FMAC8D,FMAC9,FMAC9D,FMAC10,FMAC10D,FMAC11, FMAC11D,FMAC12,FMAC12D,FMAC13,FMAC13D,FMAC14,FMAC14D,FMAC15,FMAC15D,FMAC16,FMAC16D,FMAC17,FMAC17D,FMAC18,FMAC18D,FMAC19,FMAC19D)"
	
	#read file content, get min and max time
	f=open(self.filePath,"r")
        lines=f.readlines()
	line=lines[0]
	pos=line.index("[")
	self.minTime=int(line[:pos])/1000
	count=len(lines)
	line=lines[count-1]
	pos=line.index("[")
	self.maxTime=int(line[:pos])/1000
	f.close()
	
	forwardRegList=["'nop'"]*251
	item=TimeItem()
	for i in range(self.minTime,self.maxTime+1):
	    item.__init__()
            regList=["'nop'"]*252
	    DMList=["'nop'"]*40
	    BIU0List=["'nop'"]*40
	    BIU1List=["'nop'"]*40
	    BIU2List=["'nop'"]*40
	    SHU0List=["'nop'"]*40
	    MRFList=["'nop'"]*40
	    SHU1List=["'nop'"]*40
	    IALUList=["'nop'"]*40
	    IMACList=["'nop'"]*40
	    FALUList=["'nop'"]*40
	    FMACList=["'nop'"]*40

            fetchall_sql = "SELECT * FROM "+self.snTableName+" WHERE stage0 = "+str(i)+" or "+"stage1 = "+str(i)+" or "+"stage2 = "+str(i)+" or "+"stage3 = "+str(i)+" or "+"stage4 = "+str(i)+" or "+"stage5 = "+str(i)+" or "+"stage6 = "+str(i)+" or "+"stage7 = "+str(i)+" or "+"stage8 = "+str(i)+" or "+"stage9 = "+str(i)+" or "+"stage10 = "+str(i)+" or "+"stage11 = "+str(i)+" or "+"stage12 = "+str(i)+" or "+"stage13 = "+str(i)+" or "+"stage14 = "+str(i)+" or "+"stage15 = "+str(i)+" or "+"stage16 = "+str(i)+" or "+"stage17 = "+str(i)+" or "+"stage18 = "+str(i)+" or "+"stage19 = "+str(i)
	    item.time=str(i)  #time
            r=self.fetchall(self.dbFilePath,fetchall_sql)
	    dis="nop"
	    if r!=0:
                if len(r) > 0:	
	            for e in range(len(r)):
		        if r[e][6]!="nop":  #dis (memo:ialuadd   ) IALU.T1+T2->IMAC.T0
			    s=r[e][6]
			    pos=s.find(")")
			    if pos<0:
				continue
			    s=s[(pos+2):]
			    pos=s.index(".")
			    temp=s[:pos]
			    s=s[(pos+1):]  
			    pos=s.find("->")
			    if pos!=-1:
			    	s=s[(pos+2):]
			    	pos=s.find(".")
			    	temp=temp+s[:pos]
			    	if temp=="DMBIU0":
				    item.DMBIU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.DMBIU0Value="'"+r[e][5]+"'"
				elif temp=="BIU0DM":
				    item.BIU0DM="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU0DMValue="'"+r[e][5]+"'"
				elif temp=="DMBIU1":
				    item.DMBIU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.DMBIU1Value="'"+r[e][5]+"'"
				elif temp=="BIU1DM":
				    item.BIU1DM="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU1DMValue="'"+r[e][5]+"'"
				elif temp=="DMBIU2":
				    item.DMBIU2="0"
				    if r[e][5]!="'nop'":  #pc
			                item.DMBIU2Value="'"+r[e][5]+"'"
				elif temp=="BIU2DM":
				    item.BIU2DM="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU2DMValue="'"+r[e][5]+"'"
				elif temp=="BIU0SHU0":
				    item.BIU0SHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU0SHU0Value="'"+r[e][5]+"'"
				elif temp=="SHU0BIU0":
				    item.SHU0BIU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0BIU0Value="'"+r[e][5]+"'"
				elif temp=="BIU1SHU0":
				    item.BIU1SHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU1SHU0Value="'"+r[e][5]+"'"
				elif temp=="SHU0BIU1":
				    item.SHU0BIU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0BIU1alue="'"+r[e][5]+"'"
				elif temp=="BIU2SHU0":
				    item.BIU2SHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU2SHU0Value="'"+r[e][5]+"'"
				elif temp=="SHU0BIU2":
				    item.SHU0BIU2="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0BIU2Value="'"+r[e][5]+"'"
				elif temp=="BIU0SHU1":
				    item.BIU0SHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU0SHU1Value="'"+r[e][5]+"'"
				elif temp=="SHU1BIU0":
				    item.SHU1BIU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1BIU0Value="'"+r[e][5]+"'"
				elif temp=="BIU1SHU1":
				    item.BIU1SHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU1SHU1Value="'"+r[e][5]+"'"
				elif temp=="SHU1BIU1":
				    item.SHU1BIU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1BIU1Value="'"+r[e][5]+"'"
				elif temp=="BIU2SHU1":
				    item.BIU2SHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU2SHU1Value="'"+r[e][5]+"'"
				elif temp=="SHU1BIU2":
				    item.SHU1BIU2="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1BIU2Value="'"+r[e][5]+"'"
				elif temp=="BIU0MRF":
				    item.BIU0MRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU0MRFValue="'"+r[e][5]+"'"
				elif temp=="MRFBIU0":
				    item.MRFBIU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFBIU0Value="'"+r[e][5]+"'"
				elif temp=="BIU1MRF":
				    item.BIU1MRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU1MRFValue="'"+r[e][5]+"'"
				elif temp=="MRFBIU1":
				    item.MRFBIU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFBIU1Value="'"+r[e][5]+"'"
				elif temp=="BIU2MRF":
				    item.BIU2MRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.BIU2MRFValue="'"+r[e][5]+"'"
				elif temp=="MRFBIU2":
				    item.MRFBIU2="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFBIU2Value="'"+r[e][5]+"'"
				elif temp=="SHU0IALU":
				    item.SHU0IALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0IALUValue="'"+r[e][5]+"'"
				elif temp=="IALUSHU0":
				    item.IALUSHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IALUSHU0Value="'"+r[e][5]+"'"
				elif temp=="SHU0IMAC":
				    item.SHU0IMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0IMACValue="'"+r[e][5]+"'"
				elif temp=="IMACSHU0":
				    item.IMACSHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IMACSHU0Value="'"+r[e][5]+"'"
				elif temp=="SHU0FALU":
				    item.SHU0FALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0FALUValue="'"+r[e][5]+"'"
				elif temp=="FALUSHU0":
				    item.FALUSHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FALUSHU0Value="'"+r[e][5]+"'"
				elif temp=="SHU0FMAC":
				    item.SHU0FMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0FMACValue="'"+r[e][5]+"'"
				elif temp=="FMACSHU0":
				    item.FMACSHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FMACSHU0Value="'"+r[e][5]+"'"
				elif temp=="MRFIALU":
				    item.MRFIALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFIALUValue="'"+r[e][5]+"'"
				elif temp=="IALUMRF":
				    item.IALUMRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IALUMRFValue="'"+r[e][5]+"'"
				elif temp=="MRFIMAC":
				    item.MRFIMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFIMACValue="'"+r[e][5]+"'"
				elif temp=="IMACMRF":
				    item.IMACMRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IMACMRFValue="'"+r[e][5]+"'"
				elif temp=="MRFFALU":
				    item.MRFFALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFFALUValue="'"+r[e][5]+"'"
				elif temp=="FALUMRF":
				    item.FALUMRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FALUMRFValue="'"+r[e][5]+"'"
				elif temp=="MRFFMAC":
				    item.MRFFMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFFMACValue="'"+r[e][5]+"'"
				elif temp=="FMACMRF":
				    item.FMACMRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FMACMRFValue="'"+r[e][5]+"'"
				elif temp=="SHU1IALU":
				    item.SHU1IALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1IALUValue="'"+r[e][5]+"'"
				elif temp=="IALUSHU1":
				    item.IALUSHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IALUSHU1Value="'"+r[e][5]+"'"
				elif temp=="SHU1IMAC":
				    item.SHU1IMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1IMACValue="'"+r[e][5]+"'"
				elif temp=="IMACSHU1":
				    item.IMACSHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IMACSHU1Value="'"+r[e][5]+"'"
				elif temp=="SHU1FALU":
				    item.SHU1FALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1FALUValue="'"+r[e][5]+"'"
				elif temp=="FALUSHU1":
				    item.FALUSHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FALUSHU1Value="'"+r[e][5]+"'"
				elif temp=="SHU1FMAC":
				    item.SHU1FMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1FMACValue="'"+r[e][5]+"'"
				elif temp=="FMACSHU1":
				    item.FMACSHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FMACSHU1Value="'"+r[e][5]+"'"
				elif temp=="IALUFALU":
				    item.IALUFALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IALUFALUValue="'"+r[e][5]+"'"
				elif temp=="FALUIALU":
				    item.FALUIALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FALUIALUValue="'"+r[e][5]+"'"
				elif temp=="SHU0MRF":
				    item.SHU0MRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU0MRFValue="'"+r[e][5]+"'"
				elif temp=="MRFSHU0":
				    item.MRFSHU0="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFSHU0Value="'"+r[e][5]+"'"
				elif temp=="MRFSHU1":
				    item.MRFSHU1="0"
				    if r[e][5]!="'nop'":  #pc
			                item.MRFSHU1Value="'"+r[e][5]+"'"
				elif temp=="SHU1MRF":
				    item.SHU1MRF="0"
				    if r[e][5]!="'nop'":  #pc
			                item.SHU1MRFValue="'"+r[e][5]+"'"
				elif temp=="IALUIMAC":
				    item.IALUIMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IALUIMACValue="'"+r[e][5]+"'"
				elif temp=="IMACIALU":
				    item.IMACIALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IMACIALUValue="'"+r[e][5]+"'"
				elif temp=="IMACFALU":
				    item.IMACFALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.IMACFALUValue="'"+r[e][5]+"'"
				elif temp=="FALUIMAC":
				    item.FALUIMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FALUIMACValue="'"+r[e][5]+"'"
				elif temp=="FALUFMAC":
				    item.FALUFMAC="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FALUFMACValue="'"+r[e][5]+"'"
				elif temp=="FMACFALU":
				    item.FMACFALU="0"
				    if r[e][5]!="'nop'":  #pc
			                item.FMACFALUValue="'"+r[e][5]+"'"
			    s=r[e][6]
			    pos=s.index(":")
			    s=s[(pos+1):]
			    a=s[:4]
			    stage="sn:"+str(r[e][3])
			    if r[e][4]!=-1:
				stage="'"+stage+",sln:"+str(r[e][4])+"'"
			    if a=="dm":
				if r[e][8]==i:
				     DMList[0]=stage
				     DMList[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     DMList[2]=stage
				     DMList[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     DMList[4]=stage
				     DMList[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     DMList[6]=stage
				     DMList[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     DMList[8]=stage
				     DMList[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     DMList[10]=stage
				     DMList[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     DMList[12]=stage
				     DMList[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     DMList[14]=stage
				     DMList[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     DMList[16]=stage
				     DMList[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     DMList[18]=stage
				     DMList[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     DMList[20]=stage
				     DMList[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     DMList[22]=stage
				     DMList[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     DMList[24]=stage
				     DMList[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     DMList[26]=stage
				     DMList[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     DMList[28]=stage
				     DMList[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     DMList[30]=stage
				     DMList[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     DMList[32]=stage
				     DMList[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     DMList[34]=stage
				     DMList[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     DMList[36]=stage
				     DMList[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     DMList[38]=stage
				     DMList[39]="'"+r[e][5]+"'"
			    elif a=="biu0":
				if r[e][8]==i:
				     BIU0List[0]=stage
				     BIU0List[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     BIU0List[2]=stage
				     BIU0List[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     BIU0List[4]=stage
				     BIU0List[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     BIU0List[6]=stage
				     BIU0List[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     BIU0List[8]=stage
				     BIU0List[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     BIU0List[10]=stage
				     BIU0List[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     BIU0List[12]=stage
				     BIU0List[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     BIU0List[14]=stage
				     BIU0List[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     BIU0List[16]=stage
				     BIU0List[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     BIU0List[18]=stage
				     BIU0List[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     BIU0List[20]=stage
				     BIU0List[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     BIU0List[22]=stage
				     BIU0List[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     BIU0List[24]=stage
				     BIU0List[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     BIU0List[26]=stage
				     BIU0List[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     BIU0List[28]=stage
				     BIU0List[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     BIU0List[30]=stage
				     BIU0List[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     BIU0List[32]=stage
				     BIU0List[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     BIU0List[34]=stage
				     BIU0List[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     BIU0List[36]=stage
				     BIU0List[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     BIU0List[38]=stage
				     BIU0List[39]="'"+r[e][5]+"'"
			    elif a=="biu1":
				if r[e][8]==i:
				     BIU1List[0]=stage
				     BIU1List[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     BIU1List[2]=stage
				     BIU1List[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     BIU1List[4]=stage
				     BIU1List[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     BIU1List[6]=stage
				     BIU1List[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     BIU1List[8]=stage
				     BIU1List[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     BIU1List[10]=stage
				     BIU1List[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     BIU1List[12]=stage
				     BIU1List[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     BIU1List[14]=stage
				     BIU1List[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     BIU1List[16]=stage
				     BIU1List[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     BIU1List[18]=stage
				     BIU1List[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     BIU1List[20]=stage
				     BIU1List[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     BIU1List[22]=stage
				     BIU1List[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     BIU1List[24]=stage
				     BIU1List[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     BIU1List[26]=stage
				     BIU1List[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     BIU1List[28]=stage
				     BIU1List[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     BIU1List[30]=stage
				     BIU1List[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     BIU1List[32]=stage
				     BIU1List[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     BIU1List[34]=stage
				     BIU1List[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     BIU1List[36]=stage
				     BIU1List[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     BIU1List[38]=stage
				     BIU1List[39]="'"+r[e][5]+"'"
			    elif a=="biu2":
				if r[e][8]==i:
				     BIU2List[0]=stage
				     BIU2List[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     BIU2List[2]=stage
				     BIU2List[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     BIU2List[4]=stage
				     BIU2List[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     BIU2List[6]=stage
				     BIU2List[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     BIU2List[8]=stage
				     BIU2List[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     BIU2List[10]=stage
				     BIU2List[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     BIU2List[12]=stage
				     BIU2List[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     BIU2List[14]=stage
				     BIU2List[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     BIU2List[16]=stage
				     BIU2List[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     BIU2List[18]=stage
				     BIU2List[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     BIU2List[20]=stage
				     BIU2List[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     BIU2List[22]=stage
				     BIU2List[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     BIU2List[24]=stage
				     BIU2List[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     BIU2List[26]=stage
				     BIU2List[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     BIU2List[28]=stage
				     BIU2List[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     BIU2List[30]=stage
				     BIU2List[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     BIU2List[32]=stage
				     BIU2List[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     BIU2List[34]=stage
				     BIU2List[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     BIU2List[36]=stage
				     BIU2List[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     BIU2List[38]=stage
				     BIU2List[39]="'"+r[e][5]+"'"
			    elif a=="shu0":
				if r[e][8]==i:
				     SHU0List[0]=stage
				     SHU0List[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     SHU0List[2]=stage
				     SHU0List[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     SHU0List[4]=stage
				     SHU0List[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     SHU0List[6]=stage
				     SHU0List[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     SHU0List[8]=stage
				     SHU0List[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     SHU0List[10]=stage
				     SHU0List[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     SHU0List[12]=stage
				     SHU0List[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     SHU0List[14]=stage
				     SHU0List[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     SHU0List[16]=stage
				     SHU0List[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     SHU0List[18]=stage
				     SHU0List[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     SHU0List[20]=stage
				     SHU0List[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     SHU0List[22]=stage
				     SHU0List[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     SHU0List[24]=stage
				     SHU0List[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     SHU0List[26]=stage
				     SHU0List[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     SHU0List[28]=stage
				     SHU0List[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     SHU0List[30]=stage
				     SHU0List[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     SHU0List[32]=stage
				     SHU0List[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     SHU0List[34]=stage
				     SHU0List[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     SHU0List[36]=stage
				     SHU0List[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     SHU0List[38]=stage
				     SHU0List[39]="'"+r[e][5]+"'"
			    elif a=="mrf":
				if r[e][8]==i:
				     MRFList[0]=stage
				     MRFList[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     MRFList[2]=stage
				     MRFList[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     MRFList[4]=stage
				     MRFList[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     MRFList[6]=stage
				     MRFList[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     MRFList[8]=stage
				     MRFList[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     MRFList[10]=stage
				     MRFList[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     MRFList[12]=stage
				     MRFList[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     MRFList[14]=stage
				     MRFList[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     MRFList[16]=stage
				     MRFList[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     MRFList[18]=stage
				     MRFList[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     MRFList[20]=stage
				     MRFList[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     MRFList[22]=stage
				     MRFList[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     MRFList[24]=stage
				     MRFList[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     MRFList[26]=stage
				     MRFList[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     MRFList[28]=stage
				     MRFList[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     MRFList[30]=stage
				     MRFList[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     MRFList[32]=stage
				     MRFList[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     MRFList[34]=stage
				     MRFList[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     MRFList[36]=stage
				     MRFList[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     MRFList[38]=stage
				     MRFList[39]="'"+r[e][5]+"'"
			    elif a=="shu1":
				if r[e][8]==i:
				     SHU1List[0]=stage
				     SHU1List[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     SHU1List[2]=stage
				     SHU1List[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     SHU1List[4]=stage
				     SHU1List[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     SHU1List[6]=stage
				     SHU1List[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     SHU1List[8]=stage
				     SHU1List[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     SHU1List[10]=stage
				     SHU1List[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     SHU1List[12]=stage
				     SHU1List[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     SHU1List[14]=stage
				     SHU1List[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     SHU1List[16]=stage
				     SHU1List[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     SHU1List[18]=stage
				     SHU1List[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     SHU1List[20]=stage
				     SHU1List[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     SHU1List[22]=stage
				     SHU1List[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     SHU1List[24]=stage
				     SHU1List[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     SHU1List[26]=stage
				     SHU1List[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     SHU1List[28]=stage
				     SHU1List[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     SHU1List[30]=stage
				     SHU1List[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     SHU1List[32]=stage
				     SHU1List[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     SHU1List[34]=stage
				     SHU1List[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     SHU1List[36]=stage
				     SHU1List[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     SHU1List[38]=stage
				     SHU1List[39]="'"+r[e][5]+"'"
			    elif a=="ialu":
				if r[e][8]==i:
				     IALUList[0]=stage
				     IALUList[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     IALUList[2]=stage
				     IALUList[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     IALUList[4]=stage
				     IALUList[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     IALUList[6]=stage
				     IALUList[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     IALUList[8]=stage
				     IALUList[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     IALUList[10]=stage
				     IALUList[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     IALUList[12]=stage
				     IALUList[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     IALUList[14]=stage
				     IALUList[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     IALUList[16]=stage
				     IALUList[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     IALUList[18]=stage
				     IALUList[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     IALUList[20]=stage
				     IALUList[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     IALUList[22]=stage
				     IALUList[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     IALUList[24]=stage
				     IALUList[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     IALUList[26]=stage
				     IALUList[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     IALUList[28]=stage
				     IALUList[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     IALUList[30]=stage
				     IALUList[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     IALUList[32]=stage
				     IALUList[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     IALUList[34]=stage
				     IALUList[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     IALUList[36]=stage
				     IALUList[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     IALUList[38]=stage
				     IALUList[39]="'"+r[e][5]+"'"
			    elif a=="imac":
				if r[e][8]==i:
				     IMACList[0]=stage
				     IMACList[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     IMACList[2]=stage
				     IMACList[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     IMACList[4]=stage
				     IMACList[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     IMACList[6]=stage
				     IMACList[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     IMACList[8]=stage
				     IMACList[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     IMACList[10]=stage
				     IMACList[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     IMACList[12]=stage
				     IMACList[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     IMACList[14]=stage
				     IMACList[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     IMACList[16]=stage
				     IMACList[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     IMACList[18]=stage
				     IMACList[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     IMACList[20]=stage
				     IMACList[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     IMACList[22]=stage
				     IMACList[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     IMACList[24]=stage
				     IMACList[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     IMACList[26]=stage
				     IMACList[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     IMACList[28]=stage
				     IMACList[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     IMACList[30]=stage
				     IMACList[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     IMACList[32]=stage
				     IMACList[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     IMACList[34]=stage
				     IMACList[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     IMACList[36]=stage
				     IMACList[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     IMACList[38]=stage
				     IMACList[39]="'"+r[e][5]+"'"
			    elif a=="falu":
				if r[e][8]==i:
				     FALUList[0]=stage
				     FALUList[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     FALUList[2]=stage
				     FALUList[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     FALUList[4]=stage
				     FALUList[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     FALUList[6]=stage
				     FALUList[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     FALUList[8]=stage
				     FALUList[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     FALUList[10]=stage
				     FALUList[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     FALUList[12]=stage
				     FALUList[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     FALUList[14]=stage
				     FALUList[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     FALUList[16]=stage
				     FALUList[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     FALUList[18]=stage
				     FALUList[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     FALUList[20]=stage
				     FALUList[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     FALUList[22]=stage
				     FALUList[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     FALUList[24]=stage
				     FALUList[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     FALUList[26]=stage
				     FALUList[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     FALUList[28]=stage
				     FALUList[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     FALUList[30]=stage
				     FALUList[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     FALUList[32]=stage
				     FALUList[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     FALUList[34]=stage
				     FALUList[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     FALUList[36]=stage
				     FALUList[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     FALUList[38]=stage
				     FALUList[39]="'"+r[e][5]+"'"
			    elif a=="fmac":
				if r[e][8]==i:
				     FMACList[0]=stage
				     FMACList[1]="'"+r[e][5]+"'"
				elif r[e][9]==i:
				     FMACList[2]=stage
				     FMACList[3]="'"+r[e][5]+"'"
				elif r[e][10]==i:
				     FMACList[4]=stage
				     FMACList[5]="'"+r[e][5]+"'"
				elif r[e][11]==i:
				     FMACList[6]=stage
				     FMACList[7]="'"+r[e][5]+"'"
				elif r[e][12]==i:
				     FMACList[8]=stage
				     FMACList[9]="'"+r[e][5]+"'"
				elif r[e][13]==i:
				     FMACList[10]=stage
				     FMACList[11]="'"+r[e][5]+"'"
				elif r[e][14]==i:
				     FMACList[12]=stage
				     FMACList[13]="'"+r[e][5]+"'"
				elif r[e][15]==i:
				     FMACList[14]=stage
				     FMACList[15]="'"+r[e][5]+"'"
				elif r[e][16]==i:
				     FMACList[16]=stage
				     FMACList[17]="'"+r[e][5]+"'"
				elif r[e][17]==i:
				     FMACList[18]=stage
				     FMACList[19]="'"+r[e][5]+"'"
				elif r[e][18]==i:
				     FMACList[20]=stage
				     FMACList[21]="'"+r[e][5]+"'"
				elif r[e][19]==i:
				     FMACList[22]=stage
				     FMACList[23]="'"+r[e][5]+"'"
				elif r[e][20]==i:
				     FMACList[24]=stage
				     FMACList[25]="'"+r[e][5]+"'"
				elif r[e][21]==i:
				     FMACList[26]=stage
				     FMACList[27]="'"+r[e][5]+"'"
				elif r[e][22]==i:
				     FMACList[28]=stage
				     FMACList[29]="'"+r[e][5]+"'"
				elif r[e][23]==i:
				     FMACList[30]=stage
				     FMACList[31]="'"+r[e][5]+"'"
				elif r[e][24]==i:
				     FMACList[32]=stage
				     FMACList[33]="'"+r[e][5]+"'"
				elif r[e][25]==i:
				     FMACList[34]=stage
				     FMACList[35]="'"+r[e][5]+"'"
				elif r[e][26]==i:
				     FMACList[36]=stage
				     FMACList[37]="'"+r[e][5]+"'"
				elif r[e][27]==i:
				     FMACList[38]=stage
				     FMACList[39]="'"+r[e][5]+"'"

	    fetchall_sql = "SELECT * FROM "+self.regTableName+" WHERE time = "+str(i)
            r=self.fetchall(self.dbFilePath,fetchall_sql)
	    if r!=0:
                if len(r) > 0:	
	            for e in range(len(r)):
			if r[e][6]=="MPU Reg":
			    regList[r[e][7]]="'"+r[e][8]+"'"
			elif r[e][6]=="R Reg":
			    regList[187+r[e][7]]="'"+r[e][8]+"'"
			elif r[e][6]=="J Reg":
			    regList[187+r[e][7]]="'"+r[e][8]+"'"

	    for i in range(0,251):
		if regList[i]=="'nop'":
		    if forwardRegList[i]!="'nop'":
			regList[i]=forwardRegList[i]

	    forwardRegList=copy.deepcopy(regList)

	    data="values ("+item.time+","+item.DMBIU0+","+item.DMBIU0Value+","+item.BIU0DM+","+item.BIU0DMValue+","+item.DMBIU1+","+item.DMBIU1Value+","+item.BIU1DM+","+item.BIU1DMValue+","+item.DMBIU2+","+item.DMBIU2Value+","+item.BIU2DM+","+item.BIU2DMValue+","+item.BIU0SHU0+","+item.BIU0SHU0Value+","+item.SHU0BIU0+","+item.SHU0BIU0Value+","+item.BIU1SHU0+","+item.BIU1SHU0Value+","+item.SHU0BIU1+","+item.SHU0BIU1Value+","+item.BIU2SHU0+","+item.BIU2SHU0Value+","+item.SHU0BIU2+","+item.SHU0BIU2Value+","+item.BIU0SHU1+","+item.BIU0SHU1Value+","+item.SHU1BIU0+","+item.SHU1BIU0Value+","+item.BIU1SHU1+","+item.BIU1SHU1Value+","+item.SHU1BIU1+","+item.SHU1BIU1Value+","+item.BIU2SHU1+","+item.BIU2SHU1Value+","+item.SHU1BIU2+","+item.SHU1BIU2Value+","+ item.BIU0MRF+","+item.BIU0MRFValue+","+item.MRFBIU0+","+item.MRFBIU0Value+","+item.BIU1MRF+","+item.BIU1MRFValue+","+item.MRFBIU1+","+item.MRFBIU1Value+","+item.BIU2MRF+","+item.BIU2MRFValue+","+item.MRFBIU2+","+item.MRFBIU2Value+","+item.SHU0IALU+","+item.SHU0IALUValue+","+item.IALUSHU0+","+item.IALUSHU0Value+","+item.SHU0IMAC+","+item.SHU0IMACValue+","+item.IMACSHU0+","+item.IMACSHU0Value+","+item.SHU0FALU+","+item.SHU0FALUValue+","+item.FALUSHU0+","+item.FALUSHU0Value+","+item.SHU0FMAC+","+item.SHU0FMACValue+","+item.FMACSHU0+","+item.FMACSHU0Value+","+item.MRFIALU+","+item.MRFIALUValue+","+item.IALUMRF+","+item.IALUMRFValue+","+item.MRFIMAC+","+item.MRFIMACValue+","+item.IMACMRF+","+item.IMACMRFValue+","+item.MRFFALU+","+item.MRFFALUValue+","+item.FALUMRF+","+item.FALUMRFValue+","+item.MRFFMAC+","+item.MRFFMACValue+","+item.FMACMRF+","+item.FMACMRFValue+","+item.SHU1IALU+","+item.SHU1IALUValue+","+item.IALUSHU1+","+item.IALUSHU1Value+","+item.SHU1IMAC+","+item.SHU1IMACValue+","+item.IMACSHU1+","+item.IMACSHU1Value+","+item.SHU1FALU+","+item.SHU1FALUValue+","+item.FALUSHU1+","+item.FALUSHU1Value+","+item.SHU1FMAC+","+item.SHU1FMACValue+","+item.FMACSHU1+","+item.FMACSHU1Value+","+item.IALUFALU+","+item.IALUFALUValue+","+item.FALUIALU+","+item.FALUIALUValue+","+item.SHU0MRF+","+item.SHU0MRFValue+","+item.MRFSHU0+","+item.MRFSHU0Value+","+item.MRFSHU1+","+item.MRFSHU1Value+","+item.SHU1MRF+","+item.SHU1MRFValue+","+item.IALUIMAC+","+item.IALUIMACValue+","+item.IMACIALU+","+item.IMACIALUValue+","+item.IMACFALU+","+item.IMACFALUValue+","+item.FALUIMAC+","+item.FALUIMACValue+","+item.FALUFMAC+","+item.FALUFMACValue+","+item.FMACFALU+","+item.FMACFALUValue+","+regList[0]+","+regList[1]+","+regList[2]+","+regList[3]+","+regList[4]+","+regList[5]+","+regList[6]+","+regList[7]+","+regList[8]+","+regList[9]+","+regList[10]+","+regList[11]+","+regList[12]+","+regList[13]+","+regList[14]+","+regList[15]+","+regList[16]+","+regList[17]+","+regList[18]+","+regList[19]+","+regList[20]+","+regList[21]+","+regList[22]+","+regList[23]+","+regList[24]+","+regList[25]+","+ regList[26]+","+regList[27]+","+regList[28]+","+regList[29]+","+regList[30]+","+regList[31]+","+regList[32]+","+regList[33]+","+regList[34]+","+regList[35]+","+regList[36]+","+regList[37]+","+regList[38]+","+regList[39]+","+regList[40]+","+regList[41]+","+regList[42]+","+regList[43]+","+regList[44]+","+regList[45]+","+regList[46]+","+regList[47]+","+regList[48]+","+regList[49]+","+regList[50]+","+regList[51]+","+regList[52]+","+regList[53]+","+regList[54]+","+regList[55]+","+regList[56]+","+regList[57]+","+regList[58]+","+regList[59]+","+regList[60]+","+regList[61]+","+regList[62]+","+regList[63]+","+regList[64]+","+regList[65]+","+regList[66]+","+regList[67]+","+regList[68]+","+regList[69]+","+regList[70]+","+regList[71]+","+regList[72]+","+regList[73]+","+regList[74]+","+regList[75]+","+regList[76]+","+ regList[77]+","+regList[78]+","+regList[79]+","+regList[80]+","+regList[81]+","+regList[82]+","+regList[83]+","+regList[84]+","+regList[85]+","+regList[86]+","+regList[87]+","+regList[88]+","+regList[89]+","+regList[90]+","+regList[91]+","+regList[92]+","+regList[93]+","+regList[94]+","+regList[95]+","+regList[96]+","+regList[97]+","+regList[98]+","+regList[99]+","+regList[100]+","+regList[101]+","+regList[102]+","+regList[103]+","+regList[104]+","+regList[105]+","+regList[106]+","+regList[107]+","+regList[108]+","+regList[109]+","+regList[110]+","+regList[111]+","+regList[112]+","+regList[113]+","+regList[114]+","+regList[115]+","+regList[116]+","+regList[117]+","+regList[118]+","+regList[119]+","+regList[120]+","+regList[121]+","+regList[122]+","+regList[123]+","+regList[124]+","+regList[125]+","+regList[126]+","+regList[127]+","+regList[128]+","+regList[129]+","+regList[130]+","+regList[131]+","+regList[132]+","+regList[133]+","+regList[134]+","+regList[135]+","+regList[136]+","+regList[137]+","+regList[138]+","+regList[139]+","+regList[140]+","+regList[141]+","+regList[142]+","+regList[143]+","+regList[144]+","+regList[145]+","+regList[146]+","+regList[147]+","+regList[148]+","+regList[149]+","+regList[150]+","+regList[151]+","+regList[152]+","+regList[153]+","+regList[154]+","+regList[155]+","+regList[156]+","+regList[157]+","+regList[158]+","+regList[159]+","+regList[160]+","+regList[161]+","+regList[162]+","+regList[163]+","+regList[164]+","+regList[165]+","+regList[166]+","+regList[167]+","+regList[168]+","+regList[169]+","+regList[170]+","+regList[171]+","+regList[172]+","+regList[173]+","+regList[174]+","+regList[175]+","+regList[176]+","+regList[177]+","+regList[178]+","+regList[179]+","+regList[180]+","+regList[181]+","+regList[182]+","+regList[183]+","+regList[184]+","+regList[185]+","+regList[186]+","+regList[187]+","+regList[188]+","+regList[189]+","+regList[190]+","+regList[191]+","+regList[192]+","+regList[193]+","+regList[194]+","+regList[195]+","+regList[196]+","+regList[197]+","+regList[198]+","+regList[199]+","+regList[200]+","+regList[201]+","+regList[202]+","+regList[203]+","+regList[204]+","+regList[205]+","+regList[206]+","+regList[207]+","+regList[208]+","+regList[209]+","+regList[210]+","+regList[211]+","+regList[212]+","+regList[213]+","+regList[214]+","+regList[215]+","+regList[216]+","+regList[217]+","+regList[218]+","+regList[219]+","+regList[220]+","+regList[221]+","+regList[222]+","+regList[223]+","+regList[224]+","+regList[225]+","+regList[226]+","+regList[227]+","+regList[228]+","+regList[229]+","+regList[230]+","+regList[231]+","+regList[232]+","+regList[233]+","+regList[234]+","+regList[235]+","+regList[236]+","+regList[237]+","+regList[238]+","+ regList[239]+","+regList[240]+","+regList[241]+","+regList[242]+","+regList[243]+","+regList[244]+","+regList[245]+","+regList[246]+","+regList[247]+","+regList[248]+","+regList[249]+","+regList[250]+","+DMList[0]+","+DMList[1]+","+DMList[2]+","+DMList[3]+","+DMList[4]+","+DMList[5]+","+DMList[6]+","+DMList[7]+","+DMList[8]+","+DMList[9]+","+DMList[10]+","+DMList[11]+","+DMList[12]+","+DMList[13]+","+DMList[14]+","+DMList[15]+","+DMList[16]+","+DMList[17]+","+DMList[18]+","+DMList[19]+","+DMList[20]+","+DMList[21]+","+DMList[22]+","+DMList[23]+","+DMList[24]+","+DMList[25]+","+DMList[26]+","+DMList[27]+","+DMList[28]+","+DMList[29]+","+DMList[30]+","+DMList[31]+","+DMList[32]+","+DMList[33]+","+DMList[34]+","+DMList[35]+","+DMList[36]+","+DMList[37]+","+DMList[38]+","+DMList[39]+","+BIU0List[0]+","+BIU0List[1]+","+BIU0List[2]+","+BIU0List[3]+","+BIU0List[4]+","+BIU0List[5]+","+BIU0List[6]+","+BIU0List[7]+","+BIU0List[8]+","+BIU0List[9]+","+BIU0List[10]+","+BIU0List[11]+","+BIU0List[12]+","+BIU0List[13]+","+BIU0List[14]+","+BIU0List[15]+","+BIU0List[16]+","+BIU0List[17]+","+BIU0List[18]+","+BIU0List[19]+","+BIU0List[20]+","+BIU0List[21]+","+BIU0List[22]+","+BIU0List[23]+","+BIU0List[24]+","+BIU0List[25]+","+BIU0List[26]+","+BIU0List[27]+","+BIU0List[28]+","+BIU0List[29]+","+BIU0List[30]+","+BIU0List[31]+","+BIU0List[32]+","+BIU0List[33]+","+BIU0List[34]+","+BIU0List[35]+","+BIU0List[36]+","+BIU0List[37]+","+BIU0List[38]+","+BIU0List[39]+","+BIU1List[0]+","+BIU1List[1]+","+BIU1List[2]+","+BIU1List[3]+","+BIU1List[4]+","+BIU1List[5]+","+BIU1List[6]+","+BIU1List[7]+","+BIU1List[8]+","+BIU1List[9]+","+BIU1List[10]+","+BIU1List[11]+","+BIU1List[12]+","+BIU1List[13]+","+BIU1List[14]+","+BIU1List[15]+","+BIU1List[16]+","+BIU1List[17]+","+BIU1List[18]+","+BIU1List[19]+","+BIU1List[20]+","+BIU1List[21]+","+BIU1List[22]+","+BIU1List[23]+","+BIU1List[24]+","+BIU1List[25]+","+BIU1List[26]+","+BIU1List[27]+","+BIU1List[28]+","+BIU1List[29]+","+BIU1List[30]+","+BIU1List[31]+","+BIU1List[32]+","+BIU1List[33]+","+BIU1List[34]+","+BIU1List[35]+","+BIU1List[36]+","+BIU1List[37]+","+BIU1List[38]+","+BIU1List[39]+","+BIU2List[0]+","+BIU2List[1]+","+BIU2List[2]+","+BIU2List[3]+","+BIU2List[4]+","+BIU2List[5]+","+BIU2List[6]+","+BIU2List[7]+","+BIU2List[8]+","+BIU2List[9]+","+BIU2List[10]+","+BIU2List[11]+","+BIU2List[12]+","+BIU2List[13]+","+BIU2List[14]+","+BIU2List[15]+","+BIU2List[16]+","+BIU2List[17]+","+BIU2List[18]+","+BIU2List[19]+","+BIU2List[20]+","+BIU2List[21]+","+BIU2List[22]+","+BIU2List[23]+","+BIU2List[24]+","+BIU2List[25]+","+BIU2List[26]+","+BIU2List[27]+","+BIU2List[28]+","+BIU2List[29]+","+BIU2List[30]+","+BIU2List[31]+","+BIU2List[32]+","+BIU2List[33]+","+BIU2List[34]+","+BIU2List[35]+","+BIU2List[36]+","+BIU2List[37]+","+BIU2List[38]+","+BIU2List[39]+","+SHU0List[0]+","+SHU0List[1]+","+SHU0List[2]+","+SHU0List[3]+","+SHU0List[4]+","+SHU0List[5]+","+SHU0List[6]+","+SHU0List[7]+","+SHU0List[8]+","+SHU0List[9]+","+SHU0List[10]+","+SHU0List[11]+","+SHU0List[12]+","+SHU0List[13]+","+SHU0List[14]+","+SHU0List[15]+","+SHU0List[16]+","+SHU0List[17]+","+SHU0List[18]+","+SHU0List[19]+","+SHU0List[20]+","+SHU0List[21]+","+SHU0List[22]+","+SHU0List[23]+","+SHU0List[24]+","+SHU0List[25]+","+SHU0List[26]+","+SHU0List[27]+","+SHU0List[28]+","+SHU0List[29]+","+SHU0List[30]+","+SHU0List[31]+","+SHU0List[32]+","+SHU0List[33]+","+SHU0List[34]+","+SHU0List[35]+","+SHU0List[36]+","+SHU0List[37]+","+SHU0List[38]+","+SHU0List[39]+","+MRFList[0]+","+MRFList[1]+","+MRFList[2]+","+MRFList[3]+","+MRFList[4]+","+MRFList[5]+","+MRFList[6]+","+MRFList[7]+","+MRFList[8]+","+MRFList[9]+","+MRFList[10]+","+MRFList[11]+","+MRFList[12]+","+MRFList[13]+","+MRFList[14]+","+MRFList[15]+","+MRFList[16]+","+MRFList[17]+","+MRFList[18]+","+MRFList[19]+","+MRFList[20]+","+MRFList[21]+","+MRFList[22]+","+MRFList[23]+","+MRFList[24]+","+MRFList[25]+","+MRFList[26]+","+MRFList[27]+","+MRFList[28]+","+MRFList[29]+","+MRFList[30]+","+MRFList[31]+","+MRFList[32]+","+MRFList[33]+","+MRFList[34]+","+MRFList[35]+","+MRFList[36]+","+MRFList[37]+","+MRFList[38]+","+MRFList[39]+","+SHU1List[0]+","+SHU1List[1]+","+SHU1List[2]+","+SHU1List[3]+","+SHU1List[4]+","+SHU1List[5]+","+SHU1List[6]+","+SHU1List[7]+","+SHU1List[8]+","+SHU1List[9]+","+SHU1List[10]+","+SHU1List[11]+","+SHU1List[12]+","+SHU1List[13]+","+SHU1List[14]+","+SHU1List[15]+","+SHU1List[16]+","+SHU1List[17]+","+SHU1List[18]+","+SHU1List[19]+","+SHU1List[20]+","+SHU1List[21]+","+SHU1List[22]+","+SHU1List[23]+","+SHU1List[24]+","+SHU1List[25]+","+SHU1List[26]+","+SHU1List[27]+","+SHU1List[28]+","+SHU1List[29]+","+SHU1List[30]+","+SHU1List[31]+","+SHU1List[32]+","+SHU1List[33]+","+SHU1List[34]+","+SHU1List[35]+","+SHU1List[36]+","+SHU1List[37]+","+SHU1List[38]+","+SHU1List[39]+","+IALUList[0]+","+IALUList[1]+","+IALUList[2]+","+IALUList[3]+","+IALUList[4]+","+IALUList[5]+","+IALUList[6]+","+IALUList[7]+","+IALUList[8]+","+IALUList[9]+","+IALUList[10]+","+IALUList[11]+","+IALUList[12]+","+IALUList[13]+","+IALUList[14]+","+IALUList[15]+","+IALUList[16]+","+IALUList[17]+","+IALUList[18]+","+IALUList[19]+","+IALUList[20]+","+IALUList[21]+","+IALUList[22]+","+IALUList[23]+","+IALUList[24]+","+IALUList[25]+","+IALUList[26]+","+IALUList[27]+","+IALUList[28]+","+IALUList[29]+","+IALUList[30]+","+IALUList[31]+","+IALUList[32]+","+IALUList[33]+","+IALUList[34]+","+IALUList[35]+","+IALUList[36]+","+IALUList[37]+","+IALUList[38]+","+IALUList[39]+","+IMACList[0]+","+IMACList[1]+","+IMACList[2]+","+IMACList[3]+","+IMACList[4]+","+IMACList[5]+","+IMACList[6]+","+IMACList[7]+","+IMACList[8]+","+IMACList[9]+","+IMACList[10]+","+IMACList[11]+","+IMACList[12]+","+IMACList[13]+","+IMACList[14]+","+IMACList[15]+","+IMACList[16]+","+IMACList[17]+","+IMACList[18]+","+IMACList[19]+","+IMACList[20]+","+IMACList[21]+","+IMACList[22]+","+IMACList[23]+","+IMACList[24]+","+IMACList[25]+","+IMACList[26]+","+IMACList[27]+","+IMACList[28]+","+IMACList[29]+","+IMACList[30]+","+IMACList[31]+","+IMACList[32]+","+IMACList[33]+","+IMACList[34]+","+IMACList[35]+","+IMACList[36]+","+IMACList[37]+","+IMACList[38]+","+IMACList[39]+","+FALUList[0]+","+FALUList[1]+","+FALUList[2]+","+FALUList[3]+","+FALUList[4]+","+FALUList[5]+","+FALUList[6]+","+FALUList[7]+","+FALUList[8]+","+FALUList[9]+","+FALUList[10]+","+FALUList[11]+","+FALUList[12]+","+FALUList[13]+","+FALUList[14]+","+FALUList[15]+","+FALUList[16]+","+FALUList[17]+","+FALUList[18]+","+FALUList[19]+","+FALUList[20]+","+FALUList[21]+","+FALUList[22]+","+FALUList[23]+","+FALUList[24]+","+FALUList[25]+","+FALUList[26]+","+FALUList[27]+","+FALUList[28]+","+FALUList[29]+","+FALUList[30]+","+FALUList[31]+","+FALUList[32]+","+FALUList[33]+","+FALUList[34]+","+FALUList[35]+","+FALUList[36]+","+FALUList[37]+","+FALUList[38]+","+FALUList[39]+","+FMACList[0]+","+FMACList[1]+","+FMACList[2]+","+FMACList[3]+","+FMACList[4]+","+FMACList[5]+","+FMACList[6]+","+FMACList[7]+","+FMACList[8]+","+FMACList[9]+","+FMACList[10]+","+FMACList[11]+","+FMACList[12]+","+FMACList[13]+","+FMACList[14]+","+FMACList[15]+","+FMACList[16]+","+FMACList[17]+","+FMACList[18]+","+FMACList[19]+","+FMACList[20]+","+FMACList[21]+","+FMACList[22]+","+FMACList[23]+","+FMACList[24]+","+FMACList[25]+","+FMACList[26]+","+FMACList[27]+","+FMACList[28]+","+FMACList[29]+","+FMACList[30]+","+FMACList[31]+","+FMACList[32]+","+FMACList[33]+","+FMACList[34]+","+FMACList[35]+","+FMACList[36]+","+FMACList[37]+","+FMACList[38]+","+FMACList[39]+")"

	    self.save(self.timeConn,save_sql,data)
	self.timeConn.commit()
        #show table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.timeTableName
            cu = self.get_cursor(self.timeConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])




