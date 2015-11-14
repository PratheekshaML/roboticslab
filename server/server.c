#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include<string.h>
#include<time.h>
#include<stdbool.h>
char mac_addr[18] = "00:16:53:0A:20:AE"; // MAC address of the BT device
char file_name[] ="test.csv";
int main(void)

{ 
        int client_sock = -1; // socket for the BT comm 
	int i, comm_res,packet_no, check, connect_status; 
        bdaddr_t bt_mac; 
        struct sockaddr_rc addr_data = {0};
        if(str2ba(mac_addr, &addr_data.rc_bdaddr) == -1) // Convert BT MAC addr to rc_bdaddr 
        	return -1;
        client_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM); // Socket initialization for BT comm
        addr_data.rc_family = AF_BLUETOOTH; // specify its a BT socket
        addr_data.rc_channel = (uint8_t) 1; 
        int rc, length;
        connect_status = connect(client_sock, (struct sockaddr *)&addr_data, sizeof(addr_data)); 
        if(connect_status == -1) {
        	printf("Error in BT socket initialization \n");
                close(client_sock);
                return -1;
        }
         printf("So far so good \n");
         uint8_t out_packet[2];
         FILE *fp;
         fp = fopen(file_name, "w+");
         fprintf(fp,"Sample No., Time (ms), Motor Count \n");
         out_packet[0] = 255;
         out_packet[1] = 0; 
         comm_res = send(client_sock, out_packet, 2, 0);
         sleep(5); 
         comm_res = send(client_sock, out_packet, 2, 0);
         i = 0;
         time_t start = clock(), diff;
         int end = 8000, msec = 0; 
         unsigned long sample_no = 0;
         unsigned long time_elapsed = 0;
         bool flag = false; 
         int delta;
         uint16_t in_packet[22];
         while(1){
                delta = 0;  
          	comm_res = recv(client_sock, &in_packet, sizeof(uint16_t)*21, 0); 
          	int j;
                  if(flag){
                        sample_no++;
                        delta = in_packet[2];
                        time_elapsed = time_elapsed + delta;
                	fprintf(fp,"%ld ,%ld, %d  \n", sample_no,  time_elapsed,in_packet[3]);
                }
                for(j=1; j<10; j++){
                sample_no++;
                delta = in_packet[2*j];
                time_elapsed = time_elapsed + delta;
                fprintf(fp,"%ld ,%ld, %d  \n", sample_no, time_elapsed ,in_packet[(2*j)+1]);
                i++;
                }
                flag = true;
                diff = clock() - start;
                msec = (diff * 1000) / CLOCKS_PER_SEC;
                //if(msec > end)
                //	break;
         }
         fclose(fp);
         close(client_sock);
         printf("Application exit \n");
         return 0; 
}   
