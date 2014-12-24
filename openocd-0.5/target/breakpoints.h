/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

#include <helper/types.h>

struct target;

enum breakpoint_type
{
    BKPT_UNKNOWN    = 0,
    BKPT_HARD       = 1,
    BKPT_SOFT       = 2
};

// These must match spu ocd watchpoint access mode.
enum watchpoint_rw
{
    WPT_UNKNOWN = 0,
    WPT_WRITE   = 1,
    WPT_READ    = 2,
    WPT_ACCESS  = 3
};

enum watchpoint_mode
{
    MODE_UNKNOWN    = 0,
    MODE_MEM        = 1,
    MODE_DATA       = 2,
    MODE_ALL        = 3
};

struct breakpoint
{
	uint32_t address;

	uint32_t revised_address; // for spu to support vliw breakpoint

	int length; // byte
	enum breakpoint_type type;
	int set; // whether this bp is enabled on OCD
	uint8_t *orig_instr;
	uint32_t trap; // trap instruction codes
	struct breakpoint *next;
	int unique_id;
};

struct watchpoint
{
  enum watchpoint_mode mode;
	uint32_t address;
	uint32_t length;
  enum watchpoint_rw rw;
  uint32_t value;
	uint32_t mask;

  uint32_t revised_address;
	int set;
	struct watchpoint *next;
	int unique_id;
};

void breakpoint_clear_target(struct target *target);
int breakpoint_add(struct target *target,
		uint32_t address, uint32_t length, enum breakpoint_type type);
void breakpoint_remove(struct target *target, uint32_t address);
void breakpoint_free(struct target *target, struct breakpoint *breakpoint_to_remove);

struct breakpoint* breakpoint_find(struct target *target, uint32_t address);

void watchpoint_clear_target(struct target *target);
int watchpoint_add(struct target *target, enum watchpoint_mode mode, uint32_t address,
                   uint32_t length, enum watchpoint_rw rw, uint32_t value, uint32_t mask);
void watchpoint_remove(struct target *target, uint32_t address);

#endif /* BREAKPOINTS_H */
