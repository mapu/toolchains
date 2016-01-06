'''
Created on Dec 23", " 2015

@author: wangl
'''

def initRegName():
    '''
    Define the global const string table for regster names, the prefix _NA_ 
    stands for an unused ocuppation register
    '''
    global MPURegNames
    global SPURegNames
    MPURegNames = ["M%d" % i for i in range(128)]
    MPURegNames.extend(["SHU%d_T%d" % (i, j) for i in range(2) for j in range(16)])
    MPURegNames.extend(["%s_T%d" % (name, i) for name in ["IALU", "IMAC", "FALU", "FMAC"]
                        for i in range(4)])
    MPURegNames.extend(["IMRL", "IMRH", "FMR", "BIU0W", "BIU1W", "BIU2W",
                        "DIVQU", "DIVER", "DIVCNT", "SVR0", "SVR1"])
    SPURegNames = ["R%d" % i for i in range(32)]
    SPURegNames.extend(["J%d" % i for i in range(32)])
    SPURegNames.extend(["Carry_Flag", "Loop_Enable",
                        "Loop0_Start", "Loop0_End", "Loop0_Number",
                        "Loop1_Start", "Loop1_End", "Loop1_Number"])
    SPURegNames.extend(["_NA_0_%d" % i for i in range(8)])
    SPURegNames.extend(["%s_%s" % (c, n) for c in ["MC", "MI", "MS"] 
                        for n in ["r0", "r1", "w0", "w1", "w2"]])
    SPURegNames.extend(["%s%d" % (c, i) for c in ["KB", "KE", "KS", "KI"] 
                        for i in range(16)])
    SPURegNames.extend(["KM%d_%s" % (i, n) for i in range(3)
                        for n in ["L", "L_NA", "H", "H_NA"]])
    SPURegNames.extend(["_NA_1_%d" % i for i in range(4)])
    kg = ["_NA_2_%d" % i for i in range(16)]
    kg[0], kg[4], kg[8] = ["KG%d" % i for i in range(3)]
    SPURegNames.extend(kg)
    SPURegNames.extend(["KL%d" % i for i in range(16)])
    SPURegNames.extend(["KC%d" % i for i in range(16)])
    SPURegNames.extend(["MPUCondition"])
    SPURegNames.extend(["MPU_Loop%d_%s" % (i, n) for n in ["Start", "End", "Number"]
                        for i in range(4)])
    