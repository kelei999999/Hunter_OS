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
 * File: Hunter_OS.h
 * Author: Hunter.ORG
 * Created on: 2014年2月16日
 * Revision history: 1.1
 */

/*
 * This is a guard condition so that contents of this file are not included
 * more than once.
 */
/*
 * 20200608已升级为32位计数
 */
/*
 *
 * Hunter_OS是基于时基轮询非抢占式调度操作系统最大支持32768个任务。调度的主体代码
 *	位于Hunter_OS.c中。 每一个任务默认会分配1个邮箱，邮箱的大小由用户定义。如果用户
 *	需要使用到邮箱，则必须进行注册。注册时需要指定邮件的位置和邮箱的大小。
 *
 *	使用注意事项：
 * 	1、一定要将void (*Os_us_Time_Base)(uint32 *DATA)函数注册给一个优先级比
 * 	较高的中断，具体的时间间隔可以根据应用的实际情况分别设置，建议设置成100us(直接
 * 	在100us中断函数中调用该函数即可)。
 *  2、一定要将void (*Os_ms_Time_Base)(uint32 *DATA)函数注册给另一个中断，
 *	优先级要比上一个中断的优先级低，具体的时间间隔可以根据应用的实际情况分别设置，建议
 *	设置成1ms(直接在1ms中断函数中调用该函数即可)。
 * 	3、一定要将void (*Os_ms_Time_Gape)(uint32 *DATA)拷贝到主函数中予以执
 * 	行，否则不会生成时隙。
 * 	4、只有正常优先级任务可以注册邮箱(拥有邮箱使用权)，高优先级任务不可以注册邮箱。
 * 	5、高优先级任务等效于直接在中断函数中执行，使用时务必注意任务的执行时间，切不可以超
 * 	过高优先级中断的间隔时间的二分之一。
 * 	6、正常优先级等效于在主函数中轮询执行，使用时应注意任务的执行时间不能超过正常优先级
 * 	中断函数的间隔时间。原则上执行时间越短越好。
 */
#ifndef HUNTER_OS_H_
#define HUNTER_OS_H_
//----------------------------------------------以下为系统头文件
#include "Typedef.h"
//----------------------------------------------
/*
 *	兼容模式禁止，当Compatible==1时，使能兼容模式。系统调度函数均声明为全局函数，用户在使用函数时需要注意
 *	函数名称不能与系统内部名称相同。当关闭兼容模式后，所由系统内部函数均为局部函数，不允许外部调用。
 */
#define	Compatible						0
#if (Compatible == 0)
	#define STATIC						static
#else
	#define STATIC
#endif
/*
 *	注意：程序默认正常优先级的任务均为十个，高优先级任务为五个，如果用户有更多需求请修改
 *	下面的宏定义的值。
 */
#define msTaskList_length				10		/*定义ms级(正常优先级)任务最大任务数*/
#define usTaskList_length				3		/*定义us级(高优先级)任务最大任务数*/
/*
 *	定义一个用户任务的标准函数类型指针
 * Parameter:默认为本任务所在列表的索引位置
 */
typedef void(*usertask)(void* Parameter);
/*
 *	定义返回类名
 */
#ifdef OS_OK
	#undef OS_OK
	#define OS_OK								0
#else
	#define OS_OK								0
#endif
#ifdef OS_ERROR
	#undef OS_ERROR
	#define OS_ERROR							-1
#else
	#define OS_ERROR							-1
#endif
/*
 * ***************************************************************************定义任务结构体类型
 */
typedef struct
{
	uint8 Ready;			/*任务执行标签，为1时，代表将要执行(任务已就绪)，为0时，代表已经执行完*/
	uint8 exist;			/*任务存在标签，为1时，代表该任务存在（有用户注册了）*/
	uint8 Priority;			/*任务的优先级*/
	uint8 reserve;			/*保留地址*/
	uint32 time_count;		/*该任务的执行周期，单位为脉冲数*/
	uint32 SystemCount;		/*系统计数器副本*/
	usertask Task;			/*任务函数指针，指向被注册的用户函数*/
}m_task;
/*
 * ***************************************************************************定义邮件消息结构体
 */
typedef struct
{
	usertask SrcTaskname;				/*发送者身份*/
	uint16 SystemCount;					/*发送时的时间戳*/
	void *Message;						/*指向消息内容的指针*/
}htos_message;
/*
 * ***************************************************************************定义邮箱结构体
 */
typedef struct
{
	int16 MaxMessageTotal;				/*邮箱允许的最大消息总量*/
	int16 MessageTotal;					/*当前邮箱内有效消息的数量*/
	int16 MessageIndex;					/*最后已读消息索引位置*/
	htos_message *Post;					/*邮件*/
	uint32 Password;					/*邮箱密码，初始密码为0*/
}htos_mailbox;
/*
 * ***************************************************************************定义框架函数结构体
 */
//****************************************************************************
// @函数名	static void Os_Init(void)
//----------------------------------------------------------------------------
// @描述		函数实现了OS初始化
//----------------------------------------------------------------------------
// @输入		无
//----------------------------------------------------------------------------
// @输出		无
//----------------------------------------------------------------------------
// @返回值	无
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 (*Os_Time_Gape_TaskDelete)(int16 Index,void(*UserTask)(uint32* Parameter));
//----------------------------------------------------------------------------
// @描述		函数实现了用户任务删除
//			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
//----------------------------------------------------------------------------
// @输入		Index：被删除任务所在任务列表的索引。当Index ≠ -1时，任务删除器将会按照索引号进行删除，
//					忽略用户名。
//					当Index ＝ -1时，任务删除器将会遍历任务列表查找与UserTask相同的名称进行删除。
//					通常来说，按索引删除的速度远远大于按任务名称删除，所以建议用户使用索引删除。
//			UserTask:被删除任务的函数名
//----------------------------------------------------------------------------
// @输出		无
//----------------------------------------------------------------------------
// @返回值	int8：返回-1代表删除失败，没有找到对应的任务；按索引删除时如果索引号大于任务列表时会返
//				回删除失败，其他情况均会删除成功。删除成功后返回被删除任务在任务列表中的索引位置。
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 Time_Gape_RegisterV1(uint16 ms,void(*UserTask)(uint32* Parameter),uint8 Priority)
//----------------------------------------------------------------------------
// @描述		函数实现了用户任务时间间隔函数注册
//			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
//----------------------------------------------------------------------------
// @输入	ms：指用户希望指定的函数周期调用的的周期，单位为ms
//			UserTask:用户指定的函数名
//			Priority:任务的优先级，值越大，优先级越高
//----------------------------------------------------------------------------
// @输出		无
//----------------------------------------------------------------------------
// @返回值	int8：返回-1代表注册失败，注册成功后返回所在任务列表的索引位置
//
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 Time_Gape_Register(uint16 ms,void(*UserTask)(uint32* Parameter))
//----------------------------------------------------------------------------
// @描述	函数实现了用户任务时间间隔函数注册
//			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
//----------------------------------------------------------------------------
// @输入	ms：指用户希望指定的函数周期调用的的周期，单位为ms
//			UserTask:用户指定的函数名
//----------------------------------------------------------------------------
// @输出		无
//
//----------------------------------------------------------------------------
// @返回值	int8：返回-1代表注册失败，注册成功后返回所在任务列表的索引位置
//
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 Time_Gape_RegisterHigh(uint16 PLUS,void(*UserTask)(uint32* Parameter))
//----------------------------------------------------------------------------
// @描述		函数实现了用户任务时间间隔函数注册
//			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的100us数周期的调用。
//----------------------------------------------------------------------------
// @输入	PLUS：保留
//			UserTask:用户指定的函数名
//----------------------------------------------------------------------------
// @输出	无
//
//----------------------------------------------------------------------------
// @返回值	int8：返回-1代表注册失败，注册成功后返回所在任务列表的索引位置
//
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 (*Os_Schedule_Lock)(void* Parameter);
//----------------------------------------------------------------------------
// @描述		函数实现调度器锁定
//----------------------------------------------------------------------------
// @输入		Parameter：保留
//----------------------------------------------------------------------------
// @输出	无
//----------------------------------------------------------------------------
// @返回值	int8：返回-1代表锁定失败，返回0，锁定成功
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 (*Os_Schedule_UnLock)(void* Parameter);
//----------------------------------------------------------------------------
// @描述		函数实现调度器解锁
//----------------------------------------------------------------------------
// @输入		Parameter：保留
//----------------------------------------------------------------------------
// @输出	无
//----------------------------------------------------------------------------
// @返回值	int8：返回-1代表解锁失败，返回0，解锁成功
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	uint16 (*Os_ReadSystemCount)(void* Parameter);
//----------------------------------------------------------------------------
// @描述		函数实现读取系统时间戳计数器
//----------------------------------------------------------------------------
// @输入		Parameter：保留
//----------------------------------------------------------------------------
// @输出		无
//----------------------------------------------------------------------------
// @返回值	系统时间戳计数器的值
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 (*Os_MailBox_Register)(int16 Index,usertask UserTask,htos_message* HTOS_message,uint16 MaxMessageTotal,uint32 Password);
//----------------------------------------------------------------------------
// @描述		函数实现为指定任务注册服务邮箱
//			注意：本函数首先将按索引号注册邮箱，如果索引Index=-1，则按用户名注册。按用户名注册邮箱
//			时需要遍历任务列表，速度将会慢很多，所以建议用户使用索引号注册邮箱。
//----------------------------------------------------------------------------
// @输入		Index：任务的索引号
//			UserTask：任务的名称
//			*HTOS_message：指定任务邮箱的位置
//			MaxMessageTotal：指定邮箱的大小
//			Password:邮箱的密码，通常在注册邮箱时写入，防止被别的任务盗取邮箱内容，导致邮件丢失
//----------------------------------------------------------------------------
// @输出		无
//----------------------------------------------------------------------------
// @返回值	返回-1代表注册失败，可能之前已经注册过了；返回0代表注册成功
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	int8 (*PostSent)(int16 ReciIndex,usertask Recipient,htos_message *Message);
//----------------------------------------------------------------------------
// @描述		函数实现邮件发送
//			注意：本函数首先将按接收者索引号发送邮件，如果索引ReciIndex=-1，则按用户名发送邮件。
//			按用户名发送邮箱时需要遍历任务列表，速度将会慢很多，所以建议用户使用索引号发送邮件。
//----------------------------------------------------------------------------
// @输入		ReciIndex：接收者任务的索引号
//			Recipient：接收者任务的名称
//			**Message：要发送的消息
//----------------------------------------------------------------------------
// @输出		无
//----------------------------------------------------------------------------
// @返回值	返回-1代表发送失败，可能对方邮箱已经满了或者没有找到该任务；返回值代表对应的邮箱序号已经
//			接收到邮件。
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
// @函数名	static uint16 PostRead(htos_mailbox* MailBox,htos_message* Message,uint32 Password)
//----------------------------------------------------------------------------
// @描述		函数实现邮件读取
//			注意：本函数首先将按接收者索引号发送邮件，如果索引ReciIndex=-1，则按用户名发送邮件。
//			按用户名发送邮箱时需要遍历任务列表，速度将会慢很多，所以建议用户使用索引号发送邮件。
//----------------------------------------------------------------------------
// @输入		Index：当前任务的索引号(任务形参的第一个数)
//			* Message：读取到的消息
//			Password：读取密码
//----------------------------------------------------------------------------
// @输出		无
//----------------------------------------------------------------------------
// @返回值	返回0代表没有收到任何邮件，邮箱为空；返回值1代表读取了1条消息。用户应循环读取消息直到返
//			回0(全部收取)。
//			返回-1代表索引号错误，该索引号不在列表中；返回-2代表密码错误，请核对密码或修改密码。
//----------------------------------------------------------------------------
// @日期		2018.3.1
//
//****************************************************************************
typedef struct
{
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
	void (*Os_Init)(void);
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
	int16 (*Os_Time_Gape_TaskHighDelete)(int16 id,usertask UserTask);
	/*****************************************************************************
	 * @函数名	int16 (*Os_Time_Gape_TaskDelete)(int16 id,void(*UserTask)(uint32* Parameter));
	 *----------------------------------------------------------------------------
	 * @描述	函数实现了用户任务删除
	 *			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的ms数周期的调用。
	 *----------------------------------------------------------------------------
	 * @输入	id：被删除任务所在任务列表的索引。当Index ≠ -1时，任务删除器将会按照ID号进行删除.
	 *					当id ＝ -1时，任务删除器将会遍历任务列表查找与UserTask相同的名称进行删除。
	 *					通常来说，按ID删除的速度远远大于按任务名称删除，所以建议用户使用ID删除。
	 *			UserTask:被删除任务的函数名
	 *----------------------------------------------------------------------------
	 * @输出	无
	 *----------------------------------------------------------------------------
	 * @返回值	int16：返回-1代表删除失败，没有找到对应的任务；按ID删除时如果索引号大于任务列表时会返
	 *			回删除失败，其他情况均会删除成功。删除成功后返回被删除任务在任务列表中的索引位置。
	 *----------------------------------------------------------------------------
	 * @日期	2018.3.1
	 *
	 *****************************************************************************/
	int16 (*Os_Time_Gape_TaskDelete)(int16 id,usertask UserTask);
	/*****************************************************************************
	 * @函数名	int16 Os_Time_Gape_RegisterV1(uint32 ms,usertask UserTask,uint8 Priority)
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
	 * @返回值	int16：返回-1代表注册失败，返回在列表的索引位置值代表成功
	 *
	 *----------------------------------------------------------------------------
	 * @日期	2018.3.1
	 *
	 *****************************************************************************/
	int16 (*Os_Time_Gape_RegisterV1)(uint32 ms,usertask UserTask,uint8 Priority);
	/*****************************************************************************
	 * @函数名	int16 Os_Time_Gape_Register(uint32 ms,usertask UserTask)
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
	 * @返回值	int16：返回-1代表注册失败，返回在列表的索引位置值代表成功
	 *
	 *----------------------------------------------------------------------------
	 * @日期	2018.3.1
	 *
	 *****************************************************************************/
	int16 (*Os_Time_Gape_Register)(uint32 ms,usertask UserTask);/*该程序已经升级不建议再使用*/
	/*****************************************************************************
	 * @函数名	int16 Os_Time_Gape_RegisterHigh(uint32 PLUS,usertask UserTask,uint8 Priority)
	 *----------------------------------------------------------------------------
	 * @描述	函数实现了高优先级用户任务时间间隔函数注册
	 *			本初始化函数实现的目标是，通过调用本函数，将用户指定的函数按用户指定的100us数周期的调用。
	 *----------------------------------------------------------------------------
	 * @输入	PLUS：保留
	 *			UserTask:用户指定的函数名
	 *			Priority:任务的优先级
	 *----------------------------------------------------------------------------
	 * @输出	无
	 *
	 *----------------------------------------------------------------------------
	 * @返回值	int16：返回-1代表注册失败，返回在列表的值位置代表成功
	 *
	 *----------------------------------------------------------------------------
	 * @日期	2018.3.1
	 *
	 *****************************************************************************/
	int16 (*Os_Time_Gape_RegisterHigh)(uint32 PLUS,usertask UserTask,uint8 Priority);
	/*****************************************************************************
	 * @函数名	int8 Os_Schedule_Lock(void* Parameter)
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
	int8 (*Os_Schedule_Lock)(void* Parameter);
	/*****************************************************************************
	 * @函数名	int8 Os_Schedule_UnLock(void* Parameter)
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
	int8 (*Os_Schedule_UnLock)(void* Parameter);
	/*****************************************************************************
	 * @函数名	uint32 Os_ReadSystemCount(void* Parameter)
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
	uint32 (*Os_ReadSystemCount)(void* Parameter);
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
	int16 (*Os_MailBox_Register)(int16 Index,usertask UserTaskName,htos_message* PostStoreAdress,uint16 MaxPostCount,uint32 Password);
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
	int16 (*Os_MailBox_PostSent)(int16 ReciIndex,usertask Recipient,htos_message *Message);
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
	int16 (*Os_MailBox_PostRead)(int16 Index,htos_message* Message,uint32 Password);
}htos_function;
extern volatile htos_function HTOS_Function;
/*
 * ***************************************************************************定义框架函数结构体
 */
typedef struct
{
	/*************************************************************************
	 * @函数名		void Os_us_Time_Base(void *DATA)
	 *------------------------------------------------------------------------
	 * @描述		高优先级任务调度函数，请拷贝到高优先级中断中运行，例如100uS定时器中断服务函数中
	 *------------------------------------------------------------------------
	 * @输入		*DATA:用户参数
	 *------------------------------------------------------------------------
	 * @输出		无
	 *------------------------------------------------------------------------
	 * @返回值
	 *------------------------------------------------------------------------
	 * @日期		2018.3.1
	 ************************************************************************/
	void (*Os_us_Time_Base)(void *DATA);
	/************************************************************************
	 * @函数名		void ms_Time_Base(void *DATA)
	 *------------------------------------------------------------------------
	 * @描述		低优先级任务调度函数，请拷贝到低优先级中断中运行，例如1mS定时器中断服务程序中
	 *------------------------------------------------------------------------
	 * @输入
	 *------------------------------------------------------------------------
	 * @输出		无
	 *------------------------------------------------------------------------
	 * @返回值
	 *------------------------------------------------------------------------
	 * @日期		2018.3.1
	 * ***********************************************************************/
	void (*Os_ms_Time_Base)(void *DATA);
	/*************************************************************************
	 * @函数名	void ms_Time_Gape(void *DATA)
	 *------------------------------------------------------------------------
	 * @描述		普通任务执行执行函数，请拷贝到主函数中运行
	 *------------------------------------------------------------------------
	 * @输入
	 *------------------------------------------------------------------------
	 * @输出		无
	 *------------------------------------------------------------------------
	 * @返回值
	 *------------------------------------------------------------------------
	 * @日期		2018.3.1
	 * **********************************************************************/
	void (*Os_ms_Time_Gape)(void *DATA);
}htos_schedule;
extern volatile htos_schedule HTOS_Schedule;
/*
 * ***************************************************************************定义Os的相关参数
 */
extern volatile m_task ms_TaskList[msTaskList_length];
extern volatile m_task us_TaskList[usTaskList_length];
extern volatile int16 ms_ExecuteList[msTaskList_length];
extern volatile int16 us_ExecuteList[usTaskList_length];
extern volatile htos_mailbox HTOS_MailBox[msTaskList_length];
#endif /* HUNTER_OS_H_ */

