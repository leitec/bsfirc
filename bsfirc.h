#include "irclib/irclib.h"

#ifdef TIMESTAMPS
#define TIMESTAMPS_CHANMSG
#endif

struct BSFirc {
	void *handle;
	char *lastmsg;
	char *lastchan;
	char *nick;
	int istyping;
};

enum {
	EVENT_PRIVMSG,
	EVENT_CHANJOIN,
	EVENT_CHANPART,
	EVENT_CHANMSG,
	EVENT_SENDPRIVMSG
};

#define BSF_PROMPT "\\\\"
#include "protos.h"
