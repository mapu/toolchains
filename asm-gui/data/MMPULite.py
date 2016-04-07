import sys
sys.path.insert(0,"../..")

import ply.lex as lex
import ply.yacc as yacc
import os

class Parser:
    """
    Base class for a lexer/parser that has the rules defined as methods
    """
    tokens = ()
    precedence = ()

    def __init__(self, **kw):
        self.debug = kw.get('debug', 0)
        self.names = { }
        try:
            modname = os.path.split(os.path.splitext(__file__)[0])[1] + "_" + self.__class__.__name__
        except:
            modname = "parser"+"_"+self.__class__.__name__
        self.debugfile = modname + ".dbg"
        self.tabmodule = modname + "_" + "parsetab"
        #print self.debugfile, self.tabmodule
        # Build the lexer and parser
        lex.lex(module = self, debug = self.debug)
        yacc.yacc(module = self,
                  debug = self.debug,
                  debugfile = self.debugfile,
                  tabmodule = self.tabmodule)

    def run(self, s):
        self.result = 0
        yacc.parse(s)
    
class MMPULite(Parser):
    tokens = (
        'IMM3_1', 'IMM3_2', 'IMM3_3', 'IMM', 'IMM5_1', 'IMM5_2', 'ASSIGNTO', 'EQU', 'NEQ', 'ST', 'NLT', 'LT', 'NST', 'LPAREN', 'RPAREN', 'DOT', 'COMMA', 'ADD', 'SUB', 'MUL', 'CMUL', 'LSHT', 'RSHT',
        'OR', 'AND', 'XOR', 'NOT', 'NEG', 'ADDSUB', 'ACC1', 'ACC2', 'ALPHA', 'SHU0', 'SHU1', 'BIU0', 'BIU1', 'BIU2', 'M', 'COND',    
        'IALU', 'IMAC', 'FALU', 'FMAC', 'MINDEXI', 'MINDEXS', 'TB', 'TBB', 'TBH', 'TBW', 'TBD', 'TSQ', 'IND', 'BY',
        'CPRS', 'EXPD', 'START', 'MAX', 'MIN', 'ABS', 'MERGE', 'MDIVR', 'MDIVQ', 'DIVR', 'DIVQ', 'DIVS', 'RECIP', 'RSQRT', 'SINGLE', 'DOUBLE', 'MR', 'INT', 'RMAX', 'RMIN',
        'REPEAT', 'LOOP', 'JMP', 'MPUSTOP',
        'BR', 'CR', 'APP', 'KPP', 'U', 'T', 'B', 'H', 'S', 'D', 'I', 'L', 'TC', 'C',
        'TRUE', 'ASSIGN', 'UINT', 'DM', 'KG', 'R0', 'R1', 'R2', 'R3', 'IPATH', 'WFLAG_1', 'WFLAG_2',
        'IDENTIFIER',
        'EXPR',
        'TREG', 'MINDEXN', 'KI',
        )

    t_ASSIGNTO =  r'->'
    t_EQU = r'=='
    t_NEQ = r'!='
    t_ST = r'<'
    t_NLT = r'<='
    t_LT = r'>'
    t_NST = r'>='
    t_DOT = r'\.'
    t_COMMA = r','
    t_ADD = r'\+'
    t_SUB = r'-'
    t_MUL = r'\*'
    t_CMUL = r'\*\*'
    t_ASSIGN = r'<-'
    t_LSHT = r'<<' 
    t_RSHT = r'>>' 
    t_OR = r'\|'
    t_XOR = r'\^'
    t_AND = r'&'
    t_C = r'c'
    t_NOT = r'not'
    t_NEG = r'!'
    t_ADDSUB = r'\+-'
    t_ACC1 = r'\+='  
    t_ACC2 = r'=\+' 
    t_ALPHA = r'@'  
    
    def t_IMM5_1(self, t):
	r'0[xX][0-9a-f]+'
	if int(t.value, 16) <= 8:
	    t.type = "IMM3_1"
	    return t
	elif int(t.value, 16) < 32:
	    return t	
	else:
	    t.type = "IMM"
	    return t
 
    def t_IMM5_2(self, t):
	r'[1-9][0-9]*'
	if int(t.value) <= 8:
	    t.type = "IMM3_2"
	    return t
	elif int(t.value) < 32:
	    return t	
	else:
	    t.type = "IMM"
	    return t
  
    def t_IMM3_3(self, t):
	r'0+'
	return t
      
    def t_LPAREN(self, t):
	r'\('
	return t
      
    def t_RPAREN(self, t):
	r'\)'
	return t
    
    def t_SHU0(self, t):
        r'shu0'
        return t

    def t_SHU1(self, t):
        r'shu1'
        return t

    def t_IALU(self, t):
        r'ialu'
        return t

    def t_IMAC(self, t):
        r'imac'
        return t

    def t_FALU(self, t):
        r'falu'
        return t

    def t_FMAC(self, t):
        r'fmac'
        return t

    def t_BIU0(self, t):
        r'biu0'
        return t

    def t_BIU1(self, t):
        r'biu1'
        return t

    def t_BIU2(self, t):
        r'biu2'
        return t

    def t_R0(self, t):
        r'r0'
        return t

    def t_R1(self, t):
        r'r1'
        return t

    def t_R2(self, t):
        r'r2'
        return t

    def t_R3(self, t):
        r'r3'
        return t

    def t_IPATH(self, t):
        r'i[0-2]'
        return t

    def t_TREG(self, t):
        r't[0-3]'
        return t

    def t_MINDEXN(self, t):
        r'm\[[0-9]+\]'
        return t

    def t_MINDEXS(self, t):
        r'm\[S\+\+\]'
        return t

    def t_MINDEXI(self, t):
        r'm\[I\+\+\]'
        return t

    def t_COND(self, t):
        r'cond'
        return t

    def t_TBB(self, t):
        r'tbb' 
        return t
   
    def t_TBH(self, t):
        r'tbh'
        return t

    def t_TBW(self, t):
        r'tbw'
        return t

    def t_TBD(self, t):
        r'tbd'
        return t

    def t_TB(self, t):
        r'tb'
        return t
      
    def t_TSQ(self, t):
        r'tsq'
        return t

    def t_IND(self, t):
        r'ind'
        return t

    def t_BY(self, t):
        r'comb'
        return t

    def t_CPRS(self, t):
        r'cprs'
        return t

    def t_EXPD(self, t):
        r'expd'
        return t

    def t_START(self, t):
        r'start'
        return t

    def t_MAX(self, t):
        r'max'
        return t

    def t_MIN(self, t):
        r'min'
        return t

    def t_ABS(self, t):
        r'abs'
        return t

    def t_MERGE(self, t):
        r'merge'
        return t

    def t_DIVR(self, t):
	r'divr'
	return t
      
    def t_MDIVR(self, t):
        r'mdivr'
        return t

    def t_MDIVQ(self, t):
        r'mdivq'
        return t
    
    def t_DIVQ(self, t):
	r'divq'
	return t

    def t_DIVS(self, t):
        r'divs'
        return t

    def t_RMAX(self, t):
        r'rmax'
        return t

    def t_RMIN(self, t):
        r'rmin'
        return t

    def t_RECIP(self, t):
        r'recip'
        return t

    def t_RSQRT(self, t):
        r'rsqrt'
        return t

    def t_SINGLE(self, t):
        r'single'
        return t

    def t_DOUBLE(self, t):
        r'double'
        return t

    def t_MR(self, t):
        r'mr'
        return t

    def t_INT(self, t):
        r'int'
        return t

    def t_REPEAT(self, t):
        r'repeat'
        return t

    def t_LOOP(self, t):
        r'lpto'  
        return t
      
    def t_EXPR(self, t):
	r'label'
	return t
      
    def t_JMP(self, t):
        r'jump'  
        return t

    def t_MPUSTOP(self, t):
        r'mpustop'
        return t

    def t_BR(self, t):
        r'br'
        return t

    def t_CR(self, t):
        r'cr'
        return t

    def t_APP(self, t):
        r'a\+\+'
        return t

    def t_KPP(self, t):
        r'k\+\+'
        return t

    def t_TC(self, t):
        r'tc'
        return t

    def t_KI(self, t):
        r'ki[0-9]+'
        return t

    def t_TRUE(self, t):
        r'true'
        return t

    def t_UINT(self, t):
        r'uint'
        return t

    def t_KG(self, t):
        r'kg'
        return t

    def t_DM(self, t):
        r'dm'
        return t

    def t_IDENTIFIER(self, t):
        r'{L}{A}*'
        return t

    t_ignore = " \t"

    def t_WFLAG_1(self, t):
        r'wf[1-3]+'
        return t

    def t_WFLAG_2(self, t):
        r'wx[1-4]+' 
        return t

    def t_M(self, t):
        r'm'
        return t
    
    def t_U(self, t):
        r'u'
        return t

    def t_T(self, t):
        r't'
        return t

    def t_B(self, t):
        r'b'
        return t

    def t_H(self, t):
        r'h'
        return t

    def t_S(self, t):
        r's'
        return t

    def t_D(self, t):
        r'd'
        return t

    def t_I(self, t):
        r'i'
        return t

    def t_L(self, t):
        r'l'
        return t
     
    def t_newline(self, t):
        r'\n+'
        return t
        t.lexer.lineno += t.value.count("\n")
    
    def t_error(self, t):
        print("Illegal character '%s'" % t.value[0])
        t.lexer.skip(1)

    # Parsing rules	    
    def p_mcodeline(self, p):
        """
        mcodeline : slots
        """
        p[0] = p[1]
        self.result = p[0]

    def p_slots(self, p):
        """        
        slots : slotref
        """
        p[0] = p[1]

    def p_slotref(self, p):
        'slotref : slot'
        p[0] = p[1]
	    
    def p_slot(self, p):
        """
        slot : mr012slot condflag
	     | mr012slot
             | mr3slot condflag
             | mr3slot
             | shuslot condflag
             | shuslot
             | ialuslot condflag
             | ialuslot
             | imacslot condflag
             | imacslot
             | faluslot condflag
             | faluslot
             | fmacslot condflag
             | fmacslot
             | biuslot condflag
             | biuslot
             | seqslot
             | hmacro
             | error
        """
	p[0] = p[1]

    def p_hmacro(self, p):
        """
        hmacro : IDENTIFIER
               | IDENTIFIER LPAREN RPAREN
        """

    def p_mr012slot(self, p):
        """
        mr012slot : R0 DOT r0inst
                  | R1 DOT r1inst
                  | R2 DOT r2inst
                  | R2 DOT r2instw
                  | R0 DOT error
                  | R1 DOT error
                  | R2 DOT error
        """
	if p[1] == "r0":
	    tmp = "MR0"
	elif p[1] == "r1":
	    tmp = "MR1"
	else:
	    tmp = "MR2"
	p[0] = tmp + p[3]
	    
	
    def p_mr3slot(self, p):
        """
        mr3slot : mr3slot_
                | r3inst
                | R3 DOT r3inst
        """
        if p[1] == "r3":
	    p[0] = "MR3" + p[3]
	else:
	    p[0] = "MR3" + p[1]

    def p_mr3slot_(self, p):
        """
        mr3slot_ : r3inst
                 | R3 DOT r3inst
        """
        if p[1] == "r3":
	    p[0] = "MR3" + p[3]
	else:
	    p[0] = "MR3" + p[1]
	
    def p_shuslot(self, p):
        """
        shuslot : shu0code
                | shu1code
        """
        p[0] = p[1]

    def p_shu0code(self, p):
        """
        shu0code : SHU0 DOT shu0inst
                 | shu0ctrl
        """
        if p[1] == "shu0":
	    p[0] = "SHU" + p[3]
	else:
	    p[0] = p[1]

    def p_shu1code(self, p):
        """
        shu1code : SHU1 DOT shu1inst
                 | shu1ctrl
        """
        if p[1] == "shu1":
	    p[0] = "SHU" + p[3]
	else:
	    p[0] = p[1]

    def p_biuslot(self, p):
        """
        biuslot : biuheader biucode
                | ldinst
        """
        if len(p) > 2:
	    p[0] = p[1] + p[2]
	else:
	    p[0] = p[1]

    def p_biuheader(self, p):
        """
        biuheader : BIU0
                  | BIU1
                  | BIU2
        """
	p[0] = "BIU"
	
    def p_biucode(self, p):
        """
        biucode : DOT ldinst 
                | stinst 
                | kginst
        """
        if p[1] == ".":
	    p[0] = p[2]
	else:
	    p[0] = p[1]

    def p_ialuslot(self, p):
        """
        ialuslot : ialuinst
                 | divinst
                 | ialuctrl
        """
        p[0] = "IALU" + p[1]
   
    def p_imacslot(self, p):
        """
        imacslot : imacinst
                 | imacctrl
                 | mrinst 
        """     
        p[0] = "IMAC" + p[1]

    def p_faluslot(self, p):
        """
        faluslot : faluinst
                 | faluctrl 
        """
        p[0] = "FALU" +p[1]

    def p_fmacslot(self, p):
        """
        fmacslot : fmacinst
                 | fmacctrl
                 | fmrinst
        """
        p[0] = "FMAC" + p[1]

    def p_seqslot(self, p):
        """
        seqslot : reinst
                | lpinst 
                | jmpinst 
                | mpustop 
        """
        p[0] = p[1]

    def p_r0inst(self, p):
        """
        r0inst : r1inst
               | mindexn ASSIGNTO r0dest
               | mindexi ASSIGNTO r0dest
               | mindexs ASSIGNTO r0dest      
        """
        p[0] = "ToM"
        if p[1] != None:
	    p[0] = p[1]

    def p_r1inst(self, p):
        """
        r1inst : r2inst
               | mindexn ASSIGNTO r1destp
               | mindexi ASSIGNTO r1destp
               | mindexi ASSIGNTO r2destp
               | mindexs ASSIGNTO r1destp
               | mindexs ASSIGNTO r2destp
        """
        if p[1] != None:
            p[0] = p[1]
        else:
	    p[0] = "To" + p[3]

    def p_r2inst(self, p):
        'r2inst : mindexn ASSIGNTO r2destp'
        p[0] = "To" + p[3]

    def p_r2instw(self, p):
        """
        r2instw : mindexn _flag WFLAG_1 flag_  ASSIGNTO r2destp
                | mindexn _flag WFLAG_2 flag_  ASSIGNTO r2destp
        """
        p[0] = "WTo" + p[6]

    def p_r3inst(self, p):
        """
        r3inst : mindexn ASSIGNTO r3dest _flag KG flag_ 
               | mindexn ASSIGNTO r3dest
               | mindexn error
        """
        if len(p) > 4:
	    p[0] = "To%sKG"%(p[3])
	else:
	    p[0] = "To" + p[3]

    def p_shu0inst(self, p):
        'shu0inst : shuexp ASSIGNTO shu0dest'
        p[0] = p[1] + "To" + p[3]

    def p_shu1inst(self, p):
        'shu1inst : shuexp ASSIGNTO shu1dest'
        p[0] = p[1] + "To" + p[3]
        
    def p_shuexp(self, p):
        """
        shuexp : indexp
               | byexp
               | error
        """
	p[0] = p[1]

    def p_indexp(self, p):
        'indexp : shusrct IND indclause'
        p[0] = "Ind" + p[3]

    def p_indclause(self, p):
        """
        indclause : indtnclause 
                  | indtbclause
        """
	p[0] = p[1]
	
    def p_indtnclause(self, p):
        """
        indtnclause : shusrct _flag TB ACC2 IMM3_1 flag_ 
		    | shusrct _flag TB ACC2 IMM3_2 flag_ 
		    | shusrct _flag TB ACC2 IMM3_3 flag_ 
                    | shusrct
                    | shusrct _flag error flag_ 
                    | TB _flag error flag_
        """
	p[0] = p[1].upper()

    def p_indtbclause(self, p):
        """
        indtbclause : TB _flag TB ACC1 IMM3_1 flag_ 
		    | TB _flag TB ACC1 IMM3_2 flag_ 
		    | TB _flag TB ACC1 IMM3_3 flag_ 
                    | TB
        """
	p[0] = p[1].upper()
	
    def p_byclause(self, p):
        'byclause : BY _flag shudupara flag_'
        p[0] = "Comb"

    def p_shudupara(self, p):
        'shudupara : shusrct COMMA shusrct'

    def p_byexp(self, p):
        """
        byexp : byclause _flag byflag flag_ 
              | byclause
        """
        p[0] = p[1]

    def p_shu0ctrl(self, p):
        'shu0ctrl : SHU0 DOT ctrl'
        p[0] = "SHU" + p[3]

    def p_shu1ctrl(self, p):
        'shu1ctrl : SHU1 DOT ctrl'
        p[0] = "SHU" + p[3]

    def p_ldinst(self, p):
        'ldinst : ldop lddest'
        p[0] = p[1] + p[2]

    def p_ldop(self, p):
        """
        ldop : DM _flag ldflag flag_ ASSIGNTO
             | DM ASSIGNTO
        """
        p[0] = "LdTo"
   
    def p_stinst(self, p):
        'stinst : storeop' 
        p[0] = p[1]

    def p_storeop(self, p):
        """
        storeop : ASSIGNTO DM 
                | ASSIGNTO DM _flag stflag flag_
        """
        p[0] = "St"

    def p_kginst(self, p):
        'kginst : _flag KG flag_ ASSIGNTO mindex'
        p[0] = "KG"

    def p_ialuinst(self, p):
        """
        ialuinst : ialuclause ASSIGNTO ialudest
                 | condclause ASSIGNTO ialudest
                 | condclause ASSIGNTO COND
        """   
        if p[3] == "cond":
	    p[3] = "Cond"
        p[0] = p[1] + "To" + p[3]

    def p_ialuclause(self, p):
        """
        ialuclause : utbclause
                   | ubclause
                   | bclause 
                   | b_uclause
                   | inoflagclause
                   | uthclause
        """
        p[0] = p[1]
	  
    def p_utbclause(self, p):
        """
        utbclause : iaddclause
                  | isubclause
        """
        p[0] = p[1]

    def p_iaddclause(self, p):
        """
        iaddclause : addexp _flag utbflag flag_
                   | addexp
                   | addexp _flag error flag_
        """
        p[0] = p[1]

    def p_isubclause(self, p):
        """
        isubclause : subexp _flag utbflag flag_
                   | subexp
                   | subexp _flag error flag_
        """
        p[0] = p[1]

    def p_addexp(self, p):
        'addexp : t ADD t'
	p[0] = "Add"
	
    def p_subexp(self, p):
        'subexp : t SUB t'
        p[0] = "Sub"

    def p_ubclause(self, p):
        """
        ubclause : imaxclause
                 | iminclause
                 | iabsclause
                 | iexpdclause
                 | rshtclause
        """
        p[0] = p[1]

    def p_condclause(self, p):
        """
        condclause : iltclause
                   | instclause
                   | istclause
                   | inltclause
                   | iequclause
                   | ineqclause
        """
        p[0] = p[1]

    def p_imaxclause(self, p):
        """
        imaxclause : maxexp _flag ubflag flag_
                   | maxexp
                   | maxexp _flag error flag_
        """
        p[0] = p[1]

    def p_maxexp(self, p):
        'maxexp : MAX _flag dupara  flag_'
        p[0] = "Max"

    def p_iminclause(self, p):
        """
        iminclause : minexp _flag ubflag flag_
                   | minexp
                   | minexp _flag error flag_
        """
        p[0] = p[1]

    def p_minexp(self, p):
        'minexp : MIN _flag dupara  flag_'
        p[0] = "Min"

    def p_iabsclause(self, p):
        """
        iabsclause : abssexp _flag ubflag flag_
                   | abssexp
                   | abssexp _flag error flag_
        """
        p[0] = p[1]
 
    def p_abssexp(self, p):
        'abssexp : ABS _flag dupara  flag_'
	p[0] = "Abs"
	
    def p_iexpdclause(self, p):
        """
        iexpdclause : expdexp _flag ubflag flag_
                    | expdexp
                    | expdexp _flag error flag_
        """
        p[0] = p[1]
  
    def p_expdexp(self, p):
        'expdexp : EXPD t'
        p[0] = "Expd"

    def p_iltclause(self, p):
        """
        iltclause : ltexp _flag ubflag flag_
                  | ltexp
                  | ltexp _flag error flag_
        """
        p[0] = p[1]

    def p_ltexp(self, p):
        'ltexp : t LT t'
        p[0] = "Lt"

    def p_instclause(self, p):
        """
        instclause : nstexp _flag ubflag flag_
                   | nstexp
                   | nstexp _flag error flag_
        """
        p[0] = p[1]

    def p_nstexp(self, p):
        'nstexp : t NST t'
        p[0] = "Nst"

    def p_istclause(self, p):
        """
        istclause : stexp _flag ubflag flag_
                  | stexp
                  | stexp _flag error flag_
        """
	p[0] = p[1]
	
    def p_stexp(self, p):
        'stexp : t ST t'
        p[0] = "St"

    def p_inltclause(self, p):
        """
        inltclause : nltexp _flag ubflag flag_
                   | nltexp
                   | nltexp _flag error flag_
        """
        p[0] = p[1]

    def p_nltexp(self, p):
        'nltexp : t NLT t'
        p[0] = "Nlt"

    def p_bclause(self, p):
        """
        bclause : mrgclause
                | lshtclause       
        """
        p[0] = p[1]

    def p_mrgclause(self, p):
        """
        mrgclause : mrgexp _flag bflag flag_
                  | mrgexp
                  | mrgexp _flag error flag_
        """
        p[0] = p[1]

    def p_mrgexp(self, p):
        'mrgexp : MERGE  _flag tripara flag_'
        p[0] = "Merge"

    def p_iequclause(self, p):
        """
        iequclause : equexp _flag bflag flag_
                   | equexp
                   | equexp _flag error flag_
        """
        p[0] = p[1]

    def p_equexp(self, p):
        'equexp : t EQU t'
        p[0] = "Equ"

    def p_ineqclause(self, p):
        """
        ineqclause : neqexp _flag bflag flag_
                   | neqexp
                   | neqexp _flag error flag_
        """
        p[0] = p[1]

    def p_neqexp(self, p):
        'neqexp : t NEQ t'
        p[0] = "Neq"

    def p_rshtclause(self, p):
        """
        rshtclause : rshtexp _flag ubflag flag_
                   | rshtexp
                   | rshtexp _flag error flag_
        """
	p[0] = p[1]
	
    def p_rshtexp(self, p):
        """
        rshtexp : t rshtt
                | t rshti
        """
        p[0] = p[2]

    def p_rshtt(self, p):
        'rshtt : RSHT t'
        p[0] = "Rsh"

    def p_rshti(self, p):
        'rshti : RSHT imm5'
        p[0] = "Rshi"

    def p_inoflagclause(self, p):
        'inoflagclause : inoflagexp'
        p[0] = p[1]

    def p_inoflagexp(self, p):
        """
        inoflagexp : divrop
                   | uaryexp
                   | binaryexp
                   | inoflagexp _flag error flag_      
        """
        p[0] = p[1]

    def p_divrop(self, p):
        'divrop : DIVR'
        p[0] = "Divr"

    def p_uaryexp(self, p):
        'uaryexp : NOT t'
        p[0] = "Not"

    def p_binaryexp(self, p):
        'binaryexp : t binaryop t'
        p[0] = p[2]

    def p_binaryop(self, p):
        """
        binaryop : AND
                 | OR
                 | XOR
        """
        if p[1] == "&":
	    p[0] = "And"
	elif p[1] == "|":
	    p[0] = "Or"
	elif p[1] == "^":
	    p[0] = "Xor"

    def p_lshtclause(self, p):
        """
        lshtclause : lshtexp _flag bflag flag_
                   | lshtexp    
        """
        p[0] = p[1]

    def p_lshtexp(self, p):
        """
        lshtexp : t LSHT t
                | t LSHT imm5
        """
        p[0] = "Lsh"

    def p_uthclause(self, p):
        """
        uthclause : cprsexp _flag uthflag flag_
                  | cprsexp    
                  | cprsexp _flag error flag_
        """
        p[0] = p[1]


    def p_cprsexp(self, p):
        'cprsexp : CPRS _flag dupara flag_'
        p[0] = "Cprs"

    def p_b_uclause(self, p):
        'b_uclause : reduceexp _flag b__uflag flag_'
        p[0] = p[1]
   
    def p_reduceexp(self, p):
        """
        reduceexp : RMAX t
                  | RMIN t
        """
        p[0] = p[1].capitalize()

    def p_divinst(self, p):
        """
        divinst : divexp ASSIGNTO DIVR
                | divsexp ASSIGNTO DIVR
        """
        p[0] = p[1]

    def p_divexp(self, p):
        """
        divexp : divqop
               | mdivrop
               | mdivqop     
        """
	p[0] = p[1]
	
    def p_divsexp(self, p):
        """
        divsexp : t DIVS t _flag uflag flag_
                | t DIVS t 
        """
        p[0] = "Divs"

    def p_divqop(self, p):
        'divqop : DIVQ'
        p[0] = "Divq"

    def p_mdivrop(self, p):
        'mdivrop : MDIVR'
        p[0] = "Mdivr"

    def p_mdivqop(self, p):
        'mdivqop : MDIVQ'
        p[0] = "Mdivq"

    def p_ialuctrl(self, p):
        'ialuctrl : ialu DOT ctrl'
        p[0] = p[1]

    def p_imacinst(self, p):
        'imacinst : imacclause ASSIGNTO imacdest'  
        p[0] = p[1] + "To" + p[3]

    def p_imacclause(self, p):
        """
        imacclause : imulclause
                   | muladsbexp
                   | mrclause
                   | mrinst
        """
        p[0] = p[1]

    def p_mulexp(self, p):
        'mulexp : t MUL t'
        p[0] = "Mul"

    def p_imulclause(self, p):
        """
        imulclause : mulexp _flag t_uibflag flag_
                   | mulexp _flag b_uiflag flag_
                   | mulexp _flag uiflag flag_
                   | mulexp
                   | mulexp _flag error flag_
        """
        if len(p) > 2:
	    if p[3] == "T":
		p[0] = p[1] + p[3]
	    else:
		p[0] = p[1]
	else:
            p[0] = p[1]

    def p_muladsbexp(self, p):
        """
        muladsbexp : t ADDSUB mulexp _flag hflag flag_
                   | t ADDSUB mulexp
                   | t ADDSUB mulexp _flag error flag_   
        """
        p[0] = "MAS"

    def p_mrclause(self, p):
        """
        mrclause : MR _flag lubflag flag_
                 | MR
        """

    def p_mrinst(self, p):
        """
        mrinst : exmacexp iexmacclause
               | exmacexp
               | inmacexp iinmacclause
               | inmacexp
               | accexp tregclause
               | accexp
        """
        if len(p) > 2:
	    if p[2] == "L":
		p[0] = p[1] + p[2]
	    else:
		p[0] = p[1]
	else:
            p[0] = p[1]

    def p_exmacexp(self, p):
        'exmacexp : MR ASSIGN t ADD mulexp'
        p[0] = "MA"

    def p_inmacexp(self, p):
        'inmacexp : MR ACC1 mulexp'
        p[0] = "MaC"

    def p_accexp(self, p):
       'accexp : MR ACC1 t' 
       p[0] = "Acc"

    def p_iexmacclause(self, p):
        'iexmacclause : _flag ubflag flag_'

    def p_iinmacclause(self, p):
        """        
        iinmacclause : _flag crlubflag flag_
                     | _flag error flag_
        """
        if p[2] == "L":
	    p[0] = p[2]

    def p_tregclause(self, p):
        """
        tregclause : _flag crlubflag flag_
                   | _flag error flag_
        """

    def p_imacctrl(self, p):
        'imacctrl : imac DOT ctrl'   

    def p_faluinst(self, p):
        'faluinst : faluclause ASSIGNTO faludest' 
        p[0] = p[1] + "To" + p[3]

    def p_faluclause(self, p):
        """
        faluclause : tsdclause
                   | sdclause
                   | tclause
                   | tdclause
                   | sclause
                   | fuclause
        """
        p[0] = p[1]

    def p_tsdclause(self, p):
        """
        tsdclause : faddclause
                  | fsubclause
        """
        p[0] = p[1]

    def p_faddclause(self, p):
        'faddclause : addexp _flag tsdflag flag_'
        p[0] = p[1] 

    def p_fsubclause(self, p):
        'fsubclause : subexp _flag tsdflag flag_'
        p[0] = p[1] 

    def p_sdclause(self, p):
        """
        sdclause : fmaxclause
                 | fminclause
                 | recipclause
                 | rsqrtclause
                 | intclause
                 | fabsclause
                 | fequclause
                 | fneqclause
                 | fltclause
                 | fnstclause
                 | fstclause
                 | fnltclause
        """  
        p[0] = p[1]

    def p_fmaxclause(self, p):
        'fmaxclause : maxexp _flag sdflag flag_'
        p[0] = p[1]

    def p_fminclause(self, p):
        'fminclause : minexp _flag sdflag flag_'
        p[0] = p[1]

    def p_fabsclause(self, p):
        """
        fabsclause : absexp _flag sdflag flag_
                   | absexp _flag error flag_
        """
        p[0] = p[1]

    def p_absexp(self, p):
        'absexp : ABS t'
        p[0] = "Abs"

    def p_recipclause(self, p):
        """
        recipclause : recipexp _flag sdflag flag_
                    | recipexp _flag error flag_
        """
        p[0] = p[1]
  
    def p_recipexp(self, p):
        'recipexp : RECIP t'
        p[0] = "Recip"

    def p_rsqrtclause(self, p):
        """
        rsqrtclause : rsqrtexp _flag sdflag flag_
                    | rsqrtexp _flag error flag_
        """
        p[0] = p[1]

    def p_rsqrtexp(self, p):
        'rsqrtexp : RSQRT t'
        p[0] = "Rsqrt"

    def p_fequclause(self, p):
        'fequclause : equexp _flag sdflag flag_'
        p[0] = p[1]

    def p_fneqclause(self, p):
        'fneqclause : neqexp _flag sdflag flag_'
        p[0] = p[1]

    def p_fltclause(self, p):
        'fltclause : ltexp _flag sdflag flag_'
        p[0] = p[1]

    def p_fnstclause(self, p):
        'fnstclause : nstexp _flag sdflag flag_'
        p[0] = p[1]

    def p_fstclause(self, p):
        'fstclause : stexp _flag sdflag flag_'
        p[0] = p[1]

    def p_fnltclause(self, p):
        'fnltclause : nltexp _flag sdflag flag_'
        p[0] = p[1]

    def p_intclause(self, p):
        """
        intclause : intexp _flag tsdflag flag_
                  | intexp _flag error flag_
        """
        p[0] = p[1]
   
    def p_intexp(self, p):
        """
        intexp : intop t
               | uintop t
        """   
        p[0] = p[1]

    def p_intop(self, p):
        'intop : INT'
        p[0] = "ToInt"

    def p_uintop(self, p):
        'uintop : UINT' 
        p[0] = "ToUint"

    def p_tclause(self, p):
        """
        tclause : tosexp _flag utflag flag_
                | tosexp
                | tosexp _flag error flag_
        """
        p[0] = p[1]

    def p_tdclause(self, p):
        'tdclause : tosexp _flag tdflag flag_'
        p[0] = p[1]

    def p_tosexp(self, p):
        'tosexp : SINGLE t'
        p[0] = "ToSingle"
        
    def p_sclause(self, p):
        """
        sclause : todexp _flag sflag flag_
                | fasexp _flag sflag flag_
                | todexp _flag error flag_
                | fasexp _flag error flag_
        """
        p[0] = p[1]

    def p_fasexp(self, p):
        'fasexp : t ADDSUB t'
        p[0] = "AddSub"

    def p_todexp(self, p):
        'todexp : DOUBLE t'
        p[0] = "ToDouble"

    def p_fuclause(self, p):
        """
        fuclause : todexp _flag uflag flag_
                 | todexp
        """
        p[0] = p[1]

    def p_faluctrl(self, p):
        'faluctrl : falu DOT ctrl'
        p[0] = p[1]

    def p_fmacinst(self, p):
        'fmacinst : fmacclause ASSIGNTO fmacdest'  
        p[0] = p[1] + "To" + p[3]

    def p_fmacclause(self, p):
        """
        fmacclause : fmulclause
                   | fmrclause
                   | fmrinst
        """
        p[0] = p[1]

    def p_fmulclause(self, p):
        """
        fmulclause : mulexp _flag tsdflag flag_
                   | cmulexp _flag sflag flag_
        """
        p[0] = p[1]

    def p_cmulexp(self, p):
        'cmulexp : t CMUL t'
	p[0] = "Cmul"
	
    def p_fmrclause(self, p):
        'fmrclause : MR _flag sdflag flag_'
        p[0] = "Mac"

    def p_eapression_fmrinst(self, p):
        """
        fmrinst : exmacexp fexmacclause
                | inmacexp finmacclause
        """
        p[0] = p[1]

    def p_fexmacclause(self, p):
        'fexmacclause : _flag sdflag flag_'

    def p_finmacclause(self, p):
        'finmacclause : _flag crsdflag flag_'

    def p_fmacctrl(self, p):
        'fmacctrl : fmac DOT ctrl'
        p[0] = p[1]

    def p_reinst(self, p):
        """
        reinst : REPEAT ALPHA _flag repeatexp flag_
               | REPEAT ALPHA _flag error flag_
               | lpexp error
        """
        if len(p) > 4:
            p[0] = "Repeat" + p[4]

    def p_repeatexp(self, p):
        """
        repeatexp : immrep
		  | regrep
                  | regrep SUB imm
        """
        p[0] = p[1]

    def p_immrep(self, p):
        'immrep : imm'
        p[0] = "Imm"

    def p_regrep(self, p):
        'regrep : loops' 
        p[0] = "KI"

    def p_loops(self, p):
        """
        loops : loop
              | loop andflag loops
        """

    def p_loop(self, p):
        """
        loop : negflag kiflag
             | kiflag
        """

    def p_lpinst(self, p):
        'lpinst : lpexp lpcond'
        p[0] = "LpTo"

    def p_lpexp(self, p):
        """
        lpexp : LOOP label ALPHA
	      | LOOP imm ALPHA
              | LOOP error ALPHA
        """

    def p_lpcond(self, p):
        """
        lpcond : _flag kiflag flag_
               | _flag kiflag SUB imm flag_
               | _flag imm flag_
        """

    def p_jmpinst(self, p):
        'jmpinst : JMP label ALPHA condexp'
        p[0] = "LpTo"

    def p_condexp(self, p):
        """
        condexp : _flag trueflag flag_
                | regrep
        """

    def p_mpustop(self, p):
        'mpustop : MPUSTOP'
        p[0] = "MPUSTOP"

    def p_r2destp(self, p):
        'r2destp : r2dest LPAREN IPATH RPAREN'
        p[0] = p[1]

    def p_r2dest(self, p):
        """
        r2dest : ialut
               | imact
               | falut
               | fmact
        """      
	p[0] = p[1]
	
    def p_r1destp(self, p):
        'r1destp : r1dest'
        p[0] = p[1]
	
    def p_r1dest(self, p):
        """
        r1dest : shu0t
               | shu1t
        """
        p[0] = "SHU"
 
    def p_r0dest(self, p):   
        'r0dest : mindexn'
        p[0] = "M"
	
    def p_r3dest(self, p):
        """
        r3dest : biu0
               | biu1
               | biu2
        """
        p[0] = "BIU"

    def p_lddest(self, p):
        """
        lddest : r1destp
               | r2destp
               | mindex
        """
        p[0] = p[1]

    def p_shu0dest(self, p):
        """
        shu0dest : r0dest
                 | r2destp
                 | r3dest
                 | shu0t
        """
        p[0] = p[1]

    def p_shu1dest(self, p):
        """
        shu1dest : r0dest
                 | r2destp
                 | r3dest
                 | shu1t
        """
        p[0] = p[1]
        
    def p_mindex(self, p):
        """
        mindex : mindexi
               | mindexn
        """
        p[0] = "M"

    def p_ialudest(self, p):
        """
        ialudest : ialut
                 | imact
                 | falut
                 | r0dest
                 | r1destp
                 | r3dest
        """
        p[0] = p[1]

    def p_imacdest(self, p):
        'imacdest : ialudest'
        p[0] = p[1]

    def p_faludest(self, p):
        """
        faludest : ialudest
                 | fmact
        """
        p[0] = p[1]

    def p_fmacdest(self, p):
        """
        fmacdest : r0dest
                 | r1destp
                 | r3dest
                 | falut
                 | fmact
        """
        p[0] = p[1]

    def p_ialut(self, p):
        'ialut : ialu DOT t'
        p[0] = p[1]

    def p_imact(self, p):
        'imact : imac DOT t'
	p[0] = p[1]
	
    def p_falut(self, p):
        'falut : falu DOT t'
        p[0] = p[1]

    def p_fmact(self, p):
        'fmact : fmac DOT t'
        p[0] = p[1]

    def p_biu0(self, p):
        'biu0 : BIU0'

    def p_biu1(self, p):
        'biu1 : BIU1'    

    def p_biu2(self, p):
        'biu2 : BIU2'

    def p_shu0t(self, p):
        'shu0t : SHU0 DOT t'
        p[0] = "SHU"

    def p_shu1t(self, p):
        'shu1t : SHU1 DOT t'
        p[0] = "SHU"
        
    def p_mindexs(self, p):
        'mindexs : MINDEXS'

    def p_mindexi(self, p):
        'mindexi : MINDEXI'

    def p_mindexn(self, p):
        'mindexn : MINDEXN'

    def p_t(self, p):
        't : TREG'
        p[0] = "T"

    def p__constt(self, p):
        """
        _constt : TBB
                | TBH
                | TBW
                | TBD
                | TSQ
        """
        p[0] = p[1].upper()

    def p_constt(self, p):
        'constt : _constt'

    def p_shusrct(self, p):
        """
        shusrct : t
                | constt
        """
        p[0] = p[1]

    def p_tripara(self, p):
        'tripara : dupara COMMA t'

    def p_dupara(self, p):
        'dupara : t COMMA t'

    def p_imm(self, p):
        """
        imm : imm5
            | IMM
        """

    def p_imm5(self, p):
        """
        imm5 : IMM3_1
	     | IMM3_2
	     | IMM3_3
             | IMM5_1
             | IMM5_2
        """

    def p_stflag(self, p):
        'stflag : imbrakflag'  

    def p_ldflag(self, p):
        'ldflag : mbrakflag'

    def p_imbrakflag(self, p):
        """
        imbrakflag : iflag COMMA mbrakflag
                   | iflag
                   | mflag COMMA ibrakflag
                   | mflag
                   | brflag COMMA imakflag
                   | brflag
                   | aflag COMMA imbrkflag
                   | aflag
                   | kflag COMMA imbraflag
                   | kflag
        """

    def p_ibrakflag(self, p):
        """
        ibrakflag : iflag COMMA brakflag
                  | iflag
                  | brflag COMMA iakflag
                  | brflag
                  | aflag COMMA ibrkflag
                  | aflag
                  | kflag COMMA ibraflag
                  | kflag
        """

    def p_imakflag(self, p):
        """
        imakflag : iflag COMMA makflag
                 | iflag
                 | mflag COMMA iakflag
                 | mflag
                 | aflag COMMA imkflag
                 | aflag
                 | kflag COMMA imaflag
                 | kflag
        """

    def p_imbrkflag(self, p):
        """
        imbrkflag : iflag COMMA mbrkflag
                  | iflag
                  | mflag COMMA ibrkflag
                  | mflag
                  | brflag COMMA imkflag
                  | brflag
                  | kflag COMMA imbrflag
                  | kflag
        """

    def p_imbraflag(self, p):
        """
        imbraflag : iflag COMMA mbraflag
                  | iflag
                  | mflag COMMA ibraflag
                  | mflag
                  | brflag COMMA imaflag
                  | brflag
                  | aflag COMMA imbrflag
                  | aflag
        """

    def p_iakflag(self, p):
        """
        iakflag : iflag COMMA akflag
                | iflag
                | aflag COMMA ikflag
                | aflag
                | kflag COMMA iaflag
                | kflag
        """

    def p_ibrkflag(self, p):
        """
        ibrkflag : iflag COMMA brkflag
                 | iflag
                 | brflag COMMA ikflag
                 | brflag
                 | kflag COMMA ibrflag
                 | kflag
        """

    def p_ibraflag(self, p):
        """
        ibraflag : iflag COMMA braflag
                 | iflag
                 | brflag COMMA iaflag
                 | brflag
                 | aflag COMMA ibrflag
                 | aflag        
        """

    def p_imkflag(self, p):
        """
        imkflag : iflag COMMA mkflag
                | iflag 
                | mflag COMMA ikflag 
                | mflag 
                | kflag COMMA imflag 
                | kflag
        """

    def p_imaflag(self, p):
        """
        imaflag : iflag COMMA maflag
                | iflag 
                | mflag COMMA iaflag 
                | mflag 
                | aflag COMMA imflag 
                | aflag
        """

    def p_imbrflag(self, p):
        """
        imbrflag : iflag COMMA mbrflag 
                 | iflag
                 | mflag COMMA ibrflag 
                 | mflag
                 | brflag COMMA imflag
                 | brflag
        """

    def p_ikflag(self, p):
        """
        ikflag : iflag COMMA kflag
               | iflag 
               | kflag COMMA iflag 
               | kflag
        """

    def p_iaflag(self, p):
        """
        iaflag : iflag COMMA aflag
               | iflag 
               | aflag COMMA iflag 
               | aflag
        """

    def p_ibrflag(self, p):
        """
        ibrflag : iflag COMMA brflag
                | iflag 
                | brflag COMMA iflag 
                | brflag
        """

    def p_imflag(self, p):
        """
        imflag : iflag COMMA mflag 
               | iflag 
               | mflag COMMA iflag 
               | mflag      
        """

    def p_mbrakflag(self, p):
        """
        mbrakflag : mflag COMMA brakflag 
                  | mflag
                  | brflag COMMA makflag
                  | brflag
                  | aflag COMMA mbrkflag
                  | aflag
                  | kflag COMMA mbraflag
                  | kflag
        """

    def p_brakflag(self, p):
        """
        brakflag : brflag COMMA akflag
                 | brflag
                 | aflag COMMA brkflag
                 | aflag
                 | kflag COMMA braflag
                 | kflag
        """

    def p_makflag(self, p):
        """
        makflag : mflag COMMA akflag
                | mflag
                | aflag COMMA mkflag
                | aflag
                | kflag COMMA maflag
                | kflag
        """

    def p_mbrkflag(self, p):
        """
        mbrkflag : mflag COMMA brkflag
                 | mflag
                 | brflag COMMA mkflag 
                 | brflag
                 | kflag COMMA mbrflag
                 | kflag
        """

    def p_mbraflag(self, p):
        """
        mbraflag : mflag COMMA braflag
                 | mflag
                 | brflag COMMA maflag
                 | brflag
                 | aflag COMMA mbrflag
                 | aflag
        """

    def p_akflag(self, p):
        """
        akflag : aflag COMMA kflag
               | aflag 
               | kflag 
               | kflag COMMA aflag
        """

    def p_mkflag(self, p):
        """
        mkflag : mflag COMMA kflag 
               | mflag 
               | kflag 
               | kflag COMMA mflag
        """

    def p_brkflag(self, p):
        """
        brkflag : brflag COMMA kflag 
                | brflag 
                | kflag 
                | kflag COMMA brflag
        """

    def p_maflag(self, p):
        """
        maflag : mflag COMMA aflag 
               | mflag 
               | aflag 
               | aflag COMMA mflag
        """

    def p_mbrflag(self, p):
        """
        mbrflag : mflag COMMA brflag
                | mflag
                | brflag
                | brflag COMMA mflag
        """

    def p_braflag(self, p):
        """
        braflag : brflag COMMA aflag 
                | brflag 
                | aflag 
                | aflag COMMA brflag
        """

    def p_uthflag(self, p):
        """
        uthflag : uflag COMMA thflag
                | uflag 
                | tflag COMMA uhflag 
                | tflag 
                | hflag COMMA utflag 
                | hflag
        """

    def p_thflag(self, p):
        """
        thflag : tflag COMMA hflag 
               | tflag 
               | hflag 
               | hflag COMMA tflag
        """

    def p_uhflag(self, p):
        """
        uhflag : uflag COMMA hflag
               | uflag 
               | hflag 
               | hflag COMMA uflag
        """

    def p_utbflag(self, p):
        """
        utbflag : t_ubflag 
                | ubflag
        """

    def p_t_bflag(self, p):
        """
        t_bflag : tflag COMMA bflag 
                | tflag 
                | bflag COMMA tflag
        """

    def p_ubflag(self, p):
        """
        ubflag : b_uflag 
               | uflag
        """    

    def p_b__uflag(self, p):
        """
        b__uflag : b_flag COMMA uflag
                 | b_flag
                 | uflag COMMA b_flag
        """ 

    def p_b_uflag(self, p):
        """
        b_uflag : bflag COMMA uflag 
                | bflag 
                | uflag COMMA bflag
        """

    def p_utflag(self, p):
        """
        utflag : t_uflag 
               | uflag
        """

    def p_t_uflag(self, p):
        """
        t_uflag : uflag COMMA tflag
                | tflag 
                | tflag COMMA uflag
        """

    def p_t_uibflag(self, p):
        """
        t_uibflag : tflag COMMA uibflag 
                  | tflag 
                  | uflag COMMA t_ibflag
                  | iflag COMMA t_ubflag
                  | bflag COMMA t_uiflag  
        """
        p[0] = "T"

    def p_uibflag(self, p):
        """
        uibflag : b_uiflag 
                | uiflag
        """

    def p_b_uiflag(self, p):
        """
        b_uiflag : bflag COMMA uiflag 
                 | bflag 
                 | uflag COMMA b_iflag 
                 | iflag COMMA b_uflag
        """

    def p_t_ubflag(self, p):
        """
        t_ubflag : tflag COMMA ubflag   
                 | tflag
                 | uflag COMMA t_bflag
                 | bflag COMMA t_uflag
        """

    def p_t_ibflag(self, p):
        """
        t_ibflag : tflag COMMA ibflag
                 | tflag
                 | iflag COMMA t_bflag
                 | bflag COMMA t_iflag
        """

    def p_t_uiflag(self, p):
        """
        t_uiflag : tflag COMMA uiflag
                 | tflag
                 | uflag COMMA t_iflag
                 | iflag COMMA t_uflag
        """

    def p_ibflag(self, p):
        """
        ibflag : b_iflag
               | iflag
        """

    def p_b_iflag(self, p):
        """
        b_iflag : bflag COMMA iflag
                | bflag 
                | iflag COMMA bflag
        """

    def p_t_iflag(self, p):
        """
        t_iflag : tflag COMMA iflag
                | tflag 
                | iflag COMMA tflag
        """

    def p_uiflag(self, p):
        """
        uiflag : iflag COMMA uflag
               | iflag 
               | uflag 
               | uflag COMMA iflag
        """

    def p_crlubflag(self, p):
        """
        crlubflag : crflag COMMA lubflag
                  | crflag 
                  | lflag COMMA crubflag
                  | lflag
                  | uflag COMMA crlbflag
                  | uflag
                  | bflag COMMA crluflag
                  | bflag
        """
        if p[1] == "L":
	    p[0] = p[1]
	    return
	if len(p) > 2:
	    if p[3] == "L":
	      	p[0] = p[3]

    def p_crlbflag(self, p):
        """
        crlbflag : crflag COMMA lbflag
                 | crflag
                 | lflag COMMA crbflag
                 | lflag
                 | bflag COMMA crlflag
                 | bflag
        """
        if p[1] == "L":
	    p[0] = p[1]
	    return
	if len(p) > 2:
	    if p[3] == "L":
	      	p[0] = p[3]        

    def p_crluflag(self, p):
        """     
        crluflag : crflag COMMA luflag
                 | crflag
                 | lflag COMMA cruflag
                 | lflag
                 | uflag COMMA crlflag
                 | uflag
        """
        if p[1] == "L":
	    p[0] = p[1]
	    return
	if len(p) > 2:
	    if p[3] == "L":
	      	p[0] = p[3]

    def p_crlflag(self, p):
        """
        crlflag : crflag COMMA lflag
                | crflag 
                | lflag 
                | lflag COMMA crflag
        """

    def p_crubflag(self, p):
        """
        crubflag : crflag COMMA ubflag
                 | crflag
                 | uflag COMMA crbflag
                 | uflag
                 | bflag COMMA cruflag
                 | bflag
        """

    def p_crbflag(self, p):
        """
        crbflag : crflag COMMA bflag
                | crflag 
                | bflag 
                | bflag COMMA crflag
        """

    def p_cruflag(self, p):
        """
        cruflag : crflag COMMA uflag 
                | crflag 
                | uflag 
                | uflag COMMA crflag
        """

    def p_lbflag(self, p):
        """
        lbflag : lflag COMMA bflag 
               | lflag 
               | bflag 
               | bflag COMMA lflag
        """

    def p_luflag(self, p):
        """
        luflag : lflag COMMA uflag 
               | lflag
               | uflag 
               | uflag COMMA lflag
        """
        if p[1] == "L":
	    p[0] = p[1]
	    return
	if len(p) > 2:
	    if p[3] == "L":
		p[0] = p[3]

    def p_lubflag(self, p):
        """
        lubflag : lflag COMMA ubflag 
                | lflag 
                | uflag COMMA lbflag 
                | uflag 
                | bflag COMMA luflag 
                | bflag
        """

    def p_tsdflag(self, p):
        """
        tsdflag : tflag COMMA sdflag
                | sdflag 
                | sdflag COMMA tflag
        """

    def p_sdflag(self, p):
        """
        sdflag : sflag 
               | dflag
        """

    def p_tdflag(self, p):
        """
        tdflag : tflag COMMA dflag 
               | dflag
               | dflag COMMA tflag
        """

    def p_bflag(self, p):
        """
        bflag : b_flag
              | hflag
        """

    def p_crsdflag(self, p):
        """
        crsdflag : crflag COMMA sdflag
                 | sdflag
                 | sdflag COMMA crflag
        """ 

    def p_byflag(self, p):
        """
        byflag : tcflag COMMA bflag
               | bflag COMMA tcflag
               | bflag 
               | tcflag
               | tcflag COMMA lflag
               | lflag COMMA tcflag 
               | lflag
               | error
        """

    def p_b_flag(self, p):
        'b_flag : B'

    def p_hflag(self, p):    
        'hflag : H'

    def p_brflag(self, p):
        'brflag : BR'

    def p_mflag(self, p):
        'mflag : M'

    def p_aflag(self, p):
        'aflag : APP'

    def p_kflag(self, p):
        'kflag : KPP'

    def p_tflag(self, p):
        'tflag : T'

    def p_uflag(self, p):
        'uflag : U'

    def p_sflag(self, p):
        'sflag : S'

    def p_dflag(self, p):
        'dflag : D'

    def p_lflag(self, p):
        'lflag : L'
        p[0] = "L"

    def p_crflag(self, p):
        'crflag : CR'

    def p_iflag(self, p):
        'iflag : I'

    def p_tcflag(self, p):
        'tcflag : TC'

    def p_negflag(self, p):
        'negflag : NEG'

    def p_andflag(self, p):
        'andflag : AND'
        p[0] = "And"

    def p_kiflag(self, p):
        'kiflag : KI'

    def p_trueflag(self, p):
        'trueflag : TRUE'

    def p_cflag(self, p):
        'cflag : C'

    def p_ncflag(self, p):
        'ncflag : NEG C'

    def p_condflag(self, p):
        """
        condflag : ALPHA _flag cflag flag_
                 | ALPHA _flag ncflag flag_
        """

    def p__flag(self, p):
        '_flag : LPAREN'

    def p_flag_(self, p):
        'flag_ : RPAREN'

    def p_ialu(self, p):
        'ialu : IALU'
        p[0] = "MACC"

    def p_imac(self, p):
        'imac : IMAC'
        p[0] = "MACC"

    def p_falu(self, p):
        'falu : FALU'
        p[0] = "MACC"

    def p_fmac(self, p):
        'fmac : FMAC'
        p[0] = "MACC"

    def p_ctrl(self, p):
        'ctrl : START'
        p[0] = "START"

    def p_label(self, p):
        'label : EXPR'
     
    def p_error(self, p):
        if p:
            print("Syntax error at '%s'" % p.value)
        else:
            print("Syntax error at EOF")  
        
