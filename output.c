#include "bsfirc.h"

extern int prompt_len;
extern char inputbuf[513];

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
	strftime(ts, 5, "%H:%M", now);
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
	prompt_len = strlen(BSF_PROMPT) + 1;

	printf("%s", BSF_PROMPT);
	printf(" %s", inputbuf);
	fflush(stdout);
}
