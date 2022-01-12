#include "msp.h"
#include "Motor.h"
#include "Clock.h"

#define SW1 0x10
#define SW2 0x02
#define RED 0x01
#define GREEN 0x02
#define BLUE 0x04
#define YELLOW 0x03

/**
 * main.c
 */

//初始化P1
void Port1_Init(void){
  P1->SEL0 = 0x00;
  P1->SEL1 = 0x00;
  P1->DIR = 0x01;
  P1->REN = 0x12;
  P1->OUT = 0x12;
}

//输入P1
uint8_t Port1_Input(void){
  return (P1->IN&0x12);
}

//初始化P2
void Port2_Init(void)
{
    P2->SEL0 = 0x00;
    P2->SEL1 = 0x00;
    P2->DS = 0x07;
    P2->DIR = 0x07;
    P2->OUT = 0x00;
}

//输出P1
void Port1_Output(uint8_t data){        // write all of P1.0 outputs
  P1->OUT = (P1->OUT&0xFE)|data;
}

//输出P2
void Port2_Output(uint8_t data)
{
    P2->OUT = data;
}


void main(void)
{
    uint8_t status;     //储存开关的状态
    int direction = 0;
    Port1_Init();
    Port2_Init();
    Clock_Init48MHz();
    Motor_Init();
    int right=3000;
    int left=3000;

    while(1)
    {
        status = Port1_Input();
        switch(status)
        {
            case SW1:
            {
                direction++;
                break;
            }
            case SW2:
            {
                Port1_Output(1);
                Clock_Delay1ms(500);
                Port1_Output(0);
                right += 1000;
                left += 1000;
                break;
            }
        }
        if(direction >= 4)
            direction = 0;
        switch(direction)
        {
            case 0:
            {
                Motor_Forward(left,right);
                Port2_Output(RED);
                break;
            }

            case 1:
            {
                Motor_Left(left,right);
                Port2_Output(GREEN);
                break;
            }

            case 2:
            {
                Motor_Right(left,right);
                Port2_Output(BLUE);
                break;
            }

            case 3:
            {
                Motor_Backward(left,right);
                Port2_Output(YELLOW);
                break;
            }
        }
        Clock_Delay1ms(1000);
    }
}
