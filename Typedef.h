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
 * File: Typedef.h
 * Author: Kelei999999(WangLiang)
 * Created on: 2016��2��16��
 *
 * -Modified for F28069 DSP data types for int16 and int32 Joe Zhou 01-03-2018
 *
 * Revision history: 1.1
 */

/*
 * This is a guard condition so that contents of this file are not included
 * more than once.
 */


#ifndef TYPEDEF_H_
#define TYPEDEF_H_

#define uint8_T				unsigned char
#define int8_T				char
#define uint16_T			unsigned short
#define int16_T				short
#define uint32_T			unsigned long
#define int32_T				int
#define uint64_T			unsigned long long
#define int64_T				long long
#define real32_T			float
#define real64_T			double

typedef unsigned char		uint8;		/*�޷���8λ*/
typedef char				int8;		/*�з���8λ*/
typedef unsigned short		uint16;		/*�޷���16λ*/
typedef short				int16;		//
typedef unsigned int		uint32;		/*�޷���32λ*/
typedef int					int32;		//
typedef unsigned long long	uint64;		/*�޷���64λ*/
typedef long long			int64;		/*�ַ���64λ*/
typedef float				real32;		/*�����ȸ�����*/
typedef	double				real64;		/*˫���ȸ�����*/
//----------------------------------------------�����������Ͷ���
typedef struct
{
	real32 alpha;
	real32 beta;
}complex;								/*����*/
#endif /* TYPEDEF_H_ */
