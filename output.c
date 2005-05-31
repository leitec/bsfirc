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

#include "bsfirc.h"

#ifdef PLAN9
#include  <time.h>
#endif

extern int prompt_len;
extern char inputbuf[513];
extern struct Waiting *waiting;

/* PROTO */
void
addts(void)
{
	char ts[11];
	struct tm *now;
	time_t t;

	t = time(0);
	now = localtime(&t);
	strftime(ts, 11, "[%H:%M:%S]", now);
	printf("%s", ts);
}

/* PROTO */
void
addts_short(void)
{
	char ts[6];
	struct tm *now;
	time_t t;

	t = time(0);
	now = localtime(&t);
	strftime(ts, 6, "%H:%M", now);
	printf("%s", ts);
}

/* PROTO */
void
eraseline(void)
{
	int x, l = strlen(inputbuf) + prompt_len;

	for(x = 0; x < l; x++)
		printf("\b \b");
}

/* PROTO */
void
show_prompt(void)
{
	struct Waiting *wtr;

	prompt_len = strlen(BSF_PROMPT) + 1;

	if(waiting != NULL) {
		for(wtr = waiting; wtr != NULL; wtr = wtr->next) {
			prompt_len += strlen(wtr->nick) + 3;
			printf("[%s] ", wtr->nick);
		}
	}

	printf("%s", BSF_PROMPT);
	printf(" %s", inputbuf);
	fflush(stdout);
}
