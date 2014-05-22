#ifndef __USBDESC_H_
#define	__USBDESC_H_

//设备描述符
const uint8_t Device_Descriptor[18]= 
{
 	0x12,	   //bLength域，描述符的长度：18字节
	0x01,	   //bDescriptorType域，描述符类型：0x01表示本描述符为设备描述符）
	0x00,0x02, //bcdUSB域，USB规范版本号（采用BCD码）：2.0 
	0x00,	   //bDeviceClass域，设备类代码 一般为0
	0x00,	   //bDeviceSubClass域，设备子类代码	bDeviceClass为0时bDeviceSubClass也必须为0
	0x00,	   //bDeviceProtocol域，设备协议代码（0x00表示不使用任何设备类协议）			
	0x20,	   //bMaxPacketSize0域，端点0支持最大数据包的长度：32字节
	0x88,0x88, //idVendor域，供应商ID（VID）
	0x02,0x00, //idProduct域，产品ID（PID）
	0x00,0x00, //bcdDevice域，设备版本号（采用BCD码）
	0x01,      //iManufacturer域，供应商的字符串描述符索引：1
	0x02,	   //iProduct域，产品的字符串描述符索引：2
	0x03,	   //iSerialNumber域，设备序号的字符串描述符索引：3
	0x01	   //bNumConfigurations域，该USB设备支持的配置数目：1个
};


//USB报告描述符的定义
const uint8_t Report_Descriptor[]=
{
 //每行开始的第一字节为该条目的前缀，前缀的格式为：
 //D7~D4：bTag。D3~D2：bType；D1~D0：bSize。以下分别对每个条目注释。
 
 //这是一个全局（bType为1）条目，将用途页选择为普通桌面Generic Desktop Page(0x01)
 //后面跟一字节数据（bSize为1），后面的字节数就不注释了，
 //自己根据bSize来判断。
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 
 //这是一个局部（bType为2）条目，说明接下来的集合用途用于键盘
 0x09, 0x06, // USAGE (Keyboard)
 
 //这是一个主条目（bType为0）条目，开集合，后面跟的数据0x01表示
 //该集合是一个应用集合。它的性质在前面由用途页和用途定义为
 //普通桌面用的键盘。
 0xa1, 0x01, // COLLECTION (Application)
 
 //这是一个全局条目，选择用途页为键盘（Keyboard/Keypad(0x07)）
 0x05, 0x07, //     USAGE_PAGE (Keyboard/Keypad)

 //这是一个局部条目，说明用途的最小值为0xe0。实际上是键盘左Ctrl键。
 //具体的用途值可在HID用途表中查看。
 0x19, 0xe0, //     USAGE_MINIMUM (Keyboard LeftControl)
 
 //这是一个局部条目，说明用途的最大值为0xe7。实际上是键盘右GUI键。
 0x29, 0xe7, //     USAGE_MAXIMUM (Keyboard Right GUI)
 
 //这是一个全局条目，说明返回的数据的逻辑值（就是我们返回的数据域的值）
 //最小为0。因为我们这里用Bit来表示一个数据域，因此最小为0，最大为1。
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 
 //这是一个全局条目，说明逻辑值最大为1。
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 
 //这是一个全局条目，说明数据域的数量为八个。
 0x95, 0x08, //     REPORT_COUNT (8)
 
 //这是一个全局条目，说明每个数据域的长度为1个bit。
 0x75, 0x01, //     REPORT_SIZE (1)
 
 //这是一个主条目，说明有8个长度为1bit的数据域（数量和长度
 //由前面的两个全局条目所定义）用来做为输入，
 //属性为：Data,Var,Abs。Data表示这些数据可以变动，Var表示
 //这些数据域是独立的，每个域表示一个意思。Abs表示绝对值。
 //这样定义的结果就是，当某个域的值为1时，就表示对应的键按下。
 //bit0就对应着用途最小值0xe0，bit7对应着用途最大值0xe7。
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 
 //这是一个全局条目，说明数据域数量为1个
 0x95, 0x01, //     REPORT_COUNT (1)
 
 //这是一个全局条目，说明每个数据域的长度为8bit。
 0x75, 0x08, //     REPORT_SIZE (8)
 
 //这是一个主条目，输入用，由前面两个全局条目可知，长度为8bit，
 //数量为1个。它的属性为常量（即返回的数据一直是0）。
 //该字节是保留字节（保留给OEM使用）。
 0x81, 0x03, //     INPUT (Cnst,Var,Abs)
 
 //这是一个全局条目。定义位域数量为6个。
 0x95, 0x06, //   REPORT_COUNT (6)
 
 //这是一个全局条目。定义每个位域长度为8bit。
 //其实这里这个条目不要也是可以的，因为在前面已经有一个定义
 //长度为8bit的全局条目了。
 0x75, 0x08, //   REPORT_SIZE (8)
 
 //这是一个全局条目，定义逻辑最小值为0。
 //同上，这里这个全局条目也是可以不要的，因为前面已经有一个
 //定义逻辑最小值为0的全局条目了。
 0x15, 0x00, //   LOGICAL_MINIMUM (0)
 
 //这是一个全局条目，定义逻辑最大值为255。
 0x25, 0xFF, //   LOGICAL_MAXIMUM (255)
 
 //这是一个全局条目，选择用途页为键盘。
 //前面已经选择过用途页为键盘了，所以该条目不要也可以。
 0x05, 0x07, //   USAGE_PAGE (Keyboard/Keypad)
 
 //这是一个局部条目，定义用途最小值为0（0表示没有键按下）
 0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
 
 //这是一个局部条目，定义用途最大值为0x65
 0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)
 
 //这是一个主条目。它说明这六个8bit的数据域是输入用的，
 //属性为：Data,Ary,Abs。Data说明数据是可以变的，Ary说明
 //这些数据域是一个数组，即每个8bit都可以表示某个键值，
 //如果按下的键太多（例如超过这里定义的长度或者键盘本身无法
 //扫描出按键情况时），则这些数据返回全1（二进制），表示按键无效。
 //Abs表示这些值是绝对值。
 0x81, 0x00, //     INPUT (Data,Ary,Abs)

 //以下为输出报告的描述
 //逻辑最小值前面已经有定义为0了，这里可以省略。 
 //这是一个全局条目，说明逻辑值最大为1。
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 
 //这是一个全局条目，说明数据域数量为5个。 
 0x95, 0x05, //   REPORT_COUNT (5)
 
 //这是一个全局条目，说明数据域的长度为1bit。
 0x75, 0x01, //   REPORT_SIZE (1)
 
 //这是一个全局条目，说明使用的用途页为指示灯（LED）
 0x05, 0x08, //   USAGE_PAGE (LEDs)
 
 //这是一个局部条目，说明用途最小值为数字键盘灯。
 0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
 
 //这是一个局部条目，说明用途最大值为Kana灯。
 0x29, 0x05, //   USAGE_MAXIMUM (Kana)
 
 //这是一个主条目。定义输出数据，即前面定义的5个LED。
 0x91, 0x02, //   OUTPUT (Data,Var,Abs)
 
 //这是一个全局条目。定义位域数量为1个。
 0x95, 0x01, //   REPORT_COUNT (1)
 
 //这是一个全局条目。定义位域长度为3bit。
 0x75, 0x03, //   REPORT_SIZE (3)
 
 //这是一个主条目，定义输出常量，前面用了5bit，所以这里需要
 //3个bit来凑成一字节。
 0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)
 
 //下面这个主条目用来关闭前面的集合。bSize为0，所以后面没数据。
 0xc0        // END_COLLECTION
};
//通过上面的报告描述符的定义，我们知道返回的输入报告具有8字节。
//第一字节的8个bit用来表示特殊键是否按下（例如Shift、Alt等键）。
//第二字节为保留值，值为常量0。第三到第八字节是一个普通键键值的
//数组，当没有键按下时，全部6个字节值都为0。当只有一个普通键按下时，
//这六个字节中的第一字节值即为该按键的键值（具体的键值请看HID的
//用途表文档），当有多个普通键同时按下时，则同时返回这些键的键值。
//如果按下的键太多，则这六个字节都为0xFF（不能返回0x00，这样会让
//操作系统认为所有键都已经释放）。至于键值在数组中的先后顺序是
//无所谓的，操作系统会负责检查是否有新键按下。我们应该在中断端点1
//中按照上面的格式返回实际的键盘数据。另外，报告中还定义了一个字节
//的输出报告，是用来控制LED情况的。只使用了低7位，高1位是保留值0。
//当某位的值为1时，则表示对应的LED要点亮。操作系统会负责同步各个
//键盘之间的LED，例如你有两块键盘，一块的数字键盘灯亮时，另一块
//也会跟着亮。键盘本身不需要判断各种LED应该何时亮，它只是等待主机
//发送报告给它，然后根据报告值来点亮相应的LED。我们在端点1输出中断
//中读出这1字节的输出报告，然后对它取反（因为学习板上的LED是低电平时
//亮），直接发送到LED上。这样main函数中按键点亮LED的代码就不需要了。
///////////////////////////报告描述符完毕////////////////////////////

//字符串描述符0
const uint8_t String_Descriptor0[4] = 
{
	0x04,		   //bLength域，长度：4字节
	0x03,		   //bDescriptorType域，字符串描述符
	//0x0409为美式英语的ID
	0x09,0x04	   //wLANDID0域，英语
};

const uint8_t String_Descriptor1[22]={
22,         //该描述符的长度为22字节
0x03,       //字符串描述符的类型编码为0x03
0x59, 0x00, //Y
0x61, 0x00, //a
0x6e, 0x00, //n
0x64, 0x00, //d
0x6c, 0x00, //l
0x64, 0x00, //d
0x2d, 0x00, //-
0x48, 0x00, //H
0x49, 0x00, //I
0x44, 0x00, //D
//注：请将最后一个多余的逗号删除
};

const uint8_t String_Descriptor2[22]={
22,         //该描述符的长度为22字节
0x03,       //字符串描述符的类型编码为0x03
0x59, 0x00, //Y
0x61, 0x00, //a
0x6e, 0x00, //n
0x64, 0x00, //d
0x6c, 0x00, //l
0x64, 0x00, //d
0x2d, 0x00, //-
0x48, 0x00, //H
0x49, 0x00, //I
0x44, 0x00, //D
//注：请将最后一个多余的逗号删除
};

//字符串描述符3
const uint8_t String_Descriptor3[] = 
{
	0x14,			//bLength域，长度：20字节
	0x03,		    //bDescriptorType域，字符串描述符
	'T',0x00,	    //
	'E',0x00,	
	'S',0x00,	
	'T',0x00,
	'_',0x00,	    //
  'v',0x00,       //
	'2',0x00,	
	'.',0x00,	
	'2',0x00
};	 


//配置描述符
const uint8_t Configuration_Descriptor[9+9+9+7+7]= 
{
	//配置描述符
	0x09,		    //bLength域，配置描述符的长度：9字节
	0x02,		    //bDescriptorType域：0x02表示本描述符为配置描述符
	sizeof(Configuration_Descriptor)&0xFF,(sizeof(Configuration_Descriptor)>>8)&0xFF,	    //wTotalLength域，配置信息的总长度（包括配置、接口和端点）：32字节
	0x01,		    //bNumInterfaces域，该配置所支持的接口数（至少一个）：1
	0x01,		    //bConfigurationValue域，配置值：1 
	0x00,		    //iConfiguration域，配置字符串描述符索引：0
	0x80,		    //bmAttibutes域，配置的属性(具有总线供电、自供电及过程唤醒的特性) 
	                //位7：1-必须为1；位6：0-自供电；位5：0-不支持远程唤醒
	0x32,		    //MaxPower域，设备从总线提取的最大电流以2mA为单位：50*2mA＝100mA

	//接口描述符
	0x09,           //bLength域，接口描述符长度：9字节
	0x04,           //bDescriptorType域：0x04表示本描述符为接口描述符
	0x00,           //bInterfaceNumber域，接口号
	0x00,           //bAlternateSetting域，接口的可替换设置值
	0x02,           //bNumEndpoints域，接口使用的端点数（除端点0）：2
	0x03,           //bInterfaceClass域，接口所属的USB设备类：0xFF表示供应商自定义
	0x01,           //bInterfaceSubClass域，接口所属的USB设备子类：0xFF表示供应商自定义
	0x01,           //bInterfaceProtocol域，接口采用的USB设备类协议：0xFF表示供应商自定义
	0x00,           //iInterface域，接口字符串描述符的索引：0

 /******************HID描述符************************/
 //bLength字段。本HID描述符下只有一个下级描述符。所以长度为9字节。
 0x09,
 //bDescriptorType字段。HID描述符的编号为0x21。
 0x21,
 //bcdHID字段。本协议使用的HID1.1协议。注意低字节在先。
 0x10,
 0x01,
 //bCountyCode字段。设备适用的国家代码，这里选择为美国，代码0x21。
 0x21,
 //bNumDescriptors字段。下级描述符的数目。我们只有一个报告描述符。
 0x01,
 //bDescriptorType字段。下级描述符的类型，为报告描述符，编号为0x22。
 0x22,
 //bDescriptorLength字段。下级描述符的长度。下级描述符为报告描述符。
 sizeof(Report_Descriptor)&0xFF,
 (sizeof(Report_Descriptor)>>8)&0xFF,  
  
	 //端点OUT描述符
	0x07,           //bLength域，端点描述符长度：7字节
	0x05,           //bDescriptorType域：0x05表示本描述符为端点描述符
	0x82,           //bEndpointAddress域，端点号和传输方向：端点2、IN
	0x03,           //bmAttributes域，端点特性：数据端点、块传输
	0x20,0x00,      //wMaxPacketSize域，端点支持最大数据包长度：32字节          
	0x64,           //bInterval域，轮询间隔，以ms为单位。

	 //端点IN描述符
	0x07,           //bLength域，端点描述符长度：7字节
	0x05,           //bDescriptorType域：0x05表示本描述符为端点描述符
	0x03,           //bEndpointAddress域，端点号和传输方向：端点3、OUT   
	0x03,           //bmAttributes域，端点特性：数据端点、块传输
	0x20,0x00,      //wMaxPacketSize域，端点支持最大数据包长度：32字节        
	0x64,           //bInterval域，轮询间隔，以ms为单位。
};

#endif

