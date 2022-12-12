#include "ssh_com.h"
#include <stdio.h>
#include <stdlib.h>
#include <libssh/libssh.h>

void initSession(ssh_session* session, int port, const char* user, 
                const char* host, const char* password){
    int rc;
    printf("new session\n");
    *session = ssh_new();
    if(*session == NULL) exit(-1);
    
    ssh_options_set(*session, SSH_OPTIONS_HOST, host);
    ssh_options_set(*session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(*session, SSH_OPTIONS_USER, user); 
    
    printf("connecting to session..\n");
    rc = ssh_connect(*session);
    if(rc != SSH_OK) errorSession(*session);

    printf("Password Autentication...\n");
    rc = ssh_userauth_password(*session, NULL, password);
    if(rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Error authenticating with password: %s\n",
                ssh_get_error(*session));
        freeSession(*session);
        exit(-1);
    }
}

void sendCommandToChannel(ssh_session session, ssh_channel *channel, const char* command){
    int rc;
    printf("\nChannel...\n");
    *channel = ssh_channel_new(session);
    if (*channel == NULL) exit(-1);

    printf("Opening...\n");
    rc = ssh_channel_open_session(*channel);
    if(rc != SSH_OK) errorSession(session);
    
    printf("Executing remote command...\n");
    rc = ssh_channel_request_exec(*channel, command);
    if(rc != SSH_OK) errorSession(session);
}

int getCommandOutputFromChannel(ssh_session session, const char* command){
    ssh_channel channel;
    char buffer[1024];
    unsigned int nbytes;

    FILE *fp;
    char c;
    int16_t value;
    int output[4];

    sendCommandToChannel(session, &channel, command);
    
    printf("Received:\n");
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while(nbytes > 0) {
        fp = fopen("ssh_command_log.txt", "wb");
        if(fp == NULL) {
            printf("Error opening file\n");
            return -1;
        }
        if(fwrite(buffer, sizeof(char), nbytes, fp) != nbytes){
            freeChannel(channel);
            return SSH_ERROR;
        }
        fclose(fp);

        fp = fopen("ssh_command_log.txt", "rb");
        if (fp == NULL) {
            printf("Error opening file\n");
            return -1;
        }
        int i = 0;
        while(1) {
            c = fgetc(fp);
            if(feof(fp))break;
            output[i] = c - 48; //48 is decimal value of char '0' in ASCII
            printf("%d \t", c - '0'); //value - '0'  is exactly the line above
            i++;
        }
        fclose(fp);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
    value = ((output[1])*10) + output[2];
    
    freeChannel(channel);
    return value;
}

void errorSession(ssh_session session){
    fprintf(stderr, "Error: %s\n", ssh_get_error(session));
    freeSession(session);
    exit(-1);
}

void freeSession(ssh_session session){
    ssh_disconnect(session);
    ssh_free(session);
}

void freeChannel(ssh_channel channel){
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
}