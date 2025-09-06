#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "pipe_handler.h"

#define DASHBOARD_CAR_ON_SIGNAL (SIGUSR1)
#define DASHBOARD_CAR_OFF_SIGNAL (SIGUSR2)

void sighandler(int signum){
    char* YES_MESSAGE = "YES";
    char* NO_MESSAGE = "NOT";
    if(signum == DASHBOARD_CAR_ON_SIGNAL){
        send_dashboard(CANDRIVEORNOT, YES_MESSAGE);
    } else if (signum == DASHBOARD_CAR_OFF_SIGNAL){
        send_dashboard(CANDRIVEORNOT, NO_MESSAGE);
    }
    return;
}

int main()
{
    struct sigaction newaction, oldaction;
    
    newaction.sa_handler = sighandler;
    sigemptyset(&newaction.sa_mask);
    newaction.sa_flags = 0;
    
    sigaction(DASHBOARD_CAR_ON_SIGNAL, &newaction, &oldaction);
    sigaction(DASHBOARD_CAR_OFF_SIGNAL, &newaction, &oldaction);
    
    int ret;
    int s, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    
    memset(&frame, 0, sizeof(struct can_frame));
    
    system("sudo ip link set can0 type can bitrate 500000");
    system("sudo ifconfig can0 up");
    printf("this is a can receive demo\r\n");
    
    //1.Create socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket PF_CAN failed");
        return 1;
    }
    
    //2.Specify can0 device
    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(s, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl failed");
        return 1;
    }

    //3.Bind the socket to can0
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind failed");
        return 1;
    }
    
    //4.Define receive rules
    struct can_filter rfilter[1];
    //rfilter[0].can_id = 0x123;
    //rfilter[0].can_mask = CAN_SFF_MASK;
    rfilter[0].can_mask = 0;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));


    printf("setup pipe, starting dashboard\n");
    int r = setup_dashboard();
    if(r){
        printf("something bad happened when setting up the dashboard uhhhh idk what to do here have a :3\n");
    }
    printf("dashboard started\n");

    // sanity check for stuff that we are running code
    unsigned char can_receive_cyc = 0;

    char * heartbeatbuffer = malloc(4);

    //sleep(1);

    //5.Receive data and exit
    while(1) {
        can_receive_cyc += 1;
        snprintf(heartbeatbuffer, 4, "%u", (unsigned int)(can_receive_cyc));
        //printf("sending heartbeat\n");
        send_dashboard(HEARTBEAT, heartbeatbuffer);
        nbytes = read(s, &frame, sizeof(frame));
        //if(nbytes > 0) {
            //printf("can_id = 0x%X\r\ncan_dlc = %d \r\n", frame.can_id, frame.can_dlc);
            //int i = 0;
            //for(i = 0; i < 8; i++)
            //    printf("data[%d] = %d\r\n", i, frame.data[i]);
            //break;
        //}
    }
    
    //6.Close the socket and can0
    close(s);
    system("sudo ifconfig can0 down");
    free(heartbeatbuffer);

    return 0;
}
