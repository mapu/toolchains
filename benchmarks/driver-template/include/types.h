/*
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
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
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Nathan Binkert
 */

/**
 * @file
 * Defines global host-dependent types:
 * Counter, Tick, and (indirectly) {int,uint}{8,16,32,64}_t.
 */

#ifndef __BASE_TYPES_HH__
#define __BASE_TYPES_HH__

#include <inttypes.h>

/** uint64_t constant */
#define ULL(N)          ((uint64_t)N##ULL)
/** int64_t constant */
#define LL(N)           ((int64_t)N##LL)

/** Statistics counter type.  Not much excuse for not using a 64-bit
 * integer here, but if you're desperate and only run short
 * simulations you could make this 32 bits.
 */
typedef int64_t Counter;

/**
 * Clock cycle count type.
 * @note using an unsigned breaks the cache.
 */
typedef int64_t Tick;
typedef uint64_t UTick;

/**
 * Address type
 * This will probably be moved somewhere else in the near future.
 * This should be at least as big as the biggest address width in use
 * in the system, which will probably be 64 bits.
 */
typedef uint64_t Addr;

typedef unsigned uint32_t;
typedef int int32_t;

#endif // __BASE_TYPES_HH__
