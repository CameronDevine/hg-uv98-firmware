
#include "STC15Fxxxx.H"
#include "SENSOR\DS18B20.h"
#include "tostring.H"
#include "MAIN\UART1.H"



#include  <INTRINS.H> //Keil library 

#define   uchar unsigned char	   //0-255���ֽ�	��uchar ����char
#define   uint unsigned int		   //0-65536˫�ֽ�	��uint ����int
#include "HEX_TO_ASC.H"

long DS18B20_WENDU;
uchar  DS18B20_TEMP[10];

sbit DQ = P2^1;                     //DS18B20�����ݿ�λP3.3
uint TPH;                           //����¶�ֵ�ĸ��ֽ�
uint TPL;                           //����¶�ֵ�ĵ��ֽ�

void DelayXus(uchar n);
uchar DS18B20_Reset();
void DS18B20_WriteByte(uchar dat);
uchar DS18B20_ReadByte();


/*************************************************** 
	DS18B20���¶ȳ���
****************************************************/ 
uchar DS18B20_READTEMP() 
{ 
    uchar i,n;
	uint temp;
	bit   ZERO; //0=���ϣ�1=����

	if (DS18B20_Reset()==0)	 {return 0;	 }
	
	//DS18B20_Reset();                //�豸��λ
    DS18B20_WriteByte(0xCC);        //����ROM����
    DS18B20_WriteByte(0x44);        //��ʼת������
    while (!DQ);                    //�ȴ�ת�����

	if (DS18B20_Reset()==0)	{return 0; 	 }
	
    //DS18B20_Reset();                //�豸��λ
    DS18B20_WriteByte(0xCC);        //����ROM����
    DS18B20_WriteByte(0xBE);        //���ݴ�洢������
    TPL = DS18B20_ReadByte();       //���¶ȵ��ֽ�
    TPH = DS18B20_ReadByte();       //���¶ȸ��ֽ�

// UART1_SendData(0xaa);	UART1_SendData(TPH);		UART1_SendData(TPL);

	for(i=0;i<6;i++)   {DS18B20_TEMP[i]=0x00;}

	if ((TPH&0x80)==0x80)	  //���϶�<0	
	{
		ZERO=1;
		DS18B20_WENDU=-(~((TPH*256)+TPL)+1)*0.0625*10; 	 //����1λС��
		//-0.1 ~ -54.5
		DS18B20_TEMP[0]='-';
	  	}
	else
	{					  //���϶�>=0 
		ZERO=0;
		DS18B20_WENDU=((TPH*256)+TPL)*0.0625*10; 	   //����1λС��
		//0~124.5
		DS18B20_TEMP[0]=' ';
	}
 	if (ZERO==1) { temp=(uint)-DS18B20_WENDU; }else{temp=(uint)DS18B20_WENDU;}	 
	



	tostring(temp);

	DS18B20_TEMP[1]=wan;
	DS18B20_TEMP[2]=qian;
	DS18B20_TEMP[3]=bai;
	DS18B20_TEMP[4]=shi;
	DS18B20_TEMP[5]='.';
	DS18B20_TEMP[6]=ge;
	DS18B20_TEMP[7]=0x00;

	n=0;
	while(DS18B20_TEMP[1]=='0')	 //��λ0���� ,���ٱ���һλ
	{
	for(i=1;i<8;i++) 	{ DS18B20_TEMP[i]=DS18B20_TEMP[i+1]; }
	n++; if (n>2){break;}
	}

UART1_SendString("DS18B20: ");	UART1_SendString(DS18B20_TEMP);	UART1_SendString("\r\n");

	return 1;
}

/**************************************
��ʱX΢��(STC12C5A60S2@12M)
��ͬ�Ĺ�������,��Ҫ�����˺���
����ʱ������ʹ��1T��ָ�����ڽ��м���,�봫ͳ��12T��MCU��ͬ
**************************************/
void DelayXus(uchar n)		  //22.1184M
{	 	
	while (n--)
	{
	_nop_();_nop_();_nop_();_nop_();    _nop_();_nop_();_nop_();_nop_();  	_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();    _nop_();_nop_();_nop_();_nop_();  	_nop_();_nop_();
	_nop_();       _nop_();  

//
//  _nop_();_nop_();_nop_();_nop_();    _nop_();_nop_();_nop_();_nop_();  	_nop_();_nop_();
//	_nop_();_nop_();_nop_();_nop_();    _nop_();_nop_();_nop_();_nop_();  	_nop_();_nop_();
//	_nop_();       _nop_();  
//


	}
}






/**************************************
��λDS18B20,������豸�Ƿ����

**************************************/
uchar DS18B20_Reset()
{
	CY = 1;
    DQ = 0;                     //�ͳ��͵�ƽ��λ�ź�
    DelayXus(240);              //��ʱ����480us
    DelayXus(240);              //��ʱ����480us

    DQ = 1;                     //�ͷ�������
    DelayXus(60);               //�ȴ�60us
    CY = DQ;                    //����������
    DelayXus(240);              //�ȴ��豸�ͷ�������
	DelayXus(180);
	if (CY==0) 		{return 1;}	   //����������
	return 0;
}

/**************************************
��DS18B20��1�ֽ�����
**************************************/
uchar DS18B20_ReadByte()
{
    uchar i;
    uchar dat = 0;

    for (i=0; i<8; i++)             //8λ������
    {	
		dat >>= 1;
        DQ = 0;                     //��ʼʱ��Ƭ
        DelayXus(1);                //��ʱ�ȴ�10us
        DQ = 1;                     //׼������
        DelayXus(1);                //������ʱ10us
        if (DQ) dat |= 0x80;        //��ȡ����
        DelayXus(60);               //�ȴ�60usʱ��Ƭ����
    }

    return dat;
}

/**************************************
��DS18B20д1�ֽ�����
**************************************/
void DS18B20_WriteByte(uchar dat)
{
    char i;

    for (i=0; i<8; i++)             //8λ������
    {
        DQ = 0;                     //��ʼʱ��Ƭ
        DelayXus(1);                //��ʱ�ȴ�10us
        dat >>= 1;                  //�ͳ�����
        DQ = CY;
        DelayXus(60);               //�ȴ�60usʱ��Ƭ����
        DQ = 1;                     //�ָ�������
        DelayXus(1);                //�ָ���ʱ10us
    }
}
