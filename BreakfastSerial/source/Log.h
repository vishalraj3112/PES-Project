/***********************************************************************************************************************
* File Name    : log.h
* Project      : PES Assignment 6
* Version      : 1.0
* Description  : Used to differentiate debug and run mode implementations
* Author       : Referred from PES Lecture 10 slides by Howdy Pierce
* Creation Date: 11.10.21
***********************************************************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#ifdef DEBUG
	#define LOG printf
#else
	#define LOG(...)
#endif

#endif /* LOG_H_ */

/*[EOF]*/
