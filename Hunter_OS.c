/* Hunter.ORG all rights reserved.  You may use this software
 * and any derivatives exclusively with Hunter.ORG products.
 *
 * THIS SOFTWARE IS SUPPLIED BY Hunter.ORG "AS IS".  NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH Hunter.ORG PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL Hunter.ORG BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF Hunter.ORG HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE
 * FULLEST EXTENT ALLOWED BY LAW, Hunter.ORG'S TOTAL LIABILITY ON ALL CLAIMS
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF
 * ANY, THAT YOU HAVE PAID DIRECTLY TO Hunter.ORG FOR THIS SOFTWARE.
 *
 * Hunter.ORG PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

/*
 * File: Hunter_OS.c
 * Author: Hunter.ORG
 * Created on: 2014��2��16��
 * Revision history: 1.1
 */

/*
 * This is a guard condition so that contents of this file are not included
 * more than once.
 */

#include "Hunter_OS.h"
volatile static uint32 ms_SystemCount=0;
volatile static uint32 us_SystemCount=0;
volatile static int8 Init_flag = 0;
volatile static int8 Schedule_Locked = 0;
extern volatile uint16 omsTaskList_length;
extern volatile uint16 ousTaskList_length;
extern volatile uint32 oMailBoxMessageTotal;
static uint32 Xidle = 0;
static uint32 WatchDog_Timer = 0;
static void idle(void){}
/*****************************************************************************
 * @������	int16 (*Time_Gape_TaskHighDelete)(int16 Index,void(*UserTask)(uint32* Parameter));
 *----------------------------------------------------------------------------
 * @����	����ʵ���˸����ȼ�����ɾ��
 *			����ʼ������ʵ�ֵ�Ŀ���ǣ�ͨ�����ñ����������û�ָ���ĺ������û�ָ����ms�����ڵĵ��á�
 *----------------------------------------------------------------------------
 * @����	Index����ɾ���������������б����������Index �� -1ʱ������ɾ�������ᰴ�������Ž���ɾ��.
 *					��Index �� -1ʱ������ɾ����������������б������UserTask��ͬ�����ƽ���ɾ����
 *					ͨ����˵��������ɾ�����ٶ�ԶԶ���ڰ���������ɾ�������Խ����û�ʹ������ɾ����
 *			UserTask:��ɾ������ĺ�����
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ	int16������-1����ɾ��ʧ�ܣ�û���ҵ���Ӧ�����񣻰�����ɾ��ʱ��������Ŵ��������б�ʱ�᷵
 *			��ɾ��ʧ�ܣ������������ɾ���ɹ���ɾ���ɹ��󷵻ر�ɾ�������������б��е�����λ�á�
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_TaskHighDelete(int16 Index,usertask UserTask)
{
	uint16 i=0;
	//------------------------------------------------------------------------
	if(-1 == Index)
	{
		for(i = 0;i < ousTaskList_length;i ++)
		{
			//----------------------------------------------------------------�ҵ�Ҫ��ɾ��������
			if(us_TaskList[i].Task == UserTask)
			{
				//------------------------------------------------------------ִ��ɾ��
				us_TaskList[i].exist = 0;						//ɾ��������
				us_TaskList[i].Ready = 0;						//ɾ��������
				us_TaskList[i].time_count = 0;					//ɾ���������ѭ������
				us_TaskList[i].Task = (usertask)idle;			//ɾ������������ƻ��ߵ�ַ
				us_TaskList[i].Priority = 0;					//ɾ������������ȼ�
				us_TaskList[i].SystemCount = 0;					//ɾ����ǰ��ʱ���
				return i;
			}
		}
		return OS_ERROR;
	}
	else
	{
		if(Index >= ousTaskList_length)
			return OS_ERROR;
		else
		{
			//------------------------------------------------------------ִ��ɾ��
			us_TaskList[Index].exist = 0;						//ɾ��������
			us_TaskList[Index].Ready = 0;						//ɾ��������
			us_TaskList[Index].time_count = 0;					//ɾ���������ѭ������
			us_TaskList[Index].Task = (usertask)idle;			//ɾ������������ƻ��ߵ�ַ
			us_TaskList[Index].Priority = 0;					//ɾ������������ȼ�
			us_TaskList[Index].SystemCount = 0;					//ɾ����ǰ��ʱ���
			return Index;
		}
	}
}
/*****************************************************************************
 * @������	int16 (*Os_Time_Gape_TaskDelete)(int16 Index,void(*UserTask)(uint32* Parameter));
 *----------------------------------------------------------------------------
 * @����	����ʵ�����û�����ɾ��
 *			����ʼ������ʵ�ֵ�Ŀ���ǣ�ͨ�����ñ����������û�ָ���ĺ������û�ָ����ms�����ڵĵ��á�
 *----------------------------------------------------------------------------
 * @����	Index����ɾ���������������б����������Index �� -1ʱ������ɾ�������ᰴ�������Ž���ɾ��.
 *					��Index �� -1ʱ������ɾ����������������б������UserTask��ͬ�����ƽ���ɾ����
 *					ͨ����˵��������ɾ�����ٶ�ԶԶ���ڰ���������ɾ�������Խ����û�ʹ������ɾ����
 *			UserTask:��ɾ������ĺ�����
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ	int16������-1����ɾ��ʧ�ܣ�û���ҵ���Ӧ�����񣻰�����ɾ��ʱ��������Ŵ��������б�ʱ�᷵
 *			��ɾ��ʧ�ܣ������������ɾ���ɹ���ɾ���ɹ��󷵻ر�ɾ�������������б��е�����λ�á�
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_TaskDelete(int16 Index,usertask UserTask)
{
	uint16 i=0;
	//------------------------------------------------------------------------
	if(-1 == Index)
	{
		for(i = 0;i < omsTaskList_length;i ++)
		{
			//----------------------------------------------------------------�ҵ�Ҫ��ɾ��������
			if(ms_TaskList[i].Task == UserTask)
			{
				//------------------------------------------------------------ִ��ɾ��
				ms_TaskList[i].exist = 0;						//ɾ��������
				ms_TaskList[i].Ready = 0;						//ɾ��������
				ms_TaskList[i].time_count = 0;					//ɾ���������ѭ������
				ms_TaskList[i].Task = (usertask)idle;			//ɾ������������ƻ��ߵ�ַ
				ms_TaskList[i].Priority = 0;					//ɾ������������ȼ�
				ms_TaskList[i].SystemCount = 0;					//ɾ����ǰ��ʱ���
				return i;
			}
		}
		return OS_ERROR;
	}
	else
	{
		if(Index >= omsTaskList_length)
			return OS_ERROR;
		else
		{
			//------------------------------------------------------------ִ��ɾ��
			ms_TaskList[Index].exist = 0;						//ɾ��������
			ms_TaskList[Index].Ready = 0;						//ɾ��������
			ms_TaskList[Index].time_count = 0;					//ɾ���������ѭ������
			ms_TaskList[Index].Task = (usertask)idle;			//ɾ������������ƻ��ߵ�ַ
			ms_TaskList[Index].Priority = 0;					//ɾ������������ȼ�
			ms_TaskList[Index].SystemCount = 0;					//ɾ����ǰ��ʱ���
			return Index;
		}
	}
}
/*****************************************************************************
 * @������	int8 Time_Gape_Register_V1(uint32 ms,usertask UserTask,uint8 Priority)
 *----------------------------------------------------------------------------
 * @����	����ʵ�����û�����ʱ��������ע��
 *			����ʼ������ʵ�ֵ�Ŀ���ǣ�ͨ�����ñ����������û�ָ���ĺ������û�ָ����ms�����ڵĵ��á�
 *----------------------------------------------------------------------------
 * @����	ms��ָ�û�ϣ��ָ���ĺ������ڵ��õĵ����ڣ���λΪms
 *			UserTask:�û�ָ���ĺ�����
 *			Priority:��������ȼ���ֵԽ�����ȼ�Խ��
 *----------------------------------------------------------------------------
 * @���	��
 *
 *----------------------------------------------------------------------------
 * @����ֵ	int8������-1����ע��ʧ�ܣ��������б������λ��ֵ����ɹ�
 *
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_Register_V1(uint32 ms,usertask UserTask,uint8 Priority)
{
	uint16 i = 0;
	while(ms_TaskList[i].exist)//---------------------------------------------ѭ���������б��еĿո�
	{
		i ++;
		if(i > omsTaskList_length)//------------------------------------------�б�������ע��ʧ��
		{
			return OS_ERROR;
		}
	}
	//------------------------------------------------------------------------�������
	ms_TaskList[i].exist = 1;								//��Ǹ������Ѿ�����
	ms_TaskList[i].time_count = ms;							//��¼�������ѭ������
	ms_TaskList[i].Task = UserTask;							//��¼����������ƻ��ߵ�ַ
	ms_TaskList[i].Priority = Priority;						//��¼����������ȼ�
	ms_TaskList[i].SystemCount = ms_SystemCount + ms;		//��¼��һ�����е�ʱ��
	//------------------------------------------------------------------------
	return i;
}
/*****************************************************************************
 * @������	int8 Time_Gape_Register(uint32 ms,usertask UserTask)
 *----------------------------------------------------------------------------
 * @����	����ʵ�����û�����ʱ��������ע��
 *			����ʼ������ʵ�ֵ�Ŀ���ǣ�ͨ�����ñ����������û�ָ���ĺ������û�ָ����ms�����ڵĵ��á�
 *----------------------------------------------------------------------------
 * @����	ms��ָ�û�ϣ��ָ���ĺ������ڵ��õĵ����ڣ���λΪms
 *			UserTask:�û�ָ���ĺ�����
 *----------------------------------------------------------------------------
 * @���	��
 *
 *----------------------------------------------------------------------------
 * @����ֵ	int8������-1����ע��ʧ�ܣ��������б������λ��ֵ����ɹ�
 *
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_Register(uint32 ms,usertask UserTask)
{
	return Time_Gape_Register_V1(ms,UserTask,1);
}
/*****************************************************************************
 * @������	int8 Time_Gape_RegisterHigh(uint32 PLUS,usertask UserTask,uint8 Priority)
 *----------------------------------------------------------------------------
 * @����	����ʵ���˸����ȼ��û�����ʱ��������ע��
 *			����ʼ������ʵ�ֵ�Ŀ���ǣ�ͨ�����ñ����������û�ָ���ĺ������û�ָ����100us�����ڵĵ��á�
 *----------------------------------------------------------------------------
 * @����	PLUS������
 *			UserTask:�û�ָ���ĺ�����
 *			Priority:��������ȼ�
 *----------------------------------------------------------------------------
 * @���		��
 *
 *----------------------------------------------------------------------------
 * @����ֵ	int8������-1����ע��ʧ�ܣ��������б��ֵλ�ô���ɹ�
 *
 *----------------------------------------------------------------------------
 * @����		2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_RegisterHigh(uint32 PLUS,usertask UserTask,uint8 Priority)
{
	uint16 i=0;
	while(us_TaskList[i].exist)//---------------------------------------------ѭ���������б��еĿո�
	{
		i ++;
		if(i > ousTaskList_length)//------------------------------------------�б�������ע��ʧ��
			return OS_ERROR;
	}
	us_TaskList[i].exist = 1;					//��Ǹ������Ѿ�����
	us_TaskList[i].time_count = PLUS;			//��¼�������ѭ������
	us_TaskList[i].Task = UserTask;				//��¼����������ƻ��ߵ�ַ
	us_TaskList[i].Priority = Priority;			//��¼����������ȼ�
	us_TaskList[i].SystemCount = us_SystemCount + PLUS;		//��¼��һ�����е�ʱ��
	//------------------------------------------------------------------------
	return i;
}
/*****************************************************************************
 * @������	void Osus_Time_Base(void *DATA)
 *----------------------------------------------------------------------------
 * @����	usʱ���������뽫�ú�����һ���ϸ����ȼ���ʱ���ж���ִ��
 *----------------------------------------------------------------------------
 * @����
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ
 *----------------------------------------------------------------------------
 * @����		2018.3.1
 *****************************************************************************/
STATIC void us_Time_Base(void *DATA)
{
	static uint16 i = 0;
	if(1 == Init_flag)
	{
		//--------------------------------------------------------------------�����㷨1
		#if(1)
		//--------------------------------------------------------------------
		for(i = 0;i < ousTaskList_length;i ++)
		{
			if(1 == us_TaskList[i].exist)
			{
				if(us_SystemCount > us_TaskList[i].SystemCount)
				{
					us_TaskList[i].Task((void*)&i);
					us_TaskList[i].SystemCount = us_SystemCount + us_TaskList[i].time_count;
				}
			}
		}
		//--------------------------------------------------------------------
		#endif
		//--------------------------------------------------------------------
		us_SystemCount ++;
		//--------------------------------------------------------------------
	}
}
/*****************************************************************************
 * @������	void ms_Time_Base(void *DATA)
 *----------------------------------------------------------------------------
 * @����		msʱ���������뽫�ú�����һ��1ms�Ķ�ʱ���ж���ִ��
 *----------------------------------------------------------------------------
 * @����
 *----------------------------------------------------------------------------
 * @���		��
 *----------------------------------------------------------------------------
 * @����ֵ
 *----------------------------------------------------------------------------
 * @����		2018.3.1
 *****************************************************************************/
STATIC void ms_Time_Base(void *DATA)
{
	uint16 i = 0;
	//------------------------------------------------------------------------
	if((1 == Init_flag)&&(0 == Schedule_Locked))
	{
		//--------------------------------------------------------------------�����㷨1
		#if(1)
		//--------------------------------------------------------------------
		while(i < omsTaskList_length)
		{
			/*���ҽ���iС����������������ѱ���ʼ���͵�����û�б������Ż�ִ��*/
			if(1 == ms_TaskList[i].exist)//-------�жϸó�Ա�Ƿ���ڣ�����������һ���ж�
			{
				//ms_TaskList[i].Ready |= /*ms_TaskList[i].exist & */!((ms_SystemCount - ms_TaskList[i].SystemCount) % ms_TaskList[i].time_count);
				if(ms_SystemCount >= ms_TaskList[i].SystemCount)
				{
					ms_TaskList[i].Ready = 1;
					ms_TaskList[i].SystemCount = ms_SystemCount + ms_TaskList[i].time_count;
				}
			}
			i ++;
		}
		//--------------------------------------------------------------------
		#endif
	}
	//------------------------------------------------------------------------
	ms_SystemCount ++;
	//------------------------------------------------------------------------
	WatchDog_Timer ++;
	//------------------------------------------------------------------------
}
/*****************************************************************************
 * @������	void ms_Time_Gape(void *DATA)
 *----------------------------------------------------------------------------
 * @����	msʱ϶�������뽫�ú�����main������ִ��
 *----------------------------------------------------------------------------
 * @����
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *****************************************************************************/
STATIC void ms_Time_Gape(void *DATA)
{
	static uint16 i = 0;
	//------------------------------------------------------------------------�����㷨1
	#if(1)
	//------------------------------------------------------------------------
	for(i = 0;i < omsTaskList_length;i ++)
	{
		if(1 == ms_TaskList[i].Ready)
		{
			ms_TaskList[i].Ready = 0;
			ms_TaskList[i].Task((void*)&i);
		}
	}
	//------------------------------------------------------------------------
	#endif
}
/*****************************************************************************
 * @������	void Os_Init(void)
 *----------------------------------------------------------------------------
 * @����	�����ܳ�ʼ�����򣬳�ʼ��ϵͳ�����б�
 *----------------------------------------------------------------------------
 * @����
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *****************************************************************************/
STATIC void Os_Init(void)
{
	uint16 i;
	for(i = 0;i < omsTaskList_length;i ++)/*�������ȼ������б��ʼ��*/
	{
		ms_TaskList[i].Ready = 0;
		ms_TaskList[i].exist = 0;
		ms_TaskList[i].time_count = 1;
		ms_TaskList[i].Task = (usertask)idle;
		ms_TaskList[i].Priority = 0;
		ms_TaskList[i].SystemCount = 0;
		//--------------------------------------------------------------------�����б��ʼ��
		HTOS_MailBox[i].MaxMessageTotal = 0;
		HTOS_MailBox[i].MessageIndex = 0;
		HTOS_MailBox[i].MessageTotal = 0;
		HTOS_MailBox[i].Post = (htos_message*)&Xidle;
		HTOS_MailBox[i].Password = 0;
	}
	for(i = 0;i < ousTaskList_length;i ++)/*�����ȼ������б��ʼ��*/
	{
		us_TaskList[i].Ready = 0;
		us_TaskList[i].exist = 0;
		us_TaskList[i].time_count = 1;
		us_TaskList[i].Task = (usertask)idle;
		us_TaskList[i].Priority = 0;
		us_TaskList[i].SystemCount = 0;
	}
	Init_flag = 1;
	WatchDog_Timer = 0;
}
/*****************************************************************************
 * @������	int8 Schedule_Lock(void* Parameter)
 *----------------------------------------------------------------------------
 * @����	����������
 *----------------------------------------------------------------------------
 * @����	Parameter:����
 *----------------------------------------------------------------------------
 * @���	��
 *
 *----------------------------------------------------------------------------
 * @����ֵ	OS_OK
 *
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int8 Schedule_Lock(void* Parameter)
{
	Schedule_Locked = 1;
	return OS_OK;
}
/*****************************************************************************
 * @������	int8 Schedule_UnLock(void* Parameter)
 *----------------------------------------------------------------------------
 * @����	����������
 *----------------------------------------------------------------------------
 * @����	Parameter:����
 *----------------------------------------------------------------------------
 * @���	��
 *
 *----------------------------------------------------------------------------
 * @����ֵ	OS_OK
 *
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int8 Schedule_UnLock(void* Parameter)
{
	Schedule_Locked = 0;
	return OS_OK;
}
/*****************************************************************************
 * @������	uint32 ReadSystemCount(void* Parameter)
 *----------------------------------------------------------------------------
 * @����	��ȡ��ǰϵͳʱ���������
 *----------------------------------------------------------------------------
 * @����	Parameter:����
 *----------------------------------------------------------------------------
 * @���	��
 *
 *----------------------------------------------------------------------------
 * @����ֵ	ϵͳ����ֵ
 *
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC uint32 ReadSystemCount(void* Parameter)
{
	return ms_SystemCount;
}
/*****************************************************************************
 * @������	int16 (*Os_MailBox_Register)(int16 Index,usertask UserTask,htos_message* HTOS_message,uint16 MaxMessageTotal,uint32 Password);
 *----------------------------------------------------------------------------
 * @����	����ʵ��Ϊָ������ע���������
 *			ע�⣺���������Ƚ���������ע�����䣬�������Index=-1�����û���ע�ᡣ���û���ע������
 *			ʱ��Ҫ���������б��ٶȽ������ܶ࣬���Խ����û�ʹ��������ע�����䡣
 *----------------------------------------------------------------------------
 * @����	Index�������������
 *			UserTask�����������
 *			*HTOS_message��ָ�����������λ��
 *			MaxMessageTotal��ָ������Ĵ�С
 *			Password:��������룬ͨ����ע������ʱд�룬��ֹ����������ȡ�������ݣ������ʼ���ʧ
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ	����-1����ע��ʧ�ܣ����������Ż����û����Ʋ��ԣ�����0����ע��ɹ�
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int16 MailBox_Register(int16 Index,usertask UserTask,htos_message* HTOS_message,uint16 MaxMessageTotal,uint32 Password)
{
	uint16 i = 0;
	if(-1 == Index)
	{
		for(i = 0;i < omsTaskList_length;i ++)
		{
			if(ms_TaskList[i].Task == UserTask)
			{
				HTOS_MailBox[i].MaxMessageTotal = MaxMessageTotal;
				HTOS_MailBox[i].Post = HTOS_message;
				HTOS_MailBox[i].MessageIndex = 0;
				HTOS_MailBox[i].MessageTotal = 0;
				HTOS_MailBox[i].Password = Password;
				return i;
			}
		}
		return OS_ERROR;
	}
	else
	{
		if(Index >= omsTaskList_length)
			return OS_ERROR;
		else
		{
			HTOS_MailBox[Index].MaxMessageTotal = MaxMessageTotal;
			HTOS_MailBox[Index].Post = HTOS_message;
			HTOS_MailBox[Index].MessageIndex = 0;
			HTOS_MailBox[Index].MessageTotal = 0;
			HTOS_MailBox[Index].Password = Password;
			return Index;
		}
	}
}
/*****************************************************************************
 * @������	int16 (*PostSent)(int16 ReciIndex,usertask Recipient,htos_message *Message);
 *----------------------------------------------------------------------------
 * @����	����ʵ���ʼ�����
 *			ע�⣺���������Ƚ��������������ŷ����ʼ����������ReciIndex=-1�����û��������ʼ���
 *			���û�����������ʱ��Ҫ���������б��ٶȽ������ܶ࣬���Խ����û�ʹ�������ŷ����ʼ���
 *----------------------------------------------------------------------------
 * @����	ReciIndex�������������������
 *			Recipient�����������������
 *			**Message��Ҫ���͵���Ϣ
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ	����-1������ʧ�ܣ����ܶԷ������Ѿ����˻���û���ҵ������񣻷���ֵ�����Ӧ����������Ѿ�
 *			���յ��ʼ���
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int16 PostSent(int16 ReciIndex,usertask Recipient,htos_message *Message)/*�ʼ����ͺ���*/
{
	static uint16 i;
	int16 temp;
	if(-1 == ReciIndex)
	{
		for(i = 0;i < omsTaskList_length;i ++)
		{
			if(ms_TaskList[i].Task == Recipient)
			{
				temp = HTOS_MailBox[i].MessageTotal - HTOS_MailBox[i].MessageIndex;
				if(temp < 0)
					temp += HTOS_MailBox[i].MaxMessageTotal;
				if(temp < HTOS_MailBox[i].MaxMessageTotal)/*������δ��*/
				{
					if(HTOS_MailBox[i].MessageTotal < HTOS_MailBox[i].MaxMessageTotal)
					{
						HTOS_MailBox[i].Post[HTOS_MailBox[i].MessageTotal] = *Message;/*������Ϣ*/
						HTOS_MailBox[i].MessageTotal ++;
						return i;
					}
					else
						return OS_ERROR;
				}
				else
					return OS_ERROR;
			}
		}
		return OS_ERROR;
	}
	else
	{
		if(ReciIndex < omsTaskList_length)
		{
			temp = HTOS_MailBox[ReciIndex].MessageTotal - HTOS_MailBox[ReciIndex].MessageIndex;
			if(temp < 0)
				temp += HTOS_MailBox[ReciIndex].MaxMessageTotal;

			if(temp < HTOS_MailBox[ReciIndex].MaxMessageTotal)/*������δ��*/
			{
				if(HTOS_MailBox[ReciIndex].MessageTotal < HTOS_MailBox[ReciIndex].MaxMessageTotal)
				{
					HTOS_MailBox[ReciIndex].Post[(HTOS_MailBox[ReciIndex].MessageTotal)] = *Message;/*������Ϣ*/
					HTOS_MailBox[ReciIndex].MessageTotal ++;
					return ReciIndex;
				}
				else
					return OS_ERROR;
			}
			else
				return OS_ERROR;
		}
		else
			return OS_ERROR;
	}
}
/*****************************************************************************
 * @������	static uint16 PostRead(htos_mailbox* MailBox,htos_message* Message,uint32 Password)
 *----------------------------------------------------------------------------
 * @����	����ʵ���ʼ���ȡ
 *			ע�⣺���������Ƚ��������������ŷ����ʼ����������ReciIndex=-1�����û��������ʼ���
 *			���û�����������ʱ��Ҫ���������б��ٶȽ������ܶ࣬���Խ����û�ʹ�������ŷ����ʼ���
 *----------------------------------------------------------------------------
 * @����	Index����ǰ�����������(�����βεĵ�һ����)
 *			* Message����ȡ������Ϣ
 *			Password:��ȡ����
 *----------------------------------------------------------------------------
 * @���	��
 *----------------------------------------------------------------------------
 * @����ֵ	����0����û���յ��κ��ʼ�������Ϊ�գ�����ֵ1�����ȡ��1����Ϣ���û�Ӧѭ����ȡ��Ϣֱ����
 *			��0(ȫ����ȡ)��
 *			����-1���������Ŵ��󣬸������Ų����б��У�����-2�������������˶�������޸����롣
 *----------------------------------------------------------------------------
 * @����	2018.3.1
 *
 *****************************************************************************/
STATIC int16 PostRead(int16 Index,htos_message* Message,uint32 Password)
{
	if((Index > -1)&&(Index < omsTaskList_length))
	{
		if(Password == HTOS_MailBox[Index].Password)
		{
			if(HTOS_MailBox[Index].MessageIndex != HTOS_MailBox[Index].MessageTotal)
			{
				*Message = HTOS_MailBox[Index].Post[HTOS_MailBox[Index].MessageIndex];
				if(HTOS_MailBox[Index].MessageIndex < HTOS_MailBox[Index].MaxMessageTotal)
					HTOS_MailBox[Index].MessageIndex ++;
				else
					HTOS_MailBox[Index].MessageIndex = 0;
				return 1;
			}
			else
				return 0;
		}
		else
			return -2;
	}
	else
		return -1;
}
/*
 *	�����ܳ���Ľṹ��
 */
volatile htos_schedule HTOS_Schedule = {
											us_Time_Base,
											ms_Time_Base,
											ms_Time_Gape
										};

volatile htos_function HTOS_Function = {
											Os_Init,
											Time_Gape_TaskHighDelete,
											Time_Gape_TaskDelete,
											Time_Gape_Register_V1,
											Time_Gape_Register,
											Time_Gape_RegisterHigh,
											Schedule_Lock,
											Schedule_UnLock,
											ReadSystemCount,
											MailBox_Register,
											PostSent,
											PostRead,
										};





