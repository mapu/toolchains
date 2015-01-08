//===-- asan_flags.cc -------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of AddressSanitizer, an address sanity checker.
//
// ASan flag parsing logic.
//===----------------------------------------------------------------------===//

#include "asan_activation.h"
#include "asan_flags.h"
#include "asan_interface_internal.h"
#include "asan_stack.h"
#include "lsan/lsan_common.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_flags.h"

namespace __asan {

Flags asan_flags_dont_use_directly;  // use via flags().

static const char *MaybeCallAsanDefaultOptions() {
  return (&__asan_default_options) ? __asan_default_options() : "";
}

static const char *MaybeUseAsanDefaultOptionsCompileDefinition() {
#ifdef ASAN_DEFAULT_OPTIONS
// Stringize the macro value.
# define ASAN_STRINGIZE(x) #x
# define ASAN_STRINGIZE_OPTIONS(options) ASAN_STRINGIZE(options)
  return ASAN_STRINGIZE_OPTIONS(ASAN_DEFAULT_OPTIONS);
#else
  return "";
#endif
}

void Flags::SetDefaults() {
#define ASAN_FLAG(Type, Name, DefaultValue, Description) Name = DefaultValue;
#include "asan_flags.inc"
#undef ASAN_FLAG
}

void Flags::ParseFromString(const char *str) {
#define ASAN_FLAG(Type, Name, DefaultValue, Description)                     \
  ParseFlag(str, &Name, #Name, Description);
#include "asan_flags.inc"
#undef ASAN_FLAG
}

void InitializeFlags(Flags *f) {
  SetCommonFlagsDefaults();
  {
    CommonFlags cf;
    cf.CopyFrom(*common_flags());
    cf.detect_leaks = CAN_SANITIZE_LEAKS;
    cf.external_symbolizer_path = GetEnv("ASAN_SYMBOLIZER_PATH");
    cf.malloc_context_size = kDefaultMallocContextSize;
    cf.intercept_tls_get_addr = true;
    OverrideCommonFlags(cf);
  }

  const int kDefaultQuarantineSizeMb = (ASAN_LOW_MEMORY) ? 1UL << 6 : 1UL << 8;
  f->SetDefaults();

  // Override from compile definition.
  const char *compile_def = MaybeUseAsanDefaultOptionsCompileDefinition();
  ParseCommonFlagsFromString(compile_def);
  f->ParseFromString(compile_def);

  // Override from user-specified string.
  const char *default_options = MaybeCallAsanDefaultOptions();
  ParseCommonFlagsFromString(default_options);
  f->ParseFromString(default_options);
  VReport(1, "Using the defaults from __asan_default_options: %s\n",
          MaybeCallAsanDefaultOptions());

  // Override from command line.
  if (const char *env = GetEnv("ASAN_OPTIONS")) {
    ParseCommonFlagsFromString(env);
    f->ParseFromString(env);
    VReport(1, "Parsed ASAN_OPTIONS: %s\n", env);
  }

  // Let activation flags override current settings. On Android they come
  // from a system property. On other platforms this is no-op.
  if (!flags()->start_deactivated) {
    char buf[100];
    GetExtraActivationFlags(buf, sizeof(buf));
    ParseCommonFlagsFromString(buf);
    f->ParseFromString(buf);
    if (buf[0] != '\0')
      VReport(1, "Parsed activation flags: %s\n", buf);
  }

  if (common_flags()->help) {
    PrintFlagDescriptions();
  }

  // Flag validation:
  if (!CAN_SANITIZE_LEAKS && common_flags()->detect_leaks) {
    Report("%s: detect_leaks is not supported on this platform.\n",
           SanitizerToolName);
    Die();
  }
  // Make "strict_init_order" imply "check_initialization_order".
  // TODO(samsonov): Use a single runtime flag for an init-order checker.
  if (f->strict_init_order) {
    f->check_initialization_order = true;
  }
  CHECK_LE((uptr)common_flags()->malloc_context_size, kStackTraceMax);
  CHECK_LE(f->min_uar_stack_size_log, f->max_uar_stack_size_log);
  CHECK_GE(f->redzone, 16);
  CHECK_GE(f->max_redzone, f->redzone);
  CHECK_LE(f->max_redzone, 2048);
  CHECK(IsPowerOfTwo(f->redzone));
  CHECK(IsPowerOfTwo(f->max_redzone));

  // quarantine_size is deprecated but we still honor it.
  // quarantine_size can not be used together with quarantine_size_mb.
  if (f->quarantine_size >= 0 && f->quarantine_size_mb >= 0) {
    Report("%s: please use either 'quarantine_size' (deprecated) or "
           "quarantine_size_mb, but not both\n", SanitizerToolName);
    Die();
  }
  if (f->quarantine_size >= 0)
    f->quarantine_size_mb = f->quarantine_size >> 20;
  if (f->quarantine_size_mb < 0)
    f->quarantine_size_mb = kDefaultQuarantineSizeMb;
}

}  // namespace __asan

#if !SANITIZER_SUPPORTS_WEAK_HOOKS
extern "C" {
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
const char* __asan_default_options() { return ""; }
}  // extern "C"
#endif
