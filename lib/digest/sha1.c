/*-
 * Copyright (c) 2012 Universitetet i Oslo
 * Copyright (c) 2012-2014 Dag-Erling Smørgrav
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
 *
 * Author: Dag-Erling Smørgrav <des@des.no>
 * Sponsor: the University of Oslo
 *
 * $Id$
 */

#include "cryb/impl.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#ifdef HAVE_ENDIAN_H
#define _BSD_SOURCE
#include <endian.h>
#endif

#include <cryb/digest.h>
#include <cryb/bitwise.h>

#include <cryb/sha1.h>

static uint32_t sha1_h[5] = {
	0x67452301U, 0xefcdab89U, 0x98badcfeU, 0x10325476U, 0xc3d2e1f0U,
};

static uint32_t sha1_k[4] = {
	0x5a827999U, 0x6ed9eba1U, 0x8f1bbcdcU, 0xca62c1d6U,
};

void
sha1_init(struct sha1_ctx *ctx)
{

	memset(ctx, 0, sizeof *ctx);
	memcpy(ctx->h, sha1_h, sizeof ctx->h);
	memcpy(ctx->k, sha1_k, sizeof ctx->k);
}

static void
sha1_compute(struct sha1_ctx *ctx, const uint8_t *block)
{
	uint32_t w[80], a, b, c, d, e, f, temp;

	memcpy(w, block, 64);
#if !WORDS_BIGENDIAN
	for (int i = 0; i < 16; ++i)
		w[i] = be32toh(w[i]);
#endif
	for (int i = 16; i < 80; ++i) {
		w[i] = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
		w[i] = rol(w[i], 1);
	}
	a = ctx->h[0];
	b = ctx->h[1];
	c = ctx->h[2];
	d = ctx->h[3];
	e = ctx->h[4];
	for (int t = 0; t < 80; ++t) {
		if (t < 20)
			f = (b & c) | ((~b) & d);
		else if (t < 40)
			f = b ^ c ^ d;
		else if (t < 60)
			f = (b & c) | (b & d) | (c & d);
		else
			f = b ^ c ^ d;
		temp = rol(a, 5) + f + e + w[t] + ctx->k[t/20];
		e = d;
		d = c;
		c = ror(b, 2);
		b = a;
		a = temp;
	}
	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
	ctx->h[4] += e;
}

void
sha1_update(struct sha1_ctx *ctx, const void *buf, size_t len)
{
	size_t copylen;

	while (len) {
		if (ctx->blocklen > 0 || len < 64) {
			copylen = sizeof ctx->block - ctx->blocklen;
			if (copylen > len)
				copylen = len;
			memcpy(ctx->block + ctx->blocklen, buf, copylen);
			ctx->blocklen += copylen;
			if (ctx->blocklen == 64) {
				sha1_compute(ctx, ctx->block);
				ctx->blocklen = 0;
				memset(ctx->block, 0, 64);
			}
		} else {
			copylen = 64;
			sha1_compute(ctx, buf);
		}
		ctx->bitlen += copylen * 8;
		buf += copylen;
		len -= copylen;
	}
}

void
sha1_final(struct sha1_ctx *ctx, void *digest)
{
	uint32_t hi, lo;

	ctx->block[ctx->blocklen++] = 0x80;
	if (ctx->blocklen > 56) {
		sha1_compute(ctx, ctx->block);
		ctx->blocklen = 0;
		memset(ctx->block, 0, 64);
	}
	hi = htobe32(ctx->bitlen >> 32);
	lo = htobe32(ctx->bitlen & 0xffffffffUL);
	memcpy(ctx->block + 56, &hi, 4);
	memcpy(ctx->block + 60, &lo, 4);
	ctx->blocklen = 64;
	sha1_compute(ctx, ctx->block);
	for (int i = 0; i < 5; ++i)
		ctx->h[i] = htobe32(ctx->h[i]);
	memcpy(digest, ctx->h, 20);
	memset(ctx, 0, sizeof *ctx);
}

void
sha1_complete(const void *buf, size_t len, void *digest)
{
	struct sha1_ctx ctx;

	sha1_init(&ctx);
	sha1_update(&ctx, buf, len);
	sha1_final(&ctx, digest);
}
