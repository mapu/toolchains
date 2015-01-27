import unittest2

import gdbremote_testcase
from lldbtest import *

class TestGdbRemote_vCont(gdbremote_testcase.GdbRemoteTestCaseBase):

    def vCont_supports_mode(self, mode, inferior_args=None):
        # Setup the stub and set the gdb remote command stream.
        procs = self.prep_debug_monitor_and_inferior(inferior_args=inferior_args)
        self.add_vCont_query_packets()

        # Run the gdb remote command stream.
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Pull out supported modes.
        supported_vCont_modes = self.parse_vCont_query_response(context)
        self.assertIsNotNone(supported_vCont_modes)

        # Verify we support the given mode.
        self.assertTrue(mode in supported_vCont_modes)

    def vCont_supports_c(self):
        self.vCont_supports_mode("c")

    def vCont_supports_C(self):
        self.vCont_supports_mode("C")

    def vCont_supports_s(self):
        self.vCont_supports_mode("s")

    def vCont_supports_S(self):
        self.vCont_supports_mode("S")

    @debugserver_test
    @dsym_test
    def test_vCont_supports_c_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.vCont_supports_c()

    @llgs_test
    def test_vCont_supports_c_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.vCont_supports_c()

    @debugserver_test
    @dsym_test
    def test_vCont_supports_C_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.vCont_supports_C()

    @llgs_test
    @dwarf_test
    def test_vCont_supports_C_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.vCont_supports_C()

    @debugserver_test
    @dsym_test
    def test_vCont_supports_s_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.vCont_supports_s()

    @llgs_test
    @dwarf_test
    def test_vCont_supports_s_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.vCont_supports_s()

    @debugserver_test
    @dsym_test
    def test_vCont_supports_S_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.vCont_supports_S()

    @llgs_test
    @dwarf_test
    def test_vCont_supports_S_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.vCont_supports_S()

    @debugserver_test
    @dsym_test
    def test_single_step_only_steps_one_instruction_with_Hc_vCont_s_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.set_inferior_startup_launch()
        self.single_step_only_steps_one_instruction(use_Hc_packet=True, step_instruction="vCont;s")

    @llgs_test
    @dwarf_test
    def test_single_step_only_steps_one_instruction_with_Hc_vCont_s_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.set_inferior_startup_launch()
        self.single_step_only_steps_one_instruction(use_Hc_packet=True, step_instruction="vCont;s")

    @debugserver_test
    @dsym_test
    def test_single_step_only_steps_one_instruction_with_vCont_s_thread_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.set_inferior_startup_launch()
        self.single_step_only_steps_one_instruction(use_Hc_packet=False, step_instruction="vCont;s:{thread}")

    @llgs_test
    @dwarf_test
    def test_single_step_only_steps_one_instruction_with_vCont_s_thread_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.set_inferior_startup_launch()
        self.single_step_only_steps_one_instruction(use_Hc_packet=False, step_instruction="vCont;s:{thread}")


if __name__ == '__main__':
    unittest2.main()
