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

extern struct BSFirc *bsfirc;
extern struct Waiting *waiting;

/* PROTO */
void
irc_topic(void *h, char *chan, char *topic)
{
	eraseline();
	printf("** Topic for %s: ", chan);
	wordwrap_print(topic, 15+strlen(chan));
	show_prompt();
}

/* PROTO */
void
irc_motd(void *h, char *motd)
{
	eraseline();
	printf("** %s\n", motd);
	show_prompt();
}

/* PROTO */
void
irc_notice_auth(void *h, char *msg)
{
	eraseline();
	printf("** %s\n", msg);
	show_prompt();
}

/* PROTO */
void
irc_ready(void *h)
{
	bsfirc->ready = 1;
}

/* PROTO */
void
irc_server_name(void *h, char *name)
{
	if(bsfirc->server != NULL)
		free(bsfirc->server);

	bsfirc->server = strdup(name);
}

/* PROTO */
void
irc_join(void *h, char *nick, char *host, char *channel)
{
	add_channel_user(nick, channel, 0);

	eraseline();
	printf("** ");
	addts();
	printf(" %s has joined %s.\n", nick, channel);
	log_event(EVENT_CHANJOIN, nick, host, channel, NULL);

	if(strcasecmp(bsfirc->nick, nick) == 0) {
		if(bsfirc->lastchan != NULL)
			free(bsfirc->lastchan);

		bsfirc->lastchan = strdup(channel);
	}

	show_prompt();
}

/* PROTO */
void
irc_addchanuser(void *h, char *chan, char *nick)
{
	if(nick[0] == '@') {
		add_channel_user(nick+1, chan, MODE_OP);
	} else if(nick[0] == '+') {
		add_channel_user(nick+1, chan, MODE_VOICE);
	} else {
		add_channel_user(nick, chan, 0);
	}
}

/* PROTO */
void
irc_namesdone(void *h, char *chan)
{
	eraseline();

	show_channel_users(chan);
	
	show_prompt();
}


/* PROTO */
void
irc_part(void *h, char *nick, char *host, char *channel)
{
	delete_channel_user(nick, channel);

	eraseline();
	printf("** ");
	addts();
	printf(" %s has left %s.\n", nick, channel);
	log_event(EVENT_CHANPART, nick, host, channel, NULL);
	show_prompt();
}

/* PROTO */
void
irc_quit(void *h, char *nick, char *msg)
{
	eraseline();
	printf("** ");
	addts();
	printf(" %s has quit: %s\n", nick, msg);

	/*
	 * This also handles writing to the logs 
	 */
	process_quit(nick, msg);
	
	show_prompt();
}

/* PROTO */
void
irc_nickinuse(void *h, char *nick)
{
	size_t notreallyrandomvalue;

	eraseline();
	printf("** %s: Nickname already in use.\n", nick);

	if(bsfirc->ready == 0) {
		char *newnick = strdup(nick);
		notreallyrandomvalue = (size_t)time(NULL) % strlen(bsfirc->nick);
		newnick[notreallyrandomvalue] = 'A'+notreallyrandomvalue;
		printf("** Trying \"%s\"\n", newnick);
		irclib_setnick(h, newnick);
		if(bsfirc->nick != NULL) 
			free(bsfirc->nick);

		bsfirc->nick = newnick;
	}

	show_prompt();
}
		
/* PROTO */
void
irc_ctcp(void *h, char *nick, char *host, char *target, char *msg)
{
	int offset;

	eraseline();

	if(strncmp(msg, "ACTION ", 7) == 0) {
		putchar('[');
#ifdef TIMESTAMPS
		addts_short();
		putchar('/');
		offset = 6;
#else
		offset = 0;
#endif
		printf("%s] * %s ", target, nick);
		offset += strlen(target) + strlen(nick) + 6;
		wordwrap_print(msg+7, offset);
	}

	show_prompt();
}

/* PROTO */
void
irc_mode(void *h, char *nick, char *host, char *target, int plus, int mode, char *arg)
{
	eraseline();

	switch(mode) {
	case C_MODE_OP:
		change_user_mode(arg, target, plus, C_MODE_OP);
		printf("** ");
		addts();
		printf(" %s has been %s on %s by %s.\n", arg, (plus == 1 ? "opped" : "deopped"), target, nick);
		break;
	}

	show_prompt();
}

/* PROTO */
void
irc_notice(void *h, char *nick, char *host, char *msg)
{
	int offset;

	eraseline();
#ifdef TIMESTAMPS
	addts();
	putchar(' ');
	offset = 11;
#else
	offset = 0;
#endif
	printf("!%s! ", nick);
	offset += 3 + strlen(nick);

	wordwrap_print(msg, offset);
	show_prompt();
}

/* PROTO */
void
irc_msg(void *h, char *nick, char *host, char *target, char *msg)
{
	int             offset, found = 0;
	struct Waiting *wtr, *wtmp;
#ifdef NETSPEAK_CLEANER
	char *cleanmsg = undo_netspeak(msg);
#endif

	if (bsfirc->istyping == 0) {
		if (target[0] == '#' || target[0] == '&') {
			if (bsfirc->lastchan != 0)
				free(bsfirc->lastchan);
			bsfirc->lastchan = strdup(target);
		} else {
			if (bsfirc->lastmsg != 0)
				free(bsfirc->lastmsg);

			bsfirc->lastmsg = strdup(nick);
		}
	}
	eraseline();

	if (target[0] == '#' || target[0] == '&') {
		putchar('[');
#ifdef TIMESTAMPS_CHANMSG
		addts_short();
		putchar('/');
		offset = 6;
#else
		offset = 0;
#endif
		printf("%s] (%s) ", target, nick);

		offset += strlen(target) + strlen(nick) + 6;
#ifdef NETSPEAK_CLEANER
		wordwrap_print(cleanmsg, offset);
#else
		wordwrap_print(msg, offset);
#endif
		log_event(EVENT_CHANMSG, nick, host, target, msg);
	} else {
		for(wtr = waiting; wtr != NULL; wtr = wtr->next) {
			if(strcasecmp(wtr->nick, nick) == 0) {
				found = 1;
				break;
			}
		}

		if(!found) {
			wtmp = malloc(sizeof(struct Waiting));
			wtmp->next = NULL;
			wtmp->nick = strdup(nick);
			if(waiting == NULL) {
				waiting = wtmp;
			} else {
				for(wtr = waiting; wtr->next != NULL; wtr = wtr->next);
				wtr->next = wtmp;
			}
		}

#ifdef TIMESTAMPS
		addts();
		putchar(' ');
		offset = 11;
#else
		offset = 0;
#endif
		printf("%s: ", nick);
		offset += strlen(nick) + 2;
#ifdef NETSPEAK_CLEANER
		wordwrap_print(cleanmsg, offset);
#else
		wordwrap_print(msg, offset);
#endif
		log_event(EVENT_PRIVMSG, nick, host, target, msg);
	}

#ifdef NETSPEAK_CLEANER
	free(cleanmsg);
#endif
	show_prompt();
}

/* PROTO */
void
irc_whois_userhost(void *h, char *nick, char *user, char *host, char *name)
{
	eraseline();
	printf("** [%s] is: %s@%s (%s)\n", nick, user, host, name);
	show_prompt();
}

/* PROTO */
void
irc_whois_channels(void *h, char *nick, char *channels)
{
	eraseline();
	printf("** [%s] in: ", nick);
	wordwrap_print(channels, 10+strlen(nick));
	show_prompt();
}

/* PROTO */
void
irc_whois_server(void *h, char *nick, char *server, char *serverdescr)
{
	eraseline();
	printf("** [%s] on: %s (%s)\n", nick, server, serverdescr);
	show_prompt();
}
