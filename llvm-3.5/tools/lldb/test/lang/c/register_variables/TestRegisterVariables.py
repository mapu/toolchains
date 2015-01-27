"""Check that compiler-generated register values work correctly"""

import os, time
import unittest2
import lldb
from lldbtest import *
import lldbutil

class RegisterVariableTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    @dsym_test
    @expectedFailureDarwin("llvm.org/pr20266")
    def test_with_dsym_and_run_command(self):
        """Test expressions on register values."""
        self.buildDsym()
        self.const_variable()

    @expectedFailureClang
    @dwarf_test
    def test_with_dwarf_and_run_command(self):
        """Test expressions on register values."""
        self.buildDwarf()
        self.const_variable()

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)

    def const_variable(self):
        """Test expressions on register values."""
        exe = os.path.join(os.getcwd(), "a.out")
        self.runCmd("file " + exe, CURRENT_EXECUTABLE_SET)

        # Break inside the main.
        lldbutil.run_break_set_by_source_regexp(self, "break", num_expected_locations=2)

        ####################
        # First breakpoint

        self.runCmd("run", RUN_SUCCEEDED)

        # The stop reason of the thread should be breakpoint.
        self.expect("thread list", STOPPED_DUE_TO_BREAKPOINT,
            substrs = ['stopped',
                       'stop reason = breakpoint'])

        # The breakpoint should have a hit count of 1.
        self.expect("breakpoint list -f", BREAKPOINT_HIT_ONCE,
            substrs = [' resolved, hit count = 1'])

        # Try some variables that should be visible
        self.expect("expr a", VARIABLES_DISPLAYED_CORRECTLY,
            substrs = ['(int) $0 = 2'])

        self.expect("expr b->m1", VARIABLES_DISPLAYED_CORRECTLY,
            substrs = ['(int) $1 = 3'])

        #####################
        # Second breakpoint

        self.runCmd("continue")

        # The stop reason of the thread should be breakpoint.
        self.expect("thread list", STOPPED_DUE_TO_BREAKPOINT,
            substrs = ['stopped',
                       'stop reason = breakpoint'])

        # The breakpoint should have a hit count of 1.
        self.expect("breakpoint list -f", BREAKPOINT_HIT_ONCE,
            substrs = [' resolved, hit count = 1'])

        # Try some variables that should be visible
        self.expect("expr b->m2", VARIABLES_DISPLAYED_CORRECTLY,
            substrs = ['(int) $2 = 5'])

        self.expect("expr c", VARIABLES_DISPLAYED_CORRECTLY,
            substrs = ['(int) $3 = 5'])

        self.runCmd("kill")

if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
