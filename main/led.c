#include "msp.h"
#include "Clock.h"
#include<stdio.h>

//宏定义颜色和开关
#define SW1 0x10    
#define SW2 0x02   
#define DARK 0
#define RED 0x01
#define GREEN 0x02
#define BLUE 0x04
#define YELLOW 0x03
#define SKY_BLUE 0x06
#define WHITE 0x07
#define PINK 0x05

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

//主函数
void main(void)
{
    uint8_t status;     //储存开关的状态
    uint8_t colors[] = {RED,GREEN,BLUE,YELLOW,PINK};        //储存颜色的数组
    int flag = 0;       //颜色的选择标志
    int delay_time = 1;     //延时的秒数

    Port1_Init();
    Port2_Init();
    while(1)
    {
        status = Port1_Input();
        Port1_Output(1);        //左边的红灯先亮0.5秒
        Clock_Delay1ms(500); 
        Port1_Output(0);
        Port2_Output(colors[flag]);     //右边的LED灯亮0.5秒

        switch(status)
        {
            case SW1:       //按下左边的开关，闪烁延时0.5秒
            {
                delay_time++;
                break;                
            }
            case 0x00:       //按下两边的开关，切换颜色
            {
                if(flag++ == 4)     //循环颜色
                    flag = 0;
                Port2_Output(colors[flag]);
                break;
            }
            case SW2:       //按下右边的开关，速度增加0.5秒
            {
                if(delay_time == 1)
                    break;
                delay_time--;
                break; 
            }
        }
        Clock_Delay1ms(500);
        Port2_Output(DARK);
       
        Clock_Delay1ms(delay_time*500);        //闪烁的间隔控制
    }
}
