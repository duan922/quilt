/**
  ******************************************************************************
  * @file    key.c
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/13
  * @brief   �ַ���������
  ******************************************************************************

  ******************************************************************************
  */
#define CSTRING_C_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cString.h"


char *StringStr = NULL;
void cStringFree(void){
	free(StringStr);
}

/**
* @brief  ��ȡ�����ַ���֮����ַ���
* @param  Str  Դ�ַ���
* @param  StrBegin  ��ʼ���ַ���
* @param  StrEnd    �������ַ���
* @retval �ַ����׵�ַ
* @example  printf("%s",StrBetwString("wqe5w4ew46e5w","5w","6e"));cStringFree(Str);  ���:4ew4   
**/
char *StrBetwStringMalloc(char *Str,char *StrBegin,char *StrEnd)
{
	char *StrStart=0,*StrStop=0,len=0;
	len = strlen(StrBegin);//�ַ�������
	
  StrStart=strstr(Str, StrBegin);//��һ���ַ�����ʼ�ĵ�ַ
	if(StrStart)
	{
	  StrStop = strstr(StrStart+len+1, StrEnd);//�ڶ����ַ�����ʼ�ĵ�ַ
		if(StrStop)
		{
			StringStr = (char *)malloc(((StrStop - (StrStart+len))+1) *sizeof(char));//�����һ���ռ�,��ֹ�������ݸ���
			memset(StringStr,NULL,(StrStop - (StrStart+len))+1);
			memcpy(StringStr, StrStart+len, (StrStop - (StrStart+len)));
			return StringStr;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
	  return NULL;
	}
}

char *StrStop = NULL;
char StringValue;
void cStringRestore(void){
	if(StrStop!=NULL){
		*StrStop = StringValue;//���ϵ�ʱ�ضϵ��ַ���
	}
}
/**
* @brief  ��ȡ�����ַ���֮����ַ���
* @param  Str  Դ�ַ���
* @param  StrBegin  ��ʼ���ַ���
* @param  StrEnd    �������ַ���
* @retval �ַ����׵�ַ
* @example  printf("%s",StrBetwString("wqe5w4ew46e5w","5w","6e"));cStringRestore();  ���:4ew4   
**/
char *StrBetwString(char *Str,char *StrBegin,char *StrEnd)
{
	char *StrStart=0,len=0;
	len = strlen(StrBegin);//�ַ�������
  StrStart=strstr(Str, StrBegin);//��һ���ַ�����ʼ�ĵ�ַ
	if(StrStart)
	{
	  StrStop = strstr(StrStart+len+1, StrEnd);//�ڶ����ַ�����ʼ�ĵ�ַ
		if(StrStop)
		{
      StringValue = *StrStop;
			*StrStop = 0;//�ض�
			return StrStart+len;
		}
		else return NULL;
	}
	else return NULL;
}


/**
* @brief  �ָ��ַ���
* @param  src        Դ�ַ���
* @param  separator  �ָ�
* @param  dest       �����Ӵ�������
* @param  num        ���ַ����ĸ���
* @retval None
* @example split("42,uioj,dk4,56",",",temp,&cnt);  temp[0]=42,...temp[3]=56  cnt=4
**/
void split(char *src,const char *separator,char **dest,int *num) 
{
	char *pNext;
	int count = 0;
	if (src == NULL || strlen(src) == 0)
		return;
	if (separator == NULL || strlen(separator) == 0)
		return;
	pNext = (char *)strtok(src,separator);
	while(pNext != NULL) 
	{
		if(dest != NULL)
		*dest++ = pNext;
		++count;
		pNext = (char *)strtok(NULL,separator);
	}  
	*num = count;
}
