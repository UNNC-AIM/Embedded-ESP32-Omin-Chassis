#include <Arduino.h>
#include "data.h"
#include "driver/can.h"
#include "driver/gpio.h"
#include "motor.h"
#include "pid.h"

volatile RC_Ctl_t RC_CtrlData;



void RemoteDataProcess()
{
    uint8_t pData[18] = {0};

    if (Serial2.available())
    {
        Serial2.readBytes(pData, 18);

        RC_CtrlData.rc.ch0 = (int16_t)(pData[0] | (uint16_t)(pData[1] << 8)) & 0x07FF;
        RC_CtrlData.rc.ch1 = ((int16_t)pData[1] >> 3 | (int16_t)pData[2] << 5) & 0x07FF;
        RC_CtrlData.rc.ch2 = ((int16_t)pData[2] >> 6 | (int16_t)pData[3] << 2 | (int16_t)pData[4] << 10) & 0x07FF;
        RC_CtrlData.rc.ch3 = ((int16_t)pData[4] >> 1 | (int16_t)pData[5] << 7) & 0x07FF;

        RC_CtrlData.rc.s1 = ((int16_t)pData[5] >> 4 & 0x000C) >> 2;
        RC_CtrlData.rc.s2 = ((int16_t)pData[5] & 0x0003);

        RC_CtrlData.mouse.x = ((int16_t)pData[6] | (int16_t)pData[7] << 8);
        RC_CtrlData.mouse.y = ((int16_t)pData[8] | (int16_t)pData[9] << 8);
        RC_CtrlData.mouse.z = ((int16_t)pData[10] | (int16_t)pData[11] << 8);

        RC_CtrlData.mouse.press_l = (int16_t)pData[12];
        RC_CtrlData.mouse.press_r = (int16_t)pData[13];

        RC_CtrlData.key.v = ((int16_t)pData[14] | (int16_t)pData[15] << 8);
        //Serial.printf("ch0: %d\t, ch1: %d\n",RC_CtrlData.rc.ch0, RC_CtrlData.rc.ch1);
    }
}

void datatranslate(int16_t*a,int16_t*b,int16_t*c,int16_t*d){
    int16_t Y_axis = RC_CtrlData.rc.ch3-1024;
    int16_t X_axis = RC_CtrlData.rc.ch0-1024;
    int16_t turn = RC_CtrlData.rc.ch2;
    int16_t leftcircle = RC_CtrlData.rc.s1;
    int16_t rightcircle = RC_CtrlData.rc.s2;
    int16_t w = 0;
    if(leftcircle==2){
        w=500;
    }else if(rightcircle==2){
        w=-500;
    }
    int16_t y = (Y_axis*cos(-45)+X_axis*sin(-45));
    int16_t x = (X_axis*cos(-45)-Y_axis*sin(-45));
    *a = (-1)*y+w;
    *b = x+w;
    *c = y+w;
    *d = (-1)*x+w;
}


