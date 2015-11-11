#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
char mac_addr[18] = "00:16:53:0A:20:AE";
typedef struct smam_data {
	int16_t rev_count; 
        uint32_t time_stamp;
}spam_data;

int main(void)

{
	int server_sock = -1; 
        int client_sock = -1; 
        printf("check");
	int i, comm_res,packet_no, check, connect_status; 
        bdaddr_t bt_mac; 
        struct sockaddr_rc addr_data = {0};
        //spam_data in_packet[10];
        if(str2ba(mac_addr, &addr_data.rc_bdaddr) == -1)
        	return -1;
        printf("check");
        client_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        addr_data.rc_family = AF_BLUETOOTH; 
        addr_data.rc_channel = (uint8_t) 1; 
        int rc, length; 
        
        printf("%d", client_sock);
        connect_status = connect(client_sock, (struct sockaddr *)&addr_data, sizeof(addr_data)); 
        if(connect_status == -1) {
        	printf("Error in BT socket initialization \n");
                close(client_sock);
                return -1;
        }
        
        printf("check");
        
         printf("So far so good \n");
         uint8_t out_packet[2];
         out_packet[0] = 255;
         out_packet[1] = 0; 
         printf("check before sent \n");
         comm_res = send(client_sock, out_packet, 2, 0);
         sleep(5); 
         comm_res = send(client_sock, out_packet, 2, 0);
         printf("sleep done\n");
         i = 0;
         while(1){
                spam_data in_packet[11]; 
                printf("check before recv \n"); 
          	comm_res = recv(client_sock, in_packet, sizeof(spam_data)*10 + 2, 0); 
          	int j;
                for(j=1; j<11; j++)
                	printf(" *Count %d: %d \n", (int)in_packet[j].rev_count, (int)in_packet[j].time_stamp);
                i++;
                printf("\n \n");
         }
         close(client_sock);
         printf("Application exit \n");
         return 0; 
}   
