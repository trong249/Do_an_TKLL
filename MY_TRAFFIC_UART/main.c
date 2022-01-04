#include "main.h"
// Noi khai bao hang so
#define     ON          1
#define     OFF         0
#define     STOP        2
#define  INIT_TIMER     50      // milisecond
#define  ONE_SEC        200     // milisecond

#define  IN_NOR          1
#define  IN_YELLOW       0

#define PORTD_OUT   PORTD
#define TRISD_OUT   TRISD
#define PORTE_OUT   PORTE   
#define TRISE_OUT   TRISE
#define TRISA_OUT   TRISA
#define TRISB_OUT   TRISB



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
// Den giao thong
#define     INIT_SYSTEM         255
#define     STOP_MODE           1
#define     NORMAL_MODE         2
#define     YELLOW_MODE         3
#define     GREEN_MODE          4
#define     WAIT                5

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
void UART_Func();

void appRunNormal_Phase1();
void appRunNormal_Phase2();

unsigned char isButtonNext();
unsigned char ButtonCounterUp();
unsigned char ButtonCounterDown();
unsigned char isButtonApply();

#define ADDRESS_SECOND  0x00
#define ADDRESS_MINUTE  0x01
#define ADDRESS_HOUR    0x02
#define ADDRESS_DATE    0x04
#define ADDRESS_MONTH   0x05
#define ADDRESS_YEAR    0x06
#define ADDRESS_FIRST_PROGRAM 0x20


void SetupForFirstProgram(void);
char second = 0, minute = 0, hour = 0;
char secondToNor = 15, minuteToNor = 27, hourToNor = 21;
char secondToYellow = 0, minuteToYellow = 27, hourToYellow = 21;
char secondSub = 0, minuteSub = 0, hourSub = 0;
char date = 0, month = 0, year = 0;
unsigned char signalToPrintCountDown=IN_NOR;
void SetupTimeForRealTime();
void SendTime(void);
void subTime(char h, char m, char s);
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
    delay_ms(1000);
    SetupForFirstProgram();
	while (1)
	{
        
            while (!flag_timer3);
            flag_timer3 = 0;
            
            counterTimer++;
            if(counterTimer==(ONE_SEC)/INIT_TIMER){ 
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
   TRISA_OUT = 0x00;		//setup PORTB is output
   TRISB_OUT = 0x00;
}

void init_system(void)
{
    init_output();
    init_lcd();
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
    init_key_matrix_with_uart();
    init_uart();
    init_adc();
    init_i2c();
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
//
//
//
//

////////////////////////////////////////////////////////////////////
// Hien thuc  Traffic-app
////////////////////////////////////////////////////////////////////
void AppTrafficLight()
{
    
   
    UART_Func();
    if(hour==hourToNor && minute==minuteToNor && second==secondToNor){
        statusOfApp=NORMAL_MODE;
        signalToPrintCountDown=IN_NOR;
    }
        
    if(hour==hourToYellow && minute==minuteToYellow && second==secondToYellow){
        statusOfApp=YELLOW_MODE;
        signalToPrintCountDown=IN_YELLOW;
    }
        
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
            LcdPrintStringS(0,0,"NORMAL  00:00:00");
            
            subTime(hourToYellow,minuteToYellow,secondToYellow);
            if(hourSub<10){
                LcdPrintNumS(0,8,0);
                LcdPrintNumS(0,9,hourSub);
            }
            else{
                LcdPrintNumS(0,8,hourSub);
            }
            LcdPrintStringS(0,10,":");
            if(minuteSub<10){
                LcdPrintNumS(0,11,0);
                LcdPrintNumS(0,12,minuteSub);
            }
            else{
                LcdPrintNumS(0,11,minuteSub);
            }
            LcdPrintStringS(0,13,":");
            if(secondSub<10){
                LcdPrintNumS(0,14,0);
                LcdPrintNumS(0,15,secondSub);
            }
            else{
                LcdPrintNumS(0,14,secondSub);
            }
            
            
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
            if(signalToPrintCountDown==IN_YELLOW){
                subTime(hourToNor,minuteToNor,secondToNor);
                if(hourSub<10){
                    LcdPrintNumS(1,0,0);
                    LcdPrintNumS(1,1,hourSub);
                }
                else{
                    LcdPrintNumS(1,0,hourSub);
                }
                LcdPrintStringS(1,2,":");
                if(minuteSub<10){
                    LcdPrintNumS(1,3,0);
                    LcdPrintNumS(1,4,minuteSub);
                }
                else{
                    LcdPrintNumS(1,3,minuteSub);
                }
                LcdPrintStringS(1,5,":");
                if(secondSub<10){
                    LcdPrintNumS(1,6,0);
                    LcdPrintNumS(1,7,secondSub);
                }
                else{
                    LcdPrintNumS(1,6,secondSub);
                }
                
            }
            LcdPrintNumS(1,12,counterMode);
            
            
           
            
            appRunYellowMode();
            if( isButtonNext() ){
                statusOfApp=GREEN_MODE;
            } 
            break;
            
            
        case GREEN_MODE:
            LcdClearS();
            LcdPrintStringS(0,0,"   GREEN_MODE   ");
            LcdPrintNumS(1,12,counterMode);
            
            
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
    if (key_code[4] == 1)
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
    signal7SEG=STOP;
}
void appRunNormal(){
    signal7SEG=ON;
    appRunNormal_Phase1();
    appRunNormal_Phase2();    
    updateValue7Seg(TimeLine_Phase1,TimeLine_Phase2);
    
}
void appRunYellowMode(){
    signal7SEG=OFF;
    if( counterTimer==0){
        displayTrafficLed(YELLOW,1);
        displayTrafficLed(YELLOW,2);   
    }
    if( counterTimer==(ONE_SEC/(2*INIT_TIMER))){
        CloseOutput(v1);
        CloseOutput(v2);
    }
    ButtonCounterUp();
    ButtonCounterDown();
    isButtonApply();
}
void appRunGreenMode(){
    signal7SEG=OFF;
    if( counterTimer==0){
        displayTrafficLed(GREEN,1);
        displayTrafficLed(GREEN,2);
    }
    if( counterTimer==(ONE_SEC/(2*INIT_TIMER))){
        CloseOutput(x1);
        CloseOutput(x2);
    }
    
    ButtonCounterUp();
    ButtonCounterDown();
    isButtonApply();
}

void appRunNormal_Phase1(){
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
////////////////////////////////////////////////////////////////////
// UART
////////////////////////////////////////////////////////////////////
void UART_Func(){
    int adcValue;
    unsigned int JAM;
    adcValue = get_adc_value();
    JAM = 0 + (long)(adcValue - 0) * (10000 - 0) / (1023 - 0);
    SendTime();
    UartSendString("  ");
    switch(statusOfApp){
        case NORMAL_MODE:
            UartSendString("NORMAL_MODE");
            break;
        case STOP_MODE:
            UartSendString("STOP_MODE");
            break;
        case YELLOW_MODE:
            UartSendString("YELLOW_MODE");
            break;
        case GREEN_MODE:
            UartSendString("GREEN_MODE");
            break;             
    }
    UartSendString("  ");
    UartSendString("RED:");
    UartSendNum(RedTime);
    UartSendString("  ");
    UartSendString("GREEN:");
    UartSendNum(GreenTime);
    UartSendString("  ");
    UartSendString("YELLOW:");
    UartSendNum(YellowTime);
    
    
    UartSendString("  ");
    UartSendNumPercent(JAM);
    UartSendString("%\r\n");
}

void SendTime(void)
{
    second = read_ds1307(ADDRESS_SECOND);
    minute = read_ds1307(ADDRESS_MINUTE);
    hour = read_ds1307(ADDRESS_HOUR);
    date = read_ds1307(ADDRESS_DATE);
    month = read_ds1307(ADDRESS_MONTH);
    year = read_ds1307(ADDRESS_YEAR);
    
//    UartSendString("20.04.16 09:12:07  pH=  ");
    UartSendNum(date/10);
    UartSendNum(date%10);
    UartSendString(".");
    UartSendNum(month/10);
    UartSendNum(month%10);
    UartSendString(".");
    UartSendNum(year/10);
    UartSendNum(year%10);
    UartSendString(" ");
    UartSendNum(hour/10);
    UartSendNum(hour%10);
    UartSendString(":");
    UartSendNum(minute/10);
    UartSendNum(minute%10);
    UartSendString(":");
    UartSendNum(second/10);
    UartSendNum(second%10);
}
void SetupForFirstProgram(void)
{
    if(read_ds1307(ADDRESS_FIRST_PROGRAM) != 0x22)
    {
        SetupTimeForRealTime();
        write_ds1307(ADDRESS_FIRST_PROGRAM, 0x22);
    }
}

void SetupTimeForRealTime()
{
    second = 1;
    minute = 1;
    hour = 8;
    date = 6;
    month = 1;
    year = 22;
    
    write_ds1307(ADDRESS_SECOND, second);
    write_ds1307(ADDRESS_MINUTE, minute);
    write_ds1307(ADDRESS_HOUR, hour);
    write_ds1307(ADDRESS_DATE, date);
    write_ds1307(ADDRESS_MONTH, month);
    write_ds1307(ADDRESS_YEAR, year);
}
void subTime(char h, char m, char s){
    if(s<second){
        s=s+60;
        m=m-1;
        if(m<0){
            m=59;
            h=h-1;
            if(h<0)
                h=23;
        }
    }
    secondSub=s-second;
    
    if(m<minute){
        m=m+60;
        h=h-1;
        if(h<0)
            h=23;
    }
    minuteSub=m-minute;
    
    if(h<hour){
        h=h+24;
    }
    hourSub=h-hour;
}




