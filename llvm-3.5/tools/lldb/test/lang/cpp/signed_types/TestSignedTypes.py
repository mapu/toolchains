"""
Test that variables with signed types display correctly.
"""

import os, time
import re
import unittest2
import lldb
from lldbtest import *
import lldbutil

class UnsignedTypesTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    @unittest2.skipUnless(sys.platform.startswith("darwin"), "requires Darwin")
    @dsym_test
    def test_with_dsym(self):
        """Test that variables with signed types display correctly."""
        self.buildDsym()
        self.signed_types()

    @dwarf_test
    def test_with_dwarf(self):
        """Test that variables with signed types display correctly."""
        self.buildDwarf()
        self.signed_types()

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)
        # Find the line number to break inside main().
        self.source = 'main.cpp'
        self.line = line_number(self.source, '// Set break point at this line.')

    def signed_types(self):
        # Run in synchronous mode
        self.dbg.SetAsync(False)

        # Create a target by the debugger.
        target = self.dbg.CreateTarget("a.out")
        self.assertTrue(target, VALID_TARGET)

        lldbutil.run_break_set_by_file_and_line (self, self.source, self.line, num_expected_locations=1, loc_exact=True)

        # Now launch the process, and do not stop at entry point.
        process = target.LaunchSimple (None, None, self.get_process_working_directory())
        self.assertTrue(process, PROCESS_IS_VALID)

        # The stop reason of the thread should be breakpoint.
        self.expect("thread list", STOPPED_DUE_TO_BREAKPOINT,
            substrs = ['stopped', 'stop reason = breakpoint'])

        # The breakpoint should have a hit count of 1.
        self.expect("breakpoint list -f", BREAKPOINT_HIT_ONCE,
            substrs = [' resolved, hit count = 1'])

        # Execute the assignment statement.
        self.runCmd("thread step-over")

        # Test that signed types display correctly.
        self.expect("frame variable --show-types --no-args", VARIABLES_DISPLAYED_CORRECTLY,
            patterns = ["\((short int|short)\) the_signed_short = 99",
                       "\((signed char|char)\) the_signed_char = 'c'"],
            substrs = ["(int) the_signed_int = 99",
                       "(long) the_signed_long = 99",
                       "(long long) the_signed_long_long = 99"])


if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
