#include <Arduino.h>
#include "motor.h"
#include "driver/can.h"
#include "driver/gpio.h"
#include "pid.h"
#include "data.h"

extern TaskHandle_t task1 ;
extern TaskHandle_t task2 ;
extern TaskHandle_t task3 ;

void setup() {
  // put your setup code here, to run once:


  
  can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(GPIO_NUM_33,GPIO_NUM_35,CAN_MODE_NORMAL);
  can_timing_config_t t_config = CAN_TIMING_CONFIG_1MBITS();
  can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

  can_driver_install(&g_config, &t_config, &f_config);
  can_start();

  Serial.begin(115200);
  Serial2.begin(100000);

  xTaskCreate(receive_information, "receive", 1024, NULL, 1, &task1 );

  xTaskCreate(motor_ctl_vel, "PID", 1024, NULL, 1, &task2 );  

  xTaskCreate(transmit_information, "transmit", 1024, NULL, 1, &task3 );  
}



void loop() {

}





