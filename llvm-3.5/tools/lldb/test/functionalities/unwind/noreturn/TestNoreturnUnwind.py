"""
Test that we can backtrace correctly with 'noreturn' functions on the stack
"""

import os, time
import unittest2
import lldb
from lldbtest import *
import lldbutil

class NoreturnUnwind(TestBase):
    mydir = TestBase.compute_mydir(__file__)

    @unittest2.skipUnless(sys.platform.startswith("darwin"), "requires Darwin")
    @dsym_test
    def test_with_dsym (self):
        """Test that we can backtrace correctly with 'noreturn' functions on the stack"""
        self.buildDsym()
        self.setTearDownCleanup()
        self.noreturn_unwind_tests()

    @dwarf_test
    def test_with_dwarf (self):
        """Test that we can backtrace correctly with 'noreturn' functions on the stack"""
        self.buildDwarf()
        self.setTearDownCleanup()
        self.noreturn_unwind_tests()

    def noreturn_unwind_tests (self):
        exe = os.path.join(os.getcwd(), "a.out")
        target = self.dbg.CreateTarget(exe)
        self.assertTrue(target, VALID_TARGET)

        process = target.LaunchSimple (None, None, self.get_process_working_directory())

        if not process:
            self.fail("SBTarget.Launch() failed")

        if process.GetState() != lldb.eStateStopped:
            self.fail("Process should be in the 'stopped' state, "
                      "instead the actual state is: '%s'" %
                      lldbutil.state_type_to_str(process.GetState()))

        thread = process.GetThreadAtIndex(0)
        abort_frame_number = 0
        for f in thread.frames:
            if f.GetFunctionName() == "abort":
                break
            abort_frame_number = abort_frame_number + 1

        if self.TraceOn():
            print "Backtrace once we're stopped:"
            for f in thread.frames:
                print "  %d %s" % (f.GetFrameID(), f.GetFunctionName())

        # I'm going to assume that abort() ends up calling/invoking another
        # function before halting the process.  In which case if abort_frame_number
        # equals 0, we didn't find abort() in the backtrace.
        if abort_frame_number == 0:
            self.fail("Unable to find abort() in backtrace.")

        func_c_frame_number = abort_frame_number + 1
        if thread.GetFrameAtIndex (func_c_frame_number).GetFunctionName() != "func_c":
            self.fail("Did not find func_c() above abort().")

        # This depends on whether we see the func_b inlined function in the backtrace
        # or not.  I'm not interested in testing that aspect of the backtrace here
        # right now.

        if thread.GetFrameAtIndex (func_c_frame_number + 1).GetFunctionName() == "func_b":
            func_a_frame_number = func_c_frame_number + 2
        else:
            func_a_frame_number = func_c_frame_number + 1

        if thread.GetFrameAtIndex (func_a_frame_number).GetFunctionName() != "func_a":
            self.fail("Did not find func_a() above func_c().")

        main_frame_number = func_a_frame_number + 1

        if thread.GetFrameAtIndex (main_frame_number).GetFunctionName() != "main":
            self.fail("Did not find main() above func_a().")


if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
