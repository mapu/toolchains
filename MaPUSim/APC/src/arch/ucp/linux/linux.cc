#include <fcntl.h>

#include "arch/ucp/linux/linux.hh"

// open(2) flags translation table
OpenFlagTransTable UcpLinux::openFlagTable[] = {
#ifdef _MSC_VER
  { UcpLinux::TGT_O_RDONLY,    _O_RDONLY },
  { UcpLinux::TGT_O_WRONLY,    _O_WRONLY },
  { UcpLinux::TGT_O_RDWR,      _O_RDWR },
  { UcpLinux::TGT_O_APPEND,    _O_APPEND },
  { UcpLinux::TGT_O_CREAT,     _O_CREAT },
  { UcpLinux::TGT_O_TRUNC,     _O_TRUNC },
  { UcpLinux::TGT_O_EXCL,      _O_EXCL },
#ifdef _O_NONBLOCK
  { UcpLinux::TGT_O_NONBLOCK,  _O_NONBLOCK },
#endif
#ifdef _O_NOCTTY
  { UcpLinux::TGT_O_NOCTTY,    _O_NOCTTY },
#endif
#ifdef _O_SYNC
  { UcpLinux::TGT_O_SYNC,      _O_SYNC },
#endif
#else /* !_MSC_VER */
  { UcpLinux::TGT_O_RDONLY,    O_RDONLY },
  { UcpLinux::TGT_O_WRONLY,    O_WRONLY },
  { UcpLinux::TGT_O_RDWR,      O_RDWR },
  { UcpLinux::TGT_O_APPEND,    O_APPEND },
  { UcpLinux::TGT_O_CREAT,     O_CREAT },
  { UcpLinux::TGT_O_TRUNC,     O_TRUNC },
  { UcpLinux::TGT_O_EXCL,      O_EXCL },
  { UcpLinux::TGT_O_NONBLOCK,  O_NONBLOCK },
  { UcpLinux::TGT_O_NOCTTY,    O_NOCTTY },
#ifdef O_SYNC
  { UcpLinux::TGT_O_SYNC,      O_SYNC },
#endif
#endif /* _MSC_VER */
};

const int UcpLinux::NUM_OPEN_FLAGS =
        (sizeof(UcpLinux::openFlagTable)/sizeof(UcpLinux::openFlagTable[0]));
