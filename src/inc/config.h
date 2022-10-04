#ifndef CONFIG_INC
#define CONFIG_INC

	#include <includes.h>



	#define OSSemPend		os_sem_wait
	#define OSSemPost		os_sem_send
	#define IsrSemPost		isr_sem_wait
	
	#define OSTimeDly		os_dly_wait
	#define OSSemClear		os_evt_clr
	
#endif
