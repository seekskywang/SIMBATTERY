//==========================================================
//�ļ����ƣ�TypeDefine.h
//�ļ��汾��Ver1.0
//�������ڣ�2014.09.24
//�޸����ڣ�2014.09.24 11:35
//�ļ����ߣ��ƺ���
//�ļ������������ض����ļ�
//��ע˵�����ʺ�8λ������
//ע�������
//==========================================================
#ifndef __TypeDefine_h__
#define __TypeDefine_h__

//==========================================================
//16bit���ݵ�λ��������
#define	SetD0	(0x0001)
#define	SetD1	(0x0002)
#define	SetD2	(0x0004)
#define	SetD3	(0x0008)
#define	SetD4	(0x0010)
#define	SetD5	(0x0020)
#define	SetD6	(0x0040)
#define	SetD7	(0x0080)
#define	SetD8	(0x0100)
#define	SetD9	(0x0200)
#define	SetD10	(0x0400)
#define	SetD11	(0x0800)
#define	SetD12	(0x1000)
#define	SetD13	(0x2000)
#define	SetD14	(0x4000)
#define	SetD15	(0x8000)

#define	ClrD0	(0xfffe)
#define	ClrD1	(0xfffd)
#define	ClrD2	(0xfffb)
#define	ClrD3	(0xfff7)
#define	ClrD4	(0xffef)
#define	ClrD5	(0xffdf)
#define	ClrD6	(0xffbf)
#define	ClrD7	(0xff7f)
#define	ClrD8	(0xfeff)
#define	ClrD9	(0xfdff)
#define	ClrD10	(0xfbff)
#define	ClrD11	(0xf7ff)
#define	ClrD12	(0xefff)
#define	ClrD13	(0xdfff)
#define	ClrD14	(0xbfff)
#define	ClrD15	(0x7fff)

//==========================================================
#ifndef NULL
	#define NULL ((void *)0)
#endif
#define Null NULL
#define null NULL

//Keil C51��ֲ
//#define const code

//==========================================================
//�����ض���
//typedef float f32;
//typedef float const fc32;  /* Read Only */
//typedef volatile float  vf32;
//typedef volatile float  const vfc32;  /* Read Only */
//
//typedef signed long  s32;
//typedef signed short s16;
//typedef signed char  s8;
//
//typedef signed long  const sc32;  /* Read Only */
//typedef signed short const sc16;  /* Read Only */
//typedef signed char  const sc8;   /* Read Only */
//
//typedef volatile signed long  vs32;
//typedef volatile signed int   vs16;
//typedef volatile signed char  vs8;
//
//typedef volatile signed long  const vsc32;  /* Read Only */
//typedef volatile signed int   const vsc16;  /* Read Only */
//typedef volatile signed char  const vsc8;   /* Read Only */
//
//typedef unsigned long  u32;
//typedef unsigned short u16;
//typedef unsigned char  u8;
//
//typedef unsigned long  const uc32;  /* Read Only */
//typedef unsigned short const uc16;  /* Read Only */
//typedef unsigned char  const uc8;   /* Read Only */
//
//typedef volatile unsigned long  uint32_t;
//typedef volatile unsigned short uint16_t;
//typedef volatile unsigned char  uint8_t;
//
//typedef volatile unsigned long  const vuc32;  /* Read Only */
//typedef volatile unsigned short const vuc16;  /* Read Only */
//typedef volatile unsigned char  const vuc8;   /* Read Only */
//
//typedef enum {FALSE = 0, TRUE = !FALSE} bool;
//#define Bool bool
//#define False FALSE
//#define false FALSE
//#define True TRUE
//#define true TRUE
//
//typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
//
//typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
//#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
//
//typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
//
////==========================================================
////��ֵ��Χ����
//#define U8_MAX     ((u8)255)
//#define S8_MAX     ((s8)127)
//#define S8_MIN     ((s8)-128)
//#define U16_MAX    ((u16)65535u)
//#define S16_MAX    ((s16)32767)
//#define S16_MIN    ((s16)-32768)
//#define U32_MAX    ((u32)4294967295uL)
//#define S32_MAX    ((s32)2147483647)
//#define S32_MIN    ((s32)-2147483648)

//==========================================================

#endif
