#include <stdio.h>
#include <stdlib.h>


#include "ssh_com.h"

int main(){

    ssh_session session;

    int port = 22;
    // char password[] = "2004";
    char command[] = "iwconfig wlp1s0 | grep Signal | \
        /usr/bin/awk '{print $4}' | /usr/bin/cut -d'=' -f2";

    // char host[] = "192.168.20.246";
    // char user[] = "ems";

    FILE *fp;
    char value;

    initSession(&session, port, "ems", "192.168.20.246", "2004");

    while(1){
  
    int rssi_value = -getCommandOutputFromChannel(session, command); //this function gives rssi value as positive

    }
    return 0;
}