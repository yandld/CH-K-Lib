#include "test_common.h"

#include "stdint.h"

 uint32_t Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	   

 uint8_t CHS_Str2Num(uint8_t *str,uint32_t *res)
{
	uint32_t t;
	uint8_t bnum=0;	 //数字的位数
	uint8_t *p;		  
	uint8_t hexdec=10;//默认为十进制数据
	p=str;
	*res=0;//清零.
	while(1)
	{
		if((*p<='9'&&*p>='0')||(*p<='F'&&*p>='A')||(*p=='X'&&bnum==1))//参数合法
		{
			if(*p>='A')hexdec=16;	//字符串中存在字母,为16进制格式.
			bnum++;					//位数增加.
		}else if(*p=='\0')break;	//碰到结束符,退出.
		else return 1;				//不全是十进制或者16进制数据.
		p++; 
	} 
	p=str;			    //重新定位到字符串开始的地址.
	if(hexdec==16)		//16进制数据
	{
		if(bnum<3)return 2;			//位数小于3，直接退出.因为0X就占了2个,如果0X后面不跟数据,则该数据非法.
		if(*p=='0' && (*(p+1)=='X'))//必须以'0X'开头.
		{
			p+=2;	//偏移到数据起始地址.
			bnum-=2;//减去偏移量	 
		}else return 3;//起始头的格式不对
	}else if(bnum==0)return 4;//位数为0，直接退出.	  
	while(1)
	{
		if(bnum)bnum--;
		if(*p<='9'&&*p>='0')t=*p-'0';	//得到数字的值
		else t=*p-'A'+10;				//得到A~F对应的值	    
		*res+=t*Pow(hexdec,bnum);		   
		p++;
		if(*p=='\0')break;//数据都查完了.	
	}
	return 0;//成功转换
}