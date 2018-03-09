#include<at89x52.h>                  

#define ADDO P2_0
#define ADSK P2_1
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

__bit Init_DS18B20(void)
{
    __bit dat=0;
    DQ = 1;    
    DelayUs2x(5);   
    DQ = 0;         
    DelayUs2x(200); 
    DelayUs2x(200);
    DQ = 1;        
    DelayUs2x(50); 
    dat=DQ;        
    DelayUs2x(25); 
    return dat;
}

unsigned char ReadOneChar(void)
{
    unsigned char i=0;
    unsigned char dat = 0;
    for (i=8;i>0;i--)
    {
        DQ = 0;
        dat>>=1;
        DQ = 1; 
        if(DQ)
            dat|=0x80;
        DelayUs2x(25);
    }
    return(dat);
}

void WriteOneChar(unsigned char dat)
{
    unsigned char i=0;
    for (i=8; i>0; i--)
    {
        DQ = 0;
        DQ = dat&0x01;
        DelayUs2x(25);
        DQ = 1;
        dat>>=1;
    }
    DelayUs2x(25);
}

unsigned int ReadTemperature(void)
{
    unsigned char a=0;
    unsigned int b=0;
    unsigned int t=0;
    Init_DS18B20();
    WriteOneChar(0xCC); 
    WriteOneChar(0x44); 
    DelayMs(10);
    Init_DS18B20();
    WriteOneChar(0xCC); 
    WriteOneChar(0xBE); 
    a=ReadOneChar();   
    b=ReadOneChar();   
    b<<=8;
    t=a+b;
    return(t);
}

unsigned long get_value()
{
    unsigned long count;
    unsigned char i;
    ADDO = 1;
    ADSK = 0;
    count = 0;
    while(ADDO);
    for(i=0; i<24; i++)
    {
        ADSK=1;
        count = count<<1;
        ADSK=0;
        if(ADDO) count++;
    }
    ADSK=1;
    DelayUs2x(1);
    count = count^0x800000;
    ADSK=0;
    return(count);
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
    check = value&0x000000FF;
    for(i=0;i<4;i++)
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
}   

void uitos(unsigned int temperature_o, unsigned char* weight)
{
    int i,temp,check;
    unsigned int temperature;
    temperature = temperature_o;
    for(i=0; i<4; i++)
    {
        temp = temperature&0x0000000F;
        weight[16-i] = (temp<10)?('0'+temp):('A'+temp-10);
        temperature=temperature>>4;
    }    
    temperature = temperature_o;
    check = temperature&0x000000FF;
    for(i=0;i<2;i++)
    {
        temperature = temperature>>8;
        check = check^(temperature&0x000000FF);
    }
    temp = check&0x0000000F;
    weight[18] = (temp<10)?('0'+temp):('A'+temp-10);
    check = check>>4;
    temp = check&0x0000000F;
    weight[17] = (temp<10)?('0'+temp):('A'+temp-10);
    weight[19]='\0';
}               

void main(void)
{
    long value=0;
    unsigned char weight[20]={0};
    unsigned int temperature=0;
    unsigned long offset_value=0;
    int i=0;
    
    InitUART();
    offset_value = get_tare();

    while (1)                       
    {
        LED = 1;
        value = get_weight(offset_value);
        if(i==0)
            temperature = ReadTemperature();
        ltos(value,weight);
        uitos(temperature,weight);
        SendStr(weight);
        LED = 0;
        i++;
        if(i>1000)
            i=0;    
        DelayMs(10);
    }
}


