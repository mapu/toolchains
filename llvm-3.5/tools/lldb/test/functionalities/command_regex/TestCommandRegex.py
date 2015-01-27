"""
Test lldb 'commands regex' command which allows the user to create a regular expression command.
"""

import os
import unittest2
import lldb
from lldbtest import *

class CommandRegexTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    def test_command_regex(self):
        """Test a simple scenario of 'command regex' invocation and subsequent use."""
        import pexpect
        prompt = "(lldb) "
        regex_prompt = "Enter one of more sed substitution commands in the form: 's/<regex>/<subst>/'.\r\nTerminate the substitution list with an empty line.\r\n"
        regex_prompt1 = "\r\n"

        child = pexpect.spawn('%s %s' % (self.lldbHere, self.lldbOption))
        # Turn on logging for what the child sends back.
        if self.TraceOn():
            child.logfile_read = sys.stdout
        # So that the spawned lldb session gets shutdown durng teardown.
        self.child = child

        # Substitute 'Help!' for 'help' using the 'commands regex' mechanism.
        child.expect_exact(prompt)
        child.sendline("command regex 'Help__'")
        child.expect_exact(regex_prompt)
        child.sendline('s/^$/help/')
        child.expect_exact(regex_prompt1)
        child.sendline('')
        child.expect_exact(prompt)
        # Help!
        child.sendline('Help__')
        # If we see the familiar 'help' output, the test is done.
        child.expect('The following is a list of built-in, permanent debugger commands:')

if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
