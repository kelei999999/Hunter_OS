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
 * Created on: 2014年2月16日
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
 * @函数名	int16 (*Time_Gape_TaskHighDelete)(int16 Index,void(*UserTask)(uint32* Parameter));
 *----------------------------------------------------------------------------
 * @描述	函数实现了高优先级任务删除
 *			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
 *----------------------------------------------------------------------------
 * @输入	Index：被删除任务所在任务列表的索引。当Index ≠ -1时，任务删除器将会按照索引号进行删除.
 *					当Index ＝ -1时，任务删除器将会遍历任务列表查找与UserTask相同的名称进行删除。
 *					通常来说，按索引删除的速度远远大于按任务名称删除，所以建议用户使用索引删除。
 *			UserTask:被删除任务的函数名
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值	int16：返回-1代表删除失败，没有找到对应的任务；按索引删除时如果索引号大于任务列表时会返
 *			回删除失败，其他情况均会删除成功。删除成功后返回被删除任务在任务列表中的索引位置。
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
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
			//----------------------------------------------------------------找到要被删除的任务
			if(us_TaskList[i].Task == UserTask)
			{
				//------------------------------------------------------------执行删除
				us_TaskList[i].exist = 0;						//删除该任务
				us_TaskList[i].Ready = 0;						//删除该任务
				us_TaskList[i].time_count = 0;					//删除该任务的循环周期
				us_TaskList[i].Task = (usertask)idle;			//删除该任务的名称或者地址
				us_TaskList[i].Priority = 0;					//删除该任务的优先级
				us_TaskList[i].SystemCount = 0;					//删除当前的时间戳
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
			//------------------------------------------------------------执行删除
			us_TaskList[Index].exist = 0;						//删除该任务
			us_TaskList[Index].Ready = 0;						//删除该任务
			us_TaskList[Index].time_count = 0;					//删除该任务的循环周期
			us_TaskList[Index].Task = (usertask)idle;			//删除该任务的名称或者地址
			us_TaskList[Index].Priority = 0;					//删除该任务的优先级
			us_TaskList[Index].SystemCount = 0;					//删除当前的时间戳
			return Index;
		}
	}
}
/*****************************************************************************
 * @函数名	int16 (*Os_Time_Gape_TaskDelete)(int16 Index,void(*UserTask)(uint32* Parameter));
 *----------------------------------------------------------------------------
 * @描述	函数实现了用户任务删除
 *			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
 *----------------------------------------------------------------------------
 * @输入	Index：被删除任务所在任务列表的索引。当Index ≠ -1时，任务删除器将会按照索引号进行删除.
 *					当Index ＝ -1时，任务删除器将会遍历任务列表查找与UserTask相同的名称进行删除。
 *					通常来说，按索引删除的速度远远大于按任务名称删除，所以建议用户使用索引删除。
 *			UserTask:被删除任务的函数名
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值	int16：返回-1代表删除失败，没有找到对应的任务；按索引删除时如果索引号大于任务列表时会返
 *			回删除失败，其他情况均会删除成功。删除成功后返回被删除任务在任务列表中的索引位置。
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
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
			//----------------------------------------------------------------找到要被删除的任务
			if(ms_TaskList[i].Task == UserTask)
			{
				//------------------------------------------------------------执行删除
				ms_TaskList[i].exist = 0;						//删除该任务
				ms_TaskList[i].Ready = 0;						//删除该任务
				ms_TaskList[i].time_count = 0;					//删除该任务的循环周期
				ms_TaskList[i].Task = (usertask)idle;			//删除该任务的名称或者地址
				ms_TaskList[i].Priority = 0;					//删除该任务的优先级
				ms_TaskList[i].SystemCount = 0;					//删除当前的时间戳
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
			//------------------------------------------------------------执行删除
			ms_TaskList[Index].exist = 0;						//删除该任务
			ms_TaskList[Index].Ready = 0;						//删除该任务
			ms_TaskList[Index].time_count = 0;					//删除该任务的循环周期
			ms_TaskList[Index].Task = (usertask)idle;			//删除该任务的名称或者地址
			ms_TaskList[Index].Priority = 0;					//删除该任务的优先级
			ms_TaskList[Index].SystemCount = 0;					//删除当前的时间戳
			return Index;
		}
	}
}
/*****************************************************************************
 * @函数名	int8 Time_Gape_Register_V1(uint32 ms,usertask UserTask,uint8 Priority)
 *----------------------------------------------------------------------------
 * @描述	函数实现了用户任务时间间隔函数注册
 *			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
 *----------------------------------------------------------------------------
 * @输入	ms：指用户希望指定的函数周期调用的的周期，单位为ms
 *			UserTask:用户指定的函数名
 *			Priority:任务的优先级，值越大，优先级越高
 *----------------------------------------------------------------------------
 * @输出	无
 *
 *----------------------------------------------------------------------------
 * @返回值	int8：返回-1代表注册失败，返回在列表的索引位置值代表成功
 *
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_Register_V1(uint32 ms,usertask UserTask,uint8 Priority)
{
	uint16 i = 0;
	while(ms_TaskList[i].exist)//---------------------------------------------循环检查毫秒列表中的空格
	{
		i ++;
		if(i > omsTaskList_length)//------------------------------------------列表已满，注册失败
		{
			return OS_ERROR;
		}
	}
	//------------------------------------------------------------------------添加任务
	ms_TaskList[i].exist = 1;								//标记该任务已经存在
	ms_TaskList[i].time_count = ms;							//记录该任务的循环周期
	ms_TaskList[i].Task = UserTask;							//记录该任务的名称或者地址
	ms_TaskList[i].Priority = Priority;						//记录该任务的优先级
	ms_TaskList[i].SystemCount = ms_SystemCount + ms;		//记录下一次运行的时间
	//------------------------------------------------------------------------
	return i;
}
/*****************************************************************************
 * @函数名	int8 Time_Gape_Register(uint32 ms,usertask UserTask)
 *----------------------------------------------------------------------------
 * @描述	函数实现了用户任务时间间隔函数注册
 *			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
 *----------------------------------------------------------------------------
 * @输入	ms：指用户希望指定的函数周期调用的的周期，单位为ms
 *			UserTask:用户指定的函数名
 *----------------------------------------------------------------------------
 * @输出	无
 *
 *----------------------------------------------------------------------------
 * @返回值	int8：返回-1代表注册失败，返回在列表的索引位置值代表成功
 *
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_Register(uint32 ms,usertask UserTask)
{
	return Time_Gape_Register_V1(ms,UserTask,1);
}
/*****************************************************************************
 * @函数名	int8 Time_Gape_RegisterHigh(uint32 PLUS,usertask UserTask,uint8 Priority)
 *----------------------------------------------------------------------------
 * @描述	函数实现了高优先级用户任务时间间隔函数注册
 *			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的100us数周期的调用。
 *----------------------------------------------------------------------------
 * @输入	PLUS：保留
 *			UserTask:用户指定的函数名
 *			Priority:任务的优先级
 *----------------------------------------------------------------------------
 * @输出		无
 *
 *----------------------------------------------------------------------------
 * @返回值	int8：返回-1代表注册失败，返回在列表的值位置代表成功
 *
 *----------------------------------------------------------------------------
 * @日期		2018.3.1
 *
 *****************************************************************************/
STATIC int16 Time_Gape_RegisterHigh(uint32 PLUS,usertask UserTask,uint8 Priority)
{
	uint16 i=0;
	while(us_TaskList[i].exist)//---------------------------------------------循环检查毫秒列表中的空格
	{
		i ++;
		if(i > ousTaskList_length)//------------------------------------------列表已满，注册失败
			return OS_ERROR;
	}
	us_TaskList[i].exist = 1;					//标记该任务已经存在
	us_TaskList[i].time_count = PLUS;			//记录该任务的循环周期
	us_TaskList[i].Task = UserTask;				//记录该任务的名称或者地址
	us_TaskList[i].Priority = Priority;			//记录该任务的优先级
	us_TaskList[i].SystemCount = us_SystemCount + PLUS;		//记录下一次运行的时间
	//------------------------------------------------------------------------
	return i;
}
/*****************************************************************************
 * @函数名	void Osus_Time_Base(void *DATA)
 *----------------------------------------------------------------------------
 * @描述	us时基函数，请将该函数在一个较高优先级定时器中断中执行
 *----------------------------------------------------------------------------
 * @输入
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值
 *----------------------------------------------------------------------------
 * @日期		2018.3.1
 *****************************************************************************/
STATIC void us_Time_Base(void *DATA)
{
	static uint16 i = 0;
	if(1 == Init_flag)
	{
		//--------------------------------------------------------------------调度算法1
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
 * @函数名	void ms_Time_Base(void *DATA)
 *----------------------------------------------------------------------------
 * @描述		ms时基函数，请将该函数在一个1ms的定时器中断中执行
 *----------------------------------------------------------------------------
 * @输入
 *----------------------------------------------------------------------------
 * @输出		无
 *----------------------------------------------------------------------------
 * @返回值
 *----------------------------------------------------------------------------
 * @日期		2018.3.1
 *****************************************************************************/
STATIC void ms_Time_Base(void *DATA)
{
	uint16 i = 0;
	//------------------------------------------------------------------------
	if((1 == Init_flag)&&(0 == Schedule_Locked))
	{
		//--------------------------------------------------------------------调度算法1
		#if(1)
		//--------------------------------------------------------------------
		while(i < omsTaskList_length)
		{
			/*当且仅当i小于最大任务数，且已被初始化和调度器没有被锁定才会执行*/
			if(1 == ms_TaskList[i].exist)//-------判断该成员是否存在，如果存在则进一步判断
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
 * @函数名	void ms_Time_Gape(void *DATA)
 *----------------------------------------------------------------------------
 * @描述	ms时隙函数，请将该函数在main函数中执行
 *----------------------------------------------------------------------------
 * @输入
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *****************************************************************************/
STATIC void ms_Time_Gape(void *DATA)
{
	static uint16 i = 0;
	//------------------------------------------------------------------------调度算法1
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
 * @函数名	void Os_Init(void)
 *----------------------------------------------------------------------------
 * @描述	定义框架初始化程序，初始化系统调度列表
 *----------------------------------------------------------------------------
 * @输入
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *****************************************************************************/
STATIC void Os_Init(void)
{
	uint16 i;
	for(i = 0;i < omsTaskList_length;i ++)/*正常优先级任务列表初始化*/
	{
		ms_TaskList[i].Ready = 0;
		ms_TaskList[i].exist = 0;
		ms_TaskList[i].time_count = 1;
		ms_TaskList[i].Task = (usertask)idle;
		ms_TaskList[i].Priority = 0;
		ms_TaskList[i].SystemCount = 0;
		//--------------------------------------------------------------------邮箱列表初始化
		HTOS_MailBox[i].MaxMessageTotal = 0;
		HTOS_MailBox[i].MessageIndex = 0;
		HTOS_MailBox[i].MessageTotal = 0;
		HTOS_MailBox[i].Post = (htos_message*)&Xidle;
		HTOS_MailBox[i].Password = 0;
	}
	for(i = 0;i < ousTaskList_length;i ++)/*高优先级任务列表初始化*/
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
 * @函数名	int8 Schedule_Lock(void* Parameter)
 *----------------------------------------------------------------------------
 * @描述	锁定调度器
 *----------------------------------------------------------------------------
 * @输入	Parameter:保留
 *----------------------------------------------------------------------------
 * @输出	无
 *
 *----------------------------------------------------------------------------
 * @返回值	OS_OK
 *
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *
 *****************************************************************************/
STATIC int8 Schedule_Lock(void* Parameter)
{
	Schedule_Locked = 1;
	return OS_OK;
}
/*****************************************************************************
 * @函数名	int8 Schedule_UnLock(void* Parameter)
 *----------------------------------------------------------------------------
 * @描述	解锁调度器
 *----------------------------------------------------------------------------
 * @输入	Parameter:保留
 *----------------------------------------------------------------------------
 * @输出	无
 *
 *----------------------------------------------------------------------------
 * @返回值	OS_OK
 *
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *
 *****************************************************************************/
STATIC int8 Schedule_UnLock(void* Parameter)
{
	Schedule_Locked = 0;
	return OS_OK;
}
/*****************************************************************************
 * @函数名	uint32 ReadSystemCount(void* Parameter)
 *----------------------------------------------------------------------------
 * @描述	读取当前系统时间戳计数器
 *----------------------------------------------------------------------------
 * @输入	Parameter:保留
 *----------------------------------------------------------------------------
 * @输出	无
 *
 *----------------------------------------------------------------------------
 * @返回值	系统计数值
 *
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *
 *****************************************************************************/
STATIC uint32 ReadSystemCount(void* Parameter)
{
	return ms_SystemCount;
}
/*****************************************************************************
 * @函数名	int16 (*Os_MailBox_Register)(int16 Index,usertask UserTask,htos_message* HTOS_message,uint16 MaxMessageTotal,uint32 Password);
 *----------------------------------------------------------------------------
 * @描述	函数实现为指定任务注册服务邮箱
 *			注意：本函数首先将按索引号注册邮箱，如果索引Index=-1，则按用户名注册。按用户名注册邮箱
 *			时需要遍历任务列表，速度将会慢很多，所以建议用户使用索引号注册邮箱。
 *----------------------------------------------------------------------------
 * @输入	Index：任务的索引号
 *			UserTask：任务的名称
 *			*HTOS_message：指定任务邮箱的位置
 *			MaxMessageTotal：指定邮箱的大小
 *			Password:邮箱的密码，通常在注册邮箱时写入，防止被别的任务盗取邮箱内容，导致邮件丢失
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值	返回-1代表注册失败，可能索引号或者用户名称不对；返回0代表注册成功
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
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
 * @函数名	int16 (*PostSent)(int16 ReciIndex,usertask Recipient,htos_message *Message);
 *----------------------------------------------------------------------------
 * @描述	函数实现邮件发送
 *			注意：本函数首先将按接收者索引号发送邮件，如果索引ReciIndex=-1，则按用户名发送邮件。
 *			按用户名发送邮箱时需要遍历任务列表，速度将会慢很多，所以建议用户使用索引号发送邮件。
 *----------------------------------------------------------------------------
 * @输入	ReciIndex：接收者任务的索引号
 *			Recipient：接收者任务的名称
 *			**Message：要发送的消息
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值	返回-1代表发送失败，可能对方邮箱已经满了或者没有找到该任务；返回值代表对应的邮箱序号已经
 *			接收到邮件。
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
 *
 *****************************************************************************/
STATIC int16 PostSent(int16 ReciIndex,usertask Recipient,htos_message *Message)/*邮件发送函数*/
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
				if(temp < HTOS_MailBox[i].MaxMessageTotal)/*邮箱尚未满*/
				{
					if(HTOS_MailBox[i].MessageTotal < HTOS_MailBox[i].MaxMessageTotal)
					{
						HTOS_MailBox[i].Post[HTOS_MailBox[i].MessageTotal] = *Message;/*填入消息*/
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

			if(temp < HTOS_MailBox[ReciIndex].MaxMessageTotal)/*邮箱尚未满*/
			{
				if(HTOS_MailBox[ReciIndex].MessageTotal < HTOS_MailBox[ReciIndex].MaxMessageTotal)
				{
					HTOS_MailBox[ReciIndex].Post[(HTOS_MailBox[ReciIndex].MessageTotal)] = *Message;/*填入消息*/
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
 * @函数名	static uint16 PostRead(htos_mailbox* MailBox,htos_message* Message,uint32 Password)
 *----------------------------------------------------------------------------
 * @描述	函数实现邮件读取
 *			注意：本函数首先将按接收者索引号发送邮件，如果索引ReciIndex=-1，则按用户名发送邮件。
 *			按用户名发送邮箱时需要遍历任务列表，速度将会慢很多，所以建议用户使用索引号发送邮件。
 *----------------------------------------------------------------------------
 * @输入	Index：当前任务的索引号(任务形参的第一个数)
 *			* Message：读取到的消息
 *			Password:读取密码
 *----------------------------------------------------------------------------
 * @输出	无
 *----------------------------------------------------------------------------
 * @返回值	返回0代表没有收到任何邮件，邮箱为空；返回值1代表读取了1条消息。用户应循环读取消息直到返
 *			回0(全部收取)。
 *			返回-1代表索引号错误，该索引号不在列表中；返回-2代表密码错误，请核对密码或修改密码。
 *----------------------------------------------------------------------------
 * @日期	2018.3.1
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
 *	定义框架程序的结构体
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





