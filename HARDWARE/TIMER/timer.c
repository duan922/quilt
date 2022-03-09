#include "timer.h"
#include "led.h"
#include "usart.h"

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

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
u16 count_led,count_heat;
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
               if(count_led>=5)
                  {  
                      LED=1;                                  //�ر�LED
                      fan=1;                                  //�رշ���
                      UV_lamp=1;                              //�ر������            
                      AC=0;                                   //����������           
                      
                  } 
               if(count_heat>=10)
                  {  
                      AC=1;                                       //�ر�LED   
                      fan=1;                                      //�رշ���            
                      TIM_Cmd(TIM3, DISABLE);                     //��TIM3
                      count_led=0;
                      count_heat=0;
                  } 
            }break;
            
            case 2:
            {
                LED=1;
            } 
            break;
          }
        
    }
   
}









