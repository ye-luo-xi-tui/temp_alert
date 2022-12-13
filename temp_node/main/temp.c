#include "temp.h"

void gpioInit()
{
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);
}
/*******************************************************************************
* 函 数 名         : Ds18b20Init
* 函数功能		   : 初始化
* 输    入         : 无
* 输    出         : 初始化成功返回1，失败返回0
*******************************************************************************/

uchar Ds18b20Init()
{
  gpio_set_level(23,0);			 //将总线拉低480us~960us
  vTaskDelay(642 / (portTICK_PERIOD_MS*100));//延时642us
  gpio_set_level(23,1);			//然后拉高总线，如果DS18B20做出反应会将在15us~60us后总线拉低
  int i = 0;
  while(gpio_get_level(23))	//等待DS18B20拉低总线
    {
      i++;
      if(i>5)//等待>5MS
        {
			return 0;//初始化失败
		}
      vTaskDelay(1/portTICK_PERIOD_MS);
	}
	return 1;//初始化成功
}

/*******************************************************************************
* 函 数 名         : Ds18b20WriteByte
* 函数功能		   : 向18B20写入一个字节
* 输    入         : com
* 输    出         : 无
*******************************************************************************/

void Ds18b20WriteByte(uchar dat)
{
	uint j;

	for(j=0; j<8; j++)
	{
      gpio_set_level(23,0);	     	  //每写入一位数据之前先把总线拉低1us
      vTaskDelay(1/portTICK_PERIOD_MS*100);
      gpio_set_level(23,dat & 0x01);  //然后写入一个数据，从最低位开始
      vTaskDelay(68/portTICK_PERIOD_MS*100); //延时68us，持续时间最少60us
      gpio_set_level(23,1);	//然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
		dat >>= 1;
	}
}
/*******************************************************************************
* 函 数 名         : Ds18b20ReadByte
* 函数功能		   : 读取一个字节
* 输    入         : com
* 输    出         : 无
*******************************************************************************/


uchar Ds18b20ReadByte()
{
	uchar byte = 0, bi;
	uint j;
	for(j=8; j>0; j--)
	{
      gpio_set_level(23,0);//先将总线拉低1us
      vTaskDelay(1/(portTICK_PERIOD_MS*100));
      gpio_set_level(23,1);//然后释放总线
      vTaskDelay(6/(portTICK_PERIOD_MS*100));
      bi = gpio_get_level(23);	 //读取数据，从最低位开始读取
		/*将byte左移一位，然后与上右移7位后的bi，注意移动之后移掉那位补0。*/
        byte = (byte >> 1) | (bi << 7);
      vTaskDelay(48/(portTICK_PERIOD_MS*100));
	}				
	return byte;
}
/*******************************************************************************
* 函 数 名         : Ds18b20ChangTemp
* 函数功能		   : 让18b20开始转换温度
* 输    入         : com
* 输    出         : 无
*******************************************************************************/

void  Ds18b20ChangTemp()
{
	Ds18b20Init();
    vTaskDelay(1/portTICK_PERIOD_MS);
	Ds18b20WriteByte(0xcc);		//跳过ROM操作命令		 
	Ds18b20WriteByte(0x44);	    //温度转换命令
//	Delay1ms(100);	//等待转换成功，而如果你是一直刷着的话，就不用这个延时了
   
}
/*******************************************************************************
* 函 数 名         : Ds18b20ReadTempCom
* 函数功能		   : 发送读取温度命令
* 输    入         : com
* 输    出         : 无
*******************************************************************************/

void  Ds18b20ReadTempCom()
{	

	Ds18b20Init();
    vTaskDelay(1/portTICK_PERIOD_MS);
	Ds18b20WriteByte(0xcc);	 //跳过ROM操作命令
	Ds18b20WriteByte(0xbe);	 //发送读取温度命令
}
/*******************************************************************************
* 函 数 名         : Ds18b20ReadTemp
* 函数功能		   : 读取温度
* 输    入         : com
* 输    出         : 无
*******************************************************************************/

int Ds18b20ReadTemp()
{
	int temp = 0;
	uchar tmh, tml;
	Ds18b20ChangTemp();			 	//先写入转换命令
	Ds18b20ReadTempCom();			//然后等待转换完后发送读取温度命令
	tml = Ds18b20ReadByte();		//读取温度值共16位，先读低字节
	tmh = Ds18b20ReadByte();		//再读高字节
	temp = tmh;
	temp <<= 8;
	temp |= tml;
	return temp;
}


