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

void error_callback (void *handle, int code);
char           * undo_netspeak (char *orig);
void get_input (void);
void parse_input (void);
void irc_topic (void *h, char *chan, char *topic);
void irc_motd (void *h, char *motd);
void irc_notice_auth (void *h, char *msg);
void irc_ready (void *h);
void irc_server_name (void *h, char *name);
void irc_join (void *h, char *nick, char *host, char *channel);
void irc_addchanuser (void *h, char *chan, char *nick);
void irc_namesdone (void *h, char *chan);
void irc_part (void *h, char *nick, char *host, char *channel);
void irc_quit (void *h, char *nick, char *msg);
void irc_nickinuse (void *h, char *nick);
void irc_ctcp (void *h, char *nick, char *host, char *target, char *msg);
void irc_mode (void *h, char *nick, char *host, char *target, int plus, int mode, char *arg);
void irc_notice (void *h, char *nick, char *host, char *msg);
void irc_msg (void *h, char *nick, char *host, char *target, char *msg);
void irc_whois_userhost (void *h, char *nick, char *user, char *host, char *name);
void irc_whois_channels (void *h, char *nick, char *channels);
void irc_whois_server (void *h, char *nick, char *server, char *serverdescr);
void show_channel_users (char *chan);
void delete_channel_user (char *name, char *chan);
void process_quit (char *name, char *msg);
void change_user_mode (char *name, char *chan, int plus, int mode);
void add_channel_user (char *name, char *chan, uint8_t mode);
int open_log_dir (void);
void log_event (int event_type, char *name, char *host, char *chan, char *msg);
void show_log (int lines, char *dest);
void addts (void);
void addts_short (void);
void eraseline (void);
void show_prompt (void);
void sigwinch_handler (int a);
void setup_tty (void);
void restore_tty (void);
void get_screen_size (void);
void wordwrap_print (char *str, int offset);
void wordwrap_print_echostr (char *str, char *echostr);
