#ifndef __LOCK_H
#define __LOCK_H	 
#include "System.h"

#define LOCK    PCout(6)   // Pc6
#define LOCK_s1 PCin(7)    // Pc7	
#define LOCK_s2 PCin(8)    // Pc8	
#define LOCK_s3 PCin(9)    // Pc9

//#define OPEN    1
//#define CLOSE   0    
//#define BAD     2    


void LOCK_Init(void);//≥ı ºªØ
void light_color(int color);			//µ∆π‚
		 				    
#endif
