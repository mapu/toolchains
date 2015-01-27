//===-- ScriptInterpreterPython.h -------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


#ifndef liblldb_ScriptInterpreterPython_h_
#define liblldb_ScriptInterpreterPython_h_

#ifdef LLDB_DISABLE_PYTHON

// Python is disabled in this build

#else

#include "lldb/lldb-python.h"
#include "lldb/lldb-private.h"
#include "lldb/Core/IOHandler.h"
#include "lldb/Interpreter/ScriptInterpreter.h"
#include "lldb/Interpreter/PythonDataObjects.h"
#include "lldb/Host/Terminal.h"

class IOHandlerPythonInterpreter;

namespace lldb_private {
    
class ScriptInterpreterPython :
    public ScriptInterpreter,
    public IOHandlerDelegateMultiline
{
public:

    friend class IOHandlerPythonInterpreter;

    ScriptInterpreterPython (CommandInterpreter &interpreter);

    ~ScriptInterpreterPython ();

    bool
    ExecuteOneLine (const char *command,
                    CommandReturnObject *result,
                    const ExecuteScriptOptions &options = ExecuteScriptOptions());

    void
    ExecuteInterpreterLoop ();

    bool
    ExecuteOneLineWithReturn (const char *in_string, 
                              ScriptInterpreter::ScriptReturnType return_type,
                              void *ret_value,
                              const ExecuteScriptOptions &options = ExecuteScriptOptions());

    lldb_private::Error
    ExecuteMultipleLines (const char *in_string,
                          const ExecuteScriptOptions &options = ExecuteScriptOptions());

    Error
    ExportFunctionDefinitionToInterpreter (StringList &function_def);

    bool
    GenerateTypeScriptFunction (StringList &input, std::string& output, void* name_token = NULL);
    
    bool
    GenerateTypeSynthClass (StringList &input, std::string& output, void* name_token = NULL);
    
    bool
    GenerateTypeSynthClass (const char* oneliner, std::string& output, void* name_token = NULL);
    
    // use this if the function code is just a one-liner script
    bool
    GenerateTypeScriptFunction (const char* oneliner, std::string& output, void* name_token = NULL);
    
    virtual bool
    GenerateScriptAliasFunction (StringList &input, std::string& output);
    
    lldb::ScriptInterpreterObjectSP
    CreateSyntheticScriptedProvider (const char *class_name,
                                     lldb::ValueObjectSP valobj);

    lldb::ScriptInterpreterObjectSP
    virtual CreateScriptedThreadPlan (const char *class_name,
                                      lldb::ThreadPlanSP thread_plan);

    virtual bool
    ScriptedThreadPlanExplainsStop (lldb::ScriptInterpreterObjectSP implementor_sp,
                                    Event *event,
                                    bool &script_error);
    virtual bool
    ScriptedThreadPlanShouldStop (lldb::ScriptInterpreterObjectSP implementor_sp,
                                  Event *event,
                                  bool &script_error);
    virtual lldb::StateType
    ScriptedThreadPlanGetRunState (lldb::ScriptInterpreterObjectSP implementor_sp,
                                   bool &script_error);
    
    virtual lldb::ScriptInterpreterObjectSP
    OSPlugin_CreatePluginObject (const char *class_name,
                                 lldb::ProcessSP process_sp);
    
    virtual lldb::ScriptInterpreterObjectSP
    OSPlugin_RegisterInfo (lldb::ScriptInterpreterObjectSP os_plugin_object_sp);
    
    virtual lldb::ScriptInterpreterObjectSP
    OSPlugin_ThreadsInfo (lldb::ScriptInterpreterObjectSP os_plugin_object_sp);
    
    virtual lldb::ScriptInterpreterObjectSP
    OSPlugin_RegisterContextData (lldb::ScriptInterpreterObjectSP os_plugin_object_sp,
                                  lldb::tid_t thread_id);
    
    virtual lldb::ScriptInterpreterObjectSP
    OSPlugin_CreateThread (lldb::ScriptInterpreterObjectSP os_plugin_object_sp,
                           lldb::tid_t tid,
                           lldb::addr_t context);
    
    virtual lldb::ScriptInterpreterObjectSP
    LoadPluginModule (const FileSpec& file_spec,
                      lldb_private::Error& error);
    
    virtual lldb::ScriptInterpreterObjectSP
    GetDynamicSettings (lldb::ScriptInterpreterObjectSP plugin_module_sp,
                        Target* target,
                        const char* setting_name,
                        lldb_private::Error& error);
    
    virtual size_t
    CalculateNumChildren (const lldb::ScriptInterpreterObjectSP& implementor);
    
    virtual lldb::ValueObjectSP
    GetChildAtIndex (const lldb::ScriptInterpreterObjectSP& implementor, uint32_t idx);
    
    virtual int
    GetIndexOfChildWithName (const lldb::ScriptInterpreterObjectSP& implementor, const char* child_name);
    
    virtual bool
    UpdateSynthProviderInstance (const lldb::ScriptInterpreterObjectSP& implementor);
    
    virtual bool
    MightHaveChildrenSynthProviderInstance (const lldb::ScriptInterpreterObjectSP& implementor);
    
    virtual lldb::ValueObjectSP
    GetSyntheticValue (const lldb::ScriptInterpreterObjectSP& implementor);
    
    virtual bool
    RunScriptBasedCommand(const char* impl_function,
                          const char* args,
                          ScriptedCommandSynchronicity synchronicity,
                          lldb_private::CommandReturnObject& cmd_retobj,
                          Error& error,
                          const lldb_private::ExecutionContext& exe_ctx);
    
    Error
    GenerateFunction(const char *signature, const StringList &input);
    
    Error
    GenerateBreakpointCommandCallbackData (StringList &input, std::string& output);

    bool
    GenerateWatchpointCommandCallbackData (StringList &input, std::string& output);

//    static size_t
//    GenerateBreakpointOptionsCommandCallback (void *baton, 
//                                              InputReader &reader, 
//                                              lldb::InputReaderAction notification,
//                                              const char *bytes, 
//                                              size_t bytes_len);
//    
//    static size_t
//    GenerateWatchpointOptionsCommandCallback (void *baton, 
//                                              InputReader &reader, 
//                                              lldb::InputReaderAction notification,
//                                              const char *bytes, 
//                                              size_t bytes_len);
    
    static bool
    BreakpointCallbackFunction (void *baton, 
                                StoppointCallbackContext *context, 
                                lldb::user_id_t break_id,
                                lldb::user_id_t break_loc_id);
    
    static bool
    WatchpointCallbackFunction (void *baton, 
                                StoppointCallbackContext *context, 
                                lldb::user_id_t watch_id);
    
    virtual bool
    GetScriptedSummary (const char *function_name,
                        lldb::ValueObjectSP valobj,
                        lldb::ScriptInterpreterObjectSP& callee_wrapper_sp,
                        const TypeSummaryOptions& options,
                        std::string& retval);
    
    virtual void
    Clear ();

    virtual bool
    GetDocumentationForItem (const char* item, std::string& dest);
    
    virtual bool
    CheckObjectExists (const char* name)
    {
        if (!name || !name[0])
            return false;
        std::string temp;
        return GetDocumentationForItem (name,temp);
    }
    
    virtual bool
    RunScriptFormatKeyword (const char* impl_function,
                            Process* process,
                            std::string& output,
                            Error& error);

    virtual bool
    RunScriptFormatKeyword (const char* impl_function,
                            Thread* thread,
                            std::string& output,
                            Error& error);
    
    virtual bool
    RunScriptFormatKeyword (const char* impl_function,
                            Target* target,
                            std::string& output,
                            Error& error);
    
    virtual bool
    RunScriptFormatKeyword (const char* impl_function,
                            StackFrame* frame,
                            std::string& output,
                            Error& error);
    
    virtual bool
    RunScriptFormatKeyword (const char* impl_function,
                            ValueObject* value,
                            std::string& output,
                            Error& error);
    
    virtual bool
    LoadScriptingModule (const char* filename,
                         bool can_reload,
                         bool init_session,
                         lldb_private::Error& error,
                         lldb::ScriptInterpreterObjectSP* module_sp = nullptr);
    
    virtual lldb::ScriptInterpreterObjectSP
    MakeScriptObject (void* object);
    
    virtual std::unique_ptr<ScriptInterpreterLocker>
    AcquireInterpreterLock ();
    
    void
    CollectDataForBreakpointCommandCallback (std::vector<BreakpointOptions *> &bp_options_vec,
                                             CommandReturnObject &result);

    void 
    CollectDataForWatchpointCommandCallback (WatchpointOptions *wp_options,
                                             CommandReturnObject &result);

    /// Set the callback body text into the callback for the breakpoint.
    Error
    SetBreakpointCommandCallback (BreakpointOptions *bp_options,
                                  const char *callback_body);

    void 
    SetBreakpointCommandCallbackFunction (BreakpointOptions *bp_options,
                                          const char *function_name);

    /// Set a one-liner as the callback for the watchpoint.
    void 
    SetWatchpointCommandCallback (WatchpointOptions *wp_options,
                                  const char *oneliner);

    StringList
    ReadCommandInputFromUser (FILE *in_file);
    
    virtual void
    ResetOutputFileHandle (FILE *new_fh);
    
    static void
    InitializePrivate ();

    static void
    InitializeInterpreter (SWIGInitCallback python_swig_init_callback,
                           SWIGBreakpointCallbackFunction swig_breakpoint_callback,
                           SWIGWatchpointCallbackFunction swig_watchpoint_callback,
                           SWIGPythonTypeScriptCallbackFunction swig_typescript_callback,
                           SWIGPythonCreateSyntheticProvider swig_synthetic_script,
                           SWIGPythonCalculateNumChildren swig_calc_children,
                           SWIGPythonGetChildAtIndex swig_get_child_index,
                           SWIGPythonGetIndexOfChildWithName swig_get_index_child,
                           SWIGPythonCastPyObjectToSBValue swig_cast_to_sbvalue ,
                           SWIGPythonGetValueObjectSPFromSBValue swig_get_valobj_sp_from_sbvalue,
                           SWIGPythonUpdateSynthProviderInstance swig_update_provider,
                           SWIGPythonMightHaveChildrenSynthProviderInstance swig_mighthavechildren_provider,
                           SWIGPythonGetValueSynthProviderInstance swig_getvalue_provider,
                           SWIGPythonCallCommand swig_call_command,
                           SWIGPythonCallModuleInit swig_call_module_init,
                           SWIGPythonCreateOSPlugin swig_create_os_plugin,
                           SWIGPythonScriptKeyword_Process swig_run_script_keyword_process,
                           SWIGPythonScriptKeyword_Thread swig_run_script_keyword_thread,
                           SWIGPythonScriptKeyword_Target swig_run_script_keyword_target,
                           SWIGPythonScriptKeyword_Frame swig_run_script_keyword_frame,
                           SWIGPythonScriptKeyword_Value swig_run_script_keyword_value,
                           SWIGPython_GetDynamicSetting swig_plugin_get,
                           SWIGPythonCreateScriptedThreadPlan swig_thread_plan_script,
                           SWIGPythonCallThreadPlan swig_call_thread_plan);

    const char *
    GetDictionaryName ()
    {
        return m_dictionary_name.c_str();
    }

    
    PyThreadState *
    GetThreadState()
    {
        return m_command_thread_state;
    }

    void
    SetThreadState (PyThreadState *s)
    {
        if (s)
            m_command_thread_state = s;
    }

    //----------------------------------------------------------------------
    // IOHandlerDelegate
    //----------------------------------------------------------------------
    virtual void
    IOHandlerActivated (IOHandler &io_handler);

    virtual void
    IOHandlerInputComplete (IOHandler &io_handler, std::string &data);

protected:

    bool
    EnterSession (uint16_t on_entry_flags,
                  FILE *in,
                  FILE *out,
                  FILE *err);
    
    void
    LeaveSession ();
    
    void
    SaveTerminalState (int fd);

    void
    RestoreTerminalState ();
    
    class SynchronicityHandler
    {
    private:
        lldb::DebuggerSP             m_debugger_sp;
        ScriptedCommandSynchronicity m_synch_wanted;
        bool                         m_old_asynch;
    public:
        SynchronicityHandler(lldb::DebuggerSP,
                             ScriptedCommandSynchronicity);
        ~SynchronicityHandler();
    };
    
    class ScriptInterpreterPythonObject : public ScriptInterpreterObject
    {
    public:
        ScriptInterpreterPythonObject() :
        ScriptInterpreterObject()
        {}
        
        ScriptInterpreterPythonObject(void* obj) :
        ScriptInterpreterObject(obj)
        {
            Py_XINCREF(m_object);
        }
        
        explicit operator bool ()
        {
            return m_object && m_object != Py_None;
        }
        
        
        virtual
        ~ScriptInterpreterPythonObject()
        {
            if (Py_IsInitialized())
                Py_XDECREF(m_object);
            m_object = NULL;
        }
        private:
            DISALLOW_COPY_AND_ASSIGN (ScriptInterpreterPythonObject);
    };
public:
	class Locker : public ScriptInterpreterLocker
	{
	public:
        
        enum OnEntry
        {
            AcquireLock         = 0x0001,
            InitSession         = 0x0002,
            InitGlobals         = 0x0004,
            NoSTDIN             = 0x0008
        };
        
        enum OnLeave
        {
            FreeLock            = 0x0001,
            FreeAcquiredLock    = 0x0002,    // do not free the lock if we already held it when calling constructor
            TearDownSession     = 0x0004
        };
        
        Locker (ScriptInterpreterPython *py_interpreter = NULL,
                uint16_t on_entry = AcquireLock | InitSession,
                uint16_t on_leave = FreeLock | TearDownSession,
                FILE *in = NULL,
                FILE *out = NULL,
                FILE *err = NULL);
        
    	~Locker ();

	private:
        
        bool
        DoAcquireLock ();
        
        bool
        DoInitSession (uint16_t on_entry_flags, FILE *in, FILE *out, FILE *err);
        
        bool
        DoFreeLock ();
        
        bool
        DoTearDownSession ();

        static void
        ReleasePythonLock ();
        
    	bool                     m_teardown_session;
    	ScriptInterpreterPython *m_python_interpreter;
//    	FILE*                    m_tmp_fh;
        PyGILState_STATE         m_GILState;
	};
protected:

    enum ActiveIOHandler {
        eIOHandlerNone,
        eIOHandlerBreakpoint,
        eIOHandlerWatchpoint
    };
    PythonObject &
    GetMainModule ();
    
    PythonDictionary &
    GetSessionDictionary ();
    
    PythonDictionary &
    GetSysModuleDictionary ();

    bool
    GetEmbeddedInterpreterModuleObjects ();
    
    PythonObject m_saved_stdin;
    PythonObject m_saved_stdout;
    PythonObject m_saved_stderr;
    PythonObject m_main_module;
    PythonObject m_lldb_module;
    PythonDictionary m_session_dict;
    PythonDictionary m_sys_module_dict;
    PythonObject m_run_one_line_function;
    PythonObject m_run_one_line_str_global;
    std::string m_dictionary_name;
    TerminalState m_terminal_state;
    ActiveIOHandler m_active_io_handler;
    bool m_session_is_active;
    bool m_pty_slave_is_open;
    bool m_valid_session;
    PyThreadState *m_command_thread_state;
};
} // namespace lldb_private

#endif // #ifdef LLDB_DISABLE_PYTHON

#endif // #ifndef liblldb_ScriptInterpreterPython_h_
