#include <stdio.h>
#include "headers/BRO_spam_fists.h"
#include "headers/BRO_spam_client.h"
#include <stdbool.h>
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
U32 time_rec;
/*typedef struct spam_data {
	float rev_count[10]; 
    float time_stamp[10];
}spam_data;

typedef struct outdata {
    //uint8_t buffsize;
    spam_data packets;
}outdata; */


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
    U8 in_packet[2];
    memset(in_packet, 0, 2); 
    uint16_t out_packet[20];
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
    nxt_motor_set_speed(NXT_PORT_B, 40, 1);
    uint16_t j = 0;
    bool delay_flag = 1;
    time_rec = systick_get_ms();
    while(1){
    memset((void *)&out_packet, 0, sizeof(uint16_t)*20);
    U8 i; 
    
    if(delay_flag){
    	systick_wait_ms(1);
        delay_flag = !delay_flag;
    }
    for(i=0; i< 10; i++){
    out_packet[2*i] = nxt_motor_get_count(NXT_PORT_B); 
    out_packet[(2*i)+1] = (uint16_t)(systick_get_ms() - time_rec);
    time_rec = systick_get_ms();
    systick_wait_ms(1);
    }
    ecrobot_status_monitor("Sent data");
    ecrobot_send_bt_packet((U8 *)&out_packet, sizeof(uint16_t)*20);
    if (connect_status == -1)
    	ecrobot_status_monitor("Failed to receive");
    }

    TerminateTask();
}

TASK(DisplayTask)
{
    ecrobot_status_monitor("BROFist Client");
    TerminateTask();
}
