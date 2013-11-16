#include "chgui.h"
#include "chgui_bmp.h"
#include "string.h"

static uint8_t *pBMPFileData;

void GUI_BMP_DrawEx(uint16_t xPos, uint16_t yPos, GUI_GetDataCallbackType fpAppGetData) 
{
    uint32_t br;
    uint32_t count;		    	   
    uint8_t  rgb ,color_byte;
    uint16_t x ,y,color;	 
    uint16_t image_width_in_pixel;	
    BITMAPINFO BitMapInfo1;
    uint32_t countpix=0;//记录像素 	 
    //x,y的实际坐标	 
    uint8_t *databuf;    		//数据读取存放地址
    uint32_t TotalDataSizeInBytes;
    uint8_t biCompression=0;		//记录压缩方式
    uint32_t rowlen;	  		 	//水平方向字节数  
    uint32_t offx=0;
	  //Get Header File Info 
    while(offx < sizeof(BITMAPINFO))
    {
        br = fpAppGetData(sizeof(BITMAPINFO)-offx, offx, &databuf);
			  memcpy(((&BitMapInfo1) + offx), databuf, br);
        offx += br;
    }
		offx = BitMapInfo1.bmfHeader.bfOffBits;        	//数据偏移,得到数据段的开始地址
		color_byte = BitMapInfo1.bmiHeader.biBitCount/8;	//彩色位 16/24/32  
		biCompression = BitMapInfo1.bmiHeader.biCompression;//压缩方式
    TotalDataSizeInBytes = BitMapInfo1.bmiHeader.biSizeImage ;
		y = BitMapInfo1.bmiHeader.biHeight;	//得到图片高度
		image_width_in_pixel = BitMapInfo1.bmiHeader.biWidth;
		//水平像素必须是4的倍数!!

		if((image_width_in_pixel * color_byte)%4)rowlen=((image_width_in_pixel*color_byte)/4+1)*4;
		else rowlen = image_width_in_pixel * color_byte;

		//开始解码BMP   
		color = 0;//颜色清空	 													 
		x = 0;
		rgb = 0;      
    while(1)
    {				
        br = fpAppGetData(TotalDataSizeInBytes + BitMapInfo1.bmfHeader.bfOffBits - offx, offx, &databuf);
        offx += br;
        count = 0;
        while(count < br)
        {
            if(color_byte == 3)   //24位颜色图
            {
                switch (rgb) 
                {
                    case 0:				  
                        color = databuf[count]>>3; //B
                        break ;	   
                    case 1: 	 
                        color += ((uint16_t)databuf[count]<<3)&0X07E0;//G
                        break;	  
                    case 2 : 
                        color += ((uint16_t)databuf[count]<<8)&0XF800;//R	  
                        break;			
                }   
            }
						else if(color_byte == 2)  //16位颜色图
            {
                switch(rgb)
                {
                    case 0 : 
                        if(biCompression == BI_RGB)//RGB:5,5,5
                        {
                            color = ((uint16_t)databuf[count]&0X1F);	 	//R
                            color += (((uint16_t)databuf[count])&0XE0)<<1; //G
                        }
												else		//RGB:5,6,5
                        {
                            color = databuf[count];  			//G,B
                        }  
                        break ;   
                    case 1 : 			  			 
                        if(biCompression == BI_RGB)//RGB:5,5,5
                        {
                            color += (uint16_t)databuf[count]<<9;  //R,G
                        }
												else  		//RGB:5,6,5
                        {
                            color += (uint16_t)databuf[count]<<8;	//R,G
                        }  									 
                        break ;	 
                }		     
            }
						else if(color_byte == 4)//32位颜色图
            {
                switch (rgb)
                {
                    case 0:				  
                        color = databuf[count]>>3; //B
                        break ;	   
                    case 1: 	 
                        color += ((uint16_t)databuf[count]<<3)&0X07E0;//G
                        break;	  
                    case 2 : 
                        color += ((uint16_t)databuf[count]<<8)&0XF800;//R	  
                        break ;			
                    case 3 :
                        //alphabend=bmpbuf[count];//不读取  ALPHA通道
                        break ;  		  	 
                }	
            }
						else if(color_byte == 1)//8位色,暂时不支持,需要用到颜色表.
            {
            } 
            rgb++;	  
            count++ ;		  
            if(rgb == color_byte) //水平方向读取到1像素数数据后显示
            {
                gpCHGUI->ops->ctrl_point(x + xPos, y + yPos, color);								    
                x++; //x轴增加一个像素 
                color = 0x00; 
                rgb = 0;  		  
            }
            countpix++;//像素累加
            if(countpix >= rowlen)//水平方向像素值到了.换行
            {		 
                y--; 
                if(y == 0)break;			 
                x = 0; 
                countpix = 0;
                color = 0x00;
                rgb = 0;
            }	 
        } 		
    }   					   
}		 

static uint32_t AppGetData(uint32_t ReqestedByte, uint32_t Offset, uint8_t **ppData)
{
    *ppData = (pBMPFileData + Offset);
    return ReqestedByte;
}

void GUI_BMP_Draw(uint16_t xPos, uint16_t yPos, const uint8_t *pData)
{
    pBMPFileData = (uint8_t*)pData;
    GUI_BMP_DrawEx(xPos, yPos, AppGetData);
}
