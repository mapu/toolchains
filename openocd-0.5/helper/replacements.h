/***************************************************************************
 *   Copyright (C) 2006 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   Copyright (C) 2007,2008 Øyvind Harboe                                 *
 *   oyvind.harboe@zylin.com                                               *
 *                                                                         *
 *   Copyright (C) 2008 by Spencer Oliver                                  *
 *   spen@spen-soft.co.uk                                                  *
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
#ifndef REPLACEMENTS_H
#define REPLACEMENTS_H

#include <fcntl.h>
#include <unistd.h>

/* MIN,MAX macros */
#define MIN(a,b) (((a) < (b))?(a):(b))
#define MAX(a,b) (((a) > (b))?(a):(b))

/* for systems that do not support ENOTSUP
 * win32 being one of them */
#ifndef ENOTSUP
#define ENOTSUP 134		/* Not supported */
#endif

/* for systems that do not support O_BINARY
 * linux being one of them */
#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef HAVE_SYS_TIME_H

#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED

//struct timeval {
//	long tv_sec;
//	long tv_usec;
//};

#endif /* _TIMEVAL_DEFINED */

#endif

/* gettimeofday() */
#ifndef HAVE_GETTIMEOFDAY

struct timezone;

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif

#ifndef IN_REPLACEMENTS_C
/**** clear_malloc & fill_malloc ****/
void *clear_malloc(size_t size);
void *fill_malloc(size_t size);
#endif

/*
 * Now you have 3 ways for the malloc function:
 *
 * 1. Do not change anything, use the original malloc
 *
 * 2. Use the clear_malloc function instead of the original malloc.
 *    In this case you must use the following define:
 *    #define malloc((_a)) clear_malloc((_a))
 *
 * 3. Use the fill_malloc function instead of the original malloc.
 *    In this case you must use the following define:
 *    #define malloc((_a)) fill_malloc((_a))
 *
 * We have figured out that there could exist some malloc problems
 * where variables are using without to be initialise. To find this
 * places, use the fill_malloc function. With this function we want
 * to initialize memory to some known bad state. This is quite easily
 * spotted in the debugger and will trap to an invalid address.
 *
 * clear_malloc can be used if you want to set not initialise
 * variable to 0.
 *
 * If you do not want to change the malloc function, to not use one of
 * the following macros. Which is the default way.
 */

/* #define malloc(_a) clear_malloc(_a) */
/* #define malloc(_a) fill_malloc(_a) */

/* GNU extensions to the C library that may be missing on some systems */
#ifndef HAVE_STRNDUP
char* strndup(const char *s, size_t n);
#endif /* HAVE_STRNDUP */

#ifndef HAVE_STRNLEN
size_t strnlen(const char *s, size_t maxlen);
#endif /* HAVE_STRNLEN */



/* generic socket functions for Windows and Posix */
static __inline int write_socket(int handle, const void *buffer, unsigned int count)
{
	return write(handle, buffer, count);
}

static __inline int read_socket(int handle, void *buffer, unsigned int count)
{
	return read(handle, buffer, count);
}

static __inline int close_socket(int sock)
{
	return close(sock);
}

static __inline void socket_nonblock(int fd)
{
	int oldopts = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, oldopts | O_NONBLOCK);
}

static __inline int socket_select(int max_fd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tv)
{
	return select(max_fd, rfds, wfds, efds, tv);
}

#ifndef HAVE_ELF_H

#include <helper/types.h>

typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef int32_t		Elf32_Sword;
typedef uint32_t	Elf32_Word;
typedef uint32_t	Elf32_Size;
typedef Elf32_Off	Elf32_Hashelt;

typedef struct
{
	unsigned char	e_ident[16];	/* Magic number and other info */
	Elf32_Half	e_type;			/* Object file type */
	Elf32_Half	e_machine;		/* Architecture */
	Elf32_Word	e_version;		/* Object file version */
	Elf32_Addr	e_entry;		/* Entry point virtual address */
	Elf32_Off	e_phoff;		/* Program header table file offset */
	Elf32_Off	e_shoff;		/* Section header table file offset */
	Elf32_Word	e_flags;		/* Processor-specific flags */
	Elf32_Half	e_ehsize;		/* ELF header size in bytes */
	Elf32_Half	e_phentsize;	/* Program header table entry size */
	Elf32_Half	e_phnum;		/* Program header table entry count */
	Elf32_Half	e_shentsize;	/* Section header table entry size */
	Elf32_Half	e_shnum;		/* Section header table entry count */
	Elf32_Half	e_shstrndx;		/* Section header string table index */
} Elf32_Ehdr;

#define	ELFMAG		"\177ELF"
#define	SELFMAG		4

#define EI_CLASS	4		/* File class byte index */
#define ELFCLASS32	1		/* 32-bit objects */
#define ELFCLASS64	2		/* 64-bit objects */

#define EI_DATA		5		/* Data encoding byte index */
#define ELFDATA2LSB	1		/* 2's complement, little endian */
#define ELFDATA2MSB	2		/* 2's complement, big endian */

typedef struct
{
	Elf32_Word p_type;		/* Segment type */
	Elf32_Off p_offset;		/* Segment file offset */
	Elf32_Addr p_vaddr;		/* Segment virtual address */
	Elf32_Addr p_paddr;		/* Segment physical address */
	Elf32_Size p_filesz;	/* Segment size in file */
	Elf32_Size p_memsz;		/* Segment size in memory */
	Elf32_Word p_flags;		/* Segment flags */
	Elf32_Size p_align;		/* Segment alignment */
} Elf32_Phdr;

#define PT_LOAD		1		/* Loadable program segment */

#endif /* HAVE_ELF_H */

#endif /* REPLACEMENTS_H */
