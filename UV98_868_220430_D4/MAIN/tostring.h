

#ifndef _tostring_H_
#define _tostring_H_
//���ú����͹��ñ�������


extern uchar wan, qian, bai, shi, ge;
extern void tostring(uint val); //ת����3���ֽ�

extern uchar str_txt[10];


extern void disp_Hex2Ascii2(uchar dat);

extern void READ_CPU_ID();	 //��ȡCPU���к�


extern void DEBUG_KISS(uchar *p, uint len);

extern uchar CPU_ID[16];	//��ȡCPU���к�

extern uchar TEMP_Call[10];	//��ʱ����
extern void READ_TEMP_CALL(uint call_eerom_add, uint SSID_eerom_add);	 //  ������ʱ����




extern void disp_Hex2Ascii3(uint temp)	 ;



#endif