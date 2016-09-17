/*
 * Copyright (c) 2006-2007 Christophe Devine
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
/*
 *  The SHA-384 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include "cryb/impl.h"

#include <stdint.h>
#include <string.h>

#include <cryb/sha384.h>

/*
 * 64-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT64_BE
#define GET_UINT64_BE(n,b,i)				\
do {							\
    (n) = ( (uint64_t) (b)[(i)	  ] << 56 )		\
	| ( (uint64_t) (b)[(i) + 1] << 48 )		\
	| ( (uint64_t) (b)[(i) + 2] << 40 )		\
	| ( (uint64_t) (b)[(i) + 3] << 32 )		\
	| ( (uint64_t) (b)[(i) + 4] << 24 )		\
	| ( (uint64_t) (b)[(i) + 5] << 16 )		\
	| ( (uint64_t) (b)[(i) + 6] <<	8 )		\
	| ( (uint64_t) (b)[(i) + 7]	  );		\
} while (0)
#endif

#ifndef PUT_UINT64_BE
#define PUT_UINT64_BE(n,b,i)				\
do {							\
    (b)[(i)    ] = (uint8_t) ( (n) >> 56 );		\
    (b)[(i) + 1] = (uint8_t) ( (n) >> 48 );		\
    (b)[(i) + 2] = (uint8_t) ( (n) >> 40 );		\
    (b)[(i) + 3] = (uint8_t) ( (n) >> 32 );		\
    (b)[(i) + 4] = (uint8_t) ( (n) >> 24 );		\
    (b)[(i) + 5] = (uint8_t) ( (n) >> 16 );		\
    (b)[(i) + 6] = (uint8_t) ( (n) >>  8 );		\
    (b)[(i) + 7] = (uint8_t) ( (n)	 );		\
} while (0)
#endif

/* XXX */
#define UL64(x) x##ULL

/*
 * Round constants
 */
static const uint64_t K[80] =
{
    UL64(0x428A2F98D728AE22),  UL64(0x7137449123EF65CD),
    UL64(0xB5C0FBCFEC4D3B2F),  UL64(0xE9B5DBA58189DBBC),
    UL64(0x3956C25BF348B538),  UL64(0x59F111F1B605D019),
    UL64(0x923F82A4AF194F9B),  UL64(0xAB1C5ED5DA6D8118),
    UL64(0xD807AA98A3030242),  UL64(0x12835B0145706FBE),
    UL64(0x243185BE4EE4B28C),  UL64(0x550C7DC3D5FFB4E2),
    UL64(0x72BE5D74F27B896F),  UL64(0x80DEB1FE3B1696B1),
    UL64(0x9BDC06A725C71235),  UL64(0xC19BF174CF692694),
    UL64(0xE49B69C19EF14AD2),  UL64(0xEFBE4786384F25E3),
    UL64(0x0FC19DC68B8CD5B5),  UL64(0x240CA1CC77AC9C65),
    UL64(0x2DE92C6F592B0275),  UL64(0x4A7484AA6EA6E483),
    UL64(0x5CB0A9DCBD41FBD4),  UL64(0x76F988DA831153B5),
    UL64(0x983E5152EE66DFAB),  UL64(0xA831C66D2DB43210),
    UL64(0xB00327C898FB213F),  UL64(0xBF597FC7BEEF0EE4),
    UL64(0xC6E00BF33DA88FC2),  UL64(0xD5A79147930AA725),
    UL64(0x06CA6351E003826F),  UL64(0x142929670A0E6E70),
    UL64(0x27B70A8546D22FFC),  UL64(0x2E1B21385C26C926),
    UL64(0x4D2C6DFC5AC42AED),  UL64(0x53380D139D95B3DF),
    UL64(0x650A73548BAF63DE),  UL64(0x766A0ABB3C77B2A8),
    UL64(0x81C2C92E47EDAEE6),  UL64(0x92722C851482353B),
    UL64(0xA2BFE8A14CF10364),  UL64(0xA81A664BBC423001),
    UL64(0xC24B8B70D0F89791),  UL64(0xC76C51A30654BE30),
    UL64(0xD192E819D6EF5218),  UL64(0xD69906245565A910),
    UL64(0xF40E35855771202A),  UL64(0x106AA07032BBD1B8),
    UL64(0x19A4C116B8D2D0C8),  UL64(0x1E376C085141AB53),
    UL64(0x2748774CDF8EEB99),  UL64(0x34B0BCB5E19B48A8),
    UL64(0x391C0CB3C5C95A63),  UL64(0x4ED8AA4AE3418ACB),
    UL64(0x5B9CCA4F7763E373),  UL64(0x682E6FF3D6B2B8A3),
    UL64(0x748F82EE5DEFB2FC),  UL64(0x78A5636F43172F60),
    UL64(0x84C87814A1F0AB72),  UL64(0x8CC702081A6439EC),
    UL64(0x90BEFFFA23631E28),  UL64(0xA4506CEBDE82BDE9),
    UL64(0xBEF9A3F7B2C67915),  UL64(0xC67178F2E372532B),
    UL64(0xCA273ECEEA26619C),  UL64(0xD186B8C721C0C207),
    UL64(0xEADA7DD6CDE0EB1E),  UL64(0xF57D4F7FEE6ED178),
    UL64(0x06F067AA72176FBA),  UL64(0x0A637DC5A2C898A6),
    UL64(0x113F9804BEF90DAE),  UL64(0x1B710B35131C471B),
    UL64(0x28DB77F523047D84),  UL64(0x32CAAB7B40C72493),
    UL64(0x3C9EBE0A15C9BEBC),  UL64(0x431D67C49C100D4C),
    UL64(0x4CC5D4BECB3E42B6),  UL64(0x597F299CFC657E2A),
    UL64(0x5FCB6FAB3AD6FAEC),  UL64(0x6C44198C4A475817)
};

/*
 * SHA-384 context setup
 */
void sha384_init( sha384_ctx *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = UL64(0xCBBB9D5DC1059ED8);
    ctx->state[1] = UL64(0x629A292A367CD507);
    ctx->state[2] = UL64(0x9159015A3070DD17);
    ctx->state[3] = UL64(0x152FECD8F70E5939);
    ctx->state[4] = UL64(0x67332667FFC00B31);
    ctx->state[5] = UL64(0x8EB44A8768581511);
    ctx->state[6] = UL64(0xDB0C2E0D64F98FA7);
    ctx->state[7] = UL64(0x47B5481DBEFA4FA4);
}

static void sha384_process( sha384_ctx *ctx, const uint8_t *data )
{
    int i;
    uint64_t temp1, temp2, W[80];
    uint64_t A, B, C, D, E, F, G, H;

#define	 SHR(x,n) (x >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (64 - n)))

#define S0(x) (ROTR(x, 1) ^ ROTR(x, 8) ^  SHR(x, 7))
#define S1(x) (ROTR(x,19) ^ ROTR(x,61) ^  SHR(x, 6))

#define S2(x) (ROTR(x,28) ^ ROTR(x,34) ^ ROTR(x,39))
#define S3(x) (ROTR(x,14) ^ ROTR(x,18) ^ ROTR(x,41))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define P(a,b,c,d,e,f,g,h,x,K)			\
{						\
    temp1 = h + S3(e) + F1(e,f,g) + K + x;	\
    temp2 = S2(a) + F0(a,b,c);			\
    d += temp1; h = temp1 + temp2;		\
}

    for( i = 0; i < 16; i++ )
    {
	GET_UINT64_BE( W[i], data, i << 3 );
    }

    for( ; i < 80; i++ )
    {
	W[i] = S1(W[i -	 2]) + W[i -  7] +
	       S0(W[i - 15]) + W[i - 16];
    }

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];
    i = 0;

    do
    {
	P( A, B, C, D, E, F, G, H, W[i], K[i] ); i++;
	P( H, A, B, C, D, E, F, G, W[i], K[i] ); i++;
	P( G, H, A, B, C, D, E, F, W[i], K[i] ); i++;
	P( F, G, H, A, B, C, D, E, W[i], K[i] ); i++;
	P( E, F, G, H, A, B, C, D, W[i], K[i] ); i++;
	P( D, E, F, G, H, A, B, C, W[i], K[i] ); i++;
	P( C, D, E, F, G, H, A, B, W[i], K[i] ); i++;
	P( B, C, D, E, F, G, H, A, W[i], K[i] ); i++;
    }
    while( i < 80 );

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
    ctx->state[5] += F;
    ctx->state[6] += G;
    ctx->state[7] += H;
}

/*
 * SHA-384 process buffer
 */
void sha384_update( sha384_ctx *ctx, const void *input, int ilen )
{
    int fill;
    uint64_t left;

    if( ilen <= 0 )
	return;

    left = ctx->total[0] & 0x7F;
    fill = (int)( 128 - left );

    ctx->total[0] += ilen;

    if( ctx->total[0] < (uint64_t) ilen )
	ctx->total[1]++;

    if( left && ilen >= fill )
    {
	memcpy( (ctx->buffer + left), input, fill );
	sha384_process( ctx, ctx->buffer );
	input += fill;
	ilen  -= fill;
	left = 0;
    }

    while( ilen >= 128 )
    {
	sha384_process( ctx, input );
	input += 128;
	ilen  -= 128;
    }

    if( ilen > 0 )
    {
	memcpy( (ctx->buffer + left), input, ilen );
    }
}

static const uint8_t sha384_padding[128] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-384 final digest
 */
void sha384_final( sha384_ctx *ctx, uint8_t *output )
{
    int last, padn;
    uint64_t high, low;
    uint8_t msglen[16];

    high = ( ctx->total[0] >> 61 )
	 | ( ctx->total[1] <<  3 );
    low	 = ( ctx->total[0] <<  3 );

    PUT_UINT64_BE( high, msglen, 0 );
    PUT_UINT64_BE( low,	 msglen, 8 );

    last = (int)( ctx->total[0] & 0x7F );
    padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

    sha384_update( ctx, (const uint8_t *) sha384_padding, padn );
    sha384_update( ctx, msglen, 16 );

    PUT_UINT64_BE( ctx->state[0], output,  0 );
    PUT_UINT64_BE( ctx->state[1], output,  8 );
    PUT_UINT64_BE( ctx->state[2], output, 16 );
    PUT_UINT64_BE( ctx->state[3], output, 24 );
    PUT_UINT64_BE( ctx->state[4], output, 32 );
    PUT_UINT64_BE( ctx->state[5], output, 40 );
}

/*
 * output = SHA-384( input buffer )
 */
void sha384_complete( const void *input, int ilen, uint8_t *output )
{
    sha384_ctx ctx;

    sha384_init( &ctx );
    sha384_update( &ctx, input, ilen );
    sha384_final( &ctx, output );

    memset( &ctx, 0, sizeof( sha384_ctx ) );
}

digest_algorithm sha384_digest = {
	.name			 = "sha384",
	.contextlen		 = sizeof(sha384_ctx),
	.blocklen		 = SHA384_BLOCK_LEN,
	.digestlen		 = SHA384_DIGEST_LEN,
	.init			 = (digest_init_func)sha384_init,
	.update			 = (digest_update_func)sha384_update,
	.final			 = (digest_final_func)sha384_final,
	.complete		 = (digest_complete_func)sha384_complete,
};