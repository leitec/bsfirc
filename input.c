#include "bsfirc.h"

extern char     inputbuf[513];
extern struct BSFirc *bsfirc;
extern struct Waiting *waiting;

/* PROTO */
void
get_input(void)
{
	unsigned char   inchr;
	struct Waiting *wtmp, *wtmp2;

	inchr = getchar();

	switch (inchr) {
	case '\n':
	case '\r':
		parse_input();
		memset(inputbuf, 0, sizeof(inputbuf));
		bsfirc->istyping = 0;

		for(wtmp = waiting; wtmp != NULL; wtmp = wtmp->next) {
			wtmp2 = wtmp;
			wtmp = wtmp->next;
			free(wtmp2->nick);
			free(wtmp2);
		}

		waiting = NULL;
			
		show_prompt();
		break;
	case 21:
		eraseline();
		memset(inputbuf, 0, sizeof(inputbuf));
		bsfirc->istyping = 0;
		show_prompt();
		break;
	case 12:

#if !defined(__MINGW32__) && !defined(PLAN9)
#ifdef __DJGPP__
		clrscr();
#else
		printf("\033[2J\033[1;1H");
#endif
		show_prompt();
#endif
		break;
	case '\b':
	case 127:
	case 4:
		if (inputbuf[0] == 0) {
			bsfirc->istyping = 0;
			break;
		}
		printf("\b \b");
		inputbuf[strlen(inputbuf) - 1] = 0;
		fflush(stdout);
		break;
	case 'r':
	case 'c':
		if (inputbuf[0] == 0) {
			if (inchr == 'r') {
				if (bsfirc->lastmsg) {
					printf("m%s ", bsfirc->lastmsg);
					bsfirc->istyping = 1;
					sprintf(inputbuf, "m%s ", bsfirc->lastmsg);
					fflush(stdout);
				}
			} else if (inchr == 'c') {
				if (bsfirc->lastchan) {
					printf("m%s ", bsfirc->lastchan);
					bsfirc->istyping = 1;
					sprintf(inputbuf, "m%s ", bsfirc->lastchan);
					fflush(stdout);
				}
			}
			break;
		}
	default:
		if (inchr < 32)
			break;

		bsfirc->istyping = 1;

		if (strlen(inputbuf) == 512)
			break;

		inputbuf[strlen(inputbuf)] = inchr;
		putchar(inchr);
		fflush(stdout);
	}
}

/* PROTO */
void
parse_input(void)
{
	if (strncasecmp(inputbuf, "q!", 2) == 0) {
		irclib_quit(bsfirc->handle, "Leaving");
		restore_tty();
		printf("\n");
		exit(-1);
	} else if (inputbuf[0] == 'm') {
		char           *pptr, *pptr2, *dest;
		int             offset;

		pptr2 = inputbuf + 1;

		pptr = strchr(pptr2, ' ');
		if (pptr == NULL) {
			printf("\nNo message to send.\n");
			return;
		}
		dest = malloc(pptr - pptr2 + 1);
		dest[pptr - pptr2] = 0;
		strncpy(dest, pptr2, pptr - pptr2);
		irclib_privmsg(bsfirc->handle, dest, pptr + 1);

		if(dest[0] == '#' || dest[0] == '&') {
			if(bsfirc->lastchan != NULL)
				free(bsfirc->lastchan);
			bsfirc->lastchan = strdup(dest);
		} else {
			if(bsfirc->lastmsg != NULL)
				free(bsfirc->lastmsg);
			bsfirc->lastmsg = strdup(dest);
		}
		
		eraseline();
		if(dest[0] == '#' || dest[0] == '&') {
			putchar('[');
#ifdef TIMESTAMPS_CHANMSG
			addts_short();
			putchar('/');
			offset = 6;
#else
			offset = 0;
#endif
			printf("%s] %s: ", dest, bsfirc->nick);
			offset += strlen(dest) + strlen(bsfirc->nick) + 5;
			wordwrap_print(pptr+1, offset);
			log_event(EVENT_CHANMSG, bsfirc->nick, NULL, dest, pptr+1);
		} else {
#ifdef TIMESTAMPS
		addts();
		putchar(' ');
		offset = 13;
#else
		offset = 2;
#endif

		offset += strlen(dest) + 2;
		printf("->%s", dest);
		printf(": ");
		wordwrap_print(pptr + 1, offset);
		log_event(EVENT_PRIVMSG, dest, NULL, NULL, pptr+1);
		}

		free(dest);
		return;
	} else if (inputbuf[0] == 'j') {
		irclib_join(bsfirc->handle, inputbuf + 1);
	} else if (inputbuf[0] == 'p') {
		irclib_part(bsfirc->handle, inputbuf + 1);
	} else if (inputbuf[0] == 'w') {
		if(inputbuf[1] == 0) {
			if(bsfirc->lastchan != NULL) {
				printf("\n");
				show_channel_users(bsfirc->lastchan);
				return;
			}
		} else {
			printf("\n");
			show_channel_users(inputbuf+1);
			return;
		}
	}

	printf("\n");
}
