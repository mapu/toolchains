/*
 * norflash.hh
 *
 *  Created on: Apr 15, 2014
 *      Author: wangl
 */

#ifndef NORFLASH_HH_
#define NORFLASH_HH_

#include "dev/arm/amba_device.hh"
#include "params/NorFlash.hh"

/*
 * ---------------------------------------------------------------------
 * Auto select mode: create a table with code/mask pairs
 * to decide which value has to returned
 * ---------------------------------------------------------------------
 */
#define MAX_AUTO_CODES (7)

#define CAP_CFI   (1)
#define CAP_TOPBOOT   (2)
/* CMDSETS */
#define CAP_LOCK_REG  (4)
#define CAP_PASSWD  (8)
#define CAP_PPB   (0x10)
#define CAP_PPB_LOCK  (0x20)
#define CAP_DYB   (0x40)

/*
 * -----------------------------------------------------------------
 * Calculate Byteaddress from Memory Bus address (16 Bit mode)
 * And Register number from Byte-Address
 * -----------------------------------------------------------------
 */
#define MEMBUS_TO_FLASH_ADDR(addr) ((addr) >> addr_shift)
#define FLASH_ADDR_TO_MEMBUS(addr) ((addr) << addr_shift)
#define BADDR(x) ((x)<<1)
#define FLASH_ADDR(x) ((x) >> 1)
#define FLASH_REG(x)  (((x) >> 1) & 0xfff)


#define MAP_READ_ARRAY  (0)
#define MAP_IO    (1)
#define MAP_CFI   (2)

/*
 * ----------------------------------------------------------------------------
 * State machine definitions for interpretation of incomming commands
 * The names are stolen from the Bus Cycle List of the am29lv640 documentation
 * The global state machine is left as soon as the command is identified,
 * and a command specific Sub statemachine is entered
 * ----------------------------------------------------------------------------
 */
#define STATE_START  (0)
#define STATE_FIRST  (1)
#define STATE_SECOND (2)
#define STATE_THIRD  (3)
#define STATE_FOURTH (4)
#define STATE_FIFTH  (5)
#define STATE_SIXTH  (6)

#define CYCLE_FIRST  (1)
#define CYCLE_SECOND (2)
#define CYCLE_THIRD  (3)
#define CYCLE_FOURTH (4)
#define CYCLE_FIFTH  (5)
#define CYCLE_SIXTH  (6)


#define CMD_NONE          (0)
#define CMD_AUTOSELECT    (1)
#define CMD_ENTER_SECSI   (5)
#define CMD_EXIT_SECSI    (6)
#define CMD_PROGRAM   (7)
#define CMD_WRITE_TO_BUF  (8)
#define CMD_PGM_BUFFER    (9)
#define CMD_WBUF_ABORT_RESET  (10)
#define CMD_UNLOCK_BYPASS (11)
#define CMD_UNLOCK_BYPASS_PGM   (12)
#define CMD_EXIT_CMDSET   (13)
#define CMD_ERASE   (14)

/* Sector Protection commands from S29GL-P manual */
#define CMD_ENTER_LOCK_REG  (15)
#define CMD_LOCKBITS_PGM  (16)
#define CMD_EXIT_LOCK_REG (18)

#define CMD_ENTER_PW_PROT (19)
#define CMD_PW_PGM  (20)
#define CMD_EXIT_PW_PROT  (23)

#define CMD_ENTER_PPB   (24)
#define CMD_PPB_PGM   (25)
#define CMD_EXIT_PPB    (28)

#define CMD_ENTER_PPB_LOCK  (29)
#define CMD_PPB_LOCKBIT_SET (30)
#define CMD_EXIT_PPB_LOCK (32)

#define CMD_ENTER_DYB   (33)
#define CMD_DYB_SET   (34)
#define CMD_EXIT_DYB    (37)

/* The currently enabled cmdset */
#define ENCMDSET_NONE   (0)
#define ENCMDSET_BYPASS   (1)
#define ENCMDSET_LOCK_REG (2)
#define ENCMDSET_PW_PROT  (3)
#define ENCMDSET_PPB    (4)
#define ENCMDSET_PPB_LOCK (5)
#define ENCMDSET_DYB    (6)

#define WB_BUF_SIZE 32
#define WB_BUF_MASK 0x1FUL

class NorFlash : public AmbaPioDevice {

  typedef struct AutoSel {
    int code;
    int mask;
    int value;
  } AutoSel;

  typedef struct EraseRegion {
    uint32_t sectorsize;
    int sectors;
  } EraseRegion;

  typedef struct FlashType {
    const char *name;
    int size;
    int min_sectorsize;
    uint16_t querystr[0x4c];
    AutoSel auto_sel[MAX_AUTO_CODES];
    uint32_t capabilities;
  } FlashType;

  static const FlashType flash_types[];

  typedef struct AMD_Flash {
    NorFlash *bank;
    uint8_t  *host_mem;
    const FlashType *type;

    int n_regions;
    /* The erase region table is inverted in Initialization for Top Boot devices */
    EraseRegion erase_region[4];
    int rmap_size;
    uint32_t min_sectorsize;
    uint32_t rmap_mask;
    uint32_t rmap_shift;
    EraseRegion **rmap;

    /* State Variables */
    int mode;      /* current memory mapping mode (Read Array or read registers) */
    int cycle;     /* Number of current cycle in multiword cmd sequences */
    int cmd;       /* recognized command */
    int en_cmdset; /* Enabled cmdset (bypass Programm, Lock Register, PPB ... */

    /*
     * Write buffer
     */
    uint32_t wb_sa; // write buffer sector address
    uint32_t wb_ba; // write buffer address
    uint32_t wb_wc; // write buffer word count
    uint16_t wb[WB_BUF_SIZE];
    uint32_t wb_addr[WB_BUF_SIZE];
    uint32_t wb_valid[WB_BUF_SIZE];


    int cap_bufwrite; // max blocksize
    //CycleTimer erase_timeout;
  } AMD_Flash;

  uint8_t  *host_mem;

  int nr_chips;
  int bankwidth; /* nr of bits */
  int addr_shift;
  int size;
  AMD_Flash *flash[2];

  inline uint16_t cfi_read(const FlashType *ftype, int reg);

  inline int get_sectorsize(uint8_t idx, uint32_t byte_addr);

  inline int address_to_sa(uint8_t idx, uint32_t byte_addr);

  void switch_to_readarray(uint8_t idx);

  void switch_to_iomode(uint8_t idx);

  void switch_to_cfimode(uint8_t idx);

  void first_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr);

  inline void programm_word(uint8_t idx, uint16_t value, uint32_t dev_addr);

  void second_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr);

  void third_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr);

  void fourth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr);

  void fifth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr);

  int pgm_buffer(uint8_t idx);

  void erase_sector(uint8_t idx, uint32_t sa);

  void erase_timeout(uint8_t idx);

  void sixth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr);

  void nth_cycle_write(uint8_t idx, uint16_t value, uint32_t dev_addr);

  void flash_write(uint8_t idx, uint16_t value, uint32_t address, int rqlen);

  void flashbank_write(uint32_t value, uint32_t mem_addr, int rqlen);

  uint16_t first_cycle_read(uint8_t idx, uint32_t address);

  uint16_t fourth_cycle_read(uint8_t idx, uint32_t dev_addr);

  uint16_t flash_read(uint8_t idx, uint32_t address, int rqlen);

  uint32_t flashbank_read(uint32_t mem_addr, int rqlen);

  void setup_eraseregions(AMD_Flash *flash);

  void setup_regionmap(AMD_Flash *flash);

  //void change_endian(SigNode *node, int value, void *clientData);
  AMD_Flash *AMD_Flash_New(const FlashType *ftype, int chip_nr);

public:
  typedef NorFlashParams Params;

  const Params *params() const {
    return dynamic_cast<const Params *>(_params);
  }
  NorFlash(const Params *p);
  ~NorFlash();

  /**
   * Handle a read to the device
   * @param pkt The memory request.
   * @param data Where to put the data.
   */
  virtual Tick read(PacketPtr pkt);

  /**
   * Handle writes to the device
   * @param pkt The memory request.
   * @param data the data
   */
  virtual Tick write(PacketPtr pkt);

  virtual void serialize(std::ostream &os) {}
  virtual void unserialize(Checkpoint *cp, const std::string &section) {}
  /**
   * Determine the address ranges that this device responds to.
   *
   * @return a list of non-overlapping address ranges
   */
  AddrRangeList getAddrRanges() const;
};

#endif /* NORFLASH_HH_ */
