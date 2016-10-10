#include <fcntl.h>

#include "arch/mapu/linux/linux.hh"

// open(2) flags translation table
OpenFlagTransTable MapuLinux::openFlagTable[] = {
#ifdef _MSC_VER
  { MapuLinux::TGT_O_RDONLY,    _O_RDONLY },
  { MapuLinux::TGT_O_WRONLY,    _O_WRONLY },
  { MapuLinux::TGT_O_RDWR,      _O_RDWR },
  { MapuLinux::TGT_O_APPEND,    _O_APPEND },
  { MapuLinux::TGT_O_CREAT,     _O_CREAT },
  { MapuLinux::TGT_O_TRUNC,     _O_TRUNC },
  { MapuLinux::TGT_O_EXCL,      _O_EXCL },
#ifdef _O_NONBLOCK
  { MapuLinux::TGT_O_NONBLOCK,  _O_NONBLOCK },
#endif
#ifdef _O_NOCTTY
  { MapuLinux::TGT_O_NOCTTY,    _O_NOCTTY },
#endif
#ifdef _O_SYNC
  { MapuLinux::TGT_O_SYNC,      _O_SYNC },
#endif
#else /* !_MSC_VER */
  { MapuLinux::TGT_O_RDONLY,    O_RDONLY },
  { MapuLinux::TGT_O_WRONLY,    O_WRONLY },
  { MapuLinux::TGT_O_RDWR,      O_RDWR },
  { MapuLinux::TGT_O_APPEND,    O_APPEND },
  { MapuLinux::TGT_O_CREAT,     O_CREAT },
  { MapuLinux::TGT_O_TRUNC,     O_TRUNC },
  { MapuLinux::TGT_O_EXCL,      O_EXCL },
  { MapuLinux::TGT_O_NONBLOCK,  O_NONBLOCK },
  { MapuLinux::TGT_O_NOCTTY,    O_NOCTTY },
#ifdef O_SYNC
  { MapuLinux::TGT_O_SYNC,      O_SYNC },
#endif
#endif /* _MSC_VER */
};

const int MapuLinux::NUM_OPEN_FLAGS =
        (sizeof(MapuLinux::openFlagTable)/sizeof(MapuLinux::openFlagTable[0]));
