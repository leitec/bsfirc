#include "bsfirc.h"
#include <signal.h>

#include <signal.h>

char inputbuf[513];
int prompt_len;
struct BSFirc *bsfirc;


int main(int argc, char **argv)
{
	char *ircsrv, *ircnick, *ircname, *user;
	fd_set readfs;
	struct timeval tm;

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

	ircsrv = getenv("IRCSERVER");
	ircnick = getenv("IRCNICK");
	ircname = getenv("IRCNAME");
	user = getenv("USER");

	bsfirc->handle = irclib_create_handle();
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

	open_log_dir();

	printf("** bsfirc started.\n");
	printf("** Server set to %s.\n", ircsrv);
	irclib_connect(bsfirc->handle, ircsrv, 6667);

	while(!irclib_connected(bsfirc->handle));
	printf("** Connected.\n");

	memset(inputbuf, 0, sizeof(inputbuf));
	show_prompt();

	while(1) {
		tm.tv_sec = 2;
		tm.tv_usec = 500000;

		FD_ZERO(&readfs);
		FD_SET(0, &readfs);
		irclib_select(1, &readfs, NULL, NULL, &tm);

		if(FD_ISSET(0, &readfs)) {
			get_input();
		}
	}
}
