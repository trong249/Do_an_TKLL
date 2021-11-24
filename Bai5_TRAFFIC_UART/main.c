#include "main.h"
// Noi khai bao hang so
#define     ON      1
#define     OFF     0

#define PORTD_OUT   PORTD
#define TRISD_OUT   TRISD

#define LED_data    0
#define LED_clock   1
#define LED_latch   2
#define LED1        0x01
#define LED2        0x02
#define LED3        0x04
#define LED4        0x08

void init_output(void);
// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput[9] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x01};// arrayMapOfOutput[9] use for clock;
unsigned char statusOutput[9] = {0,0,0,0,0,0,0,0,0};
unsigned char status=OFF;
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
unsigned char isButtonMotorOn();
unsigned char isButtonMotorOff();
void MotorOn();
void MotorOff();
void BaiTap_Motor();
void Test_KeyMatrix();
//Khai bao cac ham hien thi led 

unsigned char LED[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
void HC595_write(int value);
void display(int led_name, int led_data);


// Den giao thong
void Phase1_GreenOn();
void Phase1_GreenOff();
void Phase1_YellowOn();
void Phase1_YellowOff();
void Phase1_RedOn();
void Phase1_RedOff();

void Phase2_GreenOn();
void Phase2_GreenOff();
void Phase2_YellowOn();
void Phase2_YellowOff();
void Phase2_RedOn();
void Phase2_RedOff();

#define     INIT_SYSTEM         255
#define     PHASE1_GREEN        0
#define     PHASE1_YELLOW       1
#define     PHASE2_GREEN        2
#define     PHASE2_YELLOW       3
#define     WAIT                4

unsigned char statusOfLight = INIT_SYSTEM;
unsigned char timeOfGreenPhase1 = 25;
unsigned char timeOfYellowPhase1 = 3;
unsigned char timeOfGreenPhase2 = 20;
unsigned char timeOfYellowPhase2 = 3;
unsigned char timeOfLight = 0;
unsigned char cntOfLight = 0;
void AppTrafficLight();
void UartDataReceiveProcess();
void UartDataReceiveProcess_ElectronicDeviceControl();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
        delay_ms(1000);
	while (1)
	{
//            while (!flag_timer3){
//                
//            }
//            flag_timer3 = 0;
//            scan_key_matrix(); // 8 button
//            AppTrafficLight();
//            DisplayLcdScreenOld(); //Output process 14ms vs 10ms with no timer  
        display(LED1,LED[0]);
        display(LED2,LED[1]);
    }
}

////////////////////////////////////////////////////////////////////
// Hien thuc cac module co ban cua chuong trinh
////////////////////////////////////////////////////////////////////

void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_output(void)
{
   TRISD_OUT = 0x00;
   PORTD_OUT = 0x00;
}

void init_system(void)
{
    init_output();
    lcd_init();
    init_key_matrix();
    init_interrupt();
    lcd_clear();
    LcdClearS();
    delay_ms(500);
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer1(9390);//dinh thoi 2ms
    init_timer3(46950);//dinh thoi 10ms
    SetTimer0_ms(2);
    SetTimer1_ms(10);
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
//    PORTAbits.RA0 = 1;
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT | arrayMapOfOutput[index];
	}
}
void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<14 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}
////////////////////////////////////////////////////////////////////
// Hien thuc cac ham hien thi 7SEG
////////////////////////////////////////////////////////////////////
void HC595_write(int value){
    int i=0;
    for(i=0;i<8;i++){
        if(((value<<i)&0x80)==0x80 ) OpenOutput(LED_data); 
        else CloseOutput(LED_data);
        OpenOutput(LED_clock);
        CloseOutput(LED_clock);
    }
}
void display(int led_name, int led_data){
    HC595_write(led_data);
    HC595_write(led_name);
    OpenOutput(LED_latch);
    CloseOutput(LED_latch);
}





////////////////////////////////////////////////////////////////////
// Hien thuc cac module co ban cua Traffic-app
////////////////////////////////////////////////////////////////////
void Phase1_GreenOn()
{
    OpenOutput(0);
}
void Phase1_GreenOff()
{
    CloseOutput(0);
}

void Phase1_YellowOn()
{
    OpenOutput(4);
}
void Phase1_YellowOff()
{
    CloseOutput(4);
}

void Phase1_RedOn()
{
    OpenOutput(6);
}
void Phase1_RedOff()
{
    CloseOutput(6);
}

void Phase2_GreenOn()
{
    OpenOutput(1);
}
void Phase2_GreenOff()
{
    CloseOutput(1);
}

void Phase2_YellowOn()
{
    OpenOutput(5);
}
void Phase2_YellowOff()
{
    CloseOutput(5);
}

void Phase2_RedOn()
{
    OpenOutput(7);
}
void Phase2_RedOff()
{
    CloseOutput(7);
}

void AppTrafficLight()
{
    cntOfLight = (cntOfLight + 1)%20;
    if (cntOfLight == 0)
        timeOfLight --;
    switch (statusOfLight)
    {
        
    }
}





