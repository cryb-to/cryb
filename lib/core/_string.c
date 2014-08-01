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

/* size of static buffer used for short strings */
#define STATIC_BUF_SIZE	(16 * sizeof(char_t))

/* threshold at which we switch from exponential to linear growth */
#define LARGE_BUF_SIZE	(4096 * sizeof(char_t))

/* minimum buffer size to store len characters + terminating zero */
#define L2S(len) (((len) + 1) * sizeof(char_t))

/* n rounded up to nearest multiple of p */
#define RUP(n, p) ((((n) + (p) - 1) / (p)) * (p))

/*
 * Managed string structure
 */
struct cryb_string {
	char_t	*buf;		/* pointer to buffer */
	size_t	 size;		/* size of buffer in bytes */
	size_t	 len;		/* length of string in characters */
	char_t	 staticbuf[STATIC_BUF_SIZE];
};

/*
 * Allocate a new string
 */
string *
string_new(void)
{
	string *str;

	if ((str = malloc(sizeof *str)) == NULL)
		return (NULL);
	str->buf = str->staticbuf;
	str->size = sizeof str->staticbuf;
	str->len = 0;
	str->staticbuf[0] = 0;
	return (str);
}

/*
 * Duplicate an existing string
 */
string *
string_dup(const string *str)
{
	string *newstr;

	if ((newstr = string_new()) == NULL)
		return (NULL);
	if (string_expand(newstr, str->len) != 0) {
		string_delete(newstr);
		return (NULL);
	}
	memcpy(newstr->buf, str->buf, L2S(str->len));
	newstr->len = str->len;
	return (newstr);
}

/*
 * Delete a string
 */
void
string_delete(string *str)
{

	if (str != NULL) {
		if (str->buf != str->staticbuf)
			free(str->buf);
		free(str);
	}
}

/*
 * Expand the underlying storage for a string so it can hold up to newlen
 * characters.
 */
int
string_expand(string *str, size_t newlen)
{
	size_t newsize;
	char_t *newbuf;

	/* does it already fit? */
	if (L2S(newlen) <= str->size)
		return (0);

	/* compute the new size */
	if (L2S(newlen) < LARGE_BUF_SIZE) {
		/* below the threshold, grow exponentially. */
		newsize = str->size;
		while (newsize < L2S(newlen))
			newsize *= 2;
	} else {
		/* above it, grow linearly. */
		newsize = RUP(L2S(newlen), LARGE_BUF_SIZE);
	}

	/* allocate / reallocate */
	if (str->buf == str->staticbuf) {
		/* we've been using the static buffer until now */
		if ((newbuf = malloc(newsize)) == NULL)
			return (-1);
		memcpy(newbuf, str->staticbuf, L2S(str->len));
	} else {
		/* we're already using an allocated buffer */
		if ((newbuf = realloc(str->buf, newsize)) == NULL)
			return (-1);
	}

	/* replace */
	str->buf = newbuf;
	str->size = newsize;
	return (0);
}

/*
 * Shrink the underlying storage for a string to the minimum required to
 * hold its current contents.
 */
void
string_shrink(string *str)
{
	size_t newsize;
	char_t *newbuf;

	if (str->buf != str->staticbuf) {
		if (L2S(str->len) <= STATIC_BUF_SIZE) {
			memcpy(str->staticbuf, str->buf, L2S(str->len));
			free(str->buf);
			newbuf = str->staticbuf;
			newsize = STATIC_BUF_SIZE;
		} else if (L2S(str->len) >= LARGE_BUF_SIZE) {
			newsize = RUP(L2S(str->len), LARGE_BUF_SIZE);
			newbuf = realloc(str->buf, newsize);
		} else {
			newsize = LARGE_BUF_SIZE;
			newbuf = realloc(str->buf, newsize);
		}
		str->buf = newbuf;
		str->size = newsize;
	}
}

/*
 * Truncate a string to the specified length, and shrink the underlying
 * storage accordingly.
 */
ssize_t
string_trunc(string *str, size_t len)
{

	if (len < str->len) {
		str->buf[len] = 0;
		str->len = len;
		string_shrink(str);
	}
	return (str->len);
}

/*
 * Append a single character to the string.
 */
ssize_t
string_append_c(string *str, char_t ch)
{
	ssize_t ret;

	if ((ret = string_expand(str, str->len + 1)) < 0)
		return (ret);
	str->buf[str->len++] = ch;
	str->buf[str->len] = 0;
	return (str->len);
}

/*
 * Append a null-terminated string to the string.
 */
ssize_t
string_append_cs(string *str, const char_t *cs, size_t len)
{
	ssize_t ret;

	while (*cs && len--) {
		if ((ret = string_expand(str, str->len + 1)) < 0)
			return (ret);
		str->buf[str->len++] = *cs++;
		str->buf[str->len] = 0;
	}
	return (str->len);
}

/*
 * Append one string to another.
 */
ssize_t
string_append_string(string *str, const string *other, size_t len)
{
	ssize_t ret;

	if (len > other->len)
		len = other->len;
	if ((ret = string_expand(str, str->len + len)) < 0)
		return (ret);
	memcpy(str->buf + str->len, other->buf, len * sizeof(char_t));
	str->len += len;
	str->buf[str->len] = 0;
	return (str->len);
}

/*
 * Append to a string using printf()
 */
ssize_t
string_printf(string *str, const char_t *fmt, ...)
{
	ssize_t ret;
	va_list ap;

	va_start(ap, fmt);
	ret = string_vprintf(str, fmt, ap);
	va_end(ap);
	return (ret);
}

/*
 * Append to a string using vprintf()
 */
ssize_t
string_vprintf(string *str, const char_t *fmt, va_list ap)
{
	va_list ap2;
	ssize_t res, ret;
	int len;

	/*
	 * Try to to print into the remaining space.  If that fails,
	 * expand the underlying storage and try again.
	 */
	for (;;) {
		res = str->size / sizeof(char_t) - str->len;
		va_copy(ap2, ap);
		len = vsnprintf(str->buf + str->len, res, fmt, ap);
		va_end(ap2);
		if (len < res)
			break;
		str->buf[str->len] = 0;
		if ((ret = string_expand(str, str->len + len)) < 0)
			return (ret);
	}
	str->len += len;
	str->buf[str->len] = 0;
	return (str->len);
}
