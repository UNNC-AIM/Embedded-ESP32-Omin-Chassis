#ifndef MOTOR_H
#define MOTOR_H


#include <Arduino.h>


/* data */
typedef struct 
{
    int16_t angle;
    int16_t speed;
    int16_t current;
    uint16_t tempuerature;
}rx_Message;





extern void transmit_information(void*pt);
extern void receive_information(void*pt);
extern void motor_ctl_vel (void*pt);
extern TaskHandle_t task1 ;
extern TaskHandle_t task2 ;
extern TaskHandle_t task3 ;
#endif
