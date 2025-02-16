
//#include "STC8A8K64D4.H"
//#include "BEACON.H"

#include "STC8A8K64D4.H"
#include "STC_EEPROM.H"

#include "PUBLIC_BUF.H"

#include "KISS2ASC.H"



//18.5   2648   17384

//电台接收到的  KISS数据转换ASCII UI格式,,format  0=原始数据  1=插入本机呼号

uchar ASC_IDX;

void ASCII_WRITE_SSID(unsigned char *nbuff, uchar SSID)
{
    if (SSID > 0)
    {
        nbuff[ASC_IDX++] = '-';

        if (SSID < 10)
        {
            nbuff[ASC_IDX++] = SSID + 0x30;       //转换成ASCII
        }
        else
        {
            nbuff[ASC_IDX++] = SSID / 10 + 0x30;
            nbuff[ASC_IDX++] = SSID % 10 + 0x30;;
        }
    }
}


void ASCII_WRITE_CALL(unsigned char *nbuff, uchar idx)
{
    uchar temp;
    uchar i;

    for (i = idx; i < idx + 6; i++)
    {
        temp = ((KISS_DATA[i])  >> 1);

        if (temp != ' ')
        {
            nbuff[ASC_IDX++] = temp;
        }
    }
}

void KISS_TO_ASCII( unsigned char *nbuff, unsigned char format)
{
    unsigned char i, w, idx;

    /*  ---------------------      */		//转换源地址
    ASC_IDX = 0;
    ASCII_WRITE_CALL(nbuff, 7);	 //CALL
    ASCII_WRITE_SSID(nbuff, (KISS_DATA[13] & 0x1E) >> 1);	//SSID
    /*  ---------------------      */
    nbuff[ASC_IDX++] = '>';
    /*  ---------------------      */		   // 转换目标地址
    ASCII_WRITE_CALL(nbuff, 0);	 //CALL
    ASCII_WRITE_SSID(nbuff, (KISS_DATA[6] & 0x1E) >> 1); //SSID
    //----------------------------------------------------

    idx = 13;

    for (w = 0; w < 8; w++)		 //最多8个路径
    {
        if ((KISS_DATA[idx] & 0x01) == 1)
        {
            break;   //路径结束符号
        }

        nbuff[ASC_IDX++] = ',';

        idx++;
        ASCII_WRITE_CALL(nbuff, idx);	 //CALL

        idx = idx + 6;
        ASCII_WRITE_SSID(nbuff, (KISS_DATA[idx] & 0x1E) >> 1);	//SSID

        if ((KISS_DATA[idx] & 0x80) == 0x80)
        {
            nbuff[ASC_IDX++] = '*';	  	   //SSID.7=1，则为中继转发过的数据，插入符号*
        }
    }

    //----------------------------------------------------
    //  服务器数据插入网关名称  format  0=原始数据  1=插入本机呼号
    if (format == 1)
    {
        if (EEPROM_Buffer[0X1A] == 1) 	//上传信标时，是否插入自己的呼号，0=不插入 1=插入
        {
            nbuff[ASC_IDX++] = ',';

            for (i = 0; i < 6; i++) 				//CALL
            {
                if (EEPROM_Buffer[0x08 + i] == 0x00)
                {
                    break;
                }

                nbuff[ASC_IDX++] = (EEPROM_Buffer[0x08 + i]);
            }


            ASCII_WRITE_SSID(nbuff, EEPROM_Buffer[0x0f]);	 //SSID
            nbuff[ASC_IDX++] = '*';
        }
    }

    /*  ---------------------      */
    nbuff[ASC_IDX++] = ':';	  // 插入符号 “ ：”
    /*  ---------------------      */		   //发送剩余ASC信息
    idx = idx + 3;

    for (i = idx; i < KISS_LEN; i++)
    {
        nbuff[ASC_IDX++] = KISS_DATA[i];

        if (ASC_IDX > 190)
        {
            nbuff[ASC_IDX++] = '~';             //限定长度
            break;
        }
    }

    if (nbuff[ASC_IDX - 1] == 0x0d)
    {
        nbuff[ASC_IDX++] = '\n';
        nbuff[ASC_IDX++] = 0x00;
    }
    else
    {
        nbuff[ASC_IDX++] = '\r';
        nbuff[ASC_IDX++] = '\n';
        nbuff[ASC_IDX++] = 0x00;	   	//补上换行符

    }

    //以上为数据格式转换
}



////电台接收到的  KISS数据转换ASCII UI格式,,format  0=原始数据  1=插入本机呼号
//void KISS_TO_ASCII( unsigned char *nbuff,unsigned char format)
//{
//	   unsigned char i,k ,w,idx,temp,ssid;
//
//
//		/*  ---------------------      */		//转换源地址
//		k=0;
//		for (i=7;i<13;i++)
//		{  	temp= ((KISS_DATA[i])  >> 1);	if (temp !=' ')	  {	 nbuff[k]= temp;  k++;	}  	 }
//
//		ssid=	 ((KISS_DATA[13] & 0x1E)>>1) ;
//		if (ssid>0)
//		{	   nbuff[k]= '-';	 k++;
//
//			if (ssid<10)
//			{ 	ssid |= 0x30; nbuff[k]=ssid;  	k++;  }	  //转换成ASCII
//			else
//			{  nbuff[k]='1';  k++;	ssid=ssid-10;	nbuff[k]=(ssid |= 0x30);	 k++;	}
//		}
//
//		/*  ---------------------      */
//		 nbuff[k]= '>';  k++;
//		/*  ---------------------      */		   // 转换目标地址
//
//		for (i=0;i<6;i++)
//		{  	temp= ((KISS_DATA[i])  >> 1); 	if (temp !=' ')	  {	 nbuff[k]= temp;  k++;	}  	 }
//
//		ssid=	 ((KISS_DATA[6] & 0x1E)>>1) ;
//		if (ssid>0)
//		{	   nbuff[k]= '-';	 k++;
//
//			if (ssid<10)
//			{ 	ssid |= 0x30; 	nbuff[k]=ssid;  	k++;  }	  //转换成ASCII
//			else
//			{  nbuff[k]='1';  k++;	  	ssid=ssid-10; nbuff[k]=(ssid |= 0x30);	 k++;	}
//		}
//
//
//
//	  //----------------------------------------------------
//
//  	idx=13;
//	for (w=0;w<7;w++)		 //最多7个路径
//	{
//		if ((KISS_DATA[idx] & 0x01) == 1){break;}	 //路径结束符号
//
//
//		nbuff[k]= ',';	 k++;
//
//		idx++;
//		for (i=idx;i<(idx+6);i++)
//		{   	temp= ((KISS_DATA[i])  >> 1); 	if (temp !=' ')	  {	 nbuff[k]= temp;  k++;	}  	 }
//
//
//
//		idx=idx+6;
//		ssid=	 ((KISS_DATA[idx] & 0x1E)>>1) ;
//		if (ssid>0)			  //路径计数>0,显示，路径计数=0，则0消隐
//		{	nbuff[k]= '-';	 k++;
//
//			if (ssid<10)
//			{ 	ssid |= 0x30; 		nbuff[k]=ssid;  	k++;  }  //转换成ASCII
//			else
//			{  nbuff[k]='1';  k++;	ssid=ssid-10; 	 nbuff[k]=(ssid |= 0x30);	 k++;	}
//		}
//
//		if ((KISS_DATA[idx] & 0x80)==0x80)		{	nbuff[k]= '*';	 k++;	}	//SSID.7=1，则为中继转发过的数据，插入符号*
//	 }
//
//	  //----------------------------------------------------
//
//
//
////
////
////
////		   /*  ---------------------      */		   // 转换其余路径信息
////		l=7;	 //	path_count=0;
////		if ((KISS_DATA[13] & 0x01) != 1)
////		{
////			do{
////
////				nbuff[k]= ',';	 k++;
////
////				l=l+7;
////				for (i=l;i<(l+6);i++)
////				{   	temp= ((KISS_DATA[i])  >> 1); 	if (temp !=' ')	  {	 nbuff[k]= temp;  k++;	}  	 }
////
////				ssid=	 ((KISS_DATA[l+6] & 0x1E)>>1) ;
////				if (ssid>0)			  //路径计数>0,显示，路径计数=0，则0消隐
////				{	nbuff[k]= '-';	 k++;
////
////					if (ssid<10)
////					{ 	ssid |= 0x30; 		nbuff[k]=ssid;  	k++;  }  //转换成tempII
////					else
////					{  nbuff[k]='1';  k++;	ssid=ssid-10; 	 nbuff[k]=(ssid |= 0x30);	 k++;	}
////
////				}
////
////				if ((KISS_DATA[l+6] & 0x80)==0x80)		{	nbuff[k]= '*';	 k++;	}	//SSID.7=1，则为中继转发过的数据，插入符号*
////
////
////			}while ((KISS_DATA[l+6] & 0x01) != 1);
////		}
//
//
//  	//  服务器数据插入网关名称  format  0=原始数据  1=插入本机呼号
//
//	if (format==1)
//	{
//
//		if (EEPROM_Buffer[0X1A]==1) 	//上传信标时，是否插入自己的呼号，0=不插入 1=插入
//		{
//			nbuff[k]= ',';	 k++;
//
//			for (i=0;i<6;i++) 				//CALL
//			{
//			if (EEPROM_Buffer[0x08+i]==0x00){break;}
//			nbuff[k]=(EEPROM_Buffer[0x08+i]);   k++;
//			}
//
////			n=0x08;
////			while (EEPROM_Buffer[n]!=0x00)
////			{nbuff[k]=(EEPROM_Buffer[n]); n++; k++;}
//
//			nbuff[k]= '-';	 k++;
//
//			ssid=EEPROM_Buffer[0x0f];		 //SSID
//			if (ssid<10)		 //0-9
//			{	nbuff[k]=(ssid%10+0x30);   k++;}
// 			else
//			{	nbuff[k]=(ssid/10%10+0x30);  k++; 	nbuff[k]=(ssid%10+0x30);	  k++;
// 			}
//
//			nbuff[k]= '*';	 k++;
//		 }
//	}
//		//	插入TCPIP*,qAS,BH4TDV
//		//  插入网关名称   服务器自动插�
//	   /*  ---------------------      */
//	 	nbuff[k]= ':';	 k++;		  // 插入符号 “ ：”
//
//	    /*  ---------------------      */		   //发送剩余ASC信息
//		idx=idx+3;
//
//		for (i=idx;i<KISS_LEN;i++)
//		{	nbuff[k]= KISS_DATA[i];	 k++;
//		if (k>120){ nbuff[k]= '~';   k++;       break;}  //限定长度
//		}
//
//		// while (l<KISS_LEN)	 {	 	nbuff[k]= KISS_DATA[l];	 k++; l++;	}
//
//		nbuff[k]= '\r';   k++;	nbuff[k]= '\n';	  k++;	//补上换行符
// 		nbuff[k]= 0x00;	  		//补上结束符
//		//以上为数据格式转换
// 		//	return k; 	  //UI数据长�
//}
//
//






