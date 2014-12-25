/*
 * norflash.cc
 *
 *  Created on: Apr 15, 2014
 *      Author: wangl
 */

#include <sys/mman.h>
#include "debug/Flash.hh"
#include "norflash.hh"

using namespace std;

const NorFlash::FlashType NorFlash::flash_types[] = {
  { "S29GL128GP11TFI010", 16 * 1024 * 1024, 128 * 1024,
    { 'Q',  'R',  'Y', 0x02, 0x00, 0x40, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x27, 0x36, 0x00, 0x00, 0x06,
     0x06, 0x09, 0x13, 0x03, 0x05, 0x03, 0x02, 0x18,
     0x02, 0x00, 0x06, 0x00, 0x01, 0x7f, 0x00, 0x00,
     0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      'P',  'R',  'I', 0x31, 0x33,
     0x14, 0x02, 0x01, 0x00, 0x08, 0x00, 0x00, 0x02,
     0xb5, 0xc5, 0x05, 0x01 },
    {{ 0x00, 0x4f, 0x0001}, /* Manufacturer ID */
     { 0x01, 0x4f, 0x227e}, /* Device ID */
     { 0x0e, 0x4f, 0x2221}, /* Ex Device ID */
     { 0x0f, 0x4f, 0x2201}, /* Ex2 Device ID */
     { 0x02, 0x4f, 0x0000}, /* Sector group Prot. verification */
     { 0x03, 0x4f, 0x0019}, /* SecSi highest not locked */
    },
    CAP_CFI,
  },
  { "S29GL256GP11TFI010", 32 * 1024 * 1024, 128 * 1024,
    { 'Q',  'R',  'Y', 0x02, 0x00, 0x40, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x27, 0x36, 0x00, 0x00, 0x06,
     0x06, 0x09, 0x13, 0x03, 0x05, 0x03, 0x02, 0x19,
     0x02, 0x00, 0x06, 0x00, 0x01, 0xff, 0x00, 0x00,
     0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      'P',  'R',  'I', 0x31, 0x33,
     0x14, 0x02, 0x01, 0x00, 0x08, 0x00, 0x00, 0x02,
     0xb5, 0xc5, 0x05, 0x01 },
    {{ 0x00, 0x4f, 0x0001}, /* Manufacturer ID */
     { 0x01, 0x4f, 0x227e}, /* Device ID */
     { 0x0e, 0x4f, 0x2222}, /* Ex Device ID */
     { 0x0f, 0x4f, 0x2201}, /* Ex2 Device ID */
     { 0x02, 0x4f, 0x0000}, /* Sector group Prot. verification */
     { 0x03, 0x4f, 0x0019}, /* SecSi highest not locked */
    },
    CAP_CFI | CAP_TOPBOOT,
  },
  { "S29GL512GP11TFI010", 64 * 1024 * 1024, 128 * 1024,
    { 'Q',  'R',  'Y', 0x02, 0x00, 0x40, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x27, 0x36, 0x00, 0x00, 0x06,
     0x06, 0x09, 0x13, 0x03, 0x05, 0x03, 0x02, 0x1a,
     0x02, 0x00, 0x06, 0x00, 0x01, 0xff, 0x01, 0x00,
     0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      'P',  'R',  'I', 0x31, 0x33,
     0x14, 0x02, 0x01, 0x00, 0x08, 0x00, 0x00, 0x02,
     0xb5, 0xc5, 0x05, 0x01 },
    {{ 0x00, 0x4f, 0x0001}, /* Manufacturer ID */
     { 0x01, 0x4f, 0x227e}, /* Device ID */
     { 0x0e, 0x4f, 0x2223}, /* Ex Device ID */
     { 0x0f, 0x4f, 0x2201}, /* Ex2 Device ID */
     { 0x02, 0x4f, 0x0000}, /* Sector group Prot. verification */
     { 0x03, 0x4f, 0x0019}, /* SecSi highest not locked */
    },
    CAP_CFI | CAP_TOPBOOT
  },
  { "S29GL01GP11TFI010", 128 * 1024 * 1024, 128 * 1024,
    { 'Q',  'R',  'Y', 0x02, 0x00, 0x40, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x27, 0x36, 0x00, 0x00, 0x06,
     0x06, 0x09, 0x13, 0x03, 0x05, 0x03, 0x02, 0x1b,
     0x02, 0x00, 0x06, 0x00, 0x01, 0xff, 0x03, 0x00,
     0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     'P','R','I', 0x31,0x33,0x14,2,1,0,8,0,0,2,0xb5,0xc5,5
     ,1
    },
    {{ 0x00,0x4f,0x0001}, /* Manufacturer ID */
     { 0x01,0x4f,0x227e}, /* Device ID */
     { 0x0e,0x4f,0x2228}, /* Ex Device ID */
     { 0x0f,0x4f,0x2201}, /* Ex2 Device ID */
     { 0x02,0x4f,0x0000}, /* Sector group Prot. verification */
     { 0x03,0x4f,0x0019}, /* SecSi highest not locked */
    },
    CAP_CFI | CAP_TOPBOOT
  },
};

NorFlash::NorFlash(const Params *p) :
    AmbaPioDevice(p, 0), host_mem(NULL), nr_chips(p->nr_chips), bankwidth(16),
    addr_shift(0), size(0) {
  int i;
  int nr_types = sizeof(flash_types) / sizeof(FlashType);
  const FlashType *ftype = NULL;

  for (i=0; i < nr_types; i++) {
    ftype = &flash_types[i];
    if (!strcmp(ftype->name, p->flash_type.c_str())) break;
  }
  if ((i == nr_types) || !ftype->name) {
    ccprintf(cerr, "Flash type %s not found. Available types:\n", p->flash_type);
    for (i = 0; i < nr_types; i++) {
      ftype= &flash_types[i];
      ccprintf(cerr," %-20s - size %dk\n", ftype->name, ftype->size / 1024);
    }
    panic("Flash initialization failed.");
  }
  if ((nr_chips > 2) || (nr_chips < 1)) {
    ccprintf(cerr, "Illegal number of chips %d in flashbank %s\n",
             p->nr_chips, p->flash_type);
    panic("A AMD-Flash Bank must have 1 or 2 chips\n");
  }
  if (nr_chips == 2) {
    addr_shift = 1;
    bankwidth = 32;
  } else if (nr_chips == 1) {
    addr_shift = 0;
    bankwidth = 16;
  }
  size = ftype->size * nr_chips;
  pioSize = size;
  host_mem = (uint8_t*) mmap(NULL, size, PROT_READ | PROT_WRITE,
                             MAP_ANON | MAP_PRIVATE, -1, 0);;
  memset(host_mem, 0xff, size);
  /* Now add the chips */
  for(i=0; i<nr_chips; i++) {
    flash[i] = AMD_Flash_New(ftype, i);
  }
  DPRINTF(Flash, "Flash bank \"%s\" type %s Chips %d writebuf %d\n",
          p->flash_type, ftype->name, nr_chips, 1 << cfi_read(ftype, 0x2a));
}

NorFlash::~NorFlash() {
  if (host_mem) {
    munmap(host_mem, size);
  }
  for (unsigned i = 0; i < nr_chips; i++) {
    free(flash[i]->rmap);
    free(flash[i]);
  }
}

inline uint16_t NorFlash::cfi_read(const FlashType *ftype, int reg) {
  if(reg >= 0x10 && reg <= 0x5b) {
    return ftype->querystr[reg-0x10];
  } else {
    return 0;
  }
}

inline int NorFlash::get_sectorsize(uint8_t idx, uint32_t byte_addr) {
  EraseRegion *region;
  int index = ((byte_addr) & flash[idx]->rmap_mask) >> flash[idx]->rmap_shift;
  region = flash[idx]->rmap[index];
  return region->sectorsize;
}

inline int NorFlash::address_to_sa(uint8_t idx, uint32_t byte_addr) {
  return byte_addr & ~(get_sectorsize(idx, byte_addr) - 1);
}

void NorFlash::switch_to_readarray(uint8_t idx) {
  if (flash[idx]->mode != MAP_READ_ARRAY) {
    flash[idx]->mode = MAP_READ_ARRAY;
  }
}

void NorFlash::switch_to_iomode(uint8_t idx) {
  if (flash[idx]->mode != MAP_IO) {
    flash[idx]->mode = MAP_IO;
  }
}

void NorFlash::switch_to_cfimode(uint8_t idx) {
  if (flash[idx]->mode!=MAP_CFI) {
    flash[idx]->mode = MAP_CFI;
  }
}

/*
 * ------------------------------------------------------------------------------
 * The Big State machine
 *  Action of flash is dependent on the number of the Cycle (from the command
 *  definition Table in the AMD Manual) and the Command possibly recognized
 *  in an earlier Cycle.
 *  the variables flash->cycle and flash->cmd are used to store the state
 * -------------------------------------------------------------------------------
 */
void NorFlash::first_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr) {
  uint32_t reg = FLASH_REG(dev_addr);
//  fprintf(stderr,"%s: value %08x, register %08x\n",flash->type->name,value,reg);
  if (flash[idx]->cmd != CMD_NONE) {
    warn("CMD not NONE in first cycle\n");
    return;
  }
  if ((reg == 0x555) && (value == 0xaa)) {
    flash[idx]->cycle++;
    return;
  }
  /* Check if this shouldn't work in all cycles ????? */
  if ((value == 0xf0) || (value == 0xff)) {
    // reset
    switch_to_readarray(idx);
    return;
  }
  if ((value==0x98) && ((reg==0x55) | (reg == 0x555))) {
    if(flash[idx]->type->capabilities & CAP_CFI) {
      switch_to_cfimode(idx);
    }
    return;
  }
  if ((value == 0xa0)) {
    switch (flash[idx]->en_cmdset) {
    case ENCMDSET_NONE:
      break;

    case ENCMDSET_BYPASS:
      flash[idx]->cycle++;
      flash[idx]->cmd = CMD_PROGRAM;
      break;

    case ENCMDSET_LOCK_REG:
      flash[idx]->cycle++;
      flash[idx]->cmd = CMD_LOCKBITS_PGM;
      break;

    case ENCMDSET_PW_PROT:
      flash[idx]->cycle++;
      flash[idx]->cmd = CMD_PW_PGM;
      break;

    case ENCMDSET_PPB:
      flash[idx]->cycle++;
      flash[idx]->cmd = CMD_PPB_PGM;
      break;

    case ENCMDSET_PPB_LOCK:
      flash[idx]->cycle++;
      flash[idx]->cmd = CMD_PPB_LOCKBIT_SET;
      break;

    case ENCMDSET_DYB:
      flash[idx]->cycle++;
      flash[idx]->cmd = CMD_PPB_LOCKBIT_SET;
      break;

    default:
      warn("AMDFLASH: illegal cmdset selection");
    }
  }
  if ((value == 0x90) &&  (flash[idx]->en_cmdset==ENCMDSET_BYPASS)) {
    flash[idx]->cycle++;
    flash[idx]->cmd = CMD_EXIT_CMDSET;
    return;
  }
  if ((value == 0xb0)) {
    static int maxmsgs = 5;
    // ignore suspend resume
    if (maxmsgs > 0) {
      warn("AMD_Flash: Got suspend, but not busy !");
      maxmsgs--;
    }
    return;
  }
  if((value == 0x30)) {
    static int maxmsgs = 5;
    // ignore suspend resume
    if (maxmsgs > 0) {
      warn("AMD_Flash: Ignore resume because not suspended");
      maxmsgs--;
    }
    return;
  }
  static int flag = 0;
  if (!flag) {
    flag = 1;
    warn("%s: Unknown first cycle (%d) value %08x, register %08x",
         flash[idx]->type->name, curTick(), value, reg);
  }
  return;
}

/*
 * -------------------------------------------
 * Write a 16 Bit word to the flash
 * Bits can only be cleared.
 * -------------------------------------------
 */
inline void NorFlash::programm_word(uint8_t idx, uint16_t value, uint32_t dev_addr) {
  uint16_t *dst = (uint16_t*)(flash[idx]->host_mem +
                              FLASH_ADDR_TO_MEMBUS(dev_addr & ~1UL));
  *dst = value & *dst;
}

void NorFlash::second_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr) {
  uint32_t reg = FLASH_REG(dev_addr);
  if (flash[idx]->cmd == CMD_NONE) {
    if ((reg==0x2aa) && (value==0x55)) {
      flash[idx]->cycle++;
      return;
    } else if (((reg & 0xff) ==  0) && (value == 0x70)) {
      /* ignore sector unlocks */
      flash[idx]->cycle = CYCLE_FIRST;
      flash[idx]->cmd=CMD_NONE;
      return;
    }
    warn("Unknown: value %08x to reg %08x in second cycle", value, reg);
    return;
  } else if (flash[idx]->cmd==CMD_PROGRAM) {
    programm_word(idx, value, dev_addr);
    flash[idx]->cycle=CYCLE_FIRST;
    flash[idx]->cmd=CMD_NONE;
    return;
  } else if (flash[idx]->cmd == CMD_EXIT_CMDSET) {
    flash[idx]->en_cmdset = ENCMDSET_NONE;
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle = CYCLE_FIRST;
    return;
  } else {
    warn("Unknown command %d in second cycle",flash[idx]->cmd);
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle = CYCLE_FIRST;
    return;
  }
}

void NorFlash::third_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr) {
  uint32_t reg = FLASH_REG(dev_addr);
  uint32_t cap = flash[idx]->type->capabilities;
  if(flash[idx]->cmd != CMD_NONE) {
    warn("Unknown cmd %d in third cycle", flash[idx]->cmd);
    return;
  }
  /* Sector Address */
  if (value == 0x25) {
    if(flash[idx]->cap_bufwrite>1) {
      flash[idx]->wb_sa = address_to_sa(idx, dev_addr);
      flash[idx]->cmd = CMD_WRITE_TO_BUF;
      flash[idx]->cycle++;
    } else {
      flash[idx]->cmd = CMD_NONE;
      flash[idx]->cycle = CYCLE_FIRST;
      warn("%s: flash type doesn't support Buffer write", flash[idx]->type->name);
    }
    return;
  }
  if (reg != 0x555) {
    warn("Wrong reg %08x, value %08x in third cycle", reg, value);
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle=CYCLE_FIRST;
    return;
  }
  switch(value) {
  case 0x90:
    flash[idx]->cmd=CMD_AUTOSELECT;
    switch_to_iomode(idx);
    flash[idx]->cycle++;
    break;

  case 0x88:
    warn("Enter SECSI not implemented\n");
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle=CYCLE_FIRST;
    break;

  case 0xa0:
    flash[idx]->cmd = CMD_PROGRAM;
    flash[idx]->cycle++;
    break;

  /* Write to buffer abort */
  case 0xf0:
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle=CYCLE_FIRST;
    break;

  /* Unlock Bypass */
  case 0x20:
    flash[idx]->en_cmdset=ENCMDSET_BYPASS;
    flash[idx]->cmd=CMD_NONE;
    flash[idx]->cycle=CYCLE_FIRST;
    break;

  case 0x40:
    if (cap & CAP_LOCK_REG) {
      flash[idx]->en_cmdset=ENCMDSET_LOCK_REG;
      flash[idx]->cmd=CMD_NONE;
      flash[idx]->cycle=CYCLE_FIRST;
    }
    break;

  case 0x50:
    if (cap & CAP_PPB_LOCK) {
      flash[idx]->en_cmdset=ENCMDSET_PPB_LOCK;
      flash[idx]->cmd=CMD_NONE;
      flash[idx]->cycle=CYCLE_FIRST;
    }
    break;

  case 0x60:
    if (cap & CAP_PASSWD) {
      flash[idx]->en_cmdset=ENCMDSET_PW_PROT;
      flash[idx]->cmd=CMD_NONE;
      flash[idx]->cycle=CYCLE_FIRST;
    }
    break;

  case 0x80:
    flash[idx]->cmd=CMD_ERASE;
    flash[idx]->cycle++;
    break;

  case 0xC0:
    if (cap & CAP_PPB) {
      flash[idx]->en_cmdset=ENCMDSET_PPB;
      flash[idx]->cmd=CMD_NONE;
      flash[idx]->cycle=CYCLE_FIRST;
    }
    break;

  case 0xE0:
    if (cap & CAP_DYB) {
      flash[idx]->en_cmdset=ENCMDSET_DYB;
      flash[idx]->cmd=CMD_NONE;
      flash[idx]->cycle=CYCLE_FIRST;
    }
    break;

  default:
    warn("unknown value %04x in third cycle",value);
  }
  return;
}

void NorFlash::fourth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr) {
  uint32_t reg = FLASH_REG(dev_addr);
  uint32_t cmd = flash[idx]->cmd;
  if (cmd == CMD_AUTOSELECT) {
    if ((value == 0xf0) || (value == 0xff)) {
      switch_to_readarray(idx);
    } else if (value == 0) {
      warn("Leave Sec SI region");
    } else {
      warn("%s: Wrong value %04x in leave SEC-SI", flash[idx]->type->name, value);
    }
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle = CYCLE_FIRST;
    return;
  } else if (cmd == CMD_PROGRAM) {
    programm_word(idx, value, dev_addr);
    flash[idx]->cycle = CYCLE_FIRST;
    flash[idx]->cmd = CMD_NONE;
    return;
  } else if (cmd == CMD_WRITE_TO_BUF)  {
    if (flash[idx]->wb_sa != address_to_sa(idx, dev_addr)) {
      warn("Sector Address changed from 0x%08x to 0x%08x"
      " during write to buffer", flash[idx]->wb_sa, dev_addr);
      flash[idx]->cycle=CYCLE_FIRST;
      flash[idx]->cmd=CMD_NONE;
      return;
    }
    flash[idx]->wb_wc = value & 0xff;
    flash[idx]->cycle++;
    return;
  } else if (cmd == CMD_ERASE) {
    if ((reg==0x555) && (value == 0xaa)) {
      flash[idx]->cycle++;
      return;
    } else {
      warn("Error in Erase sequence reg %08x value %04x: Aborting Erase",
           reg, value);
      flash[idx]->cmd=CMD_NONE;
      flash[idx]->cycle=CYCLE_FIRST;
      return;
    }
  } else {
    warn("unknown cmd %d in cycle four", flash[idx]->cycle);
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle = CYCLE_FIRST;
    return;
  }
}

void NorFlash::fifth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr) {
  uint32_t reg = FLASH_REG(dev_addr);
  int cmd = flash[idx]->cmd;
  if(cmd == CMD_WRITE_TO_BUF) {
    int i;
    int buf_index;
    for(i=0; i<WB_BUF_SIZE; i++) {
      flash[idx]->wb_valid[i] = 0;
    }
    if (address_to_sa(idx,dev_addr) != flash[idx]->wb_sa) {
      warn("fifth: sector address changed during buffer write"
        " dev_addr %08x, wb_sa %08x\n", dev_addr, flash[idx]->wb_sa);
      flash[idx]->cycle = CYCLE_FIRST;
      flash[idx]->cmd = CMD_NONE;
      return;
    }
    buf_index = reg & WB_BUF_MASK;
    flash[idx]->wb[buf_index] = value & 0xffff;
    flash[idx]->wb_addr[buf_index] = FLASH_ADDR(dev_addr);
    flash[idx]->wb_valid[buf_index] = 1;
    flash[idx]->wb_ba = FLASH_ADDR(dev_addr) & ~(WB_BUF_MASK);
    flash[idx]->cycle++;
    if (flash[idx]->wb_wc == 0) { // do not decrement
      //fprintf(stderr,"WB Complete\n");
      flash[idx]->cmd = CMD_PGM_BUFFER;
      flash[idx]->cycle++;
      return;
    }
    return;
  } else if (cmd == CMD_ERASE) {
    if ((reg == 0x2aa) && (value == 0x55)) {
      flash[idx]->cycle++;
      return;
    } else {
      warn("Aborting erase sequence in fifth cycle reg %08x,value %04x",
           reg, value);
      flash[idx]->cycle = CYCLE_FIRST;
      flash[idx]->cmd = CMD_NONE;
      return;
    }
  } else {
    warn("Unknown command %d in fifth cycle", cmd);
    flash[idx]->cycle = CYCLE_FIRST;
    flash[idx]->cmd = CMD_NONE;
    return;
  }
}

int NorFlash::pgm_buffer(uint8_t idx) {
  int i;
  for(i=0;i<WB_BUF_SIZE;i++) {
    uint32_t addr = BADDR(flash[idx]->wb_addr[i]);
    if (!flash[idx]->wb_valid[i]) {
      continue;
    }
    programm_word(idx, flash[idx]->wb[i], addr);
  }
  return 0;
}

/*
 * --------------------------------------
 * Erase a sector
 * sa is the Start address of the sector
 * --------------------------------------
 */
void NorFlash::erase_sector(uint8_t idx, uint32_t sa) {
  int i;
  int sectorsize=get_sectorsize(idx, sa);
  DPRINTF(Flash, "erase sector at 0x%08x, size %d\n", sa, sectorsize);
  for (i = 0; i < sectorsize; i += 2) {
    uint32_t addr = FLASH_ADDR_TO_MEMBUS(i + sa);
    *(uint16_t*)(flash[idx]->host_mem + addr) = 0xffff;
  }
}

/*
 * ----------------------------------------------------
 * After erase cylce you can sent next erase command
 * without unlock sequence within 50 usec
 * ----------------------------------------------------
 */
void NorFlash::erase_timeout(uint8_t idx) {
  if ((flash[idx]->cmd == CMD_ERASE) && (flash[idx]->cycle == CYCLE_SIXTH)) {
    flash[idx]->cycle = CYCLE_FIRST;
    flash[idx]->cmd = CMD_NONE;
  } else {
    warn("erase_timeout Bug");
  }
}

void NorFlash::sixth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr) {
  uint32_t reg = FLASH_REG(dev_addr);
  int cmd = flash[idx]->cmd;
  if (cmd == CMD_WRITE_TO_BUF) {
    int buf_index;
    if ((FLASH_ADDR(dev_addr) & ~(WB_BUF_MASK)) != flash[idx]->wb_ba) {
      warn("outside of writebuffer, addr %08x ba %08x",
           dev_addr, flash[idx]->wb_ba);
      flash[idx]->cycle=CYCLE_FIRST;
      flash[idx]->cmd=CMD_NONE;
      return;
    }
    buf_index = reg & WB_BUF_MASK;
    flash[idx]->wb[buf_index] = value & 0xffff;
    flash[idx]->wb_addr[buf_index] = FLASH_ADDR(dev_addr);
    flash[idx]->wb_valid[buf_index] = 1;
    if (--flash[idx]->wb_wc == 0) {
      DPRINTF(Flash, "WB Complete\n");
      flash[idx]->cmd=CMD_PGM_BUFFER;
      flash[idx]->cycle++;
      return;
    } else {
      DPRINTF(Flash, "WB Remaining %d\n", flash[idx]->wb_wc);
    }
    flash[idx]->cycle++;
    return;
  } else if (cmd == CMD_ERASE) {
    if((reg == 0x555) && (value == 0x10)) {
      uint32_t sa;
      for (sa=0; sa<flash[idx]->type->size; sa += get_sectorsize(idx, sa)) {
        erase_sector(idx, sa);
      }
      flash[idx]->cycle = CYCLE_FIRST;
      flash[idx]->cmd = CMD_NONE;
      return;
    } else if (value == 0x30) {
      uint32_t sa = address_to_sa(idx, dev_addr);
      erase_sector(idx, sa);
      erase_timeout(idx);
      //CycleTimer_Mod(&flash->erase_timeout,MicrosecondsToCycles(50));
      return;
    } else {
      //CycleTimer_Remove(&flash->erase_timeout);
      flash[idx]->cycle = CYCLE_FIRST;
      flash[idx]->cmd = CMD_NONE;
      return;
    }
  } else if (cmd == CMD_PGM_BUFFER) {
    if ((value & 0xff) == 0x29) {
      uint32_t sa = address_to_sa(idx, dev_addr);
      if (sa != flash[idx]->wb_sa) {
        warn("Wrong Sector Address %08x instead %08x", sa, flash[idx]->wb_sa);
      } else {
        pgm_buffer(idx);
      }
    } else {
      warn("Wrong magic value in Write to buffer command");
    }
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle = CYCLE_FIRST;
    return;
  } else {
    warn("Unknown cmd %d in sixth cycle", cmd);
    flash[idx]->cycle = CYCLE_FIRST;
    flash[idx]->cmd = CMD_NONE;
    return;
  }
}

/*
 * -------------------------------------------------------------
 * n'th cycle write
 * Cycle number > 6 only reached for buffer programming sequence
 * -------------------------------------------------------------
 */
void NorFlash::nth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr)
{
  uint32_t reg = FLASH_REG(dev_addr);
  int cmd = flash[idx]->cmd;
  if(cmd == CMD_WRITE_TO_BUF) {
    int buf_index;
    if((FLASH_ADDR(dev_addr) & ~(WB_BUF_MASK)) != flash[idx]->wb_ba) {
      warn("outside of writebuffer, addr %08x ba %08x", dev_addr,
           flash[idx]->wb_ba);
      flash[idx]->cycle = CYCLE_FIRST;
      flash[idx]->cmd = CMD_NONE;
      return;
    }
    buf_index = reg & WB_BUF_MASK;
    flash[idx]->wb[buf_index] = value & 0xffff;
    flash[idx]->wb_addr[buf_index] = FLASH_ADDR(dev_addr);
    flash[idx]->wb_valid[buf_index] = 1;
    if (--flash[idx]->wb_wc==0) {
      DPRINTF(Flash, "WB Complete\n");
      flash[idx]->cmd=CMD_PGM_BUFFER;
      flash[idx]->cycle++;
      return;
    } else {
      DPRINTF(Flash, "WB Remaining %d\n", flash[idx]->wb_wc);
    }
    flash[idx]->cycle++;
    return;
  } else if (cmd == CMD_PGM_BUFFER) {
    flash[idx]->cmd = CMD_NONE;
    flash[idx]->cycle = CYCLE_FIRST;
    if (value == 0x29) {
      uint32_t sa = address_to_sa(idx, dev_addr);
      if (sa != flash[idx]->wb_sa) {
        warn("Wrong Sector Address %08x instead %08x", sa, flash[idx]->wb_sa);
      } else {
        pgm_buffer(idx);
      }
      return;
    } else {
      warn("Wrong value %04x in PGM Buffer Command", value);
      return;
    }
  } else {
    warn("Unknown cmd %d in cycle %d", cmd, flash[idx]->cycle);
    flash[idx]->cycle = CYCLE_FIRST;
    flash[idx]->cmd = CMD_NONE;
    return;
  }
}

void NorFlash::flash_write(uint8_t idx, uint16_t value, uint32_t address,
                           int rqlen) {
  uint32_t dev_addr=address & (flash[idx]->type->size-1);
  DPRINTF(Flash, "flash write value %08x address %08x cycle %d\n",
          value, address, flash[idx]->cycle);
  if (flash[idx]->cycle == CYCLE_FIRST) {
    return first_cycle_write(idx, value, dev_addr);
  } else if (flash[idx]->cycle == CYCLE_SECOND) {
    return second_cycle_write(idx, value, dev_addr);
  } else if (flash[idx]->cycle == CYCLE_THIRD) {
    return third_cycle_write(idx, value, dev_addr);
  } else if (flash[idx]->cycle == CYCLE_FOURTH) {
    return fourth_cycle_write(idx, value, dev_addr);
  } else if (flash[idx]->cycle == CYCLE_FIFTH) {
    return fifth_cycle_write(idx, value, dev_addr);
  } else if (flash[idx]->cycle == CYCLE_SIXTH) {
    return sixth_cycle_write(idx, value, dev_addr);
  } else if (flash[idx]->cycle > CYCLE_SIXTH ) {
    return nth_cycle_write(idx, value, dev_addr);
  } else {
    warn("write in cycle %d\n", flash[idx]->cycle);
    return;
  }
}

/*
* -----------------------------------------------------------
* Distribute a write access of 8/16 or 32 Bit to
* the flash chips of a Flash Bank
* -----------------------------------------------------------
*/
void NorFlash::flashbank_write(uint32_t value, uint32_t mem_addr, int rqlen) {
  uint32_t bank_addr = mem_addr & (size - 1);
  int chip, shift;
  unsigned int flash_addr = MEMBUS_TO_FLASH_ADDR(bank_addr);
  unsigned int first_chip = (bank_addr >> 1) & (nr_chips - 1);
  shift = 0;
  chip = first_chip;
  while (rqlen > 0) {
    if (rqlen > 1) {
      flash_write(chip, value >> shift, flash_addr, 2);
      rqlen -= 2;
      chip = (chip + 1) & (nr_chips - 1);
      if (!chip) {
        flash_addr += 2;
      }
      shift += 16;
    } else {
      flash_write(chip, value >> shift, flash_addr, 1);
      rqlen--;
    }
  }
}

uint16_t NorFlash::first_cycle_read(uint8_t idx, uint32_t address) {
  warn("Read in map mode");
  return 0;
}

uint16_t NorFlash::fourth_cycle_read(uint8_t idx, uint32_t dev_addr) {
  int i;
  uint32_t value = 0;
  const FlashType *ftype = flash[idx]->type;
  uint32_t reg = FLASH_REG(dev_addr);
  DPRINTF(Flash, "fourth cycle reg 0x%08x\n", reg);
  if (flash[idx]->cmd == CMD_AUTOSELECT) {
    for (i = 0; i < MAX_AUTO_CODES; i++) {
      const AutoSel *auto_sel = &ftype->auto_sel[i];
      if (auto_sel->mask == 0) {
        break;
      }
      if((reg & auto_sel->mask) == auto_sel->code) {
        value = auto_sel->value;
        break;
      }
    }
    return value;
  } else {
    warn("read fourth unknown cmd %d", flash[idx]->cmd);
    flash[idx]->cycle = CYCLE_FIRST;
    flash[idx]->cmd = CMD_NONE;
    return value;
  }
}

/*
 * ------------------------------------
 * Read access in Query mode
 * ------------------------------------
 */
uint16_t NorFlash::flash_read(uint8_t idx, uint32_t address, int rqlen) {
  uint32_t dev_addr = address & (flash[idx]->type->size - 1);
  DPRINTF(Flash, "Flash read %04x cycle %d\n", address, flash[idx]->cycle);
  if (flash[idx]->mode == MAP_READ_ARRAY) {
    uint32_t value = 0;
    uint8_t *data = (flash[idx]->host_mem + FLASH_ADDR_TO_MEMBUS(dev_addr));
    if ((rqlen == 4) && ((dev_addr & 3) == 0)) {
      warn("Bug: Got 4 Byte request for 16 Bit Chip");
    } else if ((rqlen == 2) && ((dev_addr & 1) == 0)) {
      value = *(uint16_t *)data;
    } else if (rqlen == 1) {
      value = *(uint8_t *)data;
    } else {
      warn("illegal access to flash array size %d, addr %08x", rqlen, dev_addr);
    }
    return value;
  } else if ((flash[idx]->mode == MAP_CFI)) {
    unsigned int reg = FLASH_REG((dev_addr) & 0xfff);
    return cfi_read(flash[idx]->type,reg);
  } else if ((flash[idx]->mode == MAP_IO)) {
    if (flash[idx]->cycle == CYCLE_FIRST) {
      return first_cycle_read(idx, dev_addr);
    } else if (flash[idx]->cycle == CYCLE_FOURTH) {
      return fourth_cycle_read(idx, dev_addr);
    } else {
      warn("read in cycle %d\n", flash[idx]->cycle);
      return 0;
    }
  }
  return 0;
}

/*
 * -------------------------------------------------------
 * Distribute a 8/16 or 32 Bit read access to the chips
 * of a Flash-Bank
 * -------------------------------------------------------
 */
uint32_t NorFlash::flashbank_read(uint32_t mem_addr,int rqlen) {
  uint32_t bank_addr = mem_addr & (size - 1);
  uint32_t value = 0;
  int shift;
  unsigned int flash_addr = MEMBUS_TO_FLASH_ADDR(bank_addr);
  unsigned int chip = (bank_addr >> 1) & (nr_chips - 1);

  shift = 0;
  while(rqlen) {
    if (rqlen > 1) {
      value |= flash_read(chip, flash_addr, 2) << shift;
      rqlen -= 2;
      chip = (chip + 1) & (nr_chips - 1);
      if (!chip) {
        flash_addr += 2;
      }
      shift += 16;
    } else {
      value |= flash_read(chip, flash_addr, 1) << shift;
      rqlen--;
    }
  }
  return value;
}


/*
 * ----------------------------------------------------------
 * setup_eraseregions
 *  Read the Eraseregions from CFI area
 *  and write it to a table. The order of the
 *  table entries is reverted in order to the CFI table for
 *  Top boot devices
 * ----------------------------------------------------------
 */
void NorFlash::setup_eraseregions(AMD_Flash *flash) {
  EraseRegion *region;
  const FlashType *ftype = flash->type;
  int flashsize=0;
  int i;
  int topboot;
  int n_regions;
  if (!(ftype->capabilities & CAP_CFI)) {
    region = &flash->erase_region[0];
    region->sectorsize = ftype->min_sectorsize;
    region->sectors = ftype->size / region->sectorsize;
    return;
  }
  flash->n_regions = n_regions = cfi_read(ftype,0x2c);
  if(n_regions == 0) {
    region = &flash->erase_region[0];
    flashsize = 1 << cfi_read(ftype, 0x27);
    region->sectorsize = flashsize;;
    region->sectors =  1;
  }
  if (n_regions > 4) {
    panic("AMD Flash: Illegal number of Erase Regions: 0x%02x", n_regions);
  }
  if (ftype->capabilities & CAP_TOPBOOT) {
    topboot=1;
  } else {
    topboot=0;
  }
  for (i = 0; i < n_regions; i++) {
    int n_sectors;
    int sectorsize;
    int basereg;
    /* Invert the order for top boot devices ???? */
    if(topboot) {
      basereg = 0x2d + 4 * (n_regions - i - 1);
    } else {
      basereg = 0x2d + 4 * i;
    }
    n_sectors = (cfi_read(ftype,basereg) | (cfi_read(ftype,basereg+1)<<8))+1;
    sectorsize = 256*(cfi_read(ftype,basereg+2) | (cfi_read(ftype,basereg+3)<<8));
    region = &flash->erase_region[i];
    region->sectorsize = sectorsize;
    region->sectors = n_sectors;
    flashsize += sectorsize * n_sectors;
    DPRINTF(Flash, "Region %d nsectors %d secsize %d size %d\n",
            i, n_sectors, sectorsize, sectorsize * n_sectors);

  }
  if ((1 << cfi_read(ftype, 0x27)) != flashsize) {
    panic("%s: Mismatch in flashsizes: flashsize %d\n", ftype->name, flashsize);
  }
}

/*
 * ------------------------------------------------------------------
 * setup_regionmap
 *  Setup a mapping table for mapping a address to a erase region
 *  The smallest sector of the chip determines the bits which are
 *  used as index in the mapping table
 * ------------------------------------------------------------------
 */
void NorFlash::setup_regionmap(AMD_Flash *flash) {
  int i, j, k, n;
  int mapsize;
  uint32_t minsectorsize=~0;
  uint32_t flashsize=0;
  int shift;
  EraseRegion *region;
  if (flash->n_regions == 0) {
    flash->rmap_size = 1;
    return;
  }
  for (i = 0; i < 4; i++) {
    if (i >= flash->n_regions) break;
    region = &flash->erase_region[i];
    if (region->sectorsize < minsectorsize) {
      minsectorsize = region->sectorsize;
    }
    flashsize += region->sectorsize * region->sectors;
  }
  flash->min_sectorsize = minsectorsize;
  flash->rmap_mask = ~(minsectorsize - 1) & (flashsize - 1);
  flash->rmap_size = mapsize =  flashsize / minsectorsize;
  for (shift = 0; shift<32; shift++) {
    if (((flash->rmap_mask >> shift)) & 1) {
      break;
    }
  }
  flash->rmap_shift = shift;
  flash->rmap = new EraseRegion*[flash->rmap_size];
      //(NorFlash::EraseRegion)malloc(sizeof(*flash->rmap) * flash->rmap_size);
  for (n=0, i=0; i < flash->n_regions; i++) {
    int entries_per_sector;
    region = &flash->erase_region[i];
    entries_per_sector = region->sectorsize / minsectorsize;
    DPRINTF(Flash, "Entries per sector %d, region sectors %d\n",
            entries_per_sector, region->sectors);
    for (j = 0; j < region->sectors; j++) {
      for (k = 0; k < entries_per_sector; k++) {
        flash->rmap[n++] = region;
      }
    }
  }
  DPRINTF(Flash, "Minimum sectorsize %d\n", minsectorsize);
  DPRINTF(Flash, "Mapsize %d, n %d\n", mapsize, n);
  DPRINTF(Flash, "Mask 0x%08x,shift %d\n", flash->rmap_mask, shift);
}

/*
 * -------------------------------------------------------
 *  Constructor for AMD_Flash
 * -------------------------------------------------------
 */
NorFlash::AMD_Flash *NorFlash::AMD_Flash_New(const FlashType *ftype, int chip_nr) {
  AMD_Flash *flash = new AMD_Flash;
  flash->type = ftype;
  flash->bank = this;
  flash->host_mem = host_mem + chip_nr * 2;
  if (ftype->capabilities & CAP_CFI) {
    flash->cap_bufwrite = (1 << cfi_read(ftype, 0x2a));
  }
  flash->cycle=CYCLE_FIRST;
  flash->cmd=CMD_NONE;
  setup_eraseregions(flash);
  setup_regionmap(flash);
  //CycleTimer_Init(&flash->erase_timeout,erase_timeout,flash);
  return flash;
}

Tick NorFlash::read(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();
  uint32_t data;

  if (curTick() == 0 || pkt->getSize() > 4 || pkt->getSize() == 3) {
    memcpy(pkt->getPtr<uint8_t>(), host_mem + daddr, pkt->getSize());
    if (pkt->needsResponse()) {
        pkt->makeResponse();
    }
    return pioDelay;
  }

  data = flashbank_read(daddr & 0xFFFFFFFC, 4);

  data >>= (daddr & 0x3) * 8;

  switch(pkt->getSize()) {
  case 1:
    pkt->set<uint8_t>(data);
    break;
  case 2:
    pkt->set<uint16_t>(data);
    break;
  case 4:
    pkt->set<uint32_t>(data);
    break;
  default:
    panic("NorFlash read size %d too big?\n", pkt->getSize());
    break;
  }
  DPRINTF(Flash, "Reading %#x from NorFlash at offset: %#x(size %d)\n",
          data, daddr, pkt->getSize());
  pkt->makeAtomicResponse();
  return pioDelay;
}

Tick NorFlash::write(PacketPtr pkt) {
  uint32_t data = 0;

  Addr daddr = pkt->getAddr() - pioAddr;
  if (curTick() == 0) {
    memcpy(host_mem + daddr, pkt->getPtr<uint8_t>(), pkt->getSize());
    if (pkt->needsResponse()) {
        pkt->makeResponse();
    }
    return pioDelay;
  }

  switch(pkt->getSize()) {
    case 1:
      data = pkt->get<uint8_t>();
      break;
    case 2:
      data = pkt->get<uint16_t>();
      break;
    case 4:
      data = pkt->get<uint32_t>();
      break;
    default:
      panic("NorFlash controller write size %d too big?\n", pkt->getSize());
      break;
  }

  assert(pkt->getAddr() >= pioAddr &&
         pkt->getAddr() < pioAddr + pioSize);

  DPRINTF(Flash, "Writing %#x to NorFlash at offset: %#x(size %d)\n", data, daddr, pkt->getSize());

  flashbank_write(data, daddr, pkt->getSize());

  pkt->makeAtomicResponse();
  return pioDelay;
}


AddrRangeList NorFlash::getAddrRanges() const {
  AddrRangeList ranges;
  ranges.push_back(RangeSize(pioAddr, pioSize));
  return ranges;
}

NorFlash *NorFlashParams::create() {
  return new NorFlash(this);
}
