/*
 * Copyright (C) 2005, Claudio Leite All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Neither the name of the BSF Software Project nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#define _RESEARCH_SOURCE
#define _LOCK_EXTENSION
#define _QLOCK_EXTENSION
#define _BSD_EXTENSION
#define _POSIX_EXTENSION
#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <u.h>
#include <fmt.h>
#include <lock.h>
#include <qlock.h>
#include <draw.h>

/* PROTO */
int
getwidth(void)
{
	int             linewidth, charwidth, fd, num;
	char            buf[128], *fn;
	Font           *font;

	if ((fn = getenv("font")) == NULL)
		return -1;
	if ((font = openfont(NULL, fn)) == NULL)
		return -1;
	if ((fd = open("/dev/window", O_RDONLY)) < 0) {
		font = NULL;
		return -1;
	}
	num = read(fd, buf, 60);
	close(fd);

	if (num < 60) {
		font = NULL;
		return -1;
	}
	buf[num] = 0;

	linewidth = atoi(buf + 36) - atoi(buf + 12) - 25;
	charwidth = stringwidth(font, "0");

	return linewidth / charwidth;
}
