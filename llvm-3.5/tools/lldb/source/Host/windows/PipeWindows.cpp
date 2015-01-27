//===-- PipeWindows.cpp -----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lldb/Host/windows/PipeWindows.h"

#include "llvm/Support/raw_ostream.h"

#include <fcntl.h>
#include <io.h>

#include <atomic>
#include <string>

using namespace lldb;
using namespace lldb_private;

namespace
{
std::atomic<uint32_t> g_pipe_serial(0);
}

PipeWindows::PipeWindows()
{
    m_read = INVALID_HANDLE_VALUE;
    m_write = INVALID_HANDLE_VALUE;

    m_read_fd = -1;
    m_write_fd = -1;
    ZeroMemory(&m_read_overlapped, sizeof(m_read_overlapped));
    ZeroMemory(&m_write_overlapped, sizeof(m_write_overlapped));
}

PipeWindows::~PipeWindows()
{
    Close();
}

Error
PipeWindows::CreateNew(bool child_process_inherit)
{
    // Even for anonymous pipes, we open a named pipe.  This is because you cannot get
    // overlapped i/o on Windows without using a named pipe.  So we synthesize a unique
    // name.
    uint32_t serial = g_pipe_serial.fetch_add(1);
    std::string pipe_name;
    llvm::raw_string_ostream pipe_name_stream(pipe_name);
    pipe_name_stream << "lldb.pipe." << ::GetCurrentProcessId() << "." << serial;
    pipe_name_stream.flush();

    return CreateNew(pipe_name.c_str(), child_process_inherit);
}

Error
PipeWindows::CreateNew(llvm::StringRef name, bool child_process_inherit)
{
    if (name.empty())
        return Error(ERROR_INVALID_PARAMETER, eErrorTypeWin32);

    if (CanRead() || CanWrite())
        return Error(ERROR_ALREADY_EXISTS, eErrorTypeWin32);

    std::string pipe_path = "\\\\.\\Pipe\\";
    pipe_path.append(name);

    // Always open for overlapped i/o.  We implement blocking manually in Read and Write.
    DWORD read_mode = FILE_FLAG_OVERLAPPED;
    m_read =
        ::CreateNamedPipe(pipe_path.c_str(), PIPE_ACCESS_INBOUND | read_mode, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 1024, 1024, 120 * 1000, NULL);
    if (INVALID_HANDLE_VALUE == m_read)
        return Error(::GetLastError(), eErrorTypeWin32);
    m_read_fd = _open_osfhandle((intptr_t)m_read, _O_RDONLY);
    ZeroMemory(&m_read_overlapped, sizeof(m_read_overlapped));
    m_read_overlapped.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);

    // Open the write end of the pipe.
    Error result = OpenNamedPipe(name, child_process_inherit, false);
    if (!result.Success())
    {
        CloseReadEndpoint();
        return result;
    }

    return result;
}

Error
PipeWindows::OpenAsReader(llvm::StringRef name, bool child_process_inherit)
{
    if (CanRead() || CanWrite())
        return Error(ERROR_ALREADY_EXISTS, eErrorTypeWin32);

    return OpenNamedPipe(name, child_process_inherit, true);
}

Error
PipeWindows::OpenAsWriter(llvm::StringRef name, bool child_process_inherit)
{
    if (CanRead() || CanWrite())
        return Error(ERROR_ALREADY_EXISTS, eErrorTypeWin32);

    return OpenNamedPipe(name, child_process_inherit, false);
}

Error
PipeWindows::OpenNamedPipe(llvm::StringRef name, bool child_process_inherit, bool is_read)
{
    if (name.empty())
        return Error(ERROR_INVALID_PARAMETER, eErrorTypeWin32);

    assert(is_read ? !CanRead() : !CanWrite());

    SECURITY_ATTRIBUTES attributes = {0};
    attributes.bInheritHandle = child_process_inherit;

    std::string pipe_path = "\\\\.\\Pipe\\";
    pipe_path.append(name);

    if (is_read)
    {
        m_read = ::CreateFile(pipe_path.c_str(), GENERIC_READ, 0, &attributes, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (INVALID_HANDLE_VALUE == m_read)
            return Error(::GetLastError(), eErrorTypeWin32);

        m_read_fd = _open_osfhandle((intptr_t)m_read, _O_RDONLY);

        ZeroMemory(&m_read_overlapped, sizeof(m_read_overlapped));
        m_read_overlapped.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
    }
    else
    {
        m_write = ::CreateFile(pipe_path.c_str(), GENERIC_WRITE, 0, &attributes, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (INVALID_HANDLE_VALUE == m_write)
            return Error(::GetLastError(), eErrorTypeWin32);

        m_write_fd = _open_osfhandle((intptr_t)m_write, _O_WRONLY);

        ZeroMemory(&m_write_overlapped, sizeof(m_write_overlapped));
    }

    return Error();
}

int
PipeWindows::GetReadFileDescriptor() const
{
    return m_read_fd;
}

int
PipeWindows::GetWriteFileDescriptor() const
{
    return m_write_fd;
}

int
PipeWindows::ReleaseReadFileDescriptor()
{
    if (!CanRead())
        return -1;
    int result = m_read_fd;
    m_read_fd = -1;
    if (m_read_overlapped.hEvent)
        ::CloseHandle(m_read_overlapped.hEvent);
    m_read = INVALID_HANDLE_VALUE;
    ZeroMemory(&m_read_overlapped, sizeof(m_read_overlapped));
    return result;
}

int
PipeWindows::ReleaseWriteFileDescriptor()
{
    if (!CanWrite())
        return -1;
    int result = m_write_fd;
    m_write_fd = -1;
    m_write = INVALID_HANDLE_VALUE;
    ZeroMemory(&m_write_overlapped, sizeof(m_write_overlapped));
    return result;
}

void
PipeWindows::CloseReadEndpoint()
{
    if (!CanRead())
        return;

    if (m_read_overlapped.hEvent)
        ::CloseHandle(m_read_overlapped.hEvent);
    _close(m_read_fd);
    m_read = INVALID_HANDLE_VALUE;
    m_read_fd = -1;
    ZeroMemory(&m_read_overlapped, sizeof(m_read_overlapped));
}

void
PipeWindows::CloseWriteEndpoint()
{
    if (!CanWrite())
        return;

    _close(m_write_fd);
    m_write = INVALID_HANDLE_VALUE;
    m_write_fd = -1;
    ZeroMemory(&m_write_overlapped, sizeof(m_write_overlapped));
}

void
PipeWindows::Close()
{
    CloseReadEndpoint();
    CloseWriteEndpoint();
}

bool
PipeWindows::CanRead() const
{
    return (m_read != INVALID_HANDLE_VALUE);
}

bool
PipeWindows::CanWrite() const
{
    return (m_write != INVALID_HANDLE_VALUE);
}

HANDLE
PipeWindows::GetReadNativeHandle()
{
    return m_read;
}

HANDLE
PipeWindows::GetWriteNativeHandle()
{
    return m_write;
}

Error
PipeWindows::Read(void *buf, size_t size, size_t &bytes_read)
{
    return ReadWithTimeout(buf, size, std::chrono::milliseconds::zero(), bytes_read);
}

Error
PipeWindows::ReadWithTimeout(void *buf, size_t size, const std::chrono::milliseconds &duration, size_t &bytes_read)
{
    if (!CanRead())
        return Error(ERROR_INVALID_HANDLE, eErrorTypeWin32);

    bytes_read = 0;
    DWORD sys_bytes_read = size;
    BOOL result = ::ReadFile(m_read, buf, sys_bytes_read, &sys_bytes_read, &m_read_overlapped);
    if (!result && GetLastError() != ERROR_IO_PENDING)
        return Error(::GetLastError(), eErrorTypeWin32);

    DWORD timeout = (duration == std::chrono::milliseconds::zero()) ? INFINITE : duration.count();
    DWORD wait_result = ::WaitForSingleObject(m_read_overlapped.hEvent, timeout);
    if (wait_result != WAIT_OBJECT_0)
    {
        // The operation probably failed.  However, if it timed out, we need to cancel the I/O.
        // Between the time we returned from WaitForSingleObject and the time we call CancelIoEx,
        // the operation may complete.  If that hapens, CancelIoEx will fail and return ERROR_NOT_FOUND.
        // If that happens, the original operation should be considered to have been successful.
        bool failed = true;
        DWORD failure_error = ::GetLastError();
        if (wait_result == WAIT_TIMEOUT)
        {
            BOOL cancel_result = CancelIoEx(m_read, &m_read_overlapped);
            if (!cancel_result && GetLastError() == ERROR_NOT_FOUND)
                failed = false;
        }
        if (failed)
            return Error(failure_error, eErrorTypeWin32);
    }

    // Now we call GetOverlappedResult setting bWait to false, since we've already waited
    // as long as we're willing to.
    if (!GetOverlappedResult(m_read, &m_read_overlapped, &sys_bytes_read, FALSE))
        return Error(::GetLastError(), eErrorTypeWin32);

    bytes_read = sys_bytes_read;
    return Error();
}

Error
PipeWindows::Write(const void *buf, size_t num_bytes, size_t &bytes_written)
{
    if (!CanWrite())
        return Error(ERROR_INVALID_HANDLE, eErrorTypeWin32);

    DWORD sys_bytes_written = 0;
    BOOL write_result = ::WriteFile(m_write, buf, num_bytes, &sys_bytes_written, &m_write_overlapped);
    if (!write_result && GetLastError() != ERROR_IO_PENDING)
        return Error(::GetLastError(), eErrorTypeWin32);

    BOOL result = GetOverlappedResult(m_write, &m_write_overlapped, &sys_bytes_written, TRUE);
    if (!result)
        return Error(::GetLastError(), eErrorTypeWin32);
    return Error();
}
