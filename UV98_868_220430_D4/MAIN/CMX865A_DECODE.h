

#ifndef _CMX865A_DECODE_H_
#define _CMX865A_DECODE_H_


/*************  �ⲿ�����ͱ������� *****************/

  
extern uchar CMX865A_HDLC_RX() ;   // ��ռ���뷽ʽ

extern uchar CMX865A_HDLC_RX_2();  // �жϽ��뷽ʽ

extern void CMX_RX_Initial();	//��ʱ�ж�

extern void CMX_RX_INT();	//��ʱ�ж� ,5ms�ж�һ��

extern unsigned char CMX_RX_BUSY;

/*****************************************/
#endif