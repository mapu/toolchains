"""
Test C++ virtual function and virtual inheritance.
"""

import os, time
import re
import lldb
from lldbtest import *
import lldbutil

def Msg(expr, val):
    return "'expression %s' matches the output (from compiled code): %s" % (expr, val)

class CppVirtualMadness(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    # This is the pattern by design to match the "my_expr = 'value'" output from
    # printf() stmts (see main.cpp).
    pattern = re.compile("^([^=]*) = '([^=]*)'$")

    # Assert message.
    PRINTF_OUTPUT_GROKKED = "The printf output from compiled code is parsed correctly"

    @unittest2.skipIf(sys.platform.startswith("win32"), "Process::GetSTDOUT unsupported on Windows.  This test should be re-written to use stdout re-direction")
    @unittest2.skipUnless(sys.platform.startswith("darwin"), "requires Darwin")
    def test_virtual_madness_dsym(self):
        """Test that expression works correctly with virtual inheritance as well as virtual function."""
        self.buildDsym()
        self.virtual_madness_test()

    @expectedFailureIcc('llvm.org/pr16808') # lldb does not call the correct virtual function with icc
    def test_virtual_madness_dwarf(self):
        """Test that expression works correctly with virtual inheritance as well as virtual function."""
        self.buildDwarf()
        self.virtual_madness_test()

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)
        # Find the line number to break for main.cpp.
        self.source = 'main.cpp'
        self.line = line_number(self.source, '// Set first breakpoint here.')

    def virtual_madness_test(self):
        """Test that variable expressions with basic types are evaluated correctly."""

        # Bring the program to the point where we can issue a series of
        # 'expression' command to compare against the golden output.
        self.dbg.SetAsync(False)
        
        # Create a target by the debugger.
        target = self.dbg.CreateTarget("a.out")
        self.assertTrue(target, VALID_TARGET)

        # Create the breakpoint inside function 'main'.
        breakpoint = target.BreakpointCreateByLocation(self.source, self.line)
        self.assertTrue(breakpoint, VALID_BREAKPOINT)

        # Now launch the process, and do not stop at entry point.
        process = target.LaunchSimple (None, None, self.get_process_working_directory())
        self.assertTrue(process, PROCESS_IS_VALID)
        
        self.assertTrue(process.GetState() == lldb.eStateStopped)
        thread = lldbutil.get_stopped_thread(process, lldb.eStopReasonBreakpoint)
        self.assertTrue(thread.IsValid(), "There should be a thread stopped due to breakpoint condition")

        # First, capture the golden output from the program itself from the
        # series of printf statements.
        stdout = process.GetSTDOUT(1024)
        
        # This golden list contains a list of "my_expr = 'value' pairs extracted
        # from the golden output.
        gl = []

        # Scan the golden output line by line, looking for the pattern:
        #
        #     my_expr = 'value'
        #
        for line in stdout.split(os.linesep):
            match = self.pattern.search(line)
            if match:
                my_expr, val = match.group(1), match.group(2)
                gl.append((my_expr, val))
        #print "golden list:", gl

        # Now iterate through the golden list, comparing against the output from
        # 'expression var'.
        for my_expr, val in gl:

            self.runCmd("expression %s" % my_expr)
            output = self.res.GetOutput()
            
            # The expression output must match the oracle.
            self.expect(output, Msg(my_expr, val), exe=False,
                substrs = [val])
