#include "stdio.h"
#include "string.h"
#include "time.h"

//include FreeRtos header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// include my header
#include "processing_TIME.h"
#include "processing_mem_map_extern.h"


uint16_t TIME_calc_address_oper_reg(S_TIME_address *ps_TIME_address, uint16_t adres_start){

	return 0;
}

/*
 * tm_sec	int	seconds after the minute	0-61*
   tm_min	int	minutes after the hour	    0-59
   tm_hour	int	hours since midnight	    0-23
   tm_mday	int	day of the month	        1-31
   tm_mon	int	months since January	    0-11
   tm_year	int	years since 1900
   tm_wday	int	days since Sunday	        0-6
   tm_yday	int	days since January 1	    0-365
   tm_isdst	int	Daylight Saving Time flag
 *
 *
 * */
struct tm timeUpdate;
void t_processing_TIME(void *pvParameters){


	timeUpdate.tm_sec = 0;
	timeUpdate.tm_min = 0;
	timeUpdate.tm_hour = 0;
	timeUpdate.tm_mday = 0;
	timeUpdate.tm_mon = 0;
	timeUpdate.tm_year = 0;
	timeUpdate.tm_wday = 0;
	timeUpdate.tm_yday = 0;

	//time_t rez_mktime, secTime;
	//rez_mktime = time(&secTime);
	//str = ctime(&secTime);

	while(1){

	}
}


