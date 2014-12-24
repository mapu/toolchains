/*
 * mapu_jtag.c
 *
 *  Created on: Mar 5, 2012
 *      Author: root
 */

#include <config.h>
#include <target/mapu_spu.h>
#include <assert.h>
#include <jtag/drivers/fifo.h>
#include <string.h>
#include <helper/log.h>
#include <unistd.h>

void
mapu_jtag_set_instr(struct mapu_jtag *jtag_info, int new_instr)
{
	struct jtag_tap *tap;

	tap = jtag_info->tap;
	assert(tap != NULL);

	if(buf_get_u32(tap->cur_instr, 0, tap->ir_length) != (uint32_t) new_instr) {
		struct scan_field field;
		uint8_t t[4];

		field.num_bits = tap->ir_length;
		field.out_value = t;
		buf_set_u32(t, 0, field.num_bits, new_instr);
		field.in_value = NULL;

		jtag_add_ir_scan(tap, &field, TAP_IDLE);
	}
}

// out: debugger -> ocd
// in : ocd      -> debugger
int
mapu_jtag_drscan_package(struct mapu_jtag *jtag_info, uint8_t *data, bool isout)
{
	LOG_DEBUG("mapu_jtag_drscan_package() is called");

	uint8_t scan_out[SPU_DR_BYTE_NUM];
	uint8_t scan_in [SPU_DR_BYTE_NUM];
	static uint8_t wen_tag = 0xC0;

	int retval;

#ifdef SIM
	int repeat=0;

	// first we check ocd status
	CLEAR_PACKET(scan_out);
	do {
	  if(repeat>0) usleep(100000);
	  if(repeat>100)
	  {
	    LOG_ERROR("OCD is busy or jtag packet is invalid");
	    return ERROR_FAIL;
	  }
	  retval = write_fifo((const char*) scan_out, SPU_DR_BYTE_NUM);
	  CLEAR_PACKET(scan_in);
	  retval = read_fifo_timeout((char*) scan_in, SPU_DR_BYTE_NUM, 1, 10);
	  repeat++;
	} while(CHECK_SOC_OCD_BUSY(scan_in) || !CHECK_INPUT_PACKET_VALID(scan_in));

	// then we emit jtag command
  memcpy(scan_out, data, SPU_DR_BYTE_NUM);
  scan_out[SPU_DR_BYTE_NUM - 1] &= 0x3F;
  scan_out[SPU_DR_BYTE_NUM - 1] |= wen_tag;
  wen_tag ^= 0x40;

  do {
    if(repeat>0) usleep(100000);
    if(repeat>200)
    {
      LOG_ERROR("OCD jtag packet returned is invalid after we emitted a jtag command");
      return ERROR_FAIL;
    }
		retval = write_fifo((char*) scan_out, SPU_DR_BYTE_NUM);
		CLEAR_PACKET(scan_in);
		retval = read_fifo_timeout((char*) scan_in, SPU_DR_BYTE_NUM, 1, 10);
		repeat++;
	} while(!CHECK_INPUT_PACKET_VALID(scan_in));
#else
  struct jtag_tap *tap;
  struct scan_field field;
  tap = jtag_info->tap;
  assert(tap != NULL);

  mapu_jtag_set_instr(jtag_info, 0x08);
  tap->bypass = 0;

  field.num_bits = SPU_DR_BIT_NUM;
  if(isout) {
    field.out_value = scan_out;
    buf_cpy(data, scan_out, SPU_DR_BIT_NUM);
    scan_out[SPU_DR_BYTE_NUM - 1] &= 0x3F;
    scan_out[SPU_DR_BYTE_NUM - 1] |= wen_tag;
    wen_tag ^= 0x40;
  }
  else {
    field.out_value = NULL;
  }
  field.in_value = scan_in;

  jtag_add_dr_scan(tap, 1, &field, TAP_IDLE);

  if ((retval = jtag_execute_queue()) != ERROR_OK) {
    LOG_ERROR("jtag_execute_queue() failed");
    return retval;
  }
#endif
	if(CHECK_SOC_OCD_BUSY(scan_in)) LOG_DEBUG("ocd is busy and cannot handle new incoming jtag command");
	if(CHECK_CMD_PACKET_ILLEGAL(scan_in)) LOG_DEBUG("ocd received an illegal command package");
	if(CHECK_DATA_PACKET_ILLEGAL(scan_in)) LOG_DEBUG("ocd received an illegal data package");

	// only if scan_in is valid.
	memcpy(data, scan_in, SPU_DR_BYTE_NUM);
#ifndef SIM
	// cause terrible things for gdb connection
  keep_alive();
#endif
	return ERROR_OK;
}
