/*-
 * Copyright (c) 2014 Dag-Erling Smørgrav
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef CRYB_ENDIAN_H_INCLUDED
#define CRYB_ENDIAN_H_INCLUDED

#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#ifdef HAVE_ENDIAN_H
#include <endian.h>
#endif

#ifndef HAVE_BE32ENC
#define be32enc cryb_be32enc
#endif
#ifndef HAVE_BE32DEC
#define be32dec cryb_be32dec
#endif
#ifndef HAVE_BE64ENC
#define be64enc cryb_be64enc
#endif
#ifndef HAVE_BE64DEC
#define be64dec cryb_be64dec
#endif
#ifndef HAVE_LE32ENC
#define le32enc cryb_le32enc
#endif
#ifndef HAVE_LE32DEC
#define le32dec cryb_le32dec
#endif
#ifndef HAVE_LE64ENC
#define le64enc cryb_le64enc
#endif
#ifndef HAVE_LE64DEC
#define le64dec cryb_le64dec
#endif

static inline void
cryb_be32enc(void *p, uint32_t u32)
{
	((uint8_t *)p)[3] = u32 & 0xff;
	((uint8_t *)p)[2] = (u32 >> 8) & 0xff;
	((uint8_t *)p)[1] = (u32 >> 16) & 0xff;
	((uint8_t *)p)[0] = (u32 >> 24) & 0xff;
}

static inline uint32_t
cryb_be32dec(const void *p)
{
	return ((uint32_t)((uint8_t *)p)[3] |
	    (uint32_t)((uint8_t *)p)[2] << 8 |
	    (uint32_t)((uint8_t *)p)[1] << 16 |
	    (uint32_t)((uint8_t *)p)[0] << 24);
}

static inline void
cryb_be64enc(void *p, uint64_t u64)
{
	((uint8_t *)p)[7] = u64 & 0xff;
	((uint8_t *)p)[6] = (u64 >> 8) & 0xff;
	((uint8_t *)p)[5] = (u64 >> 16) & 0xff;
	((uint8_t *)p)[4] = (u64 >> 24) & 0xff;
	((uint8_t *)p)[3] = (u64 >> 32) & 0xff;
	((uint8_t *)p)[2] = (u64 >> 40) & 0xff;
	((uint8_t *)p)[1] = (u64 >> 48) & 0xff;
	((uint8_t *)p)[0] = (u64 >> 56) & 0xff;
}

static inline uint64_t
cryb_be64dec(const void *p)
{
	return ((uint64_t)((uint8_t *)p)[7] |
	    (uint64_t)((uint8_t *)p)[6] << 8 |
	    (uint64_t)((uint8_t *)p)[5] << 16 |
	    (uint64_t)((uint8_t *)p)[4] << 24 |
	    (uint64_t)((uint8_t *)p)[3] << 32 |
	    (uint64_t)((uint8_t *)p)[2] << 40 |
	    (uint64_t)((uint8_t *)p)[1] << 48 |
	    (uint64_t)((uint8_t *)p)[0] << 56);
}

static inline void
cryb_le32enc(void *p, uint32_t u32)
{
	((uint8_t *)p)[0] = u32 & 0xff;
	((uint8_t *)p)[1] = (u32 >> 8) & 0xff;
	((uint8_t *)p)[2] = (u32 >> 16) & 0xff;
	((uint8_t *)p)[3] = (u32 >> 24) & 0xff;
}

static inline uint32_t
cryb_le32dec(const void *p)
{
	return ((uint32_t)((uint8_t *)p)[0] |
	    (uint32_t)((uint8_t *)p)[1] << 8 |
	    (uint32_t)((uint8_t *)p)[2] << 16 |
	    (uint32_t)((uint8_t *)p)[3] << 24);
}

static inline void
cryb_le64enc(void *p, uint64_t u64)
{
	((uint8_t *)p)[0] = u64 & 0xff;
	((uint8_t *)p)[1] = (u64 >> 8) & 0xff;
	((uint8_t *)p)[2] = (u64 >> 16) & 0xff;
	((uint8_t *)p)[3] = (u64 >> 24) & 0xff;
	((uint8_t *)p)[4] = (u64 >> 32) & 0xff;
	((uint8_t *)p)[5] = (u64 >> 40) & 0xff;
	((uint8_t *)p)[6] = (u64 >> 48) & 0xff;
	((uint8_t *)p)[7] = (u64 >> 56) & 0xff;
}

static inline uint64_t
cryb_le64dec(const void *p)
{
	return ((uint64_t)((uint8_t *)p)[0] |
	    (uint64_t)((uint8_t *)p)[1] << 8 |
	    (uint64_t)((uint8_t *)p)[2] << 16 |
	    (uint64_t)((uint8_t *)p)[3] << 24 |
	    (uint64_t)((uint8_t *)p)[4] << 32 |
	    (uint64_t)((uint8_t *)p)[5] << 40 |
	    (uint64_t)((uint8_t *)p)[6] << 48 |
	    (uint64_t)((uint8_t *)p)[7] << 56);
}

#endif