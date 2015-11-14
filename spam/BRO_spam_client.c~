#include <stdio.h>
#include "headers/BRO_spam_fists.h"
#include "headers/BRO_spam_client.h"

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(SysTimerCnt);
DeclareResource(lcd);
DeclareTask(BRO_Comm);
DeclareTask(PID_Controller);
DeclareTask(DisplayTask);

engines_t engines;
U8 flag = 1;
typedef struct smam_data {
	S16 rev_count; 
    U32 time_stamp;
}spam_data;

#if 0
engines_t spam_motor_start() {
    engines_t ret = {
        {.port = NXT_PORT_A, .speed_control_type = NOT_USING, .speed_ref = 0},
        {.port = NXT_PORT_B, .speed_control_type = NOT_USING, .speed_ref = 0},
        {.port = NXT_PORT_C, .speed_control_type = NOT_USING, .speed_ref = 0}
    };
    
    return ret;
};
#endif

/*--------------------------------------------------------------------------*/
/* LEJOS OSEK hooks                                                         */
/*--------------------------------------------------------------------------*/
void ecrobot_device_initialize()
{
    ecrobot_init_bt_slave("1234");
    
    memset(&engines, 0, sizeof(engines_t));

    engines.first.port = NXT_PORT_A;
    engines.first.speed_control_type = NOT_USING;
    engines.first.speed_ref = 0;

    engines.second.port = NXT_PORT_B;
    engines.second.speed_control_type = NOT_USING;
    engines.second.speed_ref = 0;

    engines.third.port = NXT_PORT_C;
    engines.third.speed_control_type = NOT_USING;
    engines.third.speed_ref = 0;

    if (CONN_SONAR) {
        ecrobot_init_sonar_sensor(SONAR_PORT);
    };
    if (CONN_LIGHT) {
        ecrobot_set_light_sensor_active(LIGHT_PORT);
    };
}


void ecrobot_device_terminate()
{
  
    memset(&engines, 0, sizeof(engines_t));

    nxt_motor_set_speed(NXT_PORT_A, 0, 1);
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
        
    ecrobot_set_light_sensor_inactive(LIGHT_PORT);
    ecrobot_term_sonar_sensor(SONAR_PORT);

    bt_reset();

    ecrobot_term_bt_connection();
}

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(void)
{
    StatusType ercd;

    /*
     *  Increment OSEK Alarm System Timer Count
    */
    ercd = SignalCounter( SysTimerCnt );
    if ( ercd != E_OK ) {
        ShutdownOS( ercd );
    }
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : PID_Controller                                                 */
/* Priority: 3                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: preemptive                                                     */
/*--------------------------------------------------------------------------*/
TASK(PID_Controller)
{
    
    /*  We will now update the powers for each motor.
     *  If a motor is set as RAW_POWER then the first "powers" value is used
     *  directly for the speed update.
     *  Oblviously it will be possible for the users to not install a motor,
     *  so we won't do anything with the ports on which there is nothing.
     */
    
    if (engines.first.speed_control_type != NOT_USING) {
        //  Now we will set the powers using the data inside the motor's
        //  structure.
        nxt_motor_set_speed(engines.first.port, engines.first.powers[0], 1);
    };
    
    //  Doing the same thing for the second motor
    if (engines.second.speed_control_type != NOT_USING) {
        nxt_motor_set_speed(engines.second.port, engines.second.powers[0], 1);
    };
    
    //  And, guess what? We are doing it even for the third motor.
    if (engines.third.speed_control_type != NOT_USING) {
        nxt_motor_set_speed(engines.third.port, engines.third.powers[0], 1);
    };    

    TerminateTask();
}

TASK(BRO_Comm)
{
    U32 connect_status = 0;
    
    /*  Declaring two buffers for communication */
    //bro_fist_t in_packet[BUFFER_SIZE];
    //bro_fist_t out_packet[BUFFER_SIZE];
    
    //memset (in_packet, 0, sizeof(bro_fist_t) * BUFFER_SIZE);
    //memset (out_packet, 0, sizeof(bro_fist_t) * BUFFER_SIZE);
    //U8 out_packet[2]; 
    U8 in_packet[2];
    memset(in_packet, 0, 2); 
    //memset(out_packet, 0, 2);

    /*  As you might know we have a problem here... :3
     *  That problem is that the BT device installed on the AT91SAM7 seems to
     *  have some speed problems with the receiving for the first data via BT.
     *  It needs ~40ms to get every kind of data (Even a uint32) so we will have
     *  to use some kind of pooling (not really pooling, because it would lock
     *  the NXT, mind you ;) ) and work on the data received only when some
     *  data, usually all of it (I won't tell you to read the device drivers
     *  for BlueTooth written for nxtOSEK, but the ecrobot_read_bt_packet checks
     *  if all the data declared in the first byte is present in the device's
     *  buffer.
     *
     *  No problem, with our drill we will pierce the Heavens!
     *  (And also with our BROFists, right?)
     */
    
    connect_status = ecrobot_read_bt_packet(in_packet, 2);
    connect_status = ecrobot_read_bt_packet(in_packet, 2);
    nxt_motor_set_speed(NXT_PORT_B, 20, 1);
    //if(in_packet[0] == 64)
    //	flag += 64;
    //flag = (U8) (*(U8*)(&in_packet[1]));
    while(1){
    spam_data out_packet[10];
    memset(out_packet, 0, sizeof(spam_data)*10);
    U8 i; 
    for(i=0; i< 10; i++){
    //out_packet[i].rev_count = (S16) nxt_motor_get_count(NXT_PORT_B);
    out_packet[i].rev_count = 0xffffff;
    //out_packet[i].time_stamp = (U32) systick_get_ms();
    out_packet[i].time_stamp =  ecrobot_get_motor_rev(NXT_PORT_B);
    systick_wait_ms(5);
    }
    ecrobot_status_monitor("Sent data");
    ecrobot_send_bt_packet((U8 *)out_packet, sizeof(spam_data)*10);
    //flag++;
    //ecrobot_status_monitor("Sent data");
    //bt_send(out_packet, 2);
    //ecrobot_status_monitor("Sent data");
    //bt_send(out_packet, 2);
    //ecrobot_status_monitor("Sent data");
    if (connect_status == -1)
    	ecrobot_status_monitor("Failed to receive");
    }
    // Se sono arrivati dei dati...
    /*if (connect_status > 0) {
        // Decodifica ed elabora i pacchetti ricevuti
        //decode_bro_fists (in_packet, out_packet, &engines);
        // Invia la risposta
    ecrobot_status_monitor("Before send");
        //out_packet[0] = 255;
        //out_packet[1] = 255;
        //out_packet[0] = in_packet[0] ;
        //out_packet[1] = 0x43;
        //ecrobot_send_bt_packet(out_packet, 2);
    ecrobot_status_monitor("Sent data");
    }*/

    TerminateTask();
}

TASK(DisplayTask)
{
    ecrobot_status_monitor("BROFist Client");
    TerminateTask();
}
