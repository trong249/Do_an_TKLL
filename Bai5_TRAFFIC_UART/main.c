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
void Test_KeyMatrix();
//Khai bao cac ham hien thi led 

unsigned char LED[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
void HC595_write(unsigned char value);
void display(unsigned char led_name, unsigned char led_data);


// Den giao thong
#define     INIT_SYSTEM         255
#define     NORMAL_MODE         0
//#define     RED_MODE            1   Red_mod k can thiet
#define     YELLOW_MODE         2
#define     GREEN_MODE          3
#define     WAIT                4

unsigned char statusOfApp = NORMAL_MODE;
unsigned int counterMode = 32;
unsigned int RedTime = 0;
unsigned int YellowTime = 0;
unsigned int GreenTime = 0;

void AppTrafficLight();
void appRunNormal();
void appRunYellowMode();
void appRunGreenMode();

void show_Ok();

unsigned char isButtonNext();
unsigned char ButtonCounterUp();
unsigned char ButtonCounterDown();
unsigned char isButtonApply();

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
            while (!flag_timer3);
            flag_timer3 = 0;
            scan_key_matrix(); // 8 button
            AppTrafficLight();
            DisplayLcdScreen();
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
void HC595_write(unsigned char value){
    int i=0;
    for(i=0;i<8;i++){
        if((  (value<<i) & 0x80) == 0x80 ){
            
            OpenOutput(LED_data);
        }
             
        else {
            
            CloseOutput(LED_data);
        }
        
        OpenOutput(LED_clock);
        CloseOutput(LED_clock);
    }
}
void display(unsigned char led_name, unsigned char led_data){
    HC595_write(led_data);
    HC595_write(led_name);
    
    OpenOutput(2);
    CloseOutput(2);
}





////////////////////////////////////////////////////////////////////
// Hien thuc  Traffic-app
////////////////////////////////////////////////////////////////////
void AppTrafficLight()
{
    switch (statusOfApp)
    {
        case NORMAL_MODE:
            LcdPrintStringS(0,0,"     NORMAL     ");
            LcdPrintStringS(1,0,"                ");
//            appRunNormal();
            if( isButtonNext() ){
                statusOfApp=YELLOW_MODE;
            }
            break;
            
            
        case YELLOW_MODE:
            LcdPrintStringS(0,0,"   YELLOW_MODE  ");
            LcdPrintStringS(1,10,YellowTime);
            appRunYellowMode();
            if( isButtonNext() ){
                statusOfApp=GREEN_MODE;
            } 
            break;
            
            
        case GREEN_MODE:
            LcdPrintStringS(0,0,"   GREEN_MODE   ");
            LcdPrintStringS(1,10,GreenTime);
            appRunGreenMode();
            if( isButtonNext() ){
                statusOfApp=NORMAL_MODE;
            }
            break;
        
        default:
            statusOfApp=NORMAL_MODE;
            break;
            
            
    }
}
////////////////////////////////////////////////////////////////////
// Hien thuc chuc nang phim
////////////////////////////////////////////////////////////////////
unsigned char isButtonNext()
{
    if (key_code[3] == 1)
        return 1;
    else
        return 0;
}
unsigned char ButtonCounterUp(){
    if (key_code[2] == 1){
        counterMode++;
        if(counterMode>99) counterMode=0;
        return 1;
    }
    else return 0;
}
unsigned char ButtonCounterDown(){
    if (key_code[1] == 1){
        counterMode--;
        if(counterMode<0) counterMode=99;
        return 1;
    }
    else return 0;
}
unsigned char isButtonApply(){
    int check=0;
    switch (statusOfApp){
        case YELLOW_MODE:   
            if (key_code[0] == 1){
                YellowTime=counterMode;
                RedTime=YellowTime+GreenTime;
                check=1;
            }        
        break;
        case GREEN_MODE:   
            if (key_code[0] == 1){
                GreenTime=counterMode;
                RedTime=YellowTime+GreenTime;
                check=1;
            }
        break;
    }
    if(check==1){
//        in ra thong bao oke
        return 1;
    }
    else return 0;
        
}
////////////////////////////////////////////////////////////////////
// Hien thuc cac trang thai cua tung mode
////////////////////////////////////////////////////////////////////
void appRunNormal(){
    
}
void appRunYellowMode(){

    ButtonCounterUp();
    ButtonCounterDown();
    isButtonApply();
}
void appRunGreenMode(){
    
    ButtonCounterUp();
    ButtonCounterDown();
    isButtonApply();
}




