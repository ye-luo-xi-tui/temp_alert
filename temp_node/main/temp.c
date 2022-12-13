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
* �� �� ��         : Ds18b20Init
* ��������		   : ��ʼ��
* ��    ��         : ��
* ��    ��         : ��ʼ���ɹ�����1��ʧ�ܷ���0
*******************************************************************************/

uchar Ds18b20Init()
{
  gpio_set_level(23,0);			 //����������480us~960us
  vTaskDelay(642 / (portTICK_PERIOD_MS*100));//��ʱ642us
  gpio_set_level(23,1);			//Ȼ���������ߣ����DS18B20������Ӧ�Ὣ��15us~60us����������
  int i = 0;
  while(gpio_get_level(23))	//�ȴ�DS18B20��������
    {
      i++;
      if(i>5)//�ȴ�>5MS
        {
			return 0;//��ʼ��ʧ��
		}
      vTaskDelay(1/portTICK_PERIOD_MS);
	}
	return 1;//��ʼ���ɹ�
}

/*******************************************************************************
* �� �� ��         : Ds18b20WriteByte
* ��������		   : ��18B20д��һ���ֽ�
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/

void Ds18b20WriteByte(uchar dat)
{
	uint j;

	for(j=0; j<8; j++)
	{
      gpio_set_level(23,0);	     	  //ÿд��һλ����֮ǰ�Ȱ���������1us
      vTaskDelay(1/portTICK_PERIOD_MS*100);
      gpio_set_level(23,dat & 0x01);  //Ȼ��д��һ�����ݣ������λ��ʼ
      vTaskDelay(68/portTICK_PERIOD_MS*100); //��ʱ68us������ʱ������60us
      gpio_set_level(23,1);	//Ȼ���ͷ����ߣ�����1us�����߻ָ�ʱ����ܽ���д��ڶ�����ֵ
		dat >>= 1;
	}
}
/*******************************************************************************
* �� �� ��         : Ds18b20ReadByte
* ��������		   : ��ȡһ���ֽ�
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/


uchar Ds18b20ReadByte()
{
	uchar byte = 0, bi;
	uint j;
	for(j=8; j>0; j--)
	{
      gpio_set_level(23,0);//�Ƚ���������1us
      vTaskDelay(1/(portTICK_PERIOD_MS*100));
      gpio_set_level(23,1);//Ȼ���ͷ�����
      vTaskDelay(6/(portTICK_PERIOD_MS*100));
      bi = gpio_get_level(23);	 //��ȡ���ݣ������λ��ʼ��ȡ
		/*��byte����һλ��Ȼ����������7λ���bi��ע���ƶ�֮���Ƶ���λ��0��*/
        byte = (byte >> 1) | (bi << 7);
      vTaskDelay(48/(portTICK_PERIOD_MS*100));
	}				
	return byte;
}
/*******************************************************************************
* �� �� ��         : Ds18b20ChangTemp
* ��������		   : ��18b20��ʼת���¶�
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/

void  Ds18b20ChangTemp()
{
	Ds18b20Init();
    vTaskDelay(1/portTICK_PERIOD_MS);
	Ds18b20WriteByte(0xcc);		//����ROM��������		 
	Ds18b20WriteByte(0x44);	    //�¶�ת������
//	Delay1ms(100);	//�ȴ�ת���ɹ������������һֱˢ�ŵĻ����Ͳ��������ʱ��
   
}
/*******************************************************************************
* �� �� ��         : Ds18b20ReadTempCom
* ��������		   : ���Ͷ�ȡ�¶�����
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/

void  Ds18b20ReadTempCom()
{	

	Ds18b20Init();
    vTaskDelay(1/portTICK_PERIOD_MS);
	Ds18b20WriteByte(0xcc);	 //����ROM��������
	Ds18b20WriteByte(0xbe);	 //���Ͷ�ȡ�¶�����
}
/*******************************************************************************
* �� �� ��         : Ds18b20ReadTemp
* ��������		   : ��ȡ�¶�
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/

int Ds18b20ReadTemp()
{
	int temp = 0;
	uchar tmh, tml;
	Ds18b20ChangTemp();			 	//��д��ת������
	Ds18b20ReadTempCom();			//Ȼ��ȴ�ת������Ͷ�ȡ�¶�����
	tml = Ds18b20ReadByte();		//��ȡ�¶�ֵ��16λ���ȶ����ֽ�
	tmh = Ds18b20ReadByte();		//�ٶ����ֽ�
	temp = tmh;
	temp <<= 8;
	temp |= tml;
	return temp;
}


