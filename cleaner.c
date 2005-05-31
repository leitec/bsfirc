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

/**
 ** bsflite - bs-free AIM client
 **           ultralight version.
 **
 ** (C) 2003-2005 by Claudio Leite <leitec at leitec dot org>
 **
 ** NO WARRANTY. Read the file COPYING for more details.
 **/

#include "bsfirc.h"

#ifdef NETSPEAK_CLEANER

/**
 ** This is probably not the right way to do it.
 **
 ** Whenever I have to do anything like this, I really
 ** begin to wish I had taken computer science classes.
 **
 ** It works, however.
 **/

struct WordTable {
	char           *orig;
	char           *clean;
};

const struct WordTable table[] = {
	{"u", "you"},
	{"r", "are"},
	{"i", "I"},
	{"i'm", "I'm"},
	{"i've", "I've"},
	{"ive", "I've"},
	{"i'd", "I'd"},
	{"ur", "you're"},	/* there's no way to remove the ambiguity;
				 * pick one. */
	{"ne1", "anyone"},
	{"omg", "oh my god,"},
	{"teh", "the"},
	{"taht", "that"},
	{"funnay", "funny"},
	{"d00d", "dude"},
	{"dood", "dude"},
	{"liek", "like"},
	{"osmething", "something"},
	{NULL, NULL}
};

/* PROTO */
char           *
undo_netspeak(char *orig)
{
	char           *cleaned, *spcptr = orig;
	size_t          x, y, newlen, origlen = strlen(orig);
	int             found;

	newlen = origlen;

	while (1) {
		for (x = 0; table[x].orig != NULL; x++) {
			int             len = strlen(table[x].orig);
			if (strncmp(table[x].orig, spcptr, len) == 0) {
				if (spcptr[len] == ' ' || spcptr[len] == 0 || spcptr[len] == '.' || spcptr[len] == ',') {
					newlen += (ssize_t) strlen(table[x].clean) - len;
					break;
				}
			}
		}

		spcptr = strchr(spcptr, ' ');
		if (spcptr == NULL)
			break;

		spcptr++;
	}

	cleaned = malloc(newlen + 1);
	cleaned[0] = 0;

	for (x = 0; x < origlen; x++) {
		found = 0;

		if (x > 0) {
			if (orig[x - 1] != ' ') {
				strncat(cleaned, &orig[x], 1);
				continue;
			}
		}
		for (y = 0; table[y].orig != NULL; y++) {
			int             len = strlen(table[y].orig);

			if (orig[x + len] != ' ' && orig[x + len] != '.' && orig[x + len] != 0 && orig[x + len] != ',')
				continue;

			if (strncmp(table[y].orig, orig + x, len) == 0) {
				strcat(cleaned, table[y].clean);
				x += len - 1;
				found = 1;
				break;
			}
		}

		if (found)
			continue;

		strncat(cleaned, &orig[x], 1);
	}

	return cleaned;
}
#endif
