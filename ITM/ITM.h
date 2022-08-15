/** 
 * @Author: 时间世纪
 * @Date: 2022-08-14 20:46:02
 * @Description: 
 */
#ifndef _ITM_H_
#define _ITM_H_

#include "stm32f4xx.h"
#include <stdio.h>

#define ITM_Port8(n)	(*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)	(*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)	(*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR			(*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA			0x01000000


#define DEBUG

#ifdef DEBUG
	// #define INFO_LEVEL 3 //打印平常信息
	// #define WARNING_LEVEL 2//打印警告信息
	// #define ERROR_LEVEL 1//打印错误信息
	// #define SILENT_LEVEL 0//不打印信息

	// 打印信息的等级,0:不打印，1:打印错误信息，2:打印警告信息，3:打印平常信息，4:打印调试信息
	#define LOG_LEVEL 4
	#if LOG_LEVEL >= 4
		#define LOG_DEBUG(format, ...) printf("DEBUG:"format, __VA_ARGS__)
	#else
		#define LOG_DEBUG(format, ...)
	#endif
    #if LOG_LEVEL >= 3
		#define LOG_INFO(format, ...) printf("INFO:"format"\r\n", __VA_ARGS__)
    #else
        #define LOG_INFO(format, ...)
    #endif
    #if LOG_LEVEL >= 2
		#define LOG_WARNING(format, ...) printf("WARNING: %s, %s, %d: "##format"\r\n", __FILE__, __func__, __LINE__, __VA_ARGS__)
    #else
        #define LOG_WARNING(format, ...)
    #endif
    #if LOG_LEVEL >= 1
        #define LOG_ERROR(format, ...) printf("ERROR: %s, %s, %d: "##format"\r\n", __FILE__, __func__, __LINE__, __VA_ARGS__)
    #else
        #define LOG_ERROR(format, ...)
    #endif
#else
	#define LOG(fmt, ...)
	#define LOG_MSG(fmt, ...)
	#define LOG_ERR(fmt, ...)
#endif


#endif
