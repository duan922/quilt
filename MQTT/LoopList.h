#ifndef LOOPLIST_H_
#define LOOPLIST_H_

#ifndef LOOPLIST_C_
#define LOOPLIST_Ex_ extern
#else
#define LOOPLIST_Ex_
#endif

#include <stm32f10x.h>
	

#define min(a, b) (a)<(b)?(a):(b)                   ///< ��ȡ��Сֵ

/** ���λ��������ݽṹ */
typedef struct {
    uint32_t  rbCapacity;//�ռ��С
    char  *rbHead; //ͷ
    char  *rbTail; //β
    char  *rbBuff; //������׵�ַ
}rb_t;


void rbCreate(rb_t *rb,void *Buff,uint32_t BuffLen);//��������˵��ʼ�����λ�����
void rbDelete(rb_t* rb);
int32_t rbCapacity(rb_t *rb);//�õ����δ�С
int32_t rbCanRead(rb_t *rb);//�ܶ������ݵĸ���
int32_t rbCanWrite(rb_t *rb);//��ʣ��Ŀռ�
int32_t rbRead(rb_t *rb, void *data, uint32_t count);//��ȡ����
int32_t rbWrite(rb_t *rb, const void *data, uint32_t count);
int32_t PutData(rb_t *rb ,void *buf, uint32_t len);


#endif





























