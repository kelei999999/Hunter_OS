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
 * File: Hunter_OS_Variable.c
 * Author: Hunter.ORG
 * Created on: 2016��2��16��
 * Revision history: 1.1
 */

/*
 * This is a guard condition so that contents of this file are not included
 * more than once.
 */
#include "Hunter_OS.h"
/*
 * ***************************************************************************����ʱ���б�
 */
volatile m_task ms_TaskList[msTaskList_length] = {0};						/*����һ�����飬�����������ȼ������б�*/

volatile m_task us_TaskList[usTaskList_length] = {0};						/*����һ�����飬���ڸ����ȼ������б�*/

volatile htos_mailbox HTOS_MailBox[msTaskList_length];						/*��������������ͬ���������飬ÿһ��������±�������±�һһ��Ӧ*/

volatile uint16 omsTaskList_length = msTaskList_length;						/*����һ������������ָʾ�������ȼ���������*/

volatile uint16 ousTaskList_length = usTaskList_length;						/*����һ������������ָʾ�����ȼ���������*/

