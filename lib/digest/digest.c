/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012 Dag-Erling Smørgrav
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
 * $Cryb$
 */

#include "cryb/impl.h"

#include <string.h>

#include <cryb/md2.h>
#include <cryb/md4.h>
#include <cryb/md5.h>
#include <cryb/sha1.h>
#include <cryb/sha256.h>
#include <cryb/sha384.h>
#include <cryb/sha512.h>

static const digest_algorithm **cryb_digest_algorithms;

static void
init_digest_algorithms(void)
{
	static const digest_algorithm *algorithms[] = {
		&md2_algorithm,
		&md4_algorithm,
		&md5_algorithm,
		&sha1_algorithm,
		&sha256_algorithm,
		&sha384_algorithm,
		&sha512_algorithm,
		NULL
	};
	cryb_digest_algorithms = algorithms;
}

const digest_algorithm *
get_digest_algorithm(const char *name)
{
	const digest_algorithm **algp;

	if (cryb_digest_algorithms == NULL)
		cryb_init_digest_algorithms();
	for (algp = cryb_digest_algorithms; *algp != NULL; ++algp)
		if (strcasecmp((*algp)->name, name) == 0)
			return (*algp);
	return (NULL);
}
