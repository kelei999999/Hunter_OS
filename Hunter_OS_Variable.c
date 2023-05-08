/* Kelei999999(WangLiang) all rights reserved.  You may use this software
 * and any derivatives exclusively with Kelei999999(WangLiang) products.
 *
 * THIS SOFTWARE IS SUPPLIED BY Kelei999999(WangLiang) "AS IS".  NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH Kelei999999(WangLiang) PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL Kelei999999(WangLiang) BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF Kelei999999(WangLiang) HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE
 * FULLEST EXTENT ALLOWED BY LAW, Kelei999999(WangLiang)'S TOTAL LIABILITY ON ALL CLAIMS
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF
 * ANY, THAT YOU HAVE PAID DIRECTLY TO Kelei999999(WangLiang) FOR THIS SOFTWARE.
 *
 * Kelei999999(WangLiang) PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

/*
 * File: Hunter_OS_Variable.c
 * Author: Kelei999999(WangLiang)
 * Created on: 2016年2月16日
 * Revision history: 1.1
 */

/*
 * This is a guard condition so that contents of this file are not included
 * more than once.
 */
#include "Hunter_OS.h"
/*
 * ***************************************************************************定义时序列表
 */
volatile m_task ms_TaskList[msTaskList_length] = {0};						/*定义一个数组，用于正常优先级任务列表*/

volatile m_task us_TaskList[usTaskList_length] = {0};						/*定义一个数组，用于高优先级任务列表*/

volatile htos_mailbox HTOS_MailBox[msTaskList_length];						/*定义与任务数相同的邮箱数组，每一个任务的下标和邮箱下标一一对应*/

volatile uint16 omsTaskList_length = msTaskList_length;						/*定义一个变量，用于指示正常优先级任务数量*/

volatile uint16 ousTaskList_length = usTaskList_length;						/*定义一个变量，用于指示高优先级任务数量*/

