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
	int ready;
};

struct UserList {
	char *name;
	uint8_t mode;
	struct UserList *next;
};

#define MODE_NONE  0x00000000
#define MODE_VOICE 0x00000001
#define MODE_OP    0x00000010

struct ChannelList {
	char *chan;
	struct UserList *users;
	struct ChannelList *next;
};

enum {
	EVENT_PRIVMSG,
	EVENT_CHANJOIN,
	EVENT_CHANPART,
	EVENT_CHANMSG,
	EVENT_QUIT,
	EVENT_SENDPRIVMSG
};

#define BSF_PROMPT "::"
#define USERLIST_ECHOSTR "  "

#include "protos.h"
