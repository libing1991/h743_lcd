/********************************************************************************
  * �� �� ��: main.c
  * �� �� ��: ����
  * �޸�����: LC
  * �޸�����: 2022��04��12��
  * ���ܽ���:          
  ******************************************************************************
  * ע������:
*********************************************************************************/

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "bsp_led.h"
#include "sys.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "includes.h"
#include "rtl.h"

#define TASK_SIZE_MAIN       	512             
#define TASK_SIZE_MUART	   		400				
#define TASK_SIZE_GUI	   		500		

#define PRIO_GUI               2
#define PRIO_TASK_UART           3
#define PRIO_MUART              5
#define PRIO_TIMER              11
#define PRIO_MAIN               10

__align(8) static INT32U p_TimerStk[128];
__align(8) static INT32U p_GstkUart[TASK_SIZE_MUART];
__align(8) static INT32U p_GstkMain[TASK_SIZE_MAIN];

__align(8) static INT32U p_GstkGui[TASK_SIZE_GUI];

static __task void TaskGui(void)
{
    delay_ms(500);
    
    while(1)
    {
        lv_task_handler();//lvgl��������
        delay_ms(5);
    }
}

static __task void TaskMain(void)
{	
	
	while(1)
	{
		delay_ms(5);	
	}
}
extern BOOLEAN PendRs232Event(int ms);

static __task void mUartTask(void)
{
	delay_ms(500);
    
	
	while(1)
	{
		delay_ms(5);
	}

}


static __task void timerTASK(void)
{

	while(1)
	{
		delay_ms(5);
	}
}

static __task void start_task(void)
{
    INT8U data[1];
    os_tsk_create_user(TaskMain    , PRIO_MAIN         , &p_GstkMain     , sizeof(p_GstkMain));	
    os_tsk_create_user(timerTASK    , PRIO_TIMER        , &p_TimerStk        , sizeof(p_TimerStk));	  
    os_tsk_create_user(mUartTask      , PRIO_TASK_UART     , &p_GstkUart        , sizeof(p_GstkUart));	    
	
    os_tsk_create_user(TaskGui    , PRIO_GUI         , &p_GstkGui     , sizeof(p_GstkGui));	
    
    while(1)
    {
        delay_ms(500);
        data[0] = 0x01;
        WriteLedPortData(data);
        
        delay_ms(500);
        data[0] = 0x00;
        WriteLedPortData(data);
    }
}

/************************************************
�������� �� main
��    �� �� ������
��    �� �� ��
�� �� ֵ �� ��
��    �� �� LC
*************************************************/
int main(void)
{

    initcpu();

    lv_init();											//lvglϵͳ��ʼ��
    lv_port_disp_init();						//lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
    lv_port_indev_init();						//lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���


    gui_app_start();							//��������
    os_sys_init(start_task); 
    while(1) {
		
    }
}

void tick_hook(void)
{
    lv_tick_inc(1);//lvgl��1ms����
}