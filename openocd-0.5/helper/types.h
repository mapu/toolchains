/***************************************************************************
 *   Copyright (C) 2004, 2005 by Dominic Rath                              *
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
#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>

#include <stdbool.h>
#define __bool_true_false_are_defined 1


#define false	0
#define true		1

#define bool _Bool


/// turns a macro argument into a string constant
#define stringify(s) __stringify(s)
#define __stringify(s) #s


/**
 * Compute the number of elements of a variable length array.
 * <code>
 * const char *strs[] = { "a", "b", "c" };
 * unsigned num_strs = ARRAY_SIZE(strs);
 * </code>
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))


/**
 * Cast a member of a structure out to the containing structure.
 * @param ptr The pointer to the member.
 * @param type The type of the container struct this is embedded in.
 * @param member The name of the member within the struct.
 *
 * This is a mechanism which is used throughout the Linux kernel.
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (void *) ( (char *)__mptr - offsetof(type,member) ) );})


/**
 * Rounds @c m up to the nearest multiple of @c n using division.
 * @param m The value to round up to @c n.
 * @param n Round @c m up to a multiple of this number.
 * @returns The rounded integer value.
 */
#define DIV_ROUND_UP(m, n)	(((m) + (n) - 1) / (n))


/* DANGER!!!! here be dragons!
 *
 * Leave these fn's as byte accesses because it is safe
 * across architectures. Clever usage of 32 bit access
 * will create problems on some hosts.
 *
 * Note that the "buf" pointer in memory is probably unaligned.
 *
 * Were these functions to be re-written to take a 32 bit wide or 16 bit wide
 * memory access shortcut, then on some CPU's, i.e. ARM7, the 2 lsbytes of the address are
 * ignored for 32 bit access, whereas on other CPU's a 32 bit wide unaligned memory access
 * will cause an exception, and lastly on x86, an unaligned "greater than bytewide"
 * memory access works as if aligned.  So what follows below will work for all
 * platforms and gives the compiler leeway to do its own platform specific optimizations.
 *
 * Again, note that the "buf" pointer in memory is probably unaligned.
 */


static inline uint32_t le_to_h_u32(const uint8_t* buf)
{
	return (uint32_t)(buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
}

static inline uint32_t le_to_h_u24(const uint8_t* buf)
{
	return (uint32_t)(buf[0] | buf[1] << 8 | buf[2] << 16);
}

static inline uint16_t le_to_h_u16(const uint8_t* buf)
{
	return (uint16_t)(buf[0] | buf[1] << 8);
}

static inline uint32_t be_to_h_u32(const uint8_t* buf)
{
	return (uint32_t)(buf[3] | buf[2] << 8 | buf[1] << 16 | buf[0] << 24);
}

static inline uint32_t be_to_h_u24(const uint8_t* buf)
{
	return (uint32_t)(buf[2] | buf[1] << 8 | buf[0] << 16);
}

static inline uint16_t be_to_h_u16(const uint8_t* buf)
{
	return (uint16_t)(buf[1] | buf[0] << 8);
}

static inline void h_u32_to_le(uint8_t* buf, int val)
{
	buf[3] = (uint8_t) (val >> 24);
	buf[2] = (uint8_t) (val >> 16);
	buf[1] = (uint8_t) (val >> 8);
	buf[0] = (uint8_t) (val >> 0);
}

static inline void h_u32_to_be(uint8_t* buf, int val)
{
	buf[0] = (uint8_t) (val >> 24);
	buf[1] = (uint8_t) (val >> 16);
	buf[2] = (uint8_t) (val >> 8);
	buf[3] = (uint8_t) (val >> 0);
}

static inline void h_u24_to_le(uint8_t* buf, int val)
{
	buf[2] = (uint8_t) (val >> 16);
	buf[1] = (uint8_t) (val >> 8);
	buf[0] = (uint8_t) (val >> 0);
}

static inline void h_u24_to_be(uint8_t* buf, int val)
{
	buf[0] = (uint8_t) (val >> 16);
	buf[1] = (uint8_t) (val >> 8);
	buf[2] = (uint8_t) (val >> 0);
}

static inline void h_u16_to_le(uint8_t* buf, int val)
{
	buf[1] = (uint8_t) (val >> 8);
	buf[0] = (uint8_t) (val >> 0);
}

static inline void h_u16_to_be(uint8_t* buf, int val)
{
	buf[0] = (uint8_t) (val >> 8);
	buf[1] = (uint8_t) (val >> 0);
}

#endif /* TYPES_H */
