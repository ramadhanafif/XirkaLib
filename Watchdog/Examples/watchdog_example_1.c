/**
 *	Example for watchdog.h
 *	Created 10-Jan-2020
 */

#include "CMSDK_CM3.h"
#include "uart_stdout.h"
#include <stdio.h>


#include "watchdog.h"

void busy_program(){
	volatile int x = 0;
	for (int i = 0 ; i< 10000; i++)
		if (x == 0)
			x = 1;
		else if (x == 1)
			x = 0;
}

void very_busy_program(){
	volatile int x = 0;
	for (int i = 0 ; i< 1000000; i++)
		if (x == 0)
			x = 1;
		else if (x == 1)
			x = 0;
}

/*Desired watchdog period in millisecond*/
#define MAX_LOOP_CYCLE 100

int main(){
	UartStdOutInit();
	
	/*Check for board previous cause for reset */
	
	/*Board reseted by watchdog*/
	if (prev_reset_source()== CMSDK_SYSCON_RSTINFO_WDOGRESETREQ_Msk)
	{
		puts("BOARD RESETED BY WATCHDOG");
		puts("DEMO ENDS");
	}
	/*Board started normally*/
	else 
	{
		puts("BOARD STARTED NORMALLY");
		
		/*Convert time from millisecond to number of watchdog cycle*/
		/*SystemCoreClock is defined at system_CMSDK_CM3.h*/
		unsigned int cycle = to_wdCycle(MAX_LOOP_CYCLE,SystemCoreClock);

		/*Initilize and start watchdog*/
		watchdog_init(cycle);
		
		/*Example of good program, able to reset watchdog timer on time*/
		puts(">>> Watchdog cycle reset demo");
		for(int i = 0; i< 5; i++)
		{
			busy_program();
			watchdog_set(cycle);
			puts("Watchdog cycle reset");
		}

		/*Bad program, failed to reset watchdog timer on time*/
		watchdog_set(cycle);	
		puts("\n\n>>> Watchdog trigered demo");
		for(int i = 0; i< 5; i++)
		{
			very_busy_program();
				
			//Should never reach here
			watchdog_set(cycle);
			puts("Watchdog cycle reset");
		}
	}
}
	
