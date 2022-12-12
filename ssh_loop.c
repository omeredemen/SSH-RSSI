#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void free_channel(ssh_channel channel) {
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
}

void free_session(ssh_session session) {
    ssh_disconnect(session);
    ssh_free(session);
}

void error(ssh_session session) {
    fprintf(stderr, "Error: %s\n", ssh_get_error(session));
    free_session(session);
    exit(-1);
}

int main() {

    ssh_session session;
    ssh_channel channel;
    int rc, port = 22;
    char buffer[1024];
    unsigned int nbytes;

    char host[] = "192.168.20.246";
    char user[] = "ems";

    FILE *fp;
    int c;

    printf("Session...\n");
    session = ssh_new();
    if (session == NULL) exit(-1);

    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, user);

    printf("Connecting...\n");
    rc = ssh_connect(session);
    if (rc != SSH_OK) error(session);

    printf("Password Autentication...\n");
    rc = ssh_userauth_password(session, NULL, "2004");
    if (rc != SSH_AUTH_SUCCESS) error(session);

    while (1)
    {
        printf("Channel...\n");
        channel = ssh_channel_new(session);
        if (channel == NULL) exit(-1);

        printf("Opening...\n");
        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) error(session);

        printf("Executing remote command...\n");
        rc = ssh_channel_request_exec(channel, "iwconfig wlp1s0 | grep Signal | /usr/bin/awk '{print $4}' | /usr/bin/cut -d'=' -f2");
        if (rc != SSH_OK) error(session);

        printf("Received:\n");
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        while (nbytes > 0) {
            fp = fopen("rssi.txt", "wb");
            if(fp == NULL) {
                printf("Error opening file\n");
                exit(1);
            }
            if (fwrite(buffer, sizeof(char), nbytes, fp) != nbytes) {   
                free_channel(channel);
                return SSH_ERROR;
            }

            fclose(fp);
            fp = fopen("rssi.txt","rb");
            while(1) {
                c = fgetc(fp);
                if( feof(fp) ) {
                    break ;
                }
                printf("%c", c);
            }
            fclose(fp);
            nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        }

        free_channel(channel);
        usleep(1000000);

    }

    free_channel(channel);
    free_session(session);

    return 0;
}
