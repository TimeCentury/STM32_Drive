#include "ITM.h"

struct __FILE 
{
	int handle; 
}; 

FILE __stdout;

int fputc(int ch, FILE *f)
{
	if (DEMCR & TRCENA) {
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;
	}
	
	return(ch);
}
