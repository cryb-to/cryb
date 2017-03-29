/*
 * Copyright (c) 2017 Dag-Erling Smørgrav
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

#include "cryb/impl.h"

#include <stdint.h>
#include <string.h>

#include <cryb/mpi.h>

#include "cryb_mpi_impl.h"

/*
 * Fast comparison of an MPI with a 32-bit integer
 */
int
mpi_cmp_abs_u32(const cryb_mpi *X, uint32_t u)
{

	/* if log2(|X|) > N then |X| > |i|  */
	if (X->msb >= 32)
		return (1);
	if (X->words[0] != u)
		return (X->words[0] > u ? 1 : -1);
	return (0);
}

int
mpi_cmp_u32(const cryb_mpi *X, uint32_t u)
{

	return (X->neg ? -1 : mpi_cmp_abs_u32(X, u));
}

int
mpi_cmp_i32(const cryb_mpi *X, int32_t i)
{

	if (X->neg) {
		if (i < 0)
			return (-mpi_cmp_abs_u32(X, -i));
		else
			return (-1);
	} else {
		if (i < 0)
			return (1);
		else
			return (mpi_cmp_abs_u32(X, i));
	}
}
