
#include <intrins.h>

#include "STC8A8K64D4.H"
#include "DELAY.H"
#include "AT24C512.h"
#include "IO.H"
#include "UART2.H"
#include "tostring.H"


sbit AT24_WP = P4 ^ 2;
sbit AT24_SCL = P2 ^ 5;
sbit AT24_SDA = P2 ^ 4;
 

unsigned char  AT24C512_RW_BUF[256];

 

/**************************************
��ʱ5US*11.0592=55
**************************************/
void AT24_Delay()
{
    uchar i;

    for(i = 0; i < 10; i++)
    {
        _nop_();
        _nop_();
        _nop_();
    }
}

/**************************************
��ʼ�ź�
**************************************/

void AT24C512_Start()
{
    AT24_SDA = 1;                    //����������
    AT24_Delay();                 //��ʱ
    AT24_SCL = 1;                    //����ʱ����
    AT24_Delay();                 //��ʱ
    AT24_SDA = 0;                    //�����½���
    AT24_Delay();                 //��ʱ
    AT24_SCL = 0;                    //����ʱ����
    AT24_Delay();                 //��ʱ
}
/**************************************
ֹͣ�ź�
**************************************/

void AT24C512_Stop()
{
    AT24_SDA = 0;                    //����������
    AT24_Delay();                 //��ʱ
    AT24_SCL = 1;                    //����ʱ����
    AT24_Delay();                 //��ʱ
    AT24_SDA = 1;                    //����������
    AT24_Delay();                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void AT24CXX_ACK(bit ACK)
{
    AT24_SDA = ACK;                  //дӦ���ź�
    AT24_SCL = 1;                    //����ʱ����
    AT24_Delay();                 //��ʱ
    AT24_SCL = 0;                    //����ʱ����
    AT24_Delay();                 //��ʱ
}


/**************************************
����Ӧ���ź�
**************************************/
bit AT24_RecvACK()
{
    AT24_SCL = 1;                    //����ʱ����

    AT24_SDA = 1;
    AT24_Delay();                 //��ʱ
    CY = AT24_SDA;                   //��Ӧ���ź�

//	if (AT24_SDA==1)	{AT24_LINK=0;}  //û��Ӧ����AT24û��װ
//	else{AT24_LINK=1;}

    AT24_SCL = 0;                    //����ʱ����
    AT24_Delay();                 //��ʱ

    return CY;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
uchar AT24CXX_write_byte(unsigned char value)
{
    unsigned char i;

    for (i = 0; i < 8; i++)     //8λ������
    {
        value <<= 1;            //�Ƴ����ݵ����λ
        AT24_SDA = CY;        //�����ݿ�
        AT24_Delay();            //��ʱ
        AT24_SCL = 1;          //����ʱ����
        AT24_Delay();            //��ʱ
        AT24_SCL = 0;          //����ʱ����
        AT24_Delay();             //��ʱ
    }

    return AT24_RecvACK();		 // 0 =��Ӧ��  1=û��Ӧ��
}


/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
uchar AT24CXX_read_byte()
{
    uchar i;
    uchar dat = 0;

    AT24_SDA = 1;            //ʹ���ڲ�����,׼����ȡ����,

    for (i = 0; i < 8; i++)    //8λ������
    {
        dat <<= 1;
        AT24_SCL = 1;         //����ʱ����
        AT24_Delay();           //��ʱ
        dat |= AT24_SDA;     //������
        AT24_SCL = 0;         //����ʱ����
        AT24_Delay();            //��ʱ
    }

    return dat;
}


void AT24CXX_WRITE(unsigned int addr, unsigned char wdata)
{
    AT24_WP = 0;
    AT24C512_Start();
    AT24CXX_write_byte(0xa0);
    AT24CXX_write_byte(addr / 256);
    AT24CXX_write_byte(addr % 256);
    AT24CXX_write_byte(wdata);
    AT24C512_Stop();
    AT24_WP = 1;
    Delay_time_1ms(5);	//д����Ҫ����ʱ����Ȼ������д���ü���ʱ
}



unsigned char AT24CXX_READ(unsigned int addr)
{
    unsigned char i;
    AT24C512_Start();
    AT24CXX_write_byte(0xa0);
    AT24CXX_write_byte(addr / 256);
    AT24CXX_write_byte(addr % 256);

    AT24C512_Start();
    AT24CXX_write_byte(0xa1);
    i = AT24CXX_read_byte();
    AT24C512_Stop();

    return i;
}
//128�ֽڵ�ҳд
unsigned char AT24CXX_WRITE_N(unsigned int addr, unsigned char  *buf, uint len)
{
    uint i;

    AT24_WP = 0;
    AT24C512_Start();
    AT24CXX_write_byte(0xa0);
    AT24CXX_write_byte(addr / 256);
    AT24CXX_write_byte(addr % 256);

    for(i = 0; i < len; i++)
    {
        AT24CXX_write_byte(*buf);
        buf++;
    }

    AT24C512_Stop();
    AT24_WP = 1;

    Delay_time_1ms(5);	//	//д���ݱ���Ҫ����ʱ

    return i;
}

//128�ֽڵ�ҳ��
void  AT24CXX_READ_N(unsigned int addr, unsigned char  *buf, uint len)
{
    uint i;
    AT24C512_Start();
    AT24CXX_write_byte(0xa0);
    AT24CXX_write_byte(addr / 256);
    AT24CXX_write_byte(addr % 256);

    AT24C512_Start();
    AT24CXX_write_byte(0xa1);

    for(i = 0; i < len; i++)
    {
        *buf = AT24CXX_read_byte();
        buf++;
        AT24CXX_ACK(i == (len - 1));
    }

    AT24C512_Stop();

}




//
//void AT2C512_TEST()
//{
//	uint i;		   uchar temp[20];
//
////	AT24CXX_WRITE(0x0000,0x0E);	 	//ָ����ַд��һ���ֽ�
////	UART1_SendData(AT24CXX_READ(0x0000));	 //ָ����ַ����һ���ֽ�
////	for(i=0;i<256;i++)	 {UART1_SendData(AT24CXX_READ(0x0000+i));  }
////	AT24CXX_WRITE_N(0x0000,AT24C512_W_BUF,128);		 //ָ����ַд��128���ֽ�
////	AT24CXX_READ_N(0x0000,AT24C512_R_BUF,128);		//ָ����ַ����128���ֽ�
////	for(i=0;i<128;i++)	 {  UART1_SendData(AT24C512_R_BUF[i]); }
//
//	AT24CXX_READ_N(0,AT24C512_RW_BUF,20);
//	DEBUG_KISS(AT24C512_RW_BUF,20);
//
//	for (i = 0; i<9; i++) { temp[i]=AT24CXX_READ(0+16+i); temp[i+1]=0; }  //����
//	DEBUG_KISS(temp,9);
//
//
//}
//

void AT2C512_CLEAN()   //���ȫ����¼0-511,�������
{
    uint i, n;
    UART2_SendString("Wait...\r\n");

    for(i = 0; i < 128; i++)
    {
        AT24C512_RW_BUF[i] = 0xff;
    }

    for(n = 0; n < 512; n++)
    {
        AT24CXX_WRITE_N(n * 128, AT24C512_RW_BUF, 128);		   //ָ����ַд��128���ֽ�
    }

    UART2_SendString("DATA Clean OK\r\n");
}

//  0x0000-0x6400  ��0-25600��,ÿ���ű�ռ��256�ֽڣ�һ��100����¼������0-99�洢�� 0x8000
//  0x6500-0x7180    ,�б�洢����ÿ16�ֽ�һ��������һ��100������
//  BH4TDV-XX 99 �ű�洢��ַ���� 0-99
//	0xFE00  ���512 �洢��������
