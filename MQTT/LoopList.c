#define LOOPLIST_C_
#include "LoopList.h"
#include "string.h"
#include <stdio.h>

void rbCreate(rb_t* rb,void *Buff,uint32_t BuffLen)//��������˵��ʼ�����λ�����
{
	if(NULL == rb)
	{
			printf("ERROR: input rb is NULL\n");
			return;
	}
	rb->rbCapacity = BuffLen;
	rb->rbBuff = Buff;
	rb->rbHead = rb->rbBuff;//ͷָ�������׵�ַ
	rb->rbTail = rb->rbBuff;//βָ�������׵�ַ
}

void rbDelete(rb_t* rb)//ɾ��һ�����λ�����
{
    if(NULL == rb)
    {
        printf("ERROR: input rb is NULL\n");
        return;
    }

    rb->rbBuff = NULL;//��ַ��ֵΪ��
    rb->rbHead = NULL;//ͷ��ַΪ��
    rb->rbTail = NULL;//β��ַβ��
    rb->rbCapacity = 0;//����Ϊ��
}

int32_t rbCapacity(rb_t *rb)//��ȡ����ĳ���
{
    if(NULL == rb)
    {
        printf("ERROR: input rb is NULL\n");
        return -1;
    }

    return rb->rbCapacity;
}

int32_t rbCanRead(rb_t *rb)//�����ܶ��Ŀռ�
{
    if(NULL == rb)
    {
        printf("ERROR: input rb is NULL\n");
        return -1;
    }

    if (rb->rbHead == rb->rbTail)//ͷ��β����
    {
        return 0;
    }

    if (rb->rbHead < rb->rbTail)//β����ͷ
    {
        return rb->rbTail - rb->rbHead;
    }

    return rbCapacity(rb) - (rb->rbHead - rb->rbTail);//ͷ����β
}

int32_t rbCanWrite(rb_t *rb)//������д��Ŀռ�
{
    if(NULL == rb)
    {
        printf("ERROR: input rb is NULL\n");
        return -1;
    }

    return rbCapacity(rb) - rbCanRead(rb);//�ܵļ�ȥ�Ѿ�д��Ŀռ�
}

/*   
  rb--Ҫ���Ļ�������
  data--����������
  count--���ĸ���
*/
int32_t rbRead(rb_t *rb, void *data, uint32_t count)
{
    int copySz = 0;

    if(NULL == rb)
    {
        printf("ERROR: input rb is NULL\n");
        return -1;
    }

    if(NULL == data)
    {
        printf("ERROR: input data is NULL\n");
        return -1;
    }

    if (rb->rbHead < rb->rbTail)//β����ͷ
    {
        copySz = min(count, rbCanRead(rb));//�鿴�ܶ��ĸ���
        memcpy(data, rb->rbHead, copySz);//�������ݵ�data
        rb->rbHead += copySz;//ͷָ����϶�ȡ�ĸ���
        return copySz;//���ض�ȡ�ĸ���
    }
    else //ͷ���ڵ�����β
    {
        if (count < rbCapacity(rb)-(rb->rbHead - rb->rbBuff))//���ĸ���С��ͷ�����������
        {
            copySz = count;//�����ĸ���
            memcpy(data, rb->rbHead, copySz);//
            rb->rbHead += copySz;
            return copySz;
        }
        else//���ĸ�������ͷ�����������
        {
            copySz = rbCapacity(rb) - (rb->rbHead - rb->rbBuff);//�ȶ�����ͷ���������
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead = rb->rbBuff;//ͷָ��ָ��������׵�ַ
					                                           //��Ҫ���ĸ���
            copySz += rbRead(rb, (char*)data+copySz, count-copySz);//���Ŷ�ʣ��Ҫ���ĸ���
            return copySz;
        }
    }
}

int32_t rbWrite(rb_t *rb, const void *data, uint32_t count)
{
    int tailAvailSz = 0;

    if(NULL == rb)
    {
        printf("ERROR: rb is empty \n");
        return -1;
    }

    if(NULL == data)
    {
        printf("ERROR: data is empty \n");
        return -1;
    }

    if (count >= rbCanWrite(rb))//���ʣ��Ŀռ䲻��
    {
        printf("ERROR: no memory \n");
        return -1;
    }

    if (rb->rbHead <= rb->rbTail)//ͷС�ڵ���β
    {
        tailAvailSz = rbCapacity(rb) - (rb->rbTail - rb->rbBuff);//�鿴β����ʣ��Ŀռ�
        if (count <= tailAvailSz)//����С�ڵ���β����ʣ��Ŀռ�
        {
            memcpy(rb->rbTail, data, count);//�������ݵ���������
            rb->rbTail += count;//βָ��������ݸ���
            if (rb->rbTail == rb->rbBuff+rbCapacity(rb))//����д�����
            {
                rb->rbTail = rb->rbBuff;//βָ��������׵�ַ
            }
            return count;//����д������ݸ���
        }
        else
        {
            memcpy(rb->rbTail, data, tailAvailSz);//����β����ʣ��Ŀռ�
            rb->rbTail = rb->rbBuff;//βָ��ָ�������׵�ַ
                   //ʣ��ռ�                   ʣ�����ݵ��׵�ַ       ʣ�����ݵĸ���
            return tailAvailSz + rbWrite(rb, (char*)data+tailAvailSz, count-tailAvailSz);//����дʣ�������
        }
    }
    else //ͷ����β
    {
      memcpy(rb->rbTail, data, count);
      rb->rbTail += count;
      return count;
    }
}
/**@} */

/**
* @brief   �����ζ�������д������
* @param   rb      ���ζ��й������ 
* @param   USARTx  ���ƴ�ĳ�����ڷ����ж�  
* @param   EnabledUsart ���ƴ��ж�
* @param   buf     ���͵�����
* @param   len     ���ݳ���
* @retval  -1:����
* @warning
* @example 
**/
int32_t PutData(rb_t *rb ,void *buf, uint32_t len)
{
    int32_t count = 0;

    if(NULL == buf)
    {
        printf("ERROR: gizPutData buf is empty \n");
        return -1;
    }
    
    count = rbWrite(rb, buf, len);
    if(count != len)
    {
        printf("ERROR: Failed to rbWrite \n");
        return -1;
    }
    return count;
}


