#include "bsfirc.h"

extern struct BSFirc *bsfirc;
extern struct ChannelList *chanlist;

/* PROTO */
void
show_channel_users(char *chan)
{
	struct ChannelList *tr;
	struct UserList *utr;
	int first;

	for(tr = chanlist; tr != NULL; tr = tr->next) {
		if(strcasecmp(tr->chan, chan) == 0) {
			printf("\n");
			addts();
			printf(" Users in %s:\n", tr->chan);
			for(utr = tr->users, first = 1; utr != NULL; utr = utr->next) {
				printf("%s", USERLIST_ECHOSTR);
				if(first) {
					printf(" %-16s\t", utr->name);
					first = 0;
				} else {
					printf(" %s\n", utr->name);
					first = 1;
				}
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

	for(tr = chanlist; tr != NULL; tr = tr->next) {
		if(strcasecmp(chan, tr->chan) == 0) {
			p = tr;
			break;
		}
	}

	if(p != NULL) {
		if(strcasecmp(p->users->name, name) == 0) {
			tmp = p->users;
			p->users = p->users->next;
			free(tmp->name);
			free(tmp);
		} else {
			for(utr = p->users; utr->next != NULL; utr = utr->next) {
				if(strcasecmp(utr->next->name, name) == 0) {
					tmp = utr->next;
					utr->next = utr->next->next;
					free(tmp->name);
					free(tmp);
					break;
				}
			}
		}
	}
}

/* PROTO */
void
add_channel_user(char *name, char *chan, uint8_t mode)
{
	struct ChannelList *tr, *p;
	struct UserList *up, *utr;

	if(chanlist == NULL) {
		chanlist = malloc(sizeof(struct ChannelList));
		chanlist->chan = strdup(chan);
		chanlist->next = NULL;
		chanlist->users = NULL;
		p = chanlist;
	} else {
		p = NULL;

		for(tr = chanlist; tr != NULL; tr = tr->next) {
			if(strcasecmp(chan, tr->chan) == 0) {
				p = tr;
				break;
			}
		}

		if(p == NULL) {
			for(tr = chanlist; tr->next != NULL; tr = tr->next);
			tr->next = malloc(sizeof(struct ChannelList));
			tr->next->chan = strdup(chan);
			tr->next->next = NULL;
			tr->next->users = NULL;
			p = tr->next;
		}
	}

	if(p->users == NULL) {
		p->users = malloc(sizeof(struct UserList));
		up = p->users;
		up->next = NULL;
	} else {
		for(utr = p->users; utr != NULL; utr = utr->next) {
			if(strcasecmp(utr->name, name) == 0)
				return;
		}

		for(utr = p->users; utr != NULL; utr = utr->next)
			if(strcasecmp(name, utr->name) < 0)
				break;

		up = malloc(sizeof(struct UserList));

		if(utr == NULL) {
			for(utr = p->users; utr->next != NULL;
					 utr = utr->next);
			up->next = NULL;
			utr->next = up;
		} else {
			if(utr == p->users) {
				up->next = p->users;
				p->users = up;
			} else {
				struct UserList *utr2;
				up->next = utr;
				for(utr2 = p->users; utr2->next != utr;
						utr2 = utr2->next);
				utr2->next = up;
			}
		}
	}

	up->name = strdup(name);
	up->mode = mode;
}