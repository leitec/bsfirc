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

#include "irclib/irclib.h"

#ifdef TIMESTAMPS
#define TIMESTAMPS_CHANMSG
#endif

struct BSFirc {
	void *handle;
	char *lastmsg;
	char *lastchan;
	char *nick;
	char *server;
	int istyping;
	int ready;
};

struct Waiting {
	char *nick;
	struct Waiting *next;
};

struct UserList {
	char *name;
	uint8_t mode;
	struct UserList *next;
};

#define MODE_NONE  0x00
#define MODE_VOICE 0x01
#define MODE_OP    0x02

struct ChannelList {
	char *chan;
	struct UserList *users;
	struct ChannelList *next;
};

enum {
	EVENT_PRIVMSG,
	EVENT_CHANJOIN,
	EVENT_CHANPART,
	EVENT_CHANMSG,
	EVENT_QUIT,
	EVENT_SENDPRIVMSG
};

#define BSF_PROMPT "::"
#define USERLIST_ECHOSTR "  "

#include "protos.h"
