import unittest2

import gdbremote_testcase
from lldbtest import *

class TestGdbRemoteThreadsInStopReply(gdbremote_testcase.GdbRemoteTestCaseBase):

    ENABLE_THREADS_IN_STOP_REPLY_ENTRIES = [
        "read packet: $QListThreadsInStopReply#21",
        "send packet: $OK#00",
    ]

    def gather_stop_reply_threads(self, post_startup_log_lines, thread_count):
        # Set up the inferior args.
        inferior_args=[]
        for i in range(thread_count - 1):
            inferior_args.append("thread:new")
        inferior_args.append("sleep:10")
        procs = self.prep_debug_monitor_and_inferior(inferior_args=inferior_args)

        # Assumes test_sequence has anything added needed to setup the initial state.
        # (Like optionally enabling QThreadsInStopReply.)
        if post_startup_log_lines:
            self.test_sequence.add_log_lines(post_startup_log_lines, True)
        self.test_sequence.add_log_lines([
            "read packet: $c#63"
            ], True)
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Give threads time to start up, then break.
        time.sleep(1)
        self.reset_test_sequence()
        self.test_sequence.add_log_lines([
            "read packet: {}".format(chr(03)),
            {"direction":"send", "regex":r"^\$T([0-9a-fA-F]+)([^#]+)#[0-9a-fA-F]{2}$", "capture":{1:"stop_result", 2:"key_vals_text"} },
            ], True)
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Wait until all threads have started.
        threads = self.wait_for_thread_count(thread_count, timeout_seconds=3)
        self.assertIsNotNone(threads)
        self.assertEquals(len(threads), thread_count)

        # Run, then stop the process, grab the stop reply content.
        self.reset_test_sequence()
        self.test_sequence.add_log_lines([
            "read packet: $c#63",
            "read packet: {}".format(chr(03)),
            {"direction":"send", "regex":r"^\$T([0-9a-fA-F]+)([^#]+)#[0-9a-fA-F]{2}$", "capture":{1:"stop_result", 2:"key_vals_text"} },
            ], True)
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Parse the stop reply contents.
        key_vals_text = context.get("key_vals_text")
        self.assertIsNotNone(key_vals_text)
        kv_dict = self.parse_key_val_dict(key_vals_text)
        self.assertIsNotNone(kv_dict)

        # Pull out threads from stop response.
        stop_reply_threads_text = kv_dict.get("threads")
        if stop_reply_threads_text:
            return [int(thread_id, 16) for thread_id in stop_reply_threads_text.split(",")]
        else:
            return []

    def QListThreadsInStopReply_supported(self):
        procs = self.prep_debug_monitor_and_inferior()
        self.test_sequence.add_log_lines(self.ENABLE_THREADS_IN_STOP_REPLY_ENTRIES, True)

        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

    @debugserver_test
    @dsym_test
    def test_QListThreadsInStopReply_supported_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.set_inferior_startup_launch()
        self.QListThreadsInStopReply_supported()

    @llgs_test
    @dwarf_test
    def test_QListThreadsInStopReply_supported_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.set_inferior_startup_launch()
        self.QListThreadsInStopReply_supported()

    def stop_reply_reports_multiple_threads(self, thread_count):
        # Gather threads from stop notification when QThreadsInStopReply is enabled.
        stop_reply_threads = self.gather_stop_reply_threads(self.ENABLE_THREADS_IN_STOP_REPLY_ENTRIES, thread_count)
        self.assertEquals(len(stop_reply_threads), thread_count)

    @debugserver_test
    @dsym_test
    def test_stop_reply_reports_multiple_threads_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.set_inferior_startup_launch()
        self.stop_reply_reports_multiple_threads(5)

    @llgs_test
    @dwarf_test
    def test_stop_reply_reports_multiple_threads_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.set_inferior_startup_launch()
        self.stop_reply_reports_multiple_threads(5)

    def no_QListThreadsInStopReply_supplies_no_threads(self, thread_count):
        # Gather threads from stop notification when QThreadsInStopReply is not enabled.
        stop_reply_threads = self.gather_stop_reply_threads(None, thread_count)
        self.assertEquals(len(stop_reply_threads), 0)

    @debugserver_test
    @dsym_test
    def test_no_QListThreadsInStopReply_supplies_no_threads_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.set_inferior_startup_launch()
        self.no_QListThreadsInStopReply_supplies_no_threads(5)

    @llgs_test
    @dwarf_test
    def test_no_QListThreadsInStopReply_supplies_no_threads_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.set_inferior_startup_launch()
        self.no_QListThreadsInStopReply_supplies_no_threads(5)

    def stop_reply_reports_correct_threads(self, thread_count):
        # Gather threads from stop notification when QThreadsInStopReply is enabled.
        stop_reply_threads = self.gather_stop_reply_threads(self.ENABLE_THREADS_IN_STOP_REPLY_ENTRIES, thread_count)
        self.assertEquals(len(stop_reply_threads), thread_count)

        # Gather threads from q{f,s}ThreadInfo.
        self.reset_test_sequence()
        self.add_threadinfo_collection_packets()

        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        threads = self.parse_threadinfo_packets(context)
        self.assertIsNotNone(threads)
        self.assertEquals(len(threads), thread_count)
        
        # Ensure each thread in q{f,s}ThreadInfo appears in stop reply threads
        for tid in threads:
            self.assertTrue(tid in stop_reply_threads)

    @debugserver_test
    @dsym_test
    def test_stop_reply_reports_correct_threads_debugserver_dsym(self):
        self.init_debugserver_test()
        self.buildDsym()
        self.set_inferior_startup_launch()
        self.stop_reply_reports_correct_threads(5)

    @llgs_test
    @dwarf_test
    def test_stop_reply_reports_correct_threads_llgs_dwarf(self):
        self.init_llgs_test()
        self.buildDwarf()
        self.set_inferior_startup_launch()
        self.stop_reply_reports_correct_threads(5)


if __name__ == '__main__':
    unittest2.main()
