# -*- coding: utf-8 -*-
'''
Created on Mar 09, 2016

@author: litt 
'''
from PyQt4.Qt import QObject
import sqlite3

class MicrocodeDB(QObject):
    '''
    The base class of entire microcode table
    '''

    def __init__(self, parent = None):
        '''
        Constructor
        '''

        super(MicrocodeDB, self).__init__(parent)
        self.DBName = "asmgui.db"
        self.DBConn = sqlite3.connect(self.DBName)
        self.itemsid = ["id", "mc", "cyc"]
        self.itemstype = ["integer primary key autoincrement", "varchar(128)", "varchar(16)"]
        self.name = "Entire"
        self.mcc = []

    def createTable(self):
        '''
        create the table
        '''
	self.clearTable()
        sql_query = "CREATE TABLE " + self.name + "("
        sql_query += ", ".join(" ".join(x)
                               for x in zip(self.itemsid, self.itemstype))
        sql_query += ")"
        self.DBConn.execute(sql_query)   
        self.initMcc()
        self.microcodeAnalyze(self.mcc)

    def clearTable(self):
        '''
        Drop the table first if it exists, and then create a new one
        '''

        sql_query = "DROP TABLE IF EXISTS " + self.name
        self.DBConn.execute(sql_query)   
      
    def microcodeAnalyze(self, code):
	insert_template = "INSERT INTO " + self.name + " ("
        insert_template += ", ".join(self.itemsid[1:])
        insert_template += ") VALUES("
        insert_template += ", ".join(["?"] * (len(self.itemsid) - 1)) + ")"
        self.DBConn.executemany(insert_template, code)
        self.DBConn.commit() 
        
    def searchMcc(self, code):
	search = "SELECT cyc from "+ self.name + " WHERE mc = '" + code + "'"
	cu = self.DBConn.execute(search)
	out = cu.fetchall()
	return out[0][0]

    def initMcc(self):
        self.mcc = [["MR0ToM", "3"],
		    ["MR0ToMACC", "2"],
		    ["MR0ToSHU", "3"],
		    ["MR1ToSHU", "3"],
		    ["MR1ToMACC", "2"],
		    ["MR2ToMACC", "2"],
		    ["MR2WToMACC", "4"],
		    ["MR3ToBIU", "3"],
		    ["MR3ToBIUKG", "4"],
		    ["SHUIndTToBIU", "3"],
		    ["SHUIndTToM", "4"],
		    ["SHUIndTToMACC", "3"],
		    ["SHUIndTToSHU", "2"],
		    ["SHUIndTBToBIU", "3"],
		    ["SHUIndTBToM", "4"],
		    ["SHUIndTBToMACC", "3"],
		    ["SHUIndTBToSHU", "2"],
		    ["SHUCombToBIU", "3"],
		    ["SHUCombToM", "4"],
		    ["SHUCombToMACC", "3"],
		    ["SHUCombToSHU", "2"],
		    ["BIULdToM", "12"],
		    ["BIULdToMACC", "11"],
		    ["BIULdToSHU", "11"],
		    ["BIUSt", "8"],
		    ["BIUKG", "4"],
		    ["IALUMergeToBIU", "4"],
		    ["IALUMergeToM", "4"],
		    ["IALUMergeToMACC", "3"],
		    ["IALUMergeToSHU", "4"],
		    ["IALUAddToBIU", "4"],
		    ["IALUAddToM", "4"],
		    ["IALUAddToMACC", "3"],
		    ["IALUAddToSHU", "4"],
		    ["IALUSubToBIU", "4"],
		    ["IALUSubToM", "4"],
		    ["IALUSubToMACC", "3"],
		    ["IALUSubToSHU", "4"],
		    ["IALUAndToBIU", "4"],
		    ["IALUAndToM", "4"],
		    ["IALUAndToMACC", "3"],
		    ["IALUAndToSHU", "4"],
		    ["IALUOrToBIU", "4"],
		    ["IALUOrToM", "4"],
		    ["IALUOrToMACC", "3"],
		    ["IALUOrToSHU", "4"],
		    ["IALUXorToBIU", "4"],
		    ["IALUXorToM", "4"],
		    ["IALUXorToMACC", "3"],
		    ["IALUXorToSHU", "4"],
		    ["IALUEquToBIU", "4"],
		    ["IALUEquToM", "4"],
		    ["IALUEquToMACC", "3"],
		    ["IALUEquToSHU", "4"],
		    ["IALUNeqToBIU", "4"],
		    ["IALUNeqToM", "4"],
		    ["IALUNeqToMACC", "3"],
		    ["IALUNeqToSHU", "4"],
		    ["IALULtToBIU", "4"],
		    ["IALULtToM", "4"],
		    ["IALULtToMACC", "3"],
		    ["IALULtToSHU", "4"],
		    ["IALUNstToBIU", "4"],
		    ["IALUNstToM", "4"],
		    ["IALUNstToMACC", "3"],
		    ["IALUNstToSHU", "4"],
		    ["IALUStToBIU", "4"],
		    ["IALUStToM", "4"],
		    ["IALUStToMACC", "3"],
		    ["IALUStToSHU", "4"],
		    ["IALUNltToBIU", "4"],
		    ["IALUNltToM", "4"],
		    ["IALUNltToMACC", "3"],
		    ["IALUNltToSHU", "4"],
		    ["IALULshToBIU", "4"],
		    ["IALULshToM", "4"],
		    ["IALULshToMACC", "3"],
		    ["IALULshToSHU", "4"],
		    ["IALURshToBIU", "4"],
		    ["IALURshToM", "4"],
		    ["IALURshToMACC", "3"],
		    ["IALURshToSHU", "4"],
		    ["IALUMaxToBIU", "4"],
		    ["IALUMaxToM", "4"],
		    ["IALUMaxToMACC", "3"],
		    ["IALUMaxToSHU", "4"],
		    ["IALUMinToBIU", "4"],
		    ["IALUMinToM", "4"],
		    ["IALUMinToMACC", "3"],
		    ["IALUMinToSHU", "4"],
		    ["IALUAbsToBIU", "4"],
		    ["IALUAbsToM", "4"],
		    ["IALUAbsToMACC", "3"],
		    ["IALUAbsToSHU", "4"],
		    ["IALURshiToBIU", "4"],
		    ["IALURshiToM", "4"],
		    ["IALURshiToMACC", "3"],
		    ["IALURshiToSHU", "4"],
		    ["IALULshiToBIU", "4"],
		    ["IALULshiToM", "4"],
		    ["IALULshiToMACC", "3"],
		    ["IALULshiToSHU", "4"],
		    ["IALUNotToBIU", "4"],
		    ["IALUNotToM", "4"],
		    ["IALUNotToMACC", "3"],
		    ["IALUNotToSHU", "4"],
		    ["IALURmaxToBIU", "4"],
		    ["IALURmaxToM", "4"],
		    ["IALURmaxToMACC", "3"],
		    ["IALURmaxToSHU", "4"],
		    ["IALURminToBIU", "4"],
		    ["IALURminToM", "4"],
		    ["IALURminToMACC", "3"],
		    ["IALURminToSHU", "4"],
		    ["IALUExpdToBIU", "4-5"],
		    ["IALUExpdToM", "4-5"],
		    ["IALUExpdToMACC", "3-4"],
		    ["IALUExpdToSHU", "4-5"],
		    ["IALUCprsToBIU", "4"],
		    ["IALUCprsToM", "4"],
		    ["IALUCprsToMACC", "3"],
		    ["IALUCprsToSHU", "4"],
		    ["IALUDivs", "1"],
		    ["IALUMdivr", "1"],
		    ["IALUMdivq", "1"],
		    ["IALUDivq", "1"],
		    ["IALUDivrToBIU", "3"],
		    ["IALUDivrToM", "3"],
		    ["IALUDivrToMACC", "2"],
		    ["IALUDivrToSHU", "3"],
		    ["IALULtToCond", "4"],
		    ["IALUNstToCond", "4"],
		    ["IALUStToCond", "4"],
		    ["IALUNltToCond", "4"],
		    ["IALUEquToCond", "4"],
		    ["IALUNeqToCond", "4"],
		    ["FALUAddToBIU", "6"],
		    ["FALUAddToM", "6"],
		    ["FALUAddToMACC", "5"],
		    ["FALUAddToSHU", "6"],
		    ["FALUSubToBIU", "6"],
		    ["FALUSubToM", "6"],
		    ["FALUSubToMACC", "5"],
		    ["FALUSubToSHU", "6"],
		    ["FALUAddSubToBIU", "6"],
		    ["FALUAddSubToM", "6"],
		    ["FALUAddSubToMACC", "5"],
		    ["FALUAddSubToSHU", "6"],
		    ["FALUEquToBIU", "6"],
		    ["FALUEquToM", "6"],
		    ["FALUEquToMACC", "5"],
		    ["FALUEquToSHU", "6"],
		    ["FALUNeqToBIU", "6"],
		    ["FALUNeqToM", "6"],
		    ["FALUNeqToMACC", "5"],
		    ["FALUNeqToSHU", "6"],
		    ["FALULtToBIU", "6"],
		    ["FALULtToM", "6"],
		    ["FALULtToMACC", "5"],
		    ["FALULtToSHU", "6"],
		    ["FALUNstToBIU", "6"],
		    ["FALUNstToM", "6"],
		    ["FALUNstToMACC", "5"],
		    ["FALUNstToSHU", "6"],
		    ["FALUStToBIU", "6"],
		    ["FALUStToM", "6"],
		    ["FALUStToMACC", "5"],
		    ["FALUStToSHU", "6"],
		    ["FALUNltToBIU", "6"],
		    ["FALUNltToM", "6"],
		    ["FALUNltToMACC", "5"],
		    ["FALUNltToSHU", "6"],
		    ["FALUAbsToBIU", "6"],
		    ["FALUAbsToM", "6"],
		    ["FALUAbsToMACC", "5"],
		    ["FALUAbsToSHU", "6"],
		    ["FALURecipToBIU", "6"],
		    ["FALURecipToM", "6"],
		    ["FALURecipToMACC", "5"],
		    ["FALURecipToSHU", "6"],
		    ["FALURsqrtToBIU", "6"],
		    ["FALURsqrtToM", "6"],
		    ["FALURsqrtToMACC", "5"],
		    ["FALURsqrtToSHU", "6"],
		    ["FALUToSingleToBIU", "6"],
		    ["FALUToSingleToM", "6"],
		    ["FALUToSingleToMACC", "5"],
		    ["FALUToSingleToSHU", "6"],
		    ["FALUToDoubleToBIU", "6"],
		    ["FALUToDoubleToM", "6"],
		    ["FALUToDoubleToMACC", "5"],
		    ["FALUToDoubleToSHU", "6"],
		    ["FALUToIntToBIU", "6"],
		    ["FALUToIntToM", "6"],
		    ["FALUToIntToMACC", "5"],
		    ["FALUToIntToSHU", "6"],
		    ["FALUToUintToBIU", "6"],
		    ["FALUToUintToM", "6"],
		    ["FALUToUintToMACC", "5"],
		    ["FALUToUintToSHU", "6"],
		    ["FALUMaxToBIU", "6"],
		    ["FALUMaxToM", "6"],
		    ["FALUMaxToMACC", "5"],
		    ["FALUMaxToSHU", "6"],
		    ["FALUMinToBIU", "6"],
		    ["FALUMinToM", "6"],
		    ["FALUMinToMACC", "5"],
		    ["FALUMinToSHU", "6"],
		    ["IMACMulTToBIU", "6"],
		    ["IMACMulTToM", "6"],
		    ["IMACMulTToMACC", "5"],
		    ["IMACMulTToSHU", "6"],
		    ["IMACMulToBIU", "6-7"],
		    ["IMACMulToM", "6-7"],
		    ["IMACMulToMACC", "5-6"],
		    ["IMACMulToSHU", "6-7"],
		    ["IMACMASToBIU", "8"],
		    ["IMACMASToM", "8"],
		    ["IMACMASToMACC", "7"],
		    ["IMACMASToSHU", "8"],
		    ["IMACMA", "6"],
		    ["IMACMaC", "6"],
		    ["IMACMaCToBIU", "9"],
		    ["IMACMaCToM", "9"],
		    ["IMACMaCToMACC", "8"],
		    ["IMACMaCToSHU", "9"],
		    ["IMACMaCLToBIU", "9-10"],
		    ["IMACMaCLToM", "9-10"],
		    ["IMACMaCLToMACC", "8-9"],
		    ["IMACMaCLToSHU", "9-10"],
		    ["IMACAcc", "1"],
		    ["IMACAccToBIU", "5"],
		    ["IMACAccToM", "5"],
		    ["IMACAccToMACC", "4"],
		    ["IMACAccToSHU", "5"],
		    ["FMACMulToBIU", "7"],
		    ["FMACMulToM", "7"],
		    ["FMACMulToMACC", "6"],
		    ["FMACMulToSHU", "7"],
		    ["FMACCmulToBIU", "11"],
		    ["FMACCmulToM", "11"],
		    ["FMACCmulToMACC", "10"],
		    ["FMACCmulToSHU", "11"],
		    ["FMACMA", "8"],
		    ["FMACMaC", "8"],
		    ["FMACMaCToBIU", "10"],
		    ["FMACMaCToM", "10"],
		    ["FMACMaCToMACC", "9"],
		    ["FMACMaCToSHU", "10"],
		    ["RepeatImm", "0"],
		    ["RepeatKI", "0"],
		    ["LpTo", "1"],
		    ["MPUSTOP", "0"]]


  



