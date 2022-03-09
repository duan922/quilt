


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
* @brief  定时器2配置

**/

void Timer2_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
			
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//定时器2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Resets the TIM2 */
    TIM_DeInit(TIM2);	                                               
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置了时钟分割。	                                                      
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 选择了向上计数器模式。
	//初值
	TIM_TimeBaseStructure.TIM_Period = 10;//定时时间1ms进一次     
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置了用来作为 TIMx 时钟频率除数的预分频值。72M / 7199+1 = 0.01M	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_Cmd(TIM2, ENABLE);
	
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
}

//定时器3的配置
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 		
	TIM_ITConfig(TIM3,TIM_IT_Update ,ENABLE);
    
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );      //清除TIMx的中断待处理位:TIM 中断源     
}

/**
* @brief  定时器2中断函数

**/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		RendTHCnt++;
	
		ConfigModuleNoBlockTimerCnt++;
		
		mqtt_time_data(&mymqtt);/*定时轮训函数*/
		
		
//		if(ConfigModuleNoBlockFlage == 1)//模块配置连接了TCP服务器
//		{
//			LedDelay++;
//			if(LedDelay>1000)//1S闪耀
//			{
//				LedDelay=0;
//				PCout(13) = PCin(13);
//			}
//		}
	}
}
/**
* @brief  定时器3中断函数
**/
u8 count_led,count_heat;
u8  xuan;
void TIM3_IRQHandler(void)   //TIM3中断
{              
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
        count_led=count_led+1; 
        count_heat=count_heat+1; 
        switch(xuan)
          { 
            case 1:
            {
               if(count_led==5)
                  {                                                          																		
                      UV_lamp=0;                                  //开启紫外灯            
                  } 
							if(count_led==10)
									{
											UV_lamp=1;                                  //关闭紫外灯            
									}
               if(count_heat==7)
                  {  
                      heater=0;                                   //开启加热器  
                  } 
							 if(count_heat==15)
                  {  
                      heater=1;                                   //关闭加热器  
                  } 		
							 if(count_heat==20)
                  {  
                      heater=1;                                   //开启加热器  
											fan=1; 																			//关风扇
											LOCK=0;																			//开锁
											light_color(3);															//完成亮黄灯
                      TIM_Cmd(TIM3, DISABLE);                     //关TIM3
                      count_led=0;
                      count_heat=0;
                  } 
									
            }break;
            
            case 2:
            {
                if(count_led>=10)
                  {                                          
                      																		
                      UV_lamp=1;                                  //关闭紫外灯            
                      heater=0;                                   //开启加热器    
											
                  } 
               if(count_heat>=20)
                  {  
                      heater=1;                                   //关闭加热器  
											fan=1; 																			//关风扇
											LOCK=0;																			//开锁
											light_color(3);															//黄灯
                      TIM_Cmd(TIM3, DISABLE);                     //关TIM3
                      count_led=0;
                      count_heat=0;
                  } 
            }break;
            
            case 3:
            {
                if(count_led>=15)
                  {                                          
                      																		
                      UV_lamp=1;                                  //关闭紫外灯            
                      heater=0;                                   //开启加热器    
											
                  } 
               if(count_heat>=25)
                  {  
                      heater=1;                                   //关闭加热器  
											fan=1; 																			//关风扇
											LOCK=0;																			//开锁
											light_color(3);															//黄灯
                      TIM_Cmd(TIM3, DISABLE);                     //关TIM3
                      count_led=0;
                      count_heat=0;
                  }  
            }break;
            
            case 4:
            {
                if(count_led>=20)
                  {                                          
                      																		
                      UV_lamp=1;                                  //关闭紫外灯            
                      heater=0;                                   //开启加热器    
											
                  } 
               if(count_heat>=30)
                  {  
                      heater=1;                                   //关闭加热器  
											fan=1; 																			//关风扇
											LOCK=0;																			//开锁
											light_color(3);															//黄灯
                      TIM_Cmd(TIM3, DISABLE);                     //关TIM3
                      count_led=0;
                      count_heat=0;
                  } 
            }break;
            
          }
        
    }
   
}



