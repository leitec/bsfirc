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

#define BSF_PROMPT "::"
#include "protos.h"
