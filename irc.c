#include "bsfirc.h"

extern struct BSFirc *bsfirc;

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
irc_ready(void *h)
{
}

/* PROTO */
void
irc_join(void *h, char *nick, char *host, char *channel)
{
	eraseline();
	printf("** ");
	addts();
	printf(" %s has joined %s.\n", nick, channel);
	show_prompt();
}

/* PROTO */
void
irc_part(void *h, char *nick, char *host, char *channel)
{
	eraseline();
	printf("** ");
	addts();
	printf(" %s has left %s.\n", nick, channel);
	show_prompt();
}

/* PROTO */
void
irc_msg(void *h, char *nick, char *host, char *target, char *msg)
{
	int             offset;

	if (bsfirc->istyping == 0) {
		if (target[0] == '#') {
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

	if (target[0] == '#') {
		putchar('[');
#ifdef TIMESTAMPS_CHANMSG
		addts_short();
		putchar('/');
		offset = 6;
#else
		offset = 0;
#endif
		printf("%s] %s: ", target, nick);

		offset += strlen(target) + strlen(nick) + 5;
		wordwrap_print(msg, offset);
	} else {
#ifdef TIMESTAMPS
		addts();
		putchar(' ');
		offset = 11;
#else
		offset = 0;
#endif
		printf("%s: ", nick);
		offset += strlen(nick) + 2;
		wordwrap_print(msg, offset);
	}

	show_prompt();
}
