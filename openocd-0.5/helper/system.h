/***************************************************************************
 *   Copyright (C) 2006 by Dominic Rath <Dominic.Rath@gmx.de>              *
 *   Copyright (C) 2007-2008 by Øyvind Harboe <oyvind.harboe@zylin.com>    *
 *   Copyright (C) 2008 by Spencer Oliver <spen@spen-soft.co.uk>           *
 *   Copyright (C) 2009 by Zachary T Welch <zw@superlucidity.net>          *
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
#ifndef SYSTEM_H
#define SYSTEM_H

// standard C library header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>

// +++ AC_HEADER_TIME +++
# include <sys/time.h>
# include <time.h>
// --- AC_HEADER_TIME ---

// +++ platform specific headers +++
// --- platform specific headers ---

#include <sys/socket.h>
#include <sys/poll.h>

/* missing from eCos */
#define	EFAULT 14	/* Bad address */

#include <netinet/in.h>
#include <sys/select.h> /* select, FD_SET and friends (POSIX.1-2001) */
#include <sys/param.h> /* for MIN/MAX macros */
#include <unistd.h>
#include <fcntl.h>

#define true	1
#define false	0

#endif // SYSTEM_H
