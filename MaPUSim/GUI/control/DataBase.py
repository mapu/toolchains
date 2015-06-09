import sys
import os
import sqlite3
sys.path.append("..")
from data.TableType import*

reload(sys)
sys.setdefaultencoding('utf-8')

class DataBase():
    def __init__(self):
	self.filePath="/home/litt/alu.out"
	self.dbFilePath="table.db"
	self.snTableName="sn"
	self.regTableName="reg"
	self.timeTableName="time"

        self.dbConn = self.get_conn(self.dbFilePath)

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
                conn.commit()
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
            conn.commit()
            self.close_all(conn, cu)

    def delete(self,conn,sql,data):
        if sql is not None and sql != '':
            if data is not None:
                cu = self.get_cursor(conn)
                for d in data:
                    cu.execute(sql, d)
                    conn.commit()
                self.close_all(conn, cu)

    def snSplit(self,string):
        s=string
        item=SnItem()
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
	    stage=s[0:pos]
	    if stage=="stage0":
	        item.stage0=item.time
	    elif stage=="stage1":
	        item.stage1=item.time
	    elif stage=="stage2":
	        item.stage2=item.time
	    elif stage=="stage3":
	        item.stage3=item.time
	    elif stage=="stage4":
	        item.stage4=item.time
	    elif stage=="stage5":
	        item.stage5=item.time
	    elif stage=="stage6":
	        item.stage6=item.time
	    elif stage=="stage7":
	        item.stage7=item.time
	    elif stage=="stage8":
	        item.stage8=item.time
	    elif stage=="stage9":
	        item.stage9=item.time
	    elif stage=="stage10":
	        item.stage10=item.time
	    elif stage=="stage11":
	        item.stage11=item.time
	    elif stage=="stage12":
	        item.stage12=item.time
	    elif stage=="stage13":
	        item.stage13=item.time
	    elif stage=="stage14":
	        item.stage14=item.time
	    elif stage=="stage15":
	        item.stage15=item.time
	    elif stage=="stage16":
	        item.stage16=item.time
	    elif stage=="stage17":
	        item.stage17=item.time
	    elif stage=="stage18":
	        item.stage18=item.time
	    elif stage=="stage19":
	        item.stage19=item.time

	    s=s[(pos+2):]
    	    #get sn
    	    pos=s.index(":")
    	    s=s[(pos+1):]
    	    pos=s.index("]")
    	    item.sn=s[0:pos]
    	    s=s[(pos+1):] 

        elif s.find("spurf_manager")>=0 or s.find("mpurf_manager")>=0:
	    #326000[163]: system.cpu3.mpurf_manager: [sn:75] : W MPU Reg 143 : 0x0 0x0 0x0 0x0 0x0
	    pos=13
     	    item.spumpu=s[0:pos] 
    	    if item.spumpu=="spurf_manager":
	        item.spumpu="'s'"
    	    else:
 	        item.spumpu="'m'"
    	    s=s[(pos+2):] 
    	    #get sn
    	    pos=s.index(":")
    	    s=s[(pos+1):]
    	    pos=s.index("]")
    	    item.sn=s[0:pos]
    	    s=s[(pos+1):] 
    	    s=s[3:]
	    pos=s.index(":")
	    temp=s[0:pos-1]
	    s=s[(pos+2):]
	    if s.index("\n"):
	        pos=s.index("\n")
	        s=s[0:pos]
	    item.dis="'"+s+"'"
	
        else:
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
	    else:
	        #4000[2]: system.cpu.im_port: [sn:2] : [sln:2] : 4 : immtransr  r1, -255
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
	    	#pos=s.index("\n")
	    	#s=s[:pos]
	    	#item.dis="'"+s+"'"

        return item

    def snTableInit(self):
        #drop table
        self.drop_table(self.dbConn,self.snTableName)
	#create table
        create_table_sql="create table "+self.snTableName+"(id integer primary key autoincrement,cpu integer,spumpu varchar(8),sn integer,sln integer,pc varchar(8),dis varchar(128),stage0 integer,stage1 integer,stage2 integer,stage3 integer,stage4 integer,stage5 integer,stage6 integer,stage7 integer,stage8 integer,stage9 integer,stage10 integer,stage11 integer,stage12 integer,stage13 integer,stage14 integer,stage15 integer,stage16 integer,stage17 integer,stage18 integer,stage19 integer)"
        self.create_table(self.dbConn, create_table_sql)

        save_sql = "INSERT INTO "+self.snTableName+" (cpu,spumpu,sn,sln,pc,dis,stage0,stage1,stage2,stage3,stage4,stage5,stage6,stage7,stage8,stage9,stage10,stage11,stage12,stage13,stage14,stage15,stage16,stage17,stage18,stage19) "

        #open out file and read 
        f=open(self.filePath,"r")
        lines=f.readlines()
        for line in lines:
	    if line.find("mpurf_manager")<0:
	        item=self.snSplit(line)
    	        fetchone_sql = "SELECT * FROM "+self.snTableName+" WHERE spumpu = "+item.spumpu+" and "+"sn = "+item.sn
                cu = self.get_cursor(self.dbConn)
                cu.execute(fetchone_sql)
                r = cu.fetchall()
                if len(r) > 0:
		    #update 
		    if line.find("stage")>=0:
		        if item.stage0!="-1":
			    stage="stage0"
			    if r[0][7]!=-1:
			        if r[0][7]>=int(item.stage0):
				    time=item.stage0
			        else:
				    time=str(r[0][7])
			    else:
			        time=item.stage0
		        elif item.stage1!="-1":
			    stage="stage1"
			    if r[0][8]!=-1:
			        if r[0][8]>=int(item.stage1):
				    time=item.stage1
			        else:
				    time=str(r[0][8])
			    else:
			        time=item.stage1
		        elif item.stage2!="-1":
			    stage="stage2"
			    if r[0][9]!=-1:
			        if r[0][9]>=int(item.stage2):
				    time=item.stage2
			        else:
				    time=str(r[0][9])
			    else:
			        time=item.stage2	
		        elif item.stage3!="-1":
			    stage="stage3"
			    if r[0][10]!=-1:
			        if r[0][10]>=int(item.stage3):
				    time=item.stage3
			        else:
				    time=str(r[0][10])
			    else:
			        time=item.stage3
		        elif item.stage4!="-1":
			    stage="stage4"
			    if r[0][11]!=-1:
			        if r[0][11]>=int(item.stage4):
				    time=item.stage4
			        else:
				    time=str(r[0][11])
			    else:
		                time=item.stage4
		        elif item.stage5!="-1":
			    stage="stage5"
			    if r[0][12]!=-1:
			        if r[0][12]>=int(item.stage5):
				    time=item.stage5
			        else:
				    time=str(r[0][12])
			    else:
			        time=item.stage5
		        elif item.stage6!="-1":
			    stage="stage6"
			    if r[0][13]!=-1:
			        if r[0][13]>=int(item.stage6):
				    time=item.stage6
			        else:
				    time=str(r[0][13])
			    else:
			        time=item.stage6
		        elif item.stage7!="-1":
			    stage="stage7"
			    if r[0][14]!=-1:
			        if r[0][14]>=int(item.stage7):
				    time=item.stage7
			        else:
				    time=str(r[0][14])
			    else:
			        time=item.stage7
		        elif item.stage8!="-1":
			    stage="stage8"
			    if r[0][15]!=-1:
			        if r[0][15]>=int(item.stage8):
				    time=item.stage8
			        else:
				    time=str(r[0][15])
			    else:
			        time=item.stage8
		        elif item.stage9!="-1":
			    stage="stage9"
			    if r[0][16]!=-1:
			        if r[0][16]>=int(item.stage9):
				    time=item.stage9
			        else:
				    time=str(r[0][16])
			    else:
			        time=item.stage9
		        elif item.stage10!="-1":
			    stage="stage10"
			    if r[0][17]!=-1:
			        if r[0][17]>=int(item.stage10):
				    time=item.stage10
			        else:
				    time=str(r[0][17])
			    else:
			        time=item.stage10
		        elif item.stage11!="-1":
			    stage="stage11"
			    if r[0][18]!=-1:
			        if r[0][18]>=int(item.stage11):
				    time=item.stage11
			        else:
				    time=str(r[0][18])
			    else:
			        time=item.stage11
		        elif item.stage12!="-1":
			    stage="stage12"
			    if r[0][19]!=-1:
			        if r[0][19]>=int(item.stage12):
				    time=item.stage12
			        else:
				    time=str(r[0][19])
			    else:
			        time=item.stage12
		        elif item.stage13!="-1":
			    stage="stage13"
			    if r[0][20]!=-1:
			        if r[0][20]>=int(item.stage13):
				    time=item.stage13
			        else:
				    time=str(r[0][20])
			    else:
			        time=item.stage13
		        elif item.stage14!="-1":
			    stage="stage14"
			    if r[0][21]!=-1:
			        if r[0][21]>=int(item.stage14):
				    time=item.stage14
			        else:
				    time=str(r[0][21])
			    else:
			        time=item.stage14
		        elif item.stage15!="-1":
			    stage="stage15"
			    if r[0][22]!=-1:
			        if r[0][22]>=int(item.stage15):
				    time=item.stage15
			        else:
				    time=str(r[0][22])
			    else:
			        time=item.stage15
		        elif item.stage16!="-1":
			    stage="stage16"
			    if r[0][23]!=-1:
			        if r[0][23]>=int(item.stage16):
				    time=item.stage16
			        else:
				    time=str(r[0][23])
			    else:
			        time=item.stage16
		        elif item.stage17!="-1":
			    stage="stage17"
			    if r[0][24]!=-1:
			        if r[0][24]>=int(item.stage17):
				    time=item.stage17
			        else:
				    time=str(r[0][24])
			    else:
			        time=item.stage17
		        elif item.stage18!="-1":
			    stage="stage18"
			    if r[0][25]!=-1:
			        if r[0][25]>=int(item.stage18):
				    time=item.stage18
			        else:
				    time=str(r[0][25])
			    else:
			        time=item.stage18
		        elif item.stage19!="-1":
			    stage="stage19"
			    if r[0][26]!=-1:
			        if r[0][26]>=int(item.stage19):
				    time=item.stage19
			        else:
				    time=str(r[0][26])
			    else:
			        time=item.stage19

		        update_sql = "UPDATE "+self.snTableName+" SET "+stage+" = "+time+" WHERE spumpu = "+item.spumpu+" and "+"sn = "+item.sn
	    	        self.update(self.dbConn, update_sql)
		    else:
		        update_sql = "UPDATE "+self.snTableName+" SET sln = "+item.sln+" , pc = "+item.pc+" , dis = "+item.dis+" WHERE spumpu = "+item.spumpu+" and "+"sn = "+item.sn
	    	        self.update(self.dbConn, update_sql)		    	
	        else:
	            data="values ("+item.cpu+","+item.spumpu+","+item.sn+","+item.sln+","+item.pc+","+item.dis+","+item.stage0+","+item.stage1+","+item.stage2+","+item.stage3+","+item.stage4+","+item.stage5+","+item.stage6+","+item.stage7+","+item.stage8+","+item.stage9+","+item.stage10+","+item.stage11+","+item.stage12+","+item.stage13+","+item.stage14+","+item.stage15+","+item.stage16+","+item.stage17+","+item.stage18+","+item.stage19+")"
	            self.save(self.dbConn, save_sql, data)
	f.close()

        #show table
	if 0:
            fetchall_sql = "SELECT * FROM "+self.snTableName
            cu = self.get_cursor(self.dbConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])

    def regSplit(self,string):
        s=string
        item=RegItem()
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

        if s.find("spurf_manager")>=0 or s.find("mpurf_manager")>=0:
	    #326000[163]: system.cpu3.mpurf_manager: [sn:75] : W MPU Reg 143 : 0x0 0x0 0x0 0x0 0x0
	    pos=13
    	    s=s[(pos+2):] 
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
        return item

    def regTableInit(self):
        #drop table
        self.drop_table(self.dbConn,self.regTableName)
	#create table
        create_table_sql="create table "+self.regTableName+"(id integer primary key autoincrement,time integer,cpu integer,sn integer,op varchar(8),type varchar(8),reg integer,dis varchar(256))"
        self.create_table(self.dbConn, create_table_sql)

        save_sql = "INSERT INTO "+self.regTableName+" (time,cpu,sn,op,type,reg,dis) "

        #open out file and read 
        f=open(self.filePath,"r")
        lines=f.readlines()
        for line in lines:
	    if line.find("mpurf_manager")>=0:
	        item=self.regSplit(line)
	        data="values ("+item.time+","+item.cpu+","+item.sn+","+item.op+","+item.type+","+item.reg+","+item.dis+")"
	        self.save(self.dbConn, save_sql, data)
	f.close()
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
	self.drop_table(self.dbConn,self.timeTableName)
	#create table
	create_table_sql="create table "+self.timeTableName+"(time integer,DMBIU0 integer,DMBIU0Value varchar(16),BIU0DM integer,BIU0DMValue varchar(16),DMBIU1 integer,DMBIU1Value varchar(16),BIU1DM integer,BIU1DMValue varchar(16),DMBIU2 integer,DMBIU2Value varchar(16),BIU2DM integer,BIU2DMValue varchar(16),BIU0SHU0 integer,BIU0SHU0Value varchar(16),SHU0BIU0 integer,SHU0BIU0Value varchar(16),BIU1SHU0 integer,BIU1SHU0Value varchar(16),SHU0BIU1 integer,SHU0BIU1Value varchar(16),BIU2SHU0 integer,BIU2SHU0Value varchar(16),SHU0BIU2 integer,SHU0BIU2Value varchar(16),BIU0SHU1 integer,BIU0SHU1Value varchar(16), SHU1BIU0 integer,SHU1BIU0Value varchar(16),BIU1SHU1 integer,BIU1SHU1Value varchar(16),SHU1BIU1 integer,SHU1BIU1Value varchar(16),BIU2SHU1 integer,BIU2SHU1Value varchar(16),SHU1BIU2 integer,SHU1BIU2Value varchar(16),BIU0MRF integer,BIU0MRFValue varchar(16),MRFBIU0 integer,MRFBIU0Value varchar(16),BIU1MRF integer,BIU1MRFValue varchar(16),MRFBIU1 integer,MRFBIU1Value varchar(16),BIU2MRF integer,BIU2MRFValue varchar(16),MRFBIU2 integer,MRFBIU2Value varchar(16),SHU0IALU integer,SHU0IALUValue varchar(16),IALUSHU0 integer,IALUSHU0Value varchar(16),SHU0IMAC integer,SHU0IMACValue varchar(16),IMACSHU0 integer,IMACSHU0Value varchar(16),SHU0FALU integer,SHU0FALUValue varchar(16),FALUSHU0 integer,FALUSHU0Value varchar(16),SHU0FMAC integer,SHU0FMACValue varchar(16),FMACSHU0 integer,FMACSHU0Value varchar(16), MRFIALU integer,MRFIALUValue varchar(16),IALUMRF integer,IALUMRFValue varchar(16),MRFIMAC integer,MRFIMACValue varchar(16),IMACMRF integer,IMACMRFValue varchar(16),MRFFALU integer,MRFFALUValue varchar(16), FALUMRF integer,FALUMRFValue varchar(16),MRFFMAC integer,MRFFMACValue varchar(16),FMACMRF integer,FMACMRFValue varchar(16),SHU1IALU integer,SHU1IALUValue varchar(16),IALUSHU1 integer,IALUSHU1Value varchar(16),SHU1IMAC integer,SHU1IMACValue varchar(16),IMACSHU1 integer,IMACSHU1Value varchar(16),SHU1FALU integer,SHU1FALUValue varchar(16),FALUSHU1 integer,FALUSHU1Value varchar(16),SHU1FMAC integer,SHU1FMACValue varchar(16),FMACSHU1 integer,FMACSHU1Value varchar(16),IALUFALU integer,IALUFALUValue varchar(16),FALUIALU integer,FALUIALUValue varchar(16),reg varchar(16),regValue integer,dis varchar(128))"
	self.create_table(self.dbConn,create_table_sql)
	save_sql = "INSERT INTO "+self.timeTableName+" (time,DMBIU0,DMBIU0Value,BIU0DM,BIU0DMValue,DMBIU1,DMBIU1Value,BIU1DM,BIU1DMValue,DMBIU2,DMBIU2Value,BIU2DM,BIU2DMValue,BIU0SHU0,BIU0SHU0Value,SHU0BIU0,SHU0BIU0Value,BIU1SHU0,BIU1SHU0Value,SHU0BIU1, SHU0BIU1Value,BIU2SHU0,BIU2SHU0Value,SHU0BIU2,SHU0BIU2Value,BIU0SHU1,BIU0SHU1Value,SHU1BIU0,SHU1BIU0Value,BIU1SHU1,BIU1SHU1Value,SHU1BIU1,SHU1BIU1Value,BIU2SHU1,BIU2SHU1Value,SHU1BIU2,SHU1BIU2Value, BIU0MRF,BIU0MRFValue,MRFBIU0,MRFBIU0Value,BIU1MRF,BIU1MRFValue,MRFBIU1,MRFBIU1Value,BIU2MRF,BIU2MRFValue,MRFBIU2,MRFBIU2Value,SHU0IALU,SHU0IALUValue,IALUSHU0,IALUSHU0Value,SHU0IMAC,SHU0IMACValue,IMACSHU0, IMACSHU0Value,SHU0FALU,SHU0FALUValue,FALUSHU0,FALUSHU0Value,SHU0FMAC,SHU0FMACValue,FMACSHU0,FMACSHU0Value,MRFIALU,MRFIALUValue,IALUMRF,IALUMRFValue,MRFIMAC,MRFIMACValue,IMACMRF,IMACMRFValue,MRFFALU, MRFFALUValue,FALUMRF,FALUMRFValue,MRFFMAC,MRFFMACValue,FMACMRF,FMACMRFValue,SHU1IALU,SHU1IALUValue,IALUSHU1,IALUSHU1Value,SHU1IMAC,SHU1IMACValue,IMACSHU1,IMACSHU1Value,SHU1FALU,SHU1FALUValue,FALUSHU1, FALUSHU1Value,SHU1FMAC,SHU1FMACValue,FMACSHU1,FMACSHU1Value,IALUFALU,IALUFALUValue,FALUIALU,FALUIALUValue,reg,regValue,dis)"
	
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
	
	for i in range(self.minTime,self.maxTime):
	    item=TimeItem()
            fetchall_sql = "SELECT * FROM "+self.snTableName+" WHERE stage0 = "+str(i)+" or "+"stage1 = "+str(i)+" or "+"stage2 = "+str(i)+" or "+"stage3 = "+str(i)+" or "+"stage4 = "+str(i)+" or "+"stage5 = "+str(i)+" or "+"stage6 = "+str(i)+" or "+"stage7 = "+str(i)+" or "+"stage8 = "+str(i)+" or "+"stage9 = "+str(i)+" or "+"stage10 = "+str(i)+" or "+"stage11 = "+str(i)+" or "+"stage12 = "+str(i)+" or "+"stage13 = "+str(i)+" or "+"stage14 = "+str(i)+" or "+"stage15 = "+str(i)+" or "+"stage16 = "+str(i)+" or "+"stage17 = "+str(i)+" or "+"stage18 = "+str(i)+" or "+"stage19 = "+str(i)
	    item.time=str(i)  #time
            r=self.fetchall(self.dbFilePath,fetchall_sql)
	    dis="nop"
	    if r!=0:
                if len(r) > 0:	
	            for e in range(len(r)):
		        if r[e][6]!="nop":  #dis (memo:ialuadd   ) IALU.T1+T2->IMAC.T0
			    dis=r[e][6]
			    s=r[e][6]
			    #print s
			    pos=s.index(")")
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

	    fetchall_sql = "SELECT * FROM "+self.regTableName+" WHERE time = "+str(i)
            r=self.fetchall(self.dbFilePath,fetchall_sql)
	    if r!=0:
                if len(r) > 0:	
	            for e in range(len(r)):
			item.reg="'"+r[e][4]+"'"
			item.regValue=str(r[e][6])
			item.dis="'"+r[e][7]+"'"
	    #if dis!="nop" or item.reg!="'nop'":	
	    data="values ("+item.time+","+item.DMBIU0+","+item.DMBIU0Value+","+item.BIU0DM+","+item.BIU0DMValue+","+item.DMBIU1+","+item.DMBIU1Value+","+item.BIU1DM+","+item.BIU1DMValue+","+item.DMBIU2+","+item.DMBIU2Value+","+item.BIU2DM+","+item.BIU2DMValue+","+item.BIU0SHU0+","+item.BIU0SHU0Value+","+item.SHU0BIU0+","+item.SHU0BIU0Value+","+item.BIU1SHU0+","+item.BIU1SHU0Value+","+item.SHU0BIU1+","+item.SHU0BIU1Value+","+item.BIU2SHU0+","+item.BIU2SHU0Value+","+item.SHU0BIU2+","+item.SHU0BIU2Value+","+item.BIU0SHU1+","+item.BIU0SHU1Value+","+item.SHU1BIU0+","+item.SHU1BIU0Value+","+item.BIU1SHU1+","+item.BIU1SHU1Value+","+item.SHU1BIU1+","+item.SHU1BIU1Value+","+item.BIU2SHU1+","+item.BIU2SHU1Value+","+item.SHU1BIU2+","+item.SHU1BIU2Value+","+ item.BIU0MRF+","+item.BIU0MRFValue+","+item.MRFBIU0+","+item.MRFBIU0Value+","+item.BIU1MRF+","+item.BIU1MRFValue+","+item.MRFBIU1+","+item.MRFBIU1Value+","+item.BIU2MRF+","+item.BIU2MRFValue+","+item.MRFBIU2+","+item.MRFBIU2Value+","+item.SHU0IALU+","+item.SHU0IALUValue+","+item.IALUSHU0+","+item.IALUSHU0Value+","+item.SHU0IMAC+","+item.SHU0IMACValue+","+item.IMACSHU0+","+item.IMACSHU0Value+","+item.SHU0FALU+","+item.SHU0FALUValue+","+item.FALUSHU0+","+item.FALUSHU0Value+","+item.SHU0FMAC+","+item.SHU0FMACValue+","+item.FMACSHU0+","+item.FMACSHU0Value+","+item.MRFIALU+","+item.MRFIALUValue+","+item.IALUMRF+","+item.IALUMRFValue+","+item.MRFIMAC+","+item.MRFIMACValue+","+item.IMACMRF+","+item.IMACMRFValue+","+item.MRFFALU+","+item.MRFFALUValue+","+item.FALUMRF+","+item.FALUMRFValue+","+item.MRFFMAC+","+item.MRFFMACValue+","+item.FMACMRF+","+item.FMACMRFValue+","+item.SHU1IALU+","+item.SHU1IALUValue+","+item.IALUSHU1+","+item.IALUSHU1Value+","+item.SHU1IMAC+","+item.SHU1IMACValue+","+item.IMACSHU1+","+item.IMACSHU1Value+","+item.SHU1FALU+","+item.SHU1FALUValue+","+item.FALUSHU1+","+item.FALUSHU1Value+","+item.SHU1FMAC+","+item.SHU1FMACValue+","+item.FMACSHU1+","+item.FMACSHU1Value+","+item.IALUFALU+","+item.IALUFALUValue+","+item.FALUIALU+","+item.FALUIALUValue+","+item.reg+","+item.regValue+","+item.dis+")"
	    self.save(self.dbConn, save_sql, data)

        #show table
	if 1:
            fetchall_sql = "SELECT * FROM "+self.timeTableName
            cu = self.get_cursor(self.dbConn)
            cu.execute(fetchall_sql)
            r = cu.fetchall()
            if len(r) > 0:
                for e in range(len(r)):
		    print (r[e])




