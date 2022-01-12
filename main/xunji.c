#include <stdio.h>
#include "msp.h"
#include "ADC14.h"
#include "Clock.h"
#include "Bump.h"
#include "Reflectance.h"
#include "PWM.h"
#include "Motor.h"
#include "IRDistance.h"
#include "CortexM.h"

#define SW1 0x10    
#define SW2 0x02   


uint32_t nr,nc,nl;
int32_t cdistance,ldistance,rdistance;   

uint8_t Data;
uint8_t Status;

void Port1_Init(void){
  P1->SEL0 = 0x00;
  P1->SEL1 = 0x00;                        
  P1->DIR = 0x01;                         
  P1->REN = 0x12;                      
  P1->OUT = 0x12;                        
}

uint8_t Port1_Input(void){
  return (P1->IN&0x12);                
}


void Port1_Output(uint8_t data){        // write all of P1.0 outputs
  P1->OUT = (P1->OUT&0xFE)|data;
}


void Walk(uint8_t Data)
{
    if(Data == 0x18){
        Motor_Forward(5000,5000);
    }
    else if((Data & 0xF0) != 0){
        Motor_Forward(5000,200);
    }
    else if((Data & 0x0F) != 0){
        Motor_Forward(200,5000);
    }
    else{
        Motor_Forward(0,0);
    }
    Clock_Delay1ms(10);
}


void main(void)
{
    uint8_t status;
    Port1_Init();
    Clock_Init48MHz();
    Motor_Init();

    while(1)
    {      
        status = Port1_Input();
        switch(status)
        {
            case SW1:{        
                Bump_Init(); 
                while(1)
                {
                    Status = Bump_Read();

                    if(Status != 0x3F)
                    {
                        Motor_Backward(5000,5000);
                        Clock_Delay1ms(1000);                
                    }
                    else
                    {
                        Motor_Forward(5000,5000);
                        Clock_Delay1ms(10);
                    }
                }            
            }  
            case SW2:{
                ADC0_InitSWTriggerCh17_12_16();   // initialize channels 17,12,16
                while(1)
                {
                    ADC_In17_12_16(&nr,&nc,&nl);  // sample
                    cdistance = CenterConvert(nc);
                    ldistance = LeftConvert(nl);
                    rdistance = RightConvert(nr);

                    if(cdistance == 50||ldistance == 100||rdistance == 100)
                    {
                        Motor_Backward(5000,5000);
                        Clock_Delay1ms(200);
                        Motor_Left(5000,5000);
                        Clock_Delay1ms(200);
                    }
                    Motor_Forward(5000,5000);
                    Clock_Delay1ms(10);
                }
            } 
            default:{
                Reflectance_Init();     
                Data = Reflectance_Read(1000);   
                switch(Data)
                {
                    case 0x1F:
                    case 0x1E:
                    case 0x0F:{
                        Motor_Forward(5000,5000);
                        Clock_Delay1ms(350);
                        Motor_Left(5000,5000);
                        Clock_Delay1ms(500);
                        break;
                    }
                
                    case 0xF0: 
                    case 0xF8:
                    case 0x78:{
                        Motor_Forward(5000,5000);
                        Clock_Delay1ms(350);
                        Data = Reflectance_Read(1000);
                        if(Data == 0x00)
                        {
                            Motor_Right(5000,5000);
                            Clock_Delay1ms(500);                    
                        }
                        else{
                            Motor_Forward(5000,5000);
                            Clock_Delay1ms(10);
                        }
                        break;                
                    }

                    case 0xFF:{
                        Motor_Forward(5000,5000);
                        Clock_Delay1ms(350);                    
                        Motor_Left(5000,5000);
                        Clock_Delay1ms(500); 
                        Data = Reflectance_Read(500);
                        break;
                    }
                }
                Walk(Data);
            }             
        }
    }
}
