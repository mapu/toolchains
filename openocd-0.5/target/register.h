/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   Copyright (C) 2007,2008 Øyvind Harboe                                 *
 *   oyvind.harboe@zylin.com                                               *
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
#ifndef REGISTER_H
#define REGISTER_H

struct target;
struct reg_arch_type;

// register infomation shared among multicores
struct reg_desc
{
    const char *name;
    uint32_t size;   // number of bit

    uint32_t offset; // offset in memory map

    void *arch_info; // arch-specific register info
    const struct reg_arch_type *type;
};

struct reg
{
    struct reg_desc *desc; // static description for registers

	// the contents of these field differs from target to target.
    void *value;
    bool *valid;

    void *rewrite_value;
    bool *dirty;

    struct target *target;
};

struct reg_cache
{
		const char *name;
		struct reg_cache *next;
		struct reg *reg_list;
		unsigned num_regs;
};

struct reg_arch_type
{
		int
		(*get)(struct reg *reg);

		int
		(*set)(struct reg *reg, uint8_t *buf);

		int
		(*print)(struct reg *reg, char* content); // how to interpret reg value and display it.
};

struct reg*
register_get_by_name(struct reg_cache *first, const char *name, bool search_all);

struct reg_cache**
register_get_last_cache_p(struct reg_cache **first);

void
register_cache_invalidate(struct reg_cache *cache);

void
register_init_dummy(struct reg *reg);

#endif /* REGISTER_H */
