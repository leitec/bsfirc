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
#include <signal.h>

#include <signal.h>

#define chomp(x) { if(x[strlen(x)-2] == '\r') x[strlen(x)-2] = 0; else if(x[strlen(x)-1] == '\n') x[strlen(x)-1] = 0; }

char            inputbuf[513];
int             prompt_len;
struct BSFirc  *bsfirc;
struct ChannelList *chanlist = NULL;
struct Waiting *waiting = NULL;

int 
main(int argc, char **argv)
{
	char           *ircsrv, *ircnick, *ircname, *user;
	fd_set          readfs;
	struct timeval  tm;

	ircsrv = getenv("IRCSERVER");
	if (ircsrv == NULL) {
		printf("** IRC server: ");
		fflush(stdout);
		fgets(inputbuf, sizeof(inputbuf), stdin);
		chomp(inputbuf);
		ircsrv = strdup(inputbuf);
	}
	ircnick = getenv("IRCNICK");
	if (ircnick == NULL) {
		printf("** Nickname: ");
		fflush(stdout);
		fgets(inputbuf, sizeof(inputbuf), stdin);
		chomp(inputbuf);
		ircnick = strdup(inputbuf);
	}
	ircname = getenv("IRCNAME");
	if (ircname == NULL) {
		ircname = strdup("bsfirc user");
	}
	setup_tty();
	get_screen_size();

	/*
	 * We don't like Ctrl-C
	 */
	signal(SIGINT, SIG_IGN);
#if !defined(__MINGW32__) && !defined(__DJGPP__)
#ifndef PLAN9
	signal(SIGWINCH, &sigwinch_handler);
#endif
	/*
	 * or Ctrl-Z in UNIX land
	 */
	signal(SIGTSTP, SIG_IGN);
#endif

	bsfirc = malloc(sizeof(struct BSFirc));
	bsfirc->istyping = 0;
	bsfirc->lastmsg = 0;
	bsfirc->lastchan = 0;
	bsfirc->ready = 0;
	bsfirc->server = NULL;

	user = getenv("USER");

	if (user == NULL)
		user = strdup("bsfirc");

	bsfirc->handle = (void *) irclib_create_handle();
	irclib_setnick(bsfirc->handle, ircnick);
	irclib_setname(bsfirc->handle, ircname);
	irclib_setusername(bsfirc->handle, user);
	bsfirc->nick = strdup(ircnick);

	irclib_register_callback(bsfirc->handle, IRCLIB_MOTD, (void (*) (void *,...)) irc_motd);
	irclib_register_callback(bsfirc->handle, IRCLIB_READY, (void (*) (void *,...)) irc_ready);
	irclib_register_callback(bsfirc->handle, IRCLIB_JOIN, (void (*) (void *,...)) irc_join);
	irclib_register_callback(bsfirc->handle, IRCLIB_PART, (void (*) (void *,...)) irc_part);
	irclib_register_callback(bsfirc->handle, IRCLIB_PRIVMSG, (void (*) (void *,...)) irc_msg);
	irclib_register_callback(bsfirc->handle, IRCLIB_QUIT, (void (*) (void *,...)) irc_quit);
	irclib_register_callback(bsfirc->handle, IRCLIB_CHANUSER, (void (*) (void *,...)) irc_addchanuser);
	irclib_register_callback(bsfirc->handle, IRCLIB_NOTICE, (void (*) (void *,...)) irc_notice);
	irclib_register_callback(bsfirc->handle, IRCLIB_NAMESDONE, (void (*) (void *,...)) irc_namesdone);
	irclib_register_callback(bsfirc->handle, IRCLIB_NICKINUSE, (void (*) (void *,...)) irc_nickinuse);
	irclib_register_callback(bsfirc->handle, IRCLIB_CTCP, (void (*) (void *,...)) irc_ctcp);
	irclib_register_callback(bsfirc->handle, IRCLIB_MODE, (void (*) (void *,...)) irc_mode);
	irclib_register_callback(bsfirc->handle, IRCLIB_SERVER_NAME, (void (*) (void *,...)) irc_server_name);
	irclib_register_callback(bsfirc->handle, IRCLIB_TOPIC, (void (*) (void *,...)) irc_topic);
	irclib_register_callback(bsfirc->handle, IRCLIB_NOTICE_AUTH, (void (*) (void *,...)) irc_notice_auth);
	irclib_register_callback(bsfirc->handle, IRCLIB_WHOIS_USERHOST, (void (*) (void *,...)) irc_whois_userhost);
	irclib_register_callback(bsfirc->handle, IRCLIB_WHOIS_SERVER, (void (*) (void *,...)) irc_whois_server);
	irclib_register_callback(bsfirc->handle, IRCLIB_WHOIS_CHANNELS, (void (*) (void *,...)) irc_whois_channels);
	irclib_register_callback(bsfirc->handle, IRCLIB_ERROR, (void (*) (void *,...)) error_callback);

	open_log_dir();

	printf("** bsfirc started.\n");
	printf("** Server set to %s.\n", ircsrv);
	irclib_connect(bsfirc->handle, ircsrv, 6667);

	while (!irclib_connected(bsfirc->handle));
	printf("** Connected.\n");

	memset(inputbuf, 0, sizeof(inputbuf));
	show_prompt();

	while (1) {
		tm.tv_sec = 2;
		tm.tv_usec = 500000;

		FD_ZERO(&readfs);
		FD_SET(0, &readfs);
		if (irclib_select(1, &readfs, NULL, NULL, &tm) != IRCLIB_RET_OK) {
			if (errno == EINTR)
				continue;
		}
		if (FD_ISSET(0, &readfs)) {
			get_input();
		}
	}
}

/* PROTO */
void
error_callback(void *handle, int code)
{
	eraseline();
	printf("** ");
	addts();
	putchar(' ');

	switch (code) {
	case IRCLIB_ERROR_DISCONNECTED:
		printf("Disconnected.\n");
		break;
	default:
		printf("Unknown error type %d\n", code);
	}

	show_prompt();
}
