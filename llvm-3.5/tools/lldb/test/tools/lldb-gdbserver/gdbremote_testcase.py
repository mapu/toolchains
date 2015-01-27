"""
Base class for gdb-remote test cases.
"""

import errno
import os
import os.path
import platform
import random
import re
import select
import sets
import signal
import socket
import subprocess
import sys
import tempfile
import time
import unittest2
from lldbtest import *
from lldbgdbserverutils import *
import logging

class GdbRemoteTestCaseBase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    _TIMEOUT_SECONDS = 5

    _GDBREMOTE_KILL_PACKET = "$k#6b"

    _LOGGING_LEVEL = logging.WARNING
    # _LOGGING_LEVEL = logging.DEBUG

    # Start the inferior separately, attach to the inferior on the stub command line.
    _STARTUP_ATTACH = "attach"
    # Start the inferior separately, start the stub without attaching, allow the test to attach to the inferior however it wants (e.g. $vAttach;pid).
    _STARTUP_ATTACH_MANUALLY = "attach_manually"
    # Start the stub, and launch the inferior with an $A packet via the initial packet stream.
    _STARTUP_LAUNCH = "launch"

    # GDB Signal numbers that are not target-specific used for common exceptions
    TARGET_EXC_BAD_ACCESS      = 0x91
    TARGET_EXC_BAD_INSTRUCTION = 0x92
    TARGET_EXC_ARITHMETIC      = 0x93
    TARGET_EXC_EMULATION       = 0x94
    TARGET_EXC_SOFTWARE        = 0x95
    TARGET_EXC_BREAKPOINT      = 0x96

    def setUp(self):
        TestBase.setUp(self)
        FORMAT = '%(asctime)-15s %(levelname)-8s %(message)s'
        logging.basicConfig(format=FORMAT)
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(self._LOGGING_LEVEL)
        self.test_sequence = GdbRemoteTestSequence(self.logger)
        self.set_inferior_startup_launch()
        self.port = self.get_next_port()
        self.named_pipe_path = None
        self.named_pipe = None
        self.named_pipe_fd = None
        self.stub_sends_two_stop_notifications_on_kill = False
        self.stub_hostname = "localhost"

    def get_next_port(self):
        return 12000 + random.randint(0,3999)

    def reset_test_sequence(self):
        self.test_sequence = GdbRemoteTestSequence(self.logger)

    def create_named_pipe(self):
        # Create a temp dir and name for a pipe.
        temp_dir = tempfile.mkdtemp()
        named_pipe_path = os.path.join(temp_dir, "stub_port_number")

        # Create the named pipe.
        os.mkfifo(named_pipe_path)

        # Open the read side of the pipe in non-blocking mode.  This will return right away, ready or not.
        named_pipe_fd = os.open(named_pipe_path, os.O_RDONLY | os.O_NONBLOCK)

        # Create the file for the named pipe.  Note this will follow semantics of
        # a non-blocking read side of a named pipe, which has different semantics
        # than a named pipe opened for read in non-blocking mode.
        named_pipe = os.fdopen(named_pipe_fd, "r")
        self.assertIsNotNone(named_pipe)

        def shutdown_named_pipe():
            # Close the pipe.
            try:
                named_pipe.close()
            except:
                print "failed to close named pipe"
                None

            # Delete the pipe.
            try:
                os.remove(named_pipe_path)
            except:
                print "failed to delete named pipe: {}".format(named_pipe_path)
                None

            # Delete the temp directory.
            try:
                os.rmdir(temp_dir)
            except:
                print "failed to delete temp dir: {}, directory contents: '{}'".format(temp_dir, os.listdir(temp_dir))
                None

        # Add the shutdown hook to clean up the named pipe.
        self.addTearDownHook(shutdown_named_pipe)

        # Clear the port so the stub selects a port number.
        self.port = 0

        return (named_pipe_path, named_pipe, named_pipe_fd)

    def get_stub_port_from_named_socket(self, read_timeout_seconds=5):
        # Wait for something to read with a max timeout.
        (ready_readers, _, _) = select.select([self.named_pipe_fd], [], [], read_timeout_seconds)
        self.assertIsNotNone(ready_readers, "write side of pipe has not written anything - stub isn't writing to pipe.")
        self.assertNotEqual(len(ready_readers), 0, "write side of pipe has not written anything - stub isn't writing to pipe.")

        # Read the port from the named pipe.
        stub_port_raw = self.named_pipe.read()
        self.assertIsNotNone(stub_port_raw)
        self.assertNotEqual(len(stub_port_raw), 0, "no content to read on pipe")

        # Trim null byte, convert to int.
        stub_port_raw = stub_port_raw[:-1]
        stub_port = int(stub_port_raw)
        self.assertTrue(stub_port > 0)

        return stub_port

    def init_llgs_test(self, use_named_pipe=True):
        self.debug_monitor_exe = get_lldb_gdbserver_exe()
        if not self.debug_monitor_exe:
            self.skipTest("lldb_gdbserver exe not found")
        self.debug_monitor_extra_args = " -c 'log enable -T -f process-{}.log lldb break process thread' -c 'log enable -T -f packets-{}.log gdb-remote packets'".format(self.id(), self.id(), self.id())
        if use_named_pipe:
            (self.named_pipe_path, self.named_pipe, self.named_pipe_fd) = self.create_named_pipe()

    def init_debugserver_test(self, use_named_pipe=True):
        self.debug_monitor_exe = get_debugserver_exe()
        if not self.debug_monitor_exe:
            self.skipTest("debugserver exe not found")
        self.debug_monitor_extra_args = " --log-file=/tmp/packets-{}.log --log-flags=0x800000".format(self._testMethodName)
        if use_named_pipe:
            (self.named_pipe_path, self.named_pipe, self.named_pipe_fd) = self.create_named_pipe()
        # The debugserver stub has a race on handling the 'k' command, so it sends an X09 right away, then sends the real X notification
        # when the process truly dies.
        self.stub_sends_two_stop_notifications_on_kill = True

    def create_socket(self):
        sock = socket.socket()
        logger = self.logger

        def shutdown_socket():
            if sock:
                try:
                    # send the kill packet so lldb-gdbserver shuts down gracefully
                    sock.sendall(GdbRemoteTestCaseBase._GDBREMOTE_KILL_PACKET)
                except:
                    logger.warning("failed to send kill packet to debug monitor: {}; ignoring".format(sys.exc_info()[0]))

                try:
                    sock.close()
                except:
                    logger.warning("failed to close socket to debug monitor: {}; ignoring".format(sys.exc_info()[0]))

        self.addTearDownHook(shutdown_socket)

        connect_info = (self.stub_hostname, self.port)
        # print "connecting to stub on {}:{}".format(connect_info[0], connect_info[1])
        sock.connect(connect_info)

        return sock

    def set_inferior_startup_launch(self):
        self._inferior_startup = self._STARTUP_LAUNCH

    def set_inferior_startup_attach(self):
        self._inferior_startup = self._STARTUP_ATTACH

    def set_inferior_startup_attach_manually(self):
        self._inferior_startup = self._STARTUP_ATTACH_MANUALLY

    def get_debug_monitor_command_line(self, attach_pid=None):
        commandline = "{}{} localhost:{}".format(self.debug_monitor_exe, self.debug_monitor_extra_args, self.port)
        if attach_pid:
            commandline += " --attach=%d" % attach_pid
        if self.named_pipe_path:
            commandline += " --named-pipe %s" % self.named_pipe_path
        return commandline

    def launch_debug_monitor(self, attach_pid=None, logfile=None):
        # Create the command line.
        import pexpect
        commandline = self.get_debug_monitor_command_line(attach_pid=attach_pid)

        # Start the server.
        server = pexpect.spawn(commandline, logfile=logfile)
        self.assertIsNotNone(server)
        server.expect(r"(debugserver|lldb-gdbserver)", timeout=10)

        # If we're receiving the stub's listening port from the named pipe, do that here.
        if self.named_pipe:
            self.port = self.get_stub_port_from_named_socket()
            # print "debug server listening on {}".format(self.port)

        # Turn on logging for what the child sends back.
        if self.TraceOn():
            server.logfile_read = sys.stdout

        return server

    def connect_to_debug_monitor(self, attach_pid=None):
        if self.named_pipe:
            # Create the stub.
            server = self.launch_debug_monitor(attach_pid=attach_pid)
            self.assertIsNotNone(server)

            def shutdown_debug_monitor():
                try:
                    server.close()
                except:
                    logger.warning("failed to close pexpect server for debug monitor: {}; ignoring".format(sys.exc_info()[0]))
            self.addTearDownHook(shutdown_debug_monitor)

            # Schedule debug monitor to be shut down during teardown.
            logger = self.logger

            # Attach to the stub and return a socket opened to it.
            self.sock = self.create_socket()
            return server

        # We're using a random port algorithm to try not to collide with other ports,
        # and retry a max # times.
        attempts = 0
        MAX_ATTEMPTS = 20

        while attempts < MAX_ATTEMPTS:
            server = self.launch_debug_monitor(attach_pid=attach_pid)

            # Wait until we receive the server ready message before continuing.
            port_good = True
            try:
                server.expect_exact('Listening to port {} for a connection from localhost'.format(self.port))
            except:
                port_good = False
                server.close()

            if port_good:
                # Schedule debug monitor to be shut down during teardown.
                logger = self.logger
                def shutdown_debug_monitor():
                    try:
                        server.close()
                    except:
                        logger.warning("failed to close pexpect server for debug monitor: {}; ignoring".format(sys.exc_info()[0]))
                self.addTearDownHook(shutdown_debug_monitor)

                # Create a socket to talk to the server
                try:
                    self.sock = self.create_socket()
                    return server
                except socket.error as serr:
                    # We're only trying to handle connection refused.
                    if serr.errno != errno.ECONNREFUSED:
                        raise serr
                    # We should close the server here to be safe.
                    server.close()

            # Increment attempts.
            print("connect to debug monitor on port %d failed, attempt #%d of %d" % (self.port, attempts + 1, MAX_ATTEMPTS))
            attempts += 1

            # And wait a random length of time before next attempt, to avoid collisions.
            time.sleep(random.randint(1,5))
            
            # Now grab a new port number.
            self.port = self.get_next_port()

        raise Exception("failed to create a socket to the launched debug monitor after %d tries" % attempts)

    def launch_process_for_attach(self,inferior_args=None, sleep_seconds=3, exe_path=None):
        # We're going to start a child process that the debug monitor stub can later attach to.
        # This process needs to be started so that it just hangs around for a while.  We'll
        # have it sleep.
        if not exe_path:
            exe_path = os.path.abspath("a.out")

        args = [exe_path]
        if inferior_args:
            args.extend(inferior_args)
        if sleep_seconds:
            args.append("sleep:%d" % sleep_seconds)

        return subprocess.Popen(args)

    def prep_debug_monitor_and_inferior(self, inferior_args=None, inferior_sleep_seconds=3, inferior_exe_path=None):
        """Prep the debug monitor, the inferior, and the expected packet stream.

        Handle the separate cases of using the debug monitor in attach-to-inferior mode
        and in launch-inferior mode.

        For attach-to-inferior mode, the inferior process is first started, then
        the debug monitor is started in attach to pid mode (using --attach on the
        stub command line), and the no-ack-mode setup is appended to the packet
        stream.  The packet stream is not yet executed, ready to have more expected
        packet entries added to it.

        For launch-inferior mode, the stub is first started, then no ack mode is
        setup on the expected packet stream, then the verified launch packets are added
        to the expected socket stream.  The packet stream is not yet executed, ready
        to have more expected packet entries added to it.

        The return value is:
        {inferior:<inferior>, server:<server>}
        """
        inferior = None
        attach_pid = None

        if self._inferior_startup == self._STARTUP_ATTACH or self._inferior_startup == self._STARTUP_ATTACH_MANUALLY:
            # Launch the process that we'll use as the inferior.
            inferior = self.launch_process_for_attach(inferior_args=inferior_args, sleep_seconds=inferior_sleep_seconds, exe_path=inferior_exe_path)
            self.assertIsNotNone(inferior)
            self.assertTrue(inferior.pid > 0)
            if self._inferior_startup == self._STARTUP_ATTACH:
                # In this case, we want the stub to attach via the command line, so set the command line attach pid here.
                attach_pid = inferior.pid

        # Launch the debug monitor stub, attaching to the inferior.
        server = self.connect_to_debug_monitor(attach_pid=attach_pid)
        self.assertIsNotNone(server)

        if self._inferior_startup == self._STARTUP_LAUNCH:
            # Build launch args
            if not inferior_exe_path:
                inferior_exe_path = os.path.abspath("a.out")
            launch_args = [inferior_exe_path]
            if inferior_args:
                launch_args.extend(inferior_args)

        # Build the expected protocol stream
        self.add_no_ack_remote_stream()
        if self._inferior_startup == self._STARTUP_LAUNCH:
            self.add_verified_launch_packets(launch_args)

        return {"inferior":inferior, "server":server}

    def expect_socket_recv(self, sock, expected_content_regex, timeout_seconds):
        response = ""
        timeout_time = time.time() + timeout_seconds

        while not expected_content_regex.match(response) and time.time() < timeout_time: 
            can_read, _, _ = select.select([sock], [], [], timeout_seconds)
            if can_read and sock in can_read:
                recv_bytes = sock.recv(4096)
                if recv_bytes:
                    response += recv_bytes

        self.assertTrue(expected_content_regex.match(response))

    def expect_socket_send(self, sock, content, timeout_seconds):
        request_bytes_remaining = content
        timeout_time = time.time() + timeout_seconds

        while len(request_bytes_remaining) > 0 and time.time() < timeout_time:
            _, can_write, _ = select.select([], [sock], [], timeout_seconds)
            if can_write and sock in can_write:
                written_byte_count = sock.send(request_bytes_remaining)
                request_bytes_remaining = request_bytes_remaining[written_byte_count:]
        self.assertEquals(len(request_bytes_remaining), 0)

    def do_handshake(self, stub_socket, timeout_seconds=5):
        # Write the ack.
        self.expect_socket_send(stub_socket, "+", timeout_seconds)

        # Send the start no ack mode packet.
        NO_ACK_MODE_REQUEST = "$QStartNoAckMode#b0"
        bytes_sent = stub_socket.send(NO_ACK_MODE_REQUEST)
        self.assertEquals(bytes_sent, len(NO_ACK_MODE_REQUEST))

        # Receive the ack and "OK"
        self.expect_socket_recv(stub_socket, re.compile(r"^\+\$OK#[0-9a-fA-F]{2}$"), timeout_seconds)

        # Send the final ack.
        self.expect_socket_send(stub_socket, "+", timeout_seconds)

    def add_no_ack_remote_stream(self):
        self.test_sequence.add_log_lines(
            ["read packet: +",
             "read packet: $QStartNoAckMode#b0",
             "send packet: +",
             "send packet: $OK#9a",
             "read packet: +"],
            True)

    def add_verified_launch_packets(self, launch_args):
        self.test_sequence.add_log_lines(
            ["read packet: %s" % build_gdbremote_A_packet(launch_args),
             "send packet: $OK#00",
             "read packet: $qLaunchSuccess#a5",
             "send packet: $OK#00"],
            True)

    def add_thread_suffix_request_packets(self):
        self.test_sequence.add_log_lines(
            ["read packet: $QThreadSuffixSupported#e4",
             "send packet: $OK#00",
            ], True)

    def add_process_info_collection_packets(self):
        self.test_sequence.add_log_lines(
            ["read packet: $qProcessInfo#dc",
              { "direction":"send", "regex":r"^\$(.+)#[0-9a-fA-F]{2}$", "capture":{1:"process_info_raw"} }],
            True)

    _KNOWN_PROCESS_INFO_KEYS = [
        "pid",
        "parent-pid",
        "real-uid",
        "real-gid",
        "effective-uid",
        "effective-gid",
        "cputype",
        "cpusubtype",
        "ostype",
        "triple",
        "vendor",
        "endian",
        "ptrsize"
        ]

    def parse_process_info_response(self, context):
        # Ensure we have a process info response.
        self.assertIsNotNone(context)
        process_info_raw = context.get("process_info_raw")
        self.assertIsNotNone(process_info_raw)

        # Pull out key:value; pairs.
        process_info_dict = { match.group(1):match.group(2) for match in re.finditer(r"([^:]+):([^;]+);", process_info_raw) }

        # Validate keys are known.
        for (key, val) in process_info_dict.items():
            self.assertTrue(key in self._KNOWN_PROCESS_INFO_KEYS)
            self.assertIsNotNone(val)

        return process_info_dict

    def add_register_info_collection_packets(self):
        self.test_sequence.add_log_lines(
            [ { "type":"multi_response", "query":"qRegisterInfo", "append_iteration_suffix":True,
              "end_regex":re.compile(r"^\$(E\d+)?#[0-9a-fA-F]{2}$"),
              "save_key":"reg_info_responses" } ],
            True)

    def parse_register_info_packets(self, context):
        """Return an array of register info dictionaries, one per register info."""
        reg_info_responses = context.get("reg_info_responses")
        self.assertIsNotNone(reg_info_responses)

        # Parse register infos.
        return [parse_reg_info_response(reg_info_response) for reg_info_response in reg_info_responses]

    def expect_gdbremote_sequence(self, timeout_seconds=None):
        if not timeout_seconds:
            timeout_seconds = self._TIMEOUT_SECONDS
        return expect_lldb_gdbserver_replay(self, self.sock, self.test_sequence, timeout_seconds, self.logger)

    _KNOWN_REGINFO_KEYS = [
        "name",
        "alt-name",
        "bitsize",
        "offset",
        "encoding",
        "format",
        "set",
        "gcc",
        "dwarf",
        "generic",
        "container-regs",
        "invalidate-regs"
    ]

    def assert_valid_reg_info(self, reg_info):
        # Assert we know about all the reginfo keys parsed.
        for key in reg_info:
            self.assertTrue(key in self._KNOWN_REGINFO_KEYS)

        # Check the bare-minimum expected set of register info keys.
        self.assertTrue("name" in reg_info)
        self.assertTrue("bitsize" in reg_info)
        self.assertTrue("offset" in reg_info)
        self.assertTrue("encoding" in reg_info)
        self.assertTrue("format" in reg_info)

    def find_pc_reg_info(self, reg_infos):
        lldb_reg_index = 0
        for reg_info in reg_infos:
            if ("generic" in reg_info) and (reg_info["generic"] == "pc"):
                return (lldb_reg_index, reg_info)
            lldb_reg_index += 1

        return (None, None)

    def add_lldb_register_index(self, reg_infos):
        """Add a "lldb_register_index" key containing the 0-baed index of each reg_infos entry.

        We'll use this when we want to call packets like P/p with a register index but do so
        on only a subset of the full register info set.
        """
        self.assertIsNotNone(reg_infos)

        reg_index = 0
        for reg_info in reg_infos:
            reg_info["lldb_register_index"] = reg_index
            reg_index += 1

    def add_query_memory_region_packets(self, address):
        self.test_sequence.add_log_lines(
            ["read packet: $qMemoryRegionInfo:{0:x}#00".format(address),
             {"direction":"send", "regex":r"^\$(.+)#[0-9a-fA-F]{2}$", "capture":{1:"memory_region_response"} }],
            True)

    def parse_key_val_dict(self, key_val_text, allow_dupes=True):
        self.assertIsNotNone(key_val_text)
        kv_dict = {}
        for match in re.finditer(r";?([^:]+):([^;]+)", key_val_text):
            key = match.group(1)
            val = match.group(2)
            if key in kv_dict:
                if allow_dupes:
                    if type(kv_dict[key]) == list:
                        kv_dict[key].append(val)
                    else:
                        # Promote to list
                        kv_dict[key] = [kv_dict[key], val]
                else:
                    self.fail("key '{}' already present when attempting to add value '{}' (text='{}', dict={})".format(key, val, key_val_text, kv_dict))
            else:
                kv_dict[key] = val
        return kv_dict

    def parse_memory_region_packet(self, context):
        # Ensure we have a context.
        self.assertIsNotNone(context.get("memory_region_response"))

        # Pull out key:value; pairs.
        mem_region_dict = self.parse_key_val_dict(context.get("memory_region_response"))

        # Validate keys are known.
        for (key, val) in mem_region_dict.items():
            self.assertTrue(key in ["start", "size", "permissions", "error"])
            self.assertIsNotNone(val)

        # Return the dictionary of key-value pairs for the memory region.
        return mem_region_dict

    def assert_address_within_memory_region(self, test_address, mem_region_dict):
        self.assertIsNotNone(mem_region_dict)
        self.assertTrue("start" in mem_region_dict)
        self.assertTrue("size" in mem_region_dict)

        range_start = int(mem_region_dict["start"], 16)
        range_size = int(mem_region_dict["size"], 16)
        range_end = range_start + range_size

        if test_address < range_start:
            self.fail("address 0x{0:x} comes before range 0x{1:x} - 0x{2:x} (size 0x{3:x})".format(test_address, range_start, range_end, range_size))
        elif test_address >= range_end:
            self.fail("address 0x{0:x} comes after range 0x{1:x} - 0x{2:x} (size 0x{3:x})".format(test_address, range_start, range_end, range_size))

    def add_threadinfo_collection_packets(self):
        self.test_sequence.add_log_lines(
            [ { "type":"multi_response", "first_query":"qfThreadInfo", "next_query":"qsThreadInfo",
                "append_iteration_suffix":False, "end_regex":re.compile(r"^\$(l)?#[0-9a-fA-F]{2}$"),
              "save_key":"threadinfo_responses" } ],
            True)

    def parse_threadinfo_packets(self, context):
        """Return an array of thread ids (decimal ints), one per thread."""
        threadinfo_responses = context.get("threadinfo_responses")
        self.assertIsNotNone(threadinfo_responses)

        thread_ids = []
        for threadinfo_response in threadinfo_responses:
            new_thread_infos = parse_threadinfo_response(threadinfo_response)
            thread_ids.extend(new_thread_infos)
        return thread_ids

    def wait_for_thread_count(self, thread_count, timeout_seconds=3):
        start_time = time.time()
        timeout_time = start_time + timeout_seconds

        actual_thread_count = 0
        while actual_thread_count < thread_count:
            self.reset_test_sequence()
            self.add_threadinfo_collection_packets()

            context = self.expect_gdbremote_sequence()
            self.assertIsNotNone(context)

            threads = self.parse_threadinfo_packets(context)
            self.assertIsNotNone(threads)

            actual_thread_count = len(threads)

            if time.time() > timeout_time:
                raise Exception(
                    'timed out after {} seconds while waiting for theads: waiting for at least {} threads, found {}'.format(
                        timeout_seconds, thread_count, actual_thread_count))

        return threads

    def add_set_breakpoint_packets(self, address, do_continue=True, breakpoint_kind=1):
        self.test_sequence.add_log_lines(
            [# Set the breakpoint.
             "read packet: $Z0,{0:x},{1}#00".format(address, breakpoint_kind),
             # Verify the stub could set it.
             "send packet: $OK#00",
             ], True)

        if (do_continue):
            self.test_sequence.add_log_lines(
                [# Continue the inferior.
                 "read packet: $c#63",
                 # Expect a breakpoint stop report.
                 {"direction":"send", "regex":r"^\$T([0-9a-fA-F]{2})thread:([0-9a-fA-F]+);", "capture":{1:"stop_signo", 2:"stop_thread_id"} },
                 ], True)        

    def add_remove_breakpoint_packets(self, address, breakpoint_kind=1):
        self.test_sequence.add_log_lines(
            [# Remove the breakpoint.
             "read packet: $z0,{0:x},{1}#00".format(address, breakpoint_kind),
             # Verify the stub could unset it.
             "send packet: $OK#00",
            ], True)

    def add_qSupported_packets(self):
        self.test_sequence.add_log_lines(
            ["read packet: $qSupported#00",
             {"direction":"send", "regex":r"^\$(.*)#[0-9a-fA-F]{2}", "capture":{1: "qSupported_response"}},
            ], True)

    _KNOWN_QSUPPORTED_STUB_FEATURES = [
        "augmented-libraries-svr4-read",
        "PacketSize",
        "QStartNoAckMode",
        "QThreadSuffixSupported",
        "QListThreadsInStopReply",
        "qXfer:auxv:read",
        "qXfer:libraries:read",
        "qXfer:libraries-svr4:read",
    ]

    def parse_qSupported_response(self, context):
        self.assertIsNotNone(context)

        raw_response = context.get("qSupported_response")
        self.assertIsNotNone(raw_response)

        # For values with key=val, the dict key and vals are set as expected.  For feature+, feature- and feature?, the
        # +,-,? is stripped from the key and set as the value.
        supported_dict = {}
        for match in re.finditer(r";?([^=;]+)(=([^;]+))?", raw_response):
            key = match.group(1)
            val = match.group(3)

            # key=val: store as is
            if val and len(val) > 0:
                supported_dict[key] = val
            else:
                if len(key) < 2:
                    raise Exception("singular stub feature is too short: must be stub_feature{+,-,?}")
                supported_type = key[-1]
                key = key[:-1]
                if not supported_type in ["+", "-", "?"]:
                    raise Exception("malformed stub feature: final character {} not in expected set (+,-,?)".format(supported_type))
                supported_dict[key] = supported_type 
            # Ensure we know the supported element
            if not key in self._KNOWN_QSUPPORTED_STUB_FEATURES:
                raise Exception("unknown qSupported stub feature reported: %s" % key)

        return supported_dict

    def run_process_then_stop(self, run_seconds=1):
        # Tell the stub to continue.
        self.test_sequence.add_log_lines(
             ["read packet: $vCont;c#a8"],
             True)
        context = self.expect_gdbremote_sequence()

        # Wait for run_seconds.
        time.sleep(run_seconds)

        # Send an interrupt, capture a T response.
        self.reset_test_sequence()
        self.test_sequence.add_log_lines(
            ["read packet: {}".format(chr(03)),
             {"direction":"send", "regex":r"^\$T([0-9a-fA-F]+)([^#]+)#[0-9a-fA-F]{2}$", "capture":{1:"stop_result"} }],
            True)
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)
        self.assertIsNotNone(context.get("stop_result"))

        return context

    def select_modifiable_register(self, reg_infos):
        """Find a register that can be read/written freely."""
        PREFERRED_REGISTER_NAMES = sets.Set(["rax",])

        # First check for the first register from the preferred register name set.
        alternative_register_index = None

        self.assertIsNotNone(reg_infos)
        for reg_info in reg_infos:
            if ("name" in reg_info) and (reg_info["name"] in PREFERRED_REGISTER_NAMES):
                # We found a preferred register.  Use it.
                return reg_info["lldb_register_index"]
            if ("generic" in reg_info) and (reg_info["generic"] == "fp"):
                # A frame pointer register will do as a register to modify temporarily.
                alternative_register_index = reg_info["lldb_register_index"]

        # We didn't find a preferred register.  Return whatever alternative register
        # we found, if any.
        return alternative_register_index

    def extract_registers_from_stop_notification(self, stop_key_vals_text):
        self.assertIsNotNone(stop_key_vals_text)
        kv_dict = self.parse_key_val_dict(stop_key_vals_text)

        registers = {}
        for (key, val) in kv_dict.items():
            if re.match(r"^[0-9a-fA-F]+$", key):
                registers[int(key, 16)] = val
        return registers

    def gather_register_infos(self):
        self.reset_test_sequence()
        self.add_register_info_collection_packets()

        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        reg_infos = self.parse_register_info_packets(context)
        self.assertIsNotNone(reg_infos)
        self.add_lldb_register_index(reg_infos)

        return reg_infos

    def find_generic_register_with_name(self, reg_infos, generic_name):
        self.assertIsNotNone(reg_infos)
        for reg_info in reg_infos:
            if ("generic" in reg_info) and (reg_info["generic"] == generic_name):
                return reg_info
        return None

    def decode_gdbremote_binary(self, encoded_bytes):
        decoded_bytes = ""
        i = 0
        while i < len(encoded_bytes):
            if encoded_bytes[i] == "}":
                # Handle escaped char.
                self.assertTrue(i + 1 < len(encoded_bytes))
                decoded_bytes += chr(ord(encoded_bytes[i+1]) ^ 0x20)
                i +=2
            elif encoded_bytes[i] == "*":
                # Handle run length encoding.
                self.assertTrue(len(decoded_bytes) > 0)
                self.assertTrue(i + 1 < len(encoded_bytes))
                repeat_count = ord(encoded_bytes[i+1]) - 29
                decoded_bytes += decoded_bytes[-1] * repeat_count
                i += 2
            else:
                decoded_bytes += encoded_bytes[i]
                i += 1
        return decoded_bytes

    def build_auxv_dict(self, endian, word_size, auxv_data):
        self.assertIsNotNone(endian)
        self.assertIsNotNone(word_size)
        self.assertIsNotNone(auxv_data)

        auxv_dict = {}

        while len(auxv_data) > 0:
            # Chop off key.
            raw_key = auxv_data[:word_size]
            auxv_data = auxv_data[word_size:]

            # Chop of value.
            raw_value = auxv_data[:word_size]
            auxv_data = auxv_data[word_size:]

            # Convert raw text from target endian.
            key = unpack_endian_binary_string(endian, raw_key)
            value = unpack_endian_binary_string(endian, raw_value)

            # Handle ending entry.
            if key == 0:
                self.assertEquals(value, 0)
                return auxv_dict

            # The key should not already be present.
            self.assertFalse(key in auxv_dict)
            auxv_dict[key] = value

        self.fail("should not reach here - implies required double zero entry not found")
        return auxv_dict

    def read_binary_data_in_chunks(self, command_prefix, chunk_length):
        """Collect command_prefix{offset:x},{chunk_length:x} until a single 'l' or 'l' with data is returned."""
        offset = 0
        done = False
        decoded_data = ""

        while not done:
            # Grab the next iteration of data.
            self.reset_test_sequence()
            self.test_sequence.add_log_lines([
                "read packet: ${}{:x},{:x}:#00".format(command_prefix, offset, chunk_length),
                {"direction":"send", "regex":re.compile(r"^\$([^E])(.*)#[0-9a-fA-F]{2}$", re.MULTILINE|re.DOTALL), "capture":{1:"response_type", 2:"content_raw"} }
                ], True)

            context = self.expect_gdbremote_sequence()
            self.assertIsNotNone(context)

            response_type = context.get("response_type")
            self.assertIsNotNone(response_type)
            self.assertTrue(response_type in ["l", "m"])

            # Move offset along.
            offset += chunk_length

            # Figure out if we're done.  We're done if the response type is l.
            done = response_type == "l"

            # Decode binary data.
            content_raw = context.get("content_raw")
            if content_raw and len(content_raw) > 0:
                self.assertIsNotNone(content_raw)
                decoded_data += self.decode_gdbremote_binary(content_raw)
        return decoded_data

    def add_interrupt_packets(self):
        self.test_sequence.add_log_lines([
            # Send the intterupt.
            "read packet: {}".format(chr(03)),
            # And wait for the stop notification.
            {"direction":"send", "regex":r"^\$T([0-9a-fA-F]{2})(.*)#[0-9a-fA-F]{2}$", "capture":{1:"stop_signo", 2:"stop_key_val_text" } },
            ], True)

    def parse_interrupt_packets(self, context):
        self.assertIsNotNone(context.get("stop_signo"))
        self.assertIsNotNone(context.get("stop_key_val_text"))
        return (int(context["stop_signo"], 16), self.parse_key_val_dict(context["stop_key_val_text"]))

    def add_QSaveRegisterState_packets(self, thread_id):
        if thread_id:
            # Use the thread suffix form.
            request = "read packet: $QSaveRegisterState;thread:{:x}#00".format(thread_id)
        else:
            request = "read packet: $QSaveRegisterState#00"
            
        self.test_sequence.add_log_lines([
            request,
            {"direction":"send", "regex":r"^\$(E?.*)#[0-9a-fA-F]{2}$", "capture":{1:"save_response" } },
            ], True)

    def parse_QSaveRegisterState_response(self, context):
        self.assertIsNotNone(context)

        save_response = context.get("save_response")
        self.assertIsNotNone(save_response)

        if len(save_response) < 1 or save_response[0] == "E":
            # error received
            return (False, None)
        else:
            return (True, int(save_response))

    def add_QRestoreRegisterState_packets(self, save_id, thread_id=None):
        if thread_id:
            # Use the thread suffix form.
            request = "read packet: $QRestoreRegisterState:{};thread:{:x}#00".format(save_id, thread_id)
        else:
            request = "read packet: $QRestoreRegisterState:{}#00".format(save_id)

        self.test_sequence.add_log_lines([
            request,
            "send packet: $OK#00"
            ], True)

    def flip_all_bits_in_each_register_value(self, reg_infos, endian, thread_id=None):
        self.assertIsNotNone(reg_infos)

        successful_writes = 0
        failed_writes = 0

        for reg_info in reg_infos:
            # Use the lldb register index added to the reg info.  We're not necessarily
            # working off a full set of register infos, so an inferred register index could be wrong. 
            reg_index = reg_info["lldb_register_index"]
            self.assertIsNotNone(reg_index)

            reg_byte_size = int(reg_info["bitsize"])/8
            self.assertTrue(reg_byte_size > 0)

            # Handle thread suffix.
            if thread_id:
                p_request = "read packet: $p{:x};thread:{:x}#00".format(reg_index, thread_id)
            else:
                p_request = "read packet: $p{:x}#00".format(reg_index)

            # Read the existing value.
            self.reset_test_sequence()
            self.test_sequence.add_log_lines([
                p_request,
                { "direction":"send", "regex":r"^\$([0-9a-fA-F]+)#", "capture":{1:"p_response"} },
                ], True)
            context = self.expect_gdbremote_sequence()
            self.assertIsNotNone(context)

            # Verify the response length.
            p_response = context.get("p_response")
            self.assertIsNotNone(p_response)
            initial_reg_value = unpack_register_hex_unsigned(endian, p_response)

            # Flip the value by xoring with all 1s
            all_one_bits_raw = "ff" * (int(reg_info["bitsize"]) / 8)
            flipped_bits_int = initial_reg_value ^ int(all_one_bits_raw, 16)
            # print "reg (index={}, name={}): val={}, flipped bits (int={}, hex={:x})".format(reg_index, reg_info["name"], initial_reg_value, flipped_bits_int, flipped_bits_int)

            # Handle thread suffix for P.
            if thread_id:
                P_request = "read packet: $P{:x}={};thread:{:x}#00".format(reg_index, pack_register_hex(endian, flipped_bits_int, byte_size=reg_byte_size), thread_id)
            else:
                P_request = "read packet: $P{:x}={}#00".format(reg_index, pack_register_hex(endian, flipped_bits_int, byte_size=reg_byte_size))

            # Write the flipped value to the register.
            self.reset_test_sequence()
            self.test_sequence.add_log_lines([
                P_request,
                { "direction":"send", "regex":r"^\$(OK|E[0-9a-fA-F]+)#[0-9a-fA-F]{2}", "capture":{1:"P_response"} },
                ], True)
            context = self.expect_gdbremote_sequence()
            self.assertIsNotNone(context)

            # Determine if the write succeeded.  There are a handful of registers that can fail, or partially fail
            # (e.g. flags, segment selectors, etc.) due to register value restrictions.  Don't worry about them
            # all flipping perfectly.
            P_response = context.get("P_response")
            self.assertIsNotNone(P_response)
            if P_response == "OK":
                successful_writes += 1
            else:
                failed_writes += 1
                # print "reg (index={}, name={}) write FAILED (error: {})".format(reg_index, reg_info["name"], P_response)

            # Read back the register value, ensure it matches the flipped value.
            if P_response == "OK":
                self.reset_test_sequence()
                self.test_sequence.add_log_lines([
                    p_request,
                    { "direction":"send", "regex":r"^\$([0-9a-fA-F]+)#", "capture":{1:"p_response"} },
                    ], True)
                context = self.expect_gdbremote_sequence()
                self.assertIsNotNone(context)

                verify_p_response_raw = context.get("p_response")
                self.assertIsNotNone(verify_p_response_raw)
                verify_bits = unpack_register_hex_unsigned(endian, verify_p_response_raw)

                if verify_bits != flipped_bits_int:
                    # Some registers, like mxcsrmask and others, will permute what's written.  Adjust succeed/fail counts.
                    # print "reg (index={}, name={}): read verify FAILED: wrote {:x}, verify read back {:x}".format(reg_index, reg_info["name"], flipped_bits_int, verify_bits)
                    successful_writes -= 1
                    failed_writes +=1

        return (successful_writes, failed_writes)

    def is_bit_flippable_register(self, reg_info):
        if not reg_info:
            return False
        if not "set" in reg_info:
            return False
        if reg_info["set"] != "General Purpose Registers":
            return False
        if ("container-regs" in reg_info) and (len(reg_info["container-regs"]) > 0):
            # Don't try to bit flip registers contained in another register.
            return False
        if re.match("^.s$", reg_info["name"]):
            # This is a 2-letter register name that ends in "s", like a segment register.
            # Don't try to bit flip these.
            return False
        # Okay, this looks fine-enough.
        return True

    def read_register_values(self, reg_infos, endian, thread_id=None):
        self.assertIsNotNone(reg_infos)
        values = {}

        for reg_info in reg_infos:
            # We append a register index when load reg infos so we can work with subsets.
            reg_index = reg_info.get("lldb_register_index")
            self.assertIsNotNone(reg_index)

            # Handle thread suffix.
            if thread_id:
                p_request = "read packet: $p{:x};thread:{:x}#00".format(reg_index, thread_id)
            else:
                p_request = "read packet: $p{:x}#00".format(reg_index)

            # Read it with p.
            self.reset_test_sequence()
            self.test_sequence.add_log_lines([
                p_request,
                { "direction":"send", "regex":r"^\$([0-9a-fA-F]+)#", "capture":{1:"p_response"} },
                ], True)
            context = self.expect_gdbremote_sequence()
            self.assertIsNotNone(context)

            # Convert value from target endian to integral.
            p_response = context.get("p_response")
            self.assertIsNotNone(p_response)
            self.assertTrue(len(p_response) > 0)
            self.assertFalse(p_response[0] == "E")
            
            values[reg_index] = unpack_register_hex_unsigned(endian, p_response)
            
        return values

    def add_vCont_query_packets(self):
        self.test_sequence.add_log_lines([
            "read packet: $vCont?#49",
            {"direction":"send", "regex":r"^\$(vCont)?(.*)#[0-9a-fA-F]{2}$", "capture":{2:"vCont_query_response" } },
            ], True)

    def parse_vCont_query_response(self, context):
        self.assertIsNotNone(context)
        vCont_query_response = context.get("vCont_query_response")

        # Handle case of no vCont support at all - in which case the capture group will be none or zero length.
        if not vCont_query_response or len(vCont_query_response) == 0:
            return {}

        return {key:1 for key in vCont_query_response.split(";") if key and len(key) > 0}

    def count_single_steps_until_true(self, thread_id, predicate, args, max_step_count=100, use_Hc_packet=True, step_instruction="s"):
        """Used by single step test that appears in a few different contexts."""
        single_step_count = 0

        while single_step_count < max_step_count:
            self.assertIsNotNone(thread_id)

            # Build the packet for the single step instruction.  We replace {thread}, if present, with the thread_id.
            step_packet = "read packet: ${}#00".format(re.sub(r"{thread}", "{:x}".format(thread_id), step_instruction))
            # print "\nstep_packet created: {}\n".format(step_packet)

            # Single step.
            self.reset_test_sequence()
            if use_Hc_packet:
                self.test_sequence.add_log_lines(
                    [# Set the continue thread.
                     "read packet: $Hc{0:x}#00".format(thread_id),
                     "send packet: $OK#00",
                     ], True)
            self.test_sequence.add_log_lines([
                 # Single step.
                 step_packet,
                 # "read packet: $vCont;s:{0:x}#00".format(thread_id),
                 # Expect a breakpoint stop report.
                 {"direction":"send", "regex":r"^\$T([0-9a-fA-F]{2})thread:([0-9a-fA-F]+);", "capture":{1:"stop_signo", 2:"stop_thread_id"} },
                 ], True)
            context = self.expect_gdbremote_sequence()
            self.assertIsNotNone(context)
            self.assertIsNotNone(context.get("stop_signo"))
            self.assertEquals(int(context.get("stop_signo"), 16), signal.SIGTRAP)

            single_step_count += 1

            # See if the predicate is true.  If so, we're done.
            if predicate(args):
                return (True, single_step_count)

        # The predicate didn't return true within the runaway step count.
        return (False, single_step_count)

    def g_c1_c2_contents_are(self, args):
        """Used by single step test that appears in a few different contexts."""
        g_c1_address = args["g_c1_address"]
        g_c2_address = args["g_c2_address"]
        expected_g_c1 = args["expected_g_c1"]
        expected_g_c2 = args["expected_g_c2"]

        # Read g_c1 and g_c2 contents.
        self.reset_test_sequence()
        self.test_sequence.add_log_lines(
            ["read packet: $m{0:x},{1:x}#00".format(g_c1_address, 1),
             {"direction":"send", "regex":r"^\$(.+)#[0-9a-fA-F]{2}$", "capture":{1:"g_c1_contents"} },
             "read packet: $m{0:x},{1:x}#00".format(g_c2_address, 1),
             {"direction":"send", "regex":r"^\$(.+)#[0-9a-fA-F]{2}$", "capture":{1:"g_c2_contents"} }],
            True)

        # Run the packet stream.
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Check if what we read from inferior memory is what we are expecting.
        self.assertIsNotNone(context.get("g_c1_contents"))
        self.assertIsNotNone(context.get("g_c2_contents"))

        return (context.get("g_c1_contents").decode("hex") == expected_g_c1) and (context.get("g_c2_contents").decode("hex") == expected_g_c2)

    def single_step_only_steps_one_instruction(self, use_Hc_packet=True, step_instruction="s"):
        """Used by single step test that appears in a few different contexts."""
        # Start up the inferior.
        procs = self.prep_debug_monitor_and_inferior(
            inferior_args=["get-code-address-hex:swap_chars", "get-data-address-hex:g_c1", "get-data-address-hex:g_c2", "sleep:1", "call-function:swap_chars", "sleep:5"])

        # Run the process
        self.test_sequence.add_log_lines(
            [# Start running after initial stop.
             "read packet: $c#63",
             # Match output line that prints the memory address of the function call entry point.
             # Note we require launch-only testing so we can get inferior otuput.
             { "type":"output_match", "regex":r"^code address: 0x([0-9a-fA-F]+)\r\ndata address: 0x([0-9a-fA-F]+)\r\ndata address: 0x([0-9a-fA-F]+)\r\n$", 
               "capture":{ 1:"function_address", 2:"g_c1_address", 3:"g_c2_address"} },
             # Now stop the inferior.
             "read packet: {}".format(chr(03)),
             # And wait for the stop notification.
             {"direction":"send", "regex":r"^\$T([0-9a-fA-F]{2})thread:([0-9a-fA-F]+);", "capture":{1:"stop_signo", 2:"stop_thread_id"} }],
            True)

        # Run the packet stream.
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Grab the main thread id.
        self.assertIsNotNone(context.get("stop_thread_id"))
        main_thread_id = int(context.get("stop_thread_id"), 16)

        # Grab the function address.
        self.assertIsNotNone(context.get("function_address"))
        function_address = int(context.get("function_address"), 16)

        # Grab the data addresses.
        self.assertIsNotNone(context.get("g_c1_address"))
        g_c1_address = int(context.get("g_c1_address"), 16)

        self.assertIsNotNone(context.get("g_c2_address"))
        g_c2_address = int(context.get("g_c2_address"), 16)

        # Set a breakpoint at the given address.
        # Note this might need to be switched per platform (ARM, mips, etc.).
        BREAKPOINT_KIND = 1
        self.reset_test_sequence()
        self.add_set_breakpoint_packets(function_address, do_continue=True, breakpoint_kind=BREAKPOINT_KIND)
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Remove the breakpoint.
        self.reset_test_sequence()
        self.add_remove_breakpoint_packets(function_address, breakpoint_kind=BREAKPOINT_KIND)
        context = self.expect_gdbremote_sequence()
        self.assertIsNotNone(context)

        # Verify g_c1 and g_c2 match expected initial state.
        args = {}
        args["g_c1_address"] = g_c1_address
        args["g_c2_address"] = g_c2_address
        args["expected_g_c1"] = "0"
        args["expected_g_c2"] = "1"

        self.assertTrue(self.g_c1_c2_contents_are(args))

        # Verify we take only a small number of steps to hit the first state.  Might need to work through function entry prologue code.
        args["expected_g_c1"] = "1"
        args["expected_g_c2"] = "1"
        (state_reached, step_count) = self.count_single_steps_until_true(main_thread_id, self.g_c1_c2_contents_are, args, max_step_count=25, use_Hc_packet=use_Hc_packet, step_instruction=step_instruction)
        self.assertTrue(state_reached)

        # Verify we hit the next state.
        args["expected_g_c1"] = "1"
        args["expected_g_c2"] = "0"
        (state_reached, step_count) = self.count_single_steps_until_true(main_thread_id, self.g_c1_c2_contents_are, args, max_step_count=5, use_Hc_packet=use_Hc_packet, step_instruction=step_instruction)
        self.assertTrue(state_reached)
        self.assertEquals(step_count, 1)

        # Verify we hit the next state.
        args["expected_g_c1"] = "0"
        args["expected_g_c2"] = "0"
        (state_reached, step_count) = self.count_single_steps_until_true(main_thread_id, self.g_c1_c2_contents_are, args, max_step_count=5, use_Hc_packet=use_Hc_packet, step_instruction=step_instruction)
        self.assertTrue(state_reached)
        self.assertEquals(step_count, 1)

        # Verify we hit the next state.
        args["expected_g_c1"] = "0"
        args["expected_g_c2"] = "1"
        (state_reached, step_count) = self.count_single_steps_until_true(main_thread_id, self.g_c1_c2_contents_are, args, max_step_count=5, use_Hc_packet=use_Hc_packet, step_instruction=step_instruction)
        self.assertTrue(state_reached)
        self.assertEquals(step_count, 1)

