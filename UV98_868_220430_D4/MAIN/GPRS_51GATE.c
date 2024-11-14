/*------------------------------------------------------*/
/* ---  		       	     -------------------------------*/
/* --- MCU = STC8A8K64D4	22.1184M				 -------------*/
/* --- WeChat: (86)13013684000  ------------------------*/
/* ---   				----------------------------------------*/
/* --- Tel: 13013684000  15052205720--------------------*/
/* --- Web: BH4TDV.TAOBAO.com --------------------------*/
/*------------------------------------------------------*/

//δָ�����ȵ��ı����ַ�����ĩβ�������Զ���0x00
/////////////////////////////////////////////////
//ע��: nϵ�е�оƬ,�ϵ��������PWM��ص�IO�ھ�Ϊ
//      ����̬,�轫��Щ������Ϊ׼˫��ڻ�ǿ����ģʽ��������ʹ��
/////////////////////////////////////////////////

#include  <INTRINS.H> //Keil library 
#include "STC8A8K64D4.H"
#include "STC_EEPROM.H"
#include "PUBLIC_BUF.H"

#include "UART1.H"
#include "UART2.H"
#include "UART3.H"
#include "UART4.H"

#include "DELAY.H"
#include "ADC.H"

#include "SENSOR\DS18B20.h"
#include "SENSOR\bmp280.h"
#include "IO.H"
#include "GPS2.H"
#include "DISP_GPS.H"
#include "CMX865A_DECODE.H"



#include "KISS_DECODE.H"


#include "KISS2ASC.H"

#include "BEACON.H"

#include "CMX865A_SPI.H"

#include "AT24C512.h"


#include "BT.H"

/************* ��ʱ�ű���� **************/
void Timer0_init();			   					//��ʱ��0��ʼ��
unsigned int  time_a;	//50MSʱ�䵥λ����
/************* ͨ�� **************/
void  Initial_51G2();



uchar BT_LINK;
uint BT_LINK_TIME;


sbit TXD_1	= P3 ^ 1;	 //
sbit RXD_1  = P3 ^ 0;	 //


sbit RXD3	= P0 ^ 0;	 //
sbit TXD3	= P0 ^ 1;


//#define SLEEP      PCON= 0x02;		  //����ģʽ������0.1uA���жϴ�������0000H��ʼ
//#define SHUT       PCON= 0x01;		  //����ģʽ������2MA���жϴ����󣬼�����һ��ָ��

//CMX865A_TX_TONE(0x01); CMX865A_TX_TONE(0);	 	BEACON_GPS_TXD();
//
//void INT0_int (void) interrupt 0	   //�ж���CPU�޷���������
//{

//}
//
////
////IE.0 0 �ⲿ�ж�0
////IE.1 1 ��ʱ��0 ���
////IE.2 2 �ⲿ�ж�1
////IE.3 3 ��ʱ��1 ���
////IE.4 4 �����ж�
////IE.5 5 ��ʱ��2 ���
//

void INT0_int (void) interrupt 0	   //�ж���CPU�޷���������
{
    BT_LINK_TIME = 0;
}

void time0_init()
{

    TIME_1S = 0;

    time_a = 0;				//40*25mS=1S
    /************* ��ʼ����ʱ�� **************/
    TR0 = 0;					//50ms�ж�1��
    TF0 = 0;

    TH0 = (65536 - 9216) / 256;	//��ʱ5MS=5000US		  ;65536-22.1184M��MHZ/12*5000US	 =65536-9216
    TL0 = (65536 - 9216) % 256;


    TR0 = 1;				//����T0


    ET0 = 1;			//����T0�ж�

}

void  Initial_uv98()
{
    uchar i;
    PTT = 0;

//ע��: оƬ�ϵ��������PWM��ص�IO�ھ�Ϊ����̬
//�轫��Щ������Ϊ׼˫��ڻ�ǿ����ģʽ��������ʹ��
//���IO:
//P0.6/P0.7
//P1.6/P1.7/
//P2.1/P2.2/P2.3/P2.7
//P3.7
//P4.2/P4.4/P4.5
//���⻹��P1.0/P1.4���������ϵ�ʱΪǿ�������,CPU����BUG
//�����ʼ��ʱҲ�轫������������Ϊ������׼˫���ģʽ

//P3.0 P3.1 �ϵ�=VCC/2
    P0M1 = 0x00;
    P0M0 = 0X00;	 //P0	ȫ������Ϊ������׼˫���ģʽ
    P1M1 = 0x00;
    P1M0 = 0X00;	 //P1	ȫ������Ϊ������׼˫���ģʽ
    P2M1 = 0x00;
    P2M0 = 0X00;	 //P2	ȫ������Ϊ������׼˫���ģʽ
    P3M1 = 0x00;
    P3M0 = 0X00;	 //P3	ȫ������Ϊ������׼˫���ģʽ
    P4M1 = 0x00;
    P4M0 = 0X00;	 //P4	ȫ������Ϊ������׼˫���ģʽ
    P5M1 = 0x00;
    P5M0 = 0X00;	 //P5	ȫ������Ϊ������׼˫���ģʽ

//�����ж����ȼ�
//CPU��λ�󣬸��жϾ�Ϊ������ȼ�0
//	IPH|=0X02; 	IP|=0X02;	//��ʱ��0��Ϊ���3
//	IPH|=0X10; 	//����0��Ϊ���ȼ�2

    P2M0 = 0X84;	 //P2.3 PTT�������  P2.7 GPS�������
    P3M0 = 0X42;	 //P3.1 3.6�������txd

 

    for(i = 0; i < 2; i++)
    {
        LED_STU = 0;
//		LED_GPRS=LED_GPS=LED_TX=LED_RX=LED_TX=0;	 //����ȫ��ָʾ�ƣ����LED��ȫ��=��ɫ
        Delay_time_25ms(1);
        LED_STU = 1;
//		LED_GPRS=LED_GPS=LED_TX=LED_RX=LED_TX=1;	 //��LED
//		Delay_time_25ms(2);
    }

    Initial_check_erom();  //���MAC��ַ	�Ƿ��Ѵ洢��û����TNCû��ʼ��

    POWER_READ_SETUP();  	//������ȡȫ���������õ�����
    adc_Initial();		   //��ʼ��ADC

    UART1_Initial();	//��ʼ������1
    UART2_Initial();	//��ʼ������2
    UART3_Initial();	//��ʼ������3
    UART4_Initial();	//��ʼ������4

    /************* �������� **************/
//---------------------------------
//	DS18B20_READTEMP() ;   //������ȡ�����¶ȣ�������ʾ85��
//	DS18B20_READTEMP() ;

    //---------------------------------
    GPS_INIT();

    CMX865A_Init();
    CMX_RX_Initial();

    //---------------------------------
    time0_init();


    EA  = 1;			//����ȫ���ж�
}



//========================================================================
// ��ע:
//========================================================================
void timer0 (void) interrupt 1
{
    CMX_RX_INT();		//5ms�ж�һ��,С��6.5ms������

    UART1_RX_TIME++;	//UARTx  �������ݼ�ʱ
    UART2_RX_TIME++;	//UARTx  �������ݼ�ʱ
    UART4_RX_TIME++;	//UARTx  �������ݼ�ʱ


    TIME_1S++;

    time_a++;

    if (time_a > (200 - 1))	 //1��
    {
        time_a = 0;

        BT_LINK_TIME++;

        UART3_OUTTIME++;   //�������ݼ����������ж�GPS�Ƿ�Ͽ�
        GPS_WAIT_TIME++;   //�����ű�ʱ���׼
        GPS_BEACON_TIME++; //GPS��ʱ�ű��ʱ
    }
}

void main()
{
    Initial_uv98();	//��ʼ��TNC

    if (EEPROM_Buffer[0X3A] == 0)
    {
        BL_PWR = 0;   //��ʼ������
    }
    else
    {
        BL_PWR = 1;
    }

    if (EEPROM_Buffer[0X3A] == 1)
    {
        SETUP_BL_NAME();   //��ʼ����������
    }

    READ_BMP280();
    UART4_TX_EEROM() ; 	//Delay_time_25ms(10);

    while (1)
    {
        UART1_FUN();    //������1���յ������ݣ����ã�9600
        UART2_FUN();	//������2���������ݣ�9600
        UART3_FUN();	//������3������GPS���ݣ�9600
        UART4_FUN(); 	//������4��GPRS G3524����,115200
        APRS_KISS_DECODE(); //RF����KISS����

        BECON_MODE();	//��ʱ�ű�

        if (TIME_1S > 200 - 1)
        {
            TIME_1S = 0;
            DISP_GPS_STU();
        }


    }
}

//$PCAS11,0*1D	   //��Яģʽ
//$PCAS11,1*1C	   //��̬ģʽ
//$PCAS11,2*1F		//����ģʽ
//$PCAS11,3*1E		//����ģʽ
//$PCAS11,4*19		//����ģʽ
//$PCAS11,5*18		//����ģʽ<1g
//$PCAS11,6*1B		//����ģʽ<2g
//$PCAS11,7*1A		//����ģʽ<4g
//$PCAS10,0*1C		//������





