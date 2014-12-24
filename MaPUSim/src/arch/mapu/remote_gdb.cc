/*
 * Copyright (c) 2010 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2002-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * $NetBSD: kgdb_stub.c,v 1.8 2001/07/07 22:58:00 wdk Exp $
 *
 * Taken from NetBSD
 *
 * "Stub" to allow remote cpu to debug over a serial line using gdb.
 */

#include <sys/signal.h>
#include <unistd.h>
#include <cstdio>

#include <string>

#include "arch/mapu/decoder.hh"
#include "arch/mapu/remote_gdb.hh"
#include "arch/mapu/vtophys.hh"
#include "cpu/thread_state.hh"
#include "cpu/inorder/cpu.hh"
#include "debug/GDBAcc.hh"
#include "debug/GDBMisc.hh"
#include "mem/page_table.hh"
#include "sim/full_system.hh"
#include "register_desc.hh"

using namespace std;
using namespace MapuISA;

RemoteGDB::RemoteGDB(System *_system, ThreadContext *tc)
    : BaseRemoteGDB(_system, tc, GdbNumRegs)
{
}

/* num: gcc/dwarf number
 * sz:  the number of bytes filled into buf
 * buf: container of register value
 */
bool
RemoteGDB::getRegVal(uint32_t num, void *buf, uint32_t *sz)
{
    struct mapu_spu_reg_desc* desc = getMAPUSPURegDesc(num);
    if(desc)
    {
        InOrderCPU* cpu = static_cast<InOrderCPU*>(context->getCpuPtr());

        uint32_t id = desc->id;
        if(e_R0<=id && id<=e_R31)
        {
            uint32_t val = context->readIntReg(id-e_R0);
            memcpy(buf, &val, sizeof(val));
            *sz = sizeof(val);
        }
        else if(e_J0 <= id && id <= e_J31)
        {
            uint32_t val = cpu->readIntJReg(id-e_J0, context->threadId());
            memcpy(buf, &val, sizeof(val));
            *sz = sizeof(val);
        }
        else if(e_S0 <= id && id <= e_S31)
        {
            uint32_t val = context->readFloatRegBits(id - e_S0);
            memcpy(buf, &val, sizeof(val));
            *sz = sizeof(val);
        }
        else if(e_DR0 <= id && id <= e_DR15)
        {
            uint64_t val = cpu->readDoubleRegBits(id - e_DR0, context->threadId());
            memcpy(buf, &val, sizeof(val));
            *sz = sizeof(val);
        }
        else if(e_RRPC == id)
        {
            uint32_t val = context->bpPC().nlpc();
            memcpy(buf, &val, sizeof(val));
            *sz = sizeof(val);
        }
        else if(e_DPC == id)
        {
            uint32_t val = context->bpPC().pc();
            memcpy(buf, &val, sizeof(val));
            *sz = sizeof(val);
        }
        else return false;

        return true;

    }
    else return false;
}

bool
RemoteGDB::setRegVal(uint32_t num, void *buf, uint32_t sz)
{
    return false;
}

bool
RemoteGDB::getRegDesc(uint32_t num, char * desc)
{
    struct mapu_spu_reg_desc *reg_desc = getMAPUSPURegDesc(num);

    if(!reg_desc)
        return false;

    struct mapu_spu_reg_type *type = reg_desc->type;
    int sz = 0;

    // register info:
    // name:r13;bitsize:64;offset:104;encoding:uint;format:hex;set:General Purpose Registers;gcc:13;dwarf:13;
    sz += sprintf(desc + sz, "name:%s;", reg_desc->name);

    if(reg_desc->alt_name) {
        sz += sprintf(desc + sz, "alt-name:%s;", reg_desc->alt_name);
        sz += sprintf(desc + sz, "generic:%s;", reg_desc->alt_name);
    }

    sz += sprintf(desc + sz, "bitsize:%d;", reg_desc->size);
    sz += sprintf(desc + sz, "offset:%d;", reg_desc->offset);
    sz += sprintf(desc + sz, "encoding:%s;", type->encoding_name);
    sz += sprintf(desc + sz, "format:%s;", type->format_name);
    sz += sprintf(desc + sz, "set:%s;", type->set_name);
    sz += sprintf(desc + sz, "gcc:%d;", reg_desc->id);
    sz += sprintf(desc + sz, "dwarf:%d;", reg_desc->id);

    if(reg_desc->num_super_regs || reg_desc->num_sub_regs)
    {
        sz += sprintf(desc + sz, "container-regs:");

        if(reg_desc->num_super_regs > 0)
                    {
            sz += sprintf(desc + sz, "%x", reg_desc->super_regs[0]);
            for(int k = 1; k < reg_desc->num_super_regs; k++)
                sz += sprintf(desc + sz, ",%x", reg_desc->super_regs[k]);

            for(int k = 0; k < reg_desc->num_sub_regs; k++)
                sz += sprintf(desc + sz, ",%x", reg_desc->sub_regs[k]);
        }
        else if(reg_desc->num_sub_regs > 0)
                    {
            sz += sprintf(desc + sz, "%x", reg_desc->sub_regs[0]);
            for(int k = 1; k < reg_desc->num_sub_regs; k++)
                sz += sprintf(desc + sz, ",%x", reg_desc->sub_regs[k]);
        }

        sz += sprintf(desc + sz, ";");

        ////////////////////////////////////////////////////////////
        sz += sprintf(desc + sz, "invalidate-regs:");

        if(reg_desc->num_super_regs > 0)
                    {
            sz += sprintf(desc + sz, "%x", reg_desc->super_regs[0]);
            for(int k = 1; k < reg_desc->num_super_regs; k++)
                sz += sprintf(desc + sz, ",%x", reg_desc->super_regs[k]);

            for(int k = 0; k < reg_desc->num_sub_regs; k++)
                sz += sprintf(desc + sz, ",%x", reg_desc->sub_regs[k]);
        }
        else if(reg_desc->num_sub_regs > 0)
                    {
            sz += sprintf(desc + sz, "%x", reg_desc->sub_regs[0]);
            for(int k = 1; k < reg_desc->num_sub_regs; k++)
                sz += sprintf(desc + sz, ",%x", reg_desc->sub_regs[k]);
        }

        sz += sprintf(desc + sz, ";");
    }

    return true;
}

/*
 * Determine if the mapping at va..(va+len) is valid.
 */
bool
RemoteGDB::acc(Addr va, size_t len)
{
    TlbEntry entry;
    //Check to make sure the first byte is mapped into the processes address
    //space.
    if (FullSystem)
        panic("acc not implemented for MAPU FS!");
    else {
      if (context->getDTBPtr()->probeEntry(va, len))
        return true;
      warn("GDB tried to access an invalid address @%#0x(len = %#x)", va, len);
      return false;
    }
}

/* Translate the kernel debugger register format into the GDB register
 * format.
 */
void
RemoteGDB::getregs()
{
    DPRINTF(GDBAcc, "getregs in remotegdb \n");
    memset(gdbregs.regs, 0, gdbregs.bytes());

    // Mapu registers are 32 bits wide, gdb registers are 64 bits wide
    // two Mapu registers are packed into one gdb register (little endian)

    // INTREG: R0~R31
    for (int i = 0; i < GdbIntArchRegs; i++) {
        gdbregs.regs[i] = pack(
                context->readIntReg(i * 2),
                context->readIntReg(i * 2 + 1));
    }
    // FLOATREG: F0~F31
    for (int i = 0; i < GdbFloatArchRegs; i++) {
        gdbregs.regs[GdbIntRegs + i] = pack(
                context->readFloatRegBits(i * 2),
                context->readFloatRegBits(i * 2 + 1));
    }
}

/*
 * Translate the GDB register format into the kernel debugger register
 * format.
 */
void
RemoteGDB::setregs()
{
    DPRINTF(GDBAcc, "setregs in remotegdb \n");

    // Mapu registers are 32 bits wide, gdb registers are 64 bits wide
    // two Mapu registers are packed into one gdb register (little endian)

    // INTREG: R0~R31
    for (int i = 0; i < GdbIntArchRegs; i++) {
        if (i) context->setIntReg(i * 2,
                unpackLo(gdbregs.regs[i]));
        context->setIntReg(i * 2 + 1,
                unpackHi(gdbregs.regs[i]));
    }
    // FLOATREG: F0~F31
    for (int i = 0; i < GdbFloatArchRegs; i++) {
        context->setFloatRegBits(i * 2,
                unpackLo(gdbregs.regs[GdbIntRegs + i]));
        context->setFloatRegBits(i * 2 + 1,
                unpackHi(gdbregs.regs[GdbIntRegs + i]));
    }
}

void
RemoteGDB::clearSingleStep()
{
    DPRINTF(GDBMisc, "clearSingleStep bt_addr=%#x nt_addr=%#x\n",
            takenBkpt, notTakenBkpt);

    if (takenBkpt != 0)
        clearTempBreakpoint(takenBkpt);

    if (notTakenBkpt != 0)
        clearTempBreakpoint(notTakenBkpt);
}

void
RemoteGDB::setSingleStep()
{
    PCState pc = context->bpPC();

    setTempBreakpoint(pc.nlpc());

    DPRINTF(GDBMisc, "setSingleStep nt_addr=%#x\n", pc.nlpc());
}
