#include "bsfirc.h"

extern struct BSFirc *bsfirc;
extern struct ChannelList *chanlist;
extern int      screen_cols;

/* PROTO */
void
show_channel_users(char *chan)
{
    struct ChannelList *tr;
    struct UserList *utr;
    int             col, row, count;
    uint8_t         ch;
    int             longest, numcols, percol, longest_column[5] = {0, 0, 0, 0, 0};

    printf(":: ");

    /*
     * I'm writing this while tired and somewhat in a daze.
     * It may be completely hideous.
     */

    for (tr = chanlist; tr != NULL; tr = tr->next) {
	if (strcasecmp(tr->chan, chan) == 0) {
	    /*
	     * find longest nick. inefficient, I know.
	     */

	    /*
	     * we could calculate some of this stuff when users join/leave,
	     * but I think that happens more frequently than users displaying
	     * the list of names in the channel. So, let's do it here.
	     */

	    for (longest = 0, utr = tr->users; utr != NULL; utr = utr->next)
		if (utr->namelen > longest)
		    longest = utr->namelen;

	    numcols = (screen_cols - 3) / (longest + 4);
	    if (numcols > 5)
		numcols = 5;

	    percol = tr->num / numcols;
	    if (tr->num % numcols != 0)
		percol++;

	    for (col = -1, count = 0, utr = tr->users; col < numcols && count < tr->num; count++, utr = utr->next) {
		if (count % percol == 0)
		    col++;

		if (utr->namelen > longest_column[col])
		    longest_column[col] = utr->namelen;
	    }

	    addts();
	    printf(" Users in %s: (%lu)\n", tr->chan, tr->num);

	    for (row = 0; row < percol; row++) {
		/* print every [percol+row] user */
		printf("   ");
		col = 0;

		for (count = 0, utr = tr->users, col = 0; utr != NULL; utr = utr->next, count++) {
		    if (count % percol == row) {

			if (utr->mode == MODE_OP)
			    ch = '@';
			else if (utr->mode == MODE_VOICE)
			    ch = '+';
			else
			    ch = ' ';

			printf("(%c%s", ch, utr->name);
			spaces(longest_column[col] - utr->namelen);
			printf(") ");
			col++;

			if (col > numcols) {
			    break;
			}
		    }
		}
		printf("\n");
	    }
	    break;
	}
    }
}

/* PROTO */
void
delete_channel_user(char *name, char *chan)
{
    struct ChannelList *tr, *p = NULL;
    struct UserList *utr, *tmp;

    for (tr = chanlist; tr != NULL; tr = tr->next) {
	if (strcasecmp(chan, tr->chan) == 0) {
	    p = tr;
	    break;
	}
    }

    if (p != NULL) {
	if (strcasecmp(p->users->name, name) == 0) {
	    tmp = p->users;
	    p->users = p->users->next;
	    free(tmp->name);
	    free(tmp);
	} else {
	    for (utr = p->users; utr->next != NULL; utr = utr->next) {
		if (strcasecmp(utr->next->name, name) == 0) {
		    tmp = utr->next;
		    utr->next = utr->next->next;
		    free(tmp->name);
		    free(tmp);
		    break;
		}
	    }
	}

	p->num--;
    }
}

/* PROTO */
void
process_quit(char *name, char *msg)
{
    struct ChannelList *tr;
    struct UserList *utr, *tmp;

    for (tr = chanlist; tr != NULL; tr = tr->next) {
	if (strcasecmp(tr->users->name, name) == 0) {
	    tmp = tr->users;
	    tr->users = tr->users->next;
	    free(tmp->name);
	    free(tmp);
	    log_event(EVENT_QUIT, name, NULL, tr->chan, msg);
	    tr->num--;
	} else {
	    for (utr = tr->users; utr->next != NULL; utr = utr->next) {
		if (strcasecmp(utr->next->name, name) == 0) {
		    tmp = utr->next;
		    utr->next = utr->next->next;
		    free(tmp->name);
		    free(tmp);
		    log_event(EVENT_QUIT, name, NULL, tr->chan, msg);
		    tr->num--;
		    break;
		}
	    }
	}
    }
}

/* PROTO */
void
change_user_mode(char *name, char *chan, int plus, int mode)
{
    struct ChannelList *tr;
    struct UserList *utr;

    for (tr = chanlist; tr != NULL; tr = tr->next) {
	if (strcasecmp(chan, tr->chan) == 0) {
	    for (utr = tr->users; utr != NULL; utr = utr->next) {
		if (strcasecmp(name, utr->name) == 0) {
		    if (mode == C_MODE_OP) {
			if (plus == 1)
			    utr->mode |= MODE_OP;
			else
			    utr->mode ^= MODE_OP;
		    }
		    break;
		}
	    }
	}
    }
}

/* PROTO */
void
change_user_nick(char *old, char *new)
{
    struct ChannelList *tr;
    struct UserList *utr;

    for (tr = chanlist; tr != NULL; tr = tr->next) {
	for (utr = tr->users; utr != NULL; utr = utr->next) {
	    if (strcasecmp(old, utr->name) == 0) {
		free(utr->name);
		utr->name = strdup(new);
		utr->namelen = strlen(new);;
		break;
	    }
	}
    }
}

/* PROTO */
void
add_channel_user(char *name, char *chan, uint8_t mode)
{
    struct ChannelList *tr, *p;
    struct UserList *up, *utr, *utprev;
    int             comp;

    if (chanlist == NULL) {
	chanlist = malloc(sizeof(struct ChannelList));
	chanlist->chan = strdup(chan);
	chanlist->next = NULL;
	chanlist->users = NULL;
	p = chanlist;
    } else {
	p = NULL;

	for (tr = chanlist; tr != NULL; tr = tr->next) {
	    if (strcasecmp(chan, tr->chan) == 0) {
		p = tr;
		break;
	    }
	}

	if (p == NULL) {
	    for (tr = chanlist; tr->next != NULL; tr = tr->next);
	    tr->next = malloc(sizeof(struct ChannelList));
	    tr->next->chan = strdup(chan);
	    tr->next->next = NULL;
	    tr->next->users = NULL;
	    p = tr->next;
	}
    }

    if (p->users == NULL) {
	p->num = 0;
	p->users = malloc(sizeof(struct UserList));
	up = p->users;
	up->next = NULL;
    } else {
	utprev = NULL;

	for (utr = p->users; utr != NULL; utr = utr->next) {
	    if ((comp = strcasecmp(name, utr->name)) <= 0) {
		if (comp == 0) {
		    utr->mode = mode;
		    return;
		}
		else
		    break;
	    }
	    utprev = utr;
	}

	up = malloc(sizeof(struct UserList));

	if (utr == NULL) {
	    up->next = NULL;
	    utprev->next = up;
	} else {
	    if (utr == p->users) {
		up->next = p->users;
		p->users = up;
	    } else {
		up->next = utr;
		utprev->next = up;
	    }
	}
    }

    up->name = strdup(name);
    up->namelen = strlen(name);
    up->mode = mode;
    p->num++;
}
