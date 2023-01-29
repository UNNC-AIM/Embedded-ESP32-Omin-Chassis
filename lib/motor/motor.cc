#include <Arduino.h>
#include "motor.h"
#include "driver/can.h"
#include "driver/gpio.h"
#include <pid.h>
#include "data.h"

rx_Message message1;
rx_Message message2;
rx_Message message3;
rx_Message message4;

TaskHandle_t task1 = NULL;
TaskHandle_t task2 = NULL;
TaskHandle_t task3 = NULL;


volatile int16_t current1;
volatile int16_t current2;
volatile int16_t current3;
volatile int16_t current4;

int16_t speed1;
int16_t speed2;
int16_t speed3;
int16_t speed4;

//int16_t setspeed = 700;
int16_t id = 0x200;

void transmit_information(void*pt)
{
  uint32_t NotificationValue;
  BaseType_t xResult;
  while(1){
    xResult = xTaskNotifyWait(0xFFFFFFFF, //在运行前这个命令之前，先清除这几位
                              0xFFFFFFFF, //运行后，重置所有的bits 0x00 or ULONG_MAX or 0xFFFFFFFF
                              &NotificationValue, //重置前的notification value
                              portMAX_DELAY ); //一直等待                         
    if (xResult == pdTRUE) {
      can_message_t tx_message;
    tx_message.identifier = id;
    tx_message.flags = CAN_MSG_FLAG_NONE;
    tx_message.data_length_code = 8;

    
    tx_message.data[0] = current1 >> 8;
    tx_message.data[1] = current1;
    tx_message.data[2] = current2 >> 8;
    tx_message.data[3] = current2;
    tx_message.data[4] = current3 >> 8;
    tx_message.data[5] = current3;
    tx_message.data[6] = current4 >> 8;
    tx_message.data[7] = current4;
  

    can_transmit(&tx_message,1);

      vTaskDelay(10);
    }
  }
}  


void receive_information(void *pt)
{
  uint32_t NotificationValue;
  BaseType_t xResult;
  
  while(1){
    /*xResult = xTaskNotifyWait(0xFFFFFFFF, //在运行前这个命令之前，先清除这几位
                              0xFFFFFFFF, //运行后，重置所有的bits 0x00 or ULONG_MAX or 0xFFFFFFFF
                              &NotificationValue, //重置前的notification value
                              portMAX_DELAY ); //一直等待                         
    if (xResult == pdTRUE) {*/
      can_message_t rx_message;
      float angle;

      can_receive(&rx_message, 10000);
      switch(rx_message.identifier)
    {
      case 0x201:
      message1.angle = (int16_t)((rx_message.data[0]<<8) | (uint8_t)(rx_message.data[1]));
      message1.speed = (int16_t)((rx_message.data[2]<<8) | (uint8_t)(rx_message.data[3]));
      message1.current = (int16_t)((rx_message.data[4]<<8) | (uint8_t)(rx_message.data[5]));
      message1.tempuerature = rx_message.data[6];
      break;

      case 0x202:
      message2.angle = (int16_t)((rx_message.data[0]<<8) | (uint8_t)(rx_message.data[1]));
      message2.speed = (int16_t)((rx_message.data[2]<<8) | (uint8_t)(rx_message.data[3]));
      message2.current = (int16_t)((rx_message.data[4]<<8) | (uint8_t)(rx_message.data[5]));
      message2.tempuerature = rx_message.data[6];
      break;

      case 0x203:
      message3.angle = (int16_t)((rx_message.data[0]<<8) | (uint8_t)(rx_message.data[1]));
      message3.speed = (int16_t)((rx_message.data[2]<<8) | (uint8_t)(rx_message.data[3]));
      message3.current = (int16_t)((rx_message.data[4]<<8) | (uint8_t)(rx_message.data[5]));
      message3.tempuerature = rx_message.data[6];
      break;

      case 0x204:
      message4.angle = (int16_t)((rx_message.data[0]<<8) | (uint8_t)(rx_message.data[1]));
      message4.speed = (int16_t)((rx_message.data[2]<<8) | (uint8_t)(rx_message.data[3]));
      message4.current = (int16_t)((rx_message.data[4]<<8) | (uint8_t)(rx_message.data[5]));
      message4.tempuerature = rx_message.data[6];
      break;

      default:
      break;
    }

      //Serial.print("angle:");
      //Serial.print(angle);
      //Serial.print(",");
      //Serial.print("current:");
      //Serial.print(message.current);
      //Serial.print(",");
      //Serial.print("speed:");
      Serial.println(message1.speed);
      xTaskNotify( task2, ( 1UL << 4UL ), eSetBits );
      vTaskDelay(10);
    //}
  }
} 
float velPid[3] = {0.75f, 0.02f, 0.0f};

void motor_ctl_vel (void*pt) {
  
  uint32_t NotificationValue;
  BaseType_t xResult;
  while(1){
    RemoteDataProcess();
    datatranslate(&speed1,&speed2,&speed3,&speed4);
    xResult = xTaskNotifyWait(0xFFFFFFFF, //在运行前这个命令之前，先清除这几位
                              0xFFFFFFFF, //运行后，重置所有的bits 0x00 or ULONG_MAX or 0xFFFFFFFF
                              &NotificationValue, //重置前的notification value
                              portMAX_DELAY ); //一直等待                         
    if (xResult == pdTRUE) {
    
      pid_type_def pid_VelocityStruct = {0};
      const float MAX_CURRENT = 3000;
      const float MAX_KI_CURRENT = 2000;
      PID_init(&pid_VelocityStruct, PID_POSITION, velPid, MAX_CURRENT, MAX_KI_CURRENT);
      current1 = PID_calc(&pid_VelocityStruct, message1.speed,speed1);
      current2 = PID_calc(&pid_VelocityStruct, message2.speed,speed2);
      current3 = PID_calc(&pid_VelocityStruct, message3.speed,speed3);
      current4 = PID_calc(&pid_VelocityStruct, message4.speed,speed4);
      xTaskNotify( task3, ( 1UL << 4UL ), eSetBits );
      vTaskDelay(10);
    }
  } 
}






