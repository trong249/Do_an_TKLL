#include "main.h"
// Noi khai bao hang so
#define     ON          1
#define     OFF         0
#define  INIT_TIMER     50

#define PORTD_OUT   PORTD
#define TRISD_OUT   TRISD
#define PORTE_OUT   PORTE   
#define TRISE_OUT   TRISE

#define LED_data    0
#define LED_clock   1
#define LED_latch   2
#define LED1        0x01
#define LED2        0x02
#define LED3        0x04
#define LED4        0x08

#define d1          3
#define v1          4
#define x1          5
#define d2          6
#define v2          7
#define x2          8


void init_output(void);
// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [9] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x01};
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
void display7Seg(unsigned char led_name, unsigned char led_data);
void displayTrafficLed(unsigned char statusLight, int LightPhase);
void turn_off_7Seg();

// Den giao thong
#define     INIT_SYSTEM         255
#define     NORMAL_MODE         0
#define     STOP_MODE       1
#define     YELLOW_MODE         2
#define     GREEN_MODE          3
#define     WAIT                4

#define     RED                 1
#define     YELLOW              2
#define     GREEN               3



unsigned char statusOfApp = NORMAL_MODE;
unsigned char status_phase1=RED;
unsigned char status_phase2=GREEN;

unsigned char statusLight = RED;
unsigned char counterTimer = 0;
unsigned char counterMode = 0;
unsigned char RedTime = 5;
unsigned char YellowTime = 2;
unsigned char GreenTime = 3;
unsigned char TimeLine_Phase1=5;
unsigned char TimeLine_Phase2=3;

void AppTrafficLight();

void appRunStopMode();
void appRunNormal();
void appRunYellowMode();
void appRunGreenMode();

void appRunNormal_Phase1();
void appRunNormal_Phase2();

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
            
            counterTimer++;
            if(counterTimer==(1000)/INIT_TIMER){ 
//                ReverseOutput(0); test dem 1s
                    counterTimer=0;  // sau 1 chu ki neu counterTimer=0 thi da dem dc 1s
            }
            
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
   TRISE_OUT = 0x08;
   PORTE_OUT = 0x08;
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
    SetTimer3_ms(INIT_TIMER); //Chu ky thuc hien viec xu ly input,proccess,output
//    PORTAbits.RA0 = 1;
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT | arrayMapOfOutput[index];
	}
    else if(index==8){
        PORTE_OUT = PORTE_OUT | arrayMapOfOutput[index];
    }
}
void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT & ~arrayMapOfOutput[index];
	}
    else if(index==8){
        PORTE_OUT = PORTE_OUT & ~arrayMapOfOutput[index];
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
// Hien thuc cac ham hien thi 7SEG va cac led xanh do vang
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
void display7Seg(unsigned char led_name, unsigned char led_data){
    HC595_write(led_data);
    HC595_write(led_name);
    
    OpenOutput(LED_latch);
    CloseOutput(LED_latch);
}
void turn_off_7Seg(){
    HC595_write(0x00);
    HC595_write(0x00);
    
    OpenOutput(LED_latch);
    CloseOutput(LED_latch);
}


void displayTrafficLed(unsigned char statusLight, int LightPhase){
    
    switch (statusLight){
        case RED:
            if(LightPhase==1){
                OpenOutput(d1);
                CloseOutput(v1);
                CloseOutput(x1);
            }
            else{
                OpenOutput(d2);
                CloseOutput(v2);
                CloseOutput(x2);
            }
            break;

                    
        case YELLOW:
            if(LightPhase==1){
                OpenOutput(v1);
                CloseOutput(d1);
                CloseOutput(x1);
            }
            else{
                OpenOutput(v2);
                CloseOutput(d2);
                CloseOutput(x2);
            }
            break;
            
            
        case GREEN:
            if(LightPhase==1){
                OpenOutput(x1);
                CloseOutput(v1);
                CloseOutput(d1);
            }
            else{
                OpenOutput(x2);
                CloseOutput(v2);
                CloseOutput(d2);
            }
            break;

    }
    
}





////////////////////////////////////////////////////////////////////
// Hien thuc  Traffic-app
////////////////////////////////////////////////////////////////////
void AppTrafficLight()
{
    
    
    switch (statusOfApp)
    {
        case STOP_MODE:
            LcdClearS();
            LcdPrintStringS(0,0,"   STOP_MODE    ");            
            appRunStopMode();
            if( isButtonNext() ){
                statusOfApp=NORMAL_MODE;
                
                status_phase1=RED;
                status_phase2=GREEN;

                counterTimer = 0;
                TimeLine_Phase1=RedTime;
                TimeLine_Phase2=GreenTime;
            }
            break;
            
        case NORMAL_MODE:
            LcdClearS();
            LcdPrintStringS(0,0,"     NORMAL     ");
            LcdPrintStringS(1,0,"                ");
            LcdPrintStringS(1,0,"R:");
            LcdPrintNumS(1,2,RedTime);
            LcdPrintStringS(1,6,"Y:");
            LcdPrintNumS(1,8,YellowTime);
            LcdPrintStringS(1,12,"G:");
            LcdPrintNumS(1,14,GreenTime);
            
            appRunNormal();
            if( isButtonNext() ){
                statusOfApp=YELLOW_MODE;
            }
            if( key_code[0] == 1 ){
                statusOfApp=STOP_MODE;
            }
            break;
            
            
        case YELLOW_MODE:
            LcdClearS();
            LcdPrintStringS(0,0,"   YELLOW_MODE  ");
            LcdPrintNumS(1,10,counterMode);
            
            
            appRunYellowMode();
            if( isButtonNext() ){
                statusOfApp=GREEN_MODE;
            } 
            break;
            
            
        case GREEN_MODE:
            LcdClearS();
            LcdPrintStringS(0,0,"   GREEN_MODE   ");
            LcdPrintNumS(1,10,counterMode);
            
            
            appRunGreenMode();
            if( isButtonNext() ){
                statusOfApp=NORMAL_MODE;
                
                status_phase1=RED;
                status_phase2=GREEN;

                counterTimer = 0;
                TimeLine_Phase1=RedTime;
                TimeLine_Phase2=GreenTime;
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
        LcdPrintStringS(1,10,"  ");
        LcdPrintNumS(1,10,counterMode);
        return 1;
    }
    else return 0;
}
unsigned char ButtonCounterDown(){
    if (key_code[1] == 1){
        counterMode--;
        if(counterMode<=0) counterMode=99;
        LcdPrintStringS(1,10,"  ");
        LcdPrintNumS(1,10,counterMode);
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
void appRunStopMode(){
    displayTrafficLed(RED,1);
    displayTrafficLed(RED,2);
    
    display7Seg(LED1,LED[8]);
    display7Seg(LED2,LED[8]);
    display7Seg(LED3,LED[8]);
    display7Seg(LED4,LED[8]);
}
void appRunNormal(){
    appRunNormal_Phase1();
    appRunNormal_Phase2();                
}
void appRunYellowMode(){
    turn_off_7Seg();
    if( counterTimer==0){
        displayTrafficLed(YELLOW,1);
        displayTrafficLed(YELLOW,2);
    }
    if( counterTimer==(1000/(2*INIT_TIMER))){
        CloseOutput(v1);
        CloseOutput(v2);
    }
    ButtonCounterUp();
    ButtonCounterDown();
    isButtonApply();
}
void appRunGreenMode(){
    turn_off_7Seg();
    if( counterTimer==0){
        displayTrafficLed(GREEN,1);
        displayTrafficLed(GREEN,2);
    }
    if( counterTimer==(1000/(2*INIT_TIMER))){
        CloseOutput(x1);
        CloseOutput(x2);
    }
    
    ButtonCounterUp();
    ButtonCounterDown();
    isButtonApply();
}

void appRunNormal_Phase1(){
    
    unsigned char dv=TimeLine_Phase1%10;
    unsigned char chuc=TimeLine_Phase1/10;
    display7Seg(LED1,LED[chuc]);
    display7Seg(LED2,LED[dv]);
    
    switch(status_phase1){
        case RED:
            displayTrafficLed(RED,1);
            if(counterTimer==0){
                TimeLine_Phase1--;
                if(TimeLine_Phase1==0){
                    TimeLine_Phase1=GreenTime;
                    status_phase1=GREEN;
                }
            }
            break;
            
        case GREEN:
            displayTrafficLed(GREEN,1);
            if(counterTimer==0){
                TimeLine_Phase1--;
                if(TimeLine_Phase1==0){
                    TimeLine_Phase1=YellowTime;
                    status_phase1=YELLOW;
                }
            }
            break;
        
        case YELLOW:
            displayTrafficLed(YELLOW,1);
            if(counterTimer==0){
                TimeLine_Phase1--;
                if(TimeLine_Phase1==0){
                    TimeLine_Phase1=RedTime;
                    status_phase1=RED;
                }
            }
            break;
            
        
    }
}
void appRunNormal_Phase2(){
    
    unsigned char dv=TimeLine_Phase2%10;
    unsigned char chuc=TimeLine_Phase2/10;
    display7Seg(LED3,LED[chuc]);
    display7Seg(LED4,LED[dv]);
    
    switch(status_phase2){
        case RED:
            displayTrafficLed(RED,2);
            if(counterTimer==0){
                TimeLine_Phase2--;
                if(TimeLine_Phase2==0){
                    TimeLine_Phase2=GreenTime;
                    status_phase2=GREEN;
                }
            }
            break;
            
        case YELLOW:
            displayTrafficLed(YELLOW,2);
            if(counterTimer==0){
                TimeLine_Phase2--;
                if(TimeLine_Phase2==0){
                    TimeLine_Phase2=RedTime;
                    status_phase2=RED;
                }
            }
            break;
            
        case GREEN:
            displayTrafficLed(GREEN,2);
            if(counterTimer==0){
                TimeLine_Phase2--;
                if(TimeLine_Phase2==0){
                    TimeLine_Phase2=YellowTime;
                    status_phase2=YELLOW;
                }
            }
            break;
  }
}
    




