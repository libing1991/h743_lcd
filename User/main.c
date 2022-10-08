/********************************************************************************
  * 文 件 名: main.c
  * 版 本 号: 初版
  * 修改作者: LC
  * 修改日期: 2022年04月12日
  * 功能介绍:          
  ******************************************************************************
  * 注意事项:
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
        lv_task_handler();//lvgl的事务处理
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
函数名称 ： main
功    能 ： 主函数
参    数 ： 无
返 回 值 ： 无
作    者 ： LC
*************************************************/
int main(void)
{

    initcpu();

    lv_init();											//lvgl系统初始化
    lv_port_disp_init();						//lvgl显示接口初始化,放在lv_init()的后面
    lv_port_indev_init();						//lvgl输入接口初始化,放在lv_init()的后面


    gui_app_start();							//运行例程
    os_sys_init(start_task); 
    while(1) {
		
    }
}

void tick_hook(void)
{
    lv_tick_inc(1);//lvgl的1ms心跳
}