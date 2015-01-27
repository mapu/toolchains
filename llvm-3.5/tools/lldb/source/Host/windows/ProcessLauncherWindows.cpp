//===-- ProcessLauncherWindows.cpp ------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lldb/Host/HostProcess.h"
#include "lldb/Host/windows/ProcessLauncherWindows.h"
#include "lldb/Target/ProcessLaunchInfo.h"

#include <string>
#include <vector>

using namespace lldb;
using namespace lldb_private;

HostProcess
ProcessLauncherWindows::LaunchProcess(const ProcessLaunchInfo &launch_info, Error &error)
{
    error.Clear();

    std::string executable;
    std::string commandLine;
    std::vector<char> environment;
    STARTUPINFO startupinfo = {0};
    PROCESS_INFORMATION pi = {0};

    HANDLE stdin_handle = GetStdioHandle(launch_info, STDIN_FILENO);
    HANDLE stdout_handle = GetStdioHandle(launch_info, STDOUT_FILENO);
    HANDLE stderr_handle = GetStdioHandle(launch_info, STDERR_FILENO);

    startupinfo.cb = sizeof(startupinfo);
    startupinfo.dwFlags |= STARTF_USESTDHANDLES;
    startupinfo.hStdError = stderr_handle;
    startupinfo.hStdInput = stdin_handle;
    startupinfo.hStdOutput = stdout_handle;

    const char *hide_console_var = getenv("LLDB_LAUNCH_INFERIORS_WITHOUT_CONSOLE");
    if (hide_console_var && llvm::StringRef(hide_console_var).equals_lower("true"))
    {
        startupinfo.dwFlags |= STARTF_USESHOWWINDOW;
        startupinfo.wShowWindow = SW_HIDE;
    }

    DWORD flags = CREATE_NEW_CONSOLE;
    if (launch_info.GetFlags().Test(eLaunchFlagDebug))
        flags |= DEBUG_ONLY_THIS_PROCESS;

    executable = launch_info.GetExecutableFile().GetPath();
    launch_info.GetArguments().GetQuotedCommandString(commandLine);
    BOOL result = ::CreateProcessA(executable.c_str(), const_cast<char *>(commandLine.c_str()), NULL, NULL, TRUE, flags, NULL,
                                   launch_info.GetWorkingDirectory(), &startupinfo, &pi);
    if (result)
    {
        // Do not call CloseHandle on pi.hProcess, since we want to pass that back through the HostProcess.
        ::CloseHandle(pi.hThread);
    }

    if (stdin_handle)
        ::CloseHandle(stdin_handle);
    if (stdout_handle)
        ::CloseHandle(stdout_handle);
    if (stderr_handle)
        ::CloseHandle(stderr_handle);

    if (!result)
        error.SetError(::GetLastError(), eErrorTypeWin32);
    return HostProcess(pi.hProcess);
}

HANDLE
ProcessLauncherWindows::GetStdioHandle(const ProcessLaunchInfo &launch_info, int fd)
{
    const FileAction *action = launch_info.GetFileActionForFD(fd);
    if (action == nullptr)
        return NULL;
    SECURITY_ATTRIBUTES secattr = {0};
    secattr.nLength = sizeof(SECURITY_ATTRIBUTES);
    secattr.bInheritHandle = TRUE;

    const char *path = action->GetPath();
    DWORD access = 0;
    DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD create = 0;
    DWORD flags = 0;
    if (fd == STDIN_FILENO)
    {
        access = GENERIC_READ;
        create = OPEN_EXISTING;
        flags = FILE_ATTRIBUTE_READONLY;
    }
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        access = GENERIC_WRITE;
        create = CREATE_ALWAYS;
        if (fd == STDERR_FILENO)
            flags = FILE_FLAG_WRITE_THROUGH;
    }

    HANDLE result = ::CreateFile(path, access, share, &secattr, create, flags, NULL);
    return (result == INVALID_HANDLE_VALUE) ? NULL : result;
}
