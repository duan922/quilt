


#define _TIME_C_

#include "timer.h"
#include "BufferManage.h"
#include "mqtt.h"
#include "DHT11.h"
#include "ConfigModuleNoBlock.h"
#include "Gpio.h"
#include "delay.h"
#include "lock.h"


int LedDelay = 0;

/**
* @brief  ��ʱ��2����

**/

void Timer2_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
			
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//��ʱ��2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Resets the TIM2 */
    TIM_DeInit(TIM2);	                                               
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;//������ʱ�ӷָ	                                                      
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ѡ�������ϼ�����ģʽ��
	//��ֵ
	TIM_TimeBaseStructure.TIM_Period = 10;//��ʱʱ��1ms��һ��     
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //������������Ϊ TIMx ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ��72M / 7199+1 = 0.01M	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_Cmd(TIM2, ENABLE);
	
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
}

//��ʱ��3������
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 		
	TIM_ITConfig(TIM3,TIM_IT_Update ,ENABLE);
    
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );      //���TIMx���жϴ�����λ:TIM �ж�Դ     
}

/**
* @brief  ��ʱ��2�жϺ���

**/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		RendTHCnt++;
	
		ConfigModuleNoBlockTimerCnt++;
		
		mqtt_time_data(&mymqtt);/*��ʱ��ѵ����*/
		
		
//		if(ConfigModuleNoBlockFlage == 1)//ģ������������TCP������
//		{
//			LedDelay++;
//			if(LedDelay>1000)//1S��ҫ
//			{
//				LedDelay=0;
//				PCout(13) = PCin(13);
//			}
//		}
	}
}
/**
* @brief  ��ʱ��3�жϺ���
**/
u8 count_led,count_heat;
u8  xuan;
void TIM3_IRQHandler(void)   //TIM3�ж�
{              
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ
        count_led=count_led+1; 
        count_heat=count_heat+1; 
        switch(xuan)
          { 
            case 1:
            {
               if(count_led==5)
                  {                                                          																		
                      UV_lamp=0;                                  //���������            
                  } 
							if(count_led==10)
									{
											UV_lamp=1;                                  //�ر������            
									}
               if(count_heat==7)
                  {  
                      heater=0;                                   //����������  
                  } 
							 if(count_heat==15)
                  {  
                      heater=1;                                   //�رռ�����  
                  } 		
							 if(count_heat==20)
                  {  
                      heater=1;                                   //����������  
											fan=1; 																			//�ط���
											LOCK=0;																			//����
											light_color(3);															//������Ƶ�
                      TIM_Cmd(TIM3, DISABLE);                     //��TIM3
                      count_led=0;
                      count_heat=0;
                  } 
									
            }break;
            
            case 2:
            {
                if(count_led>=10)
                  {                                          
                      																		
                      UV_lamp=1;                                  //�ر������            
                      heater=0;                                   //����������    
											
                  } 
               if(count_heat>=20)
                  {  
                      heater=1;                                   //�رռ�����  
											fan=1; 																			//�ط���
											LOCK=0;																			//����
											light_color(3);															//�Ƶ�
                      TIM_Cmd(TIM3, DISABLE);                     //��TIM3
                      count_led=0;
                      count_heat=0;
                  } 
            }break;
            
            case 3:
            {
                if(count_led>=15)
                  {                                          
                      																		
                      UV_lamp=1;                                  //�ر������            
                      heater=0;                                   //����������    
											
                  } 
               if(count_heat>=25)
                  {  
                      heater=1;                                   //�رռ�����  
											fan=1; 																			//�ط���
											LOCK=0;																			//����
											light_color(3);															//�Ƶ�
                      TIM_Cmd(TIM3, DISABLE);                     //��TIM3
                      count_led=0;
                      count_heat=0;
                  }  
            }break;
            
            case 4:
            {
                if(count_led>=20)
                  {                                          
                      																		
                      UV_lamp=1;                                  //�ر������            
                      heater=0;                                   //����������    
											
                  } 
               if(count_heat>=30)
                  {  
                      heater=1;                                   //�رռ�����  
											fan=1; 																			//�ط���
											LOCK=0;																			//����
											light_color(3);															//�Ƶ�
                      TIM_Cmd(TIM3, DISABLE);                     //��TIM3
                      count_led=0;
                      count_heat=0;
                  } 
            }break;
            
          }
        
    }
   
}



