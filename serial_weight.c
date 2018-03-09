#include<at89x52.h>                  

#define ADDO1 P1_0
#define ADSK1 P1_1
#define ADDO2 P1_2
#define ADSK2 P1_3
#define ADDO3 P3_2
#define ADSK3 P3_3
#define ADDO4 P3_4
#define ADSK4 P3_5
#define LED P1_0
#define DQ P1_1 
__sfr __at 0xE7 IAP_CONTR;
#define RESET IAP_CONTR=IAP_CONTR|0x20  

void InitUART (void)
{

    SCON  = 0x50;		        
    TMOD |= 0x20;               
    TH1   = 0xFD;
    TL1   = 0xFD;               
    TR1   = 1;                                       
    EA    = 1;                 
    ES    = 1;                
}   

void SendByte(unsigned char dat)
{
    SBUF = dat;
    while(!TI);
        TI = 0;
}

void SendStr(unsigned char *s)
{
    while(*s!='\0')
    {
        SendByte(*s);
        s++;
    }
}

void DelayUs2x(unsigned char t)
{   
    while(--t);
}

void DelayMs(unsigned char t)
{
    while(t--)
    {
        DelayUs2x(245);
	DelayUs2x(245);
    }
}

void UART_SER(void) __interrupt 4
{
    unsigned char Temp=0;
    if(RI)
    {
        RI=0;
        Temp=SBUF;
        if(Temp=='1')
            RESET;
    }
}

unsigned long get_value1()
{
    unsigned long count;
    unsigned char i;
    ADDO1 = 1;
    ADSK1 = 0;
    count = 0;
    while(ADDO1);
    for(i=0; i<24; i++)
    {
        ADSK1=1;
        count = count<<1;
        ADSK1=0;
        if(ADDO1) count++;
    }
    ADSK1=1;
    DelayUs2x(1);
    count = count^0x800000;
    ADSK1=0;
    return(count);
}   

unsigned long get_value2()
{
    unsigned long count;
    unsigned char i;
    ADDO2 = 1;
    ADSK2 = 0;
    count = 0;
    while(ADDO2);
    for(i=0; i<24; i++)
    {
        ADSK2=1;
        count = count<<1;
        ADSK2=0;
        if(ADDO2) count++;
    }
    ADSK2=1;
    DelayUs2x(1);
    count = count^0x800000;
    ADSK2=0;
    return(count);
}   

unsigned long get_value3()
{
    unsigned long count;
    unsigned char i;
    ADDO3 = 1;
    ADSK3 = 0;
    count = 0;
    while(ADDO3);
    for(i=0; i<24; i++)
    {
        ADSK3=1;
        count = count<<1;
        ADSK3=0;
        if(ADDO3) count++;
    }
    ADSK3=1;
    DelayUs2x(1);
    count = count^0x800000;
    ADSK3=0;
    return(count);
}   

unsigned long get_value4()
{
    unsigned long count;
    unsigned char i;
    ADSK4 = 0;
    count = 0;
    while(ADDO4);
    for(i=0; i<24; i++)
    {
        ADSK4=1;
        count = count<<1;
        ADSK4=0;
        if(ADDO4) count++;
    }
    ADSK4=1;
    DelayUs2x(1);
    count = count^0x800000;
    ADSK4=0;
    return(count);
}  

unsigned long get_value()
{
    unsigned long count_sum=0;
    count_sum = count_sum + get_value1() + get_value2() + get_value3() + get_value4();
    return count_sum;
}


long get_weight(unsigned long offset)
{
    long result = 0;
    result = get_value() - offset;
    return result;
}   

unsigned long get_tare()
{
    unsigned long tare = 0;
    int i;
    for(i=0; i<15; i++)
        tare = tare + get_value();
    tare = tare/15;
    return tare;
}

void ltos(long value_o, unsigned char* weight)
{
    int i,temp,check;
    long value;
    value = value_o;
    if(value<0)
    {
        value = value-1;
        value = ~value;
        for(i=0;i<8;i++)
        {   
            temp = value&0x0000000F;
            weight[10-i] = (temp<10)?('0'+temp):('A'+temp-10);
            value=value>>4;
        }
        weight[0]='1';
    }
    else
    {
        for(i=0; i<8; i++)
        {
            temp = value&0x0000000F;
            weight[10-i] = (temp<10)?('0'+temp):('A'+temp-10);
            value=value>>4;
        }
        weight[0]='0';
    }
    value = value_o;
    if(value<0)
    {
        value = value-1;
        value = ~value;
    }
    check = value&0x000000FF;
    for(i=0;i<3;i++)
    {
        value = value>>8;
        check = check^(value&0x000000FF);
    }
    temp = check&0x0000000F;
    weight[12] = (temp<10)?('0'+temp):('A'+temp-10);
    check = check>>4;
    temp = check&0x0000000F;
    weight[11] = (temp<10)?('0'+temp):('A'+temp-10);
    weight[1]='0';
    weight[2]='x';
    weight[13]='\0';
}                

void main(void)
{
    long value=0;
    unsigned char weight[14]={0};
    unsigned long offset_value=0;
    int i=0;
    
    InitUART();
    offset_value = get_tare();

    while (1)                       
    {
        value = get_weight(offset_value);
        ltos(value,weight);
        SendStr(weight);
        DelayMs(10);
    }
}


