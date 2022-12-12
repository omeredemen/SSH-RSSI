#ifndef _SSH_COM_
#define _SSH_COM_


#include <libssh/libssh.h>

void initSession(ssh_session *session, int port, const char* user, 
                const char* host, const char* password);
void sendCommandToChannel(ssh_session session, ssh_channel *channel, const char* command);
int getCommandOutputFromChannel(ssh_session session, const char* command);    

void errorSession(ssh_session session);
void freeSession(ssh_session session);
void freeChannel(ssh_channel channel);


#endif