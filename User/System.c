#include "System.h"

void NVIC_PriorityGroup(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

/*��λоƬ*/
void Reset_MCU(void)
{
  __disable_fault_irq();   
	NVIC_SystemReset();
}

