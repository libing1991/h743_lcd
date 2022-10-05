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

#define SDRAMADDRESS 0xC0000000



static INT32U p_errReadData = 0;

extern void lcd_clear(uint16_t color);

static void sdramTest(void)
{
    INT32U index;
    INT32U data;
    INT32U *datap;
    
    datap = &data;
    
    for(index = 0 ; index < (32 * 1024 * 1024 / 4); index += 4)
    {
        *(INT32U *)(SDRAMADDRESS + index) = index;
    }
    
    for(index = 0 ; index < (32 * 1024 * 1024 / 4); index+= 4)
    {
        *datap = *(INT32U *)(SDRAMADDRESS + index);
        
        if(*datap != index)
        {
           p_errReadData ++; 
        }
    }
    
    if(p_errReadData > 0)
    {
        while(1)
        {
           p_errReadData ++; 
        }
    }
    
    p_errReadData = p_errReadData * 1;
}

#define LCD_COLOR_WHITE              0xFFFF
#define LCD_COLOR_BLACK              0x0000
#define LCD_COLOR_GREY               0xF7DE
#define LCD_COLOR_BLUE               0x001F
#define LCD_COLOR_BLUE2              0x051F
#define LCD_COLOR_RED                0xF800
#define LCD_COLOR_MAGENTA            0xF81F
#define LCD_COLOR_GREEN              0x07E0
#define LCD_COLOR_CYAN               0x7FFF
#define LCD_COLOR_YELLOW             0xFFE0
static INT32U p_colors[] = 
{
    LCD_COLOR_WHITE  , 
    LCD_COLOR_BLACK   ,
    LCD_COLOR_GREY    ,
    LCD_COLOR_BLUE    ,
    LCD_COLOR_BLUE2   ,
    LCD_COLOR_RED     ,
    LCD_COLOR_MAGENTA ,
    LCD_COLOR_GREEN   ,
    LCD_COLOR_CYAN    ,
    LCD_COLOR_YELLOW    
};

static INT32U getLcdColor(void)
{ 
    static INT16U s_index = 0;
    
    if(s_index >= 10)
    {
        s_index = 0;
    }
    
    return p_colors[s_index++];
}

static __task void start_task(void)
{
    INT8U data[1];
    initcpu();
    
    while(1)
    {
        delay_ms(500);
        data[0] = 0x01;
        WriteLedPortData(data);
        
        delay_ms(500);
        data[0] = 0x00;
        WriteLedPortData(data);
        
        lcd_clear(getLcdColor());
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

	  uint16_t uicount = 0;
	  float fcount = 0.0;
	
	  //nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);  // ���ȼ�����
	
//    systick_config();          // �δ�ʱ����ʼ��
//	  led_gpio_config();   			 // led��ʼ��
//    key_gpio_config(); 				 // key��ʼ��
//	  usart_gpio_config(9600U);  // ����0��ʼ��
//	sdramTest();
    os_sys_init(start_task); 
    while(1) {
		
    }
}

void tick_hook(void)
{

}