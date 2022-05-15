/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-01-08     zylx         first version
 */

#include <board.h>

#ifdef BSP_USING_LCD
#include <lcd_port.h>
#include <string.h>

//#define DRV_DEBUG
#define LOG_TAG             "drv.lcd"
#include <drv_log.h>

#define LCD_FRAME_BUF_ADDR			0XC0000000  
#define LCD_PIXEL_FORMAT_RGB565         0X02    

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t u8;

#define LCD_DEVICE(dev)     (struct drv_lcd_device*)(dev)
    
u16 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小


//LCD LTDC重要参数集
typedef struct  
{							 
	u32 pwidth;			//LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
	u32 pheight;		//LCD面板的高度,固定参数,不随显示方向改变
	u16 hsw;			//水平同步宽度
	u16 vsw;			//垂直同步宽度
	u16 hbp;			//水平后廊
	u16 vbp;			//垂直后廊
	u16 hfp;			//水平前廊
	u16 vfp;			//垂直前廊 
	u8 activelayer;		//当前层编号:0/1	
	u8 dir;				//0,竖屏;1,横屏;
	u16 width;			//LCD宽度
	u16 height;			//LCD高度
	u32 pixsize;		//每个像素所占字节数
}_ltdc_dev; 

_ltdc_dev lcdltdc;	
u32 *ltdc_framebuf[2];	

static LTDC_HandleTypeDef LTDC_Handler = {0};

struct drv_lcd_device
{
    struct rt_device parent;

    struct rt_device_graphic_info lcd_info;

    struct rt_semaphore lcd_lock;

    /* 0:front_buf is being used 1: back_buf is being used*/
    rt_uint8_t cur_buf;
    rt_uint8_t *front_buf;
    rt_uint8_t *back_buf;
};

struct drv_lcd_device _lcd;

static rt_err_t drv_lcd_init(struct rt_device *device)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);
    /* nothing, right now */
    lcd = lcd;
    return RT_EOK;
}


void HAL_LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc)
{
    /* emable line interupt */
    //__HAL_LTDC_ENABLE_IT(&LtdcHandle, LTDC_IER_LIE);
}

void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc)
{
    //rt_sem_release(&_lcd.lcd_lock);
}

void LTDC_IRQHandler(void)
{
//    rt_enter_critical();

//    HAL_LTDC_IRQHandler(&LtdcHandle);

//    rt_exit_critical();
}


//LTDC,层颜窗口设置,窗口以LCD面板坐标系为基准
//注意:此函数必须在LTDC_Layer_Parameter_Config之后再设置.
//layerx:层值,0/1.
//sx,sy:起始坐标
//width,height:宽度和高度
void LTDC_Layer_Window_Config(uint8_t layerx,uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
    HAL_LTDC_SetWindowPosition(&LTDC_Handler,sx,sy,layerx);  //设置窗口的位置
    HAL_LTDC_SetWindowSize(&LTDC_Handler,width,height,layerx);//设置窗口大小    
}

//LTDC,基本参数设置.
//注意:此函数,必须在LTDC_Layer_Window_Config之前设置.
//layerx:层值,0/1.
//bufaddr:层颜色帧缓存起始地址
//pixformat:颜色格式.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
//alpha:层颜色Alpha值,0,全透明;255,不透明
//alpha0:默认颜色Alpha值,0,全透明;255,不透明
//bfac1:混合系数1,4(100),恒定的Alpha;6(101),像素Alpha*恒定Alpha
//bfac2:混合系数2,5(101),恒定的Alpha;7(111),像素Alpha*恒定Alpha
//bkcolor:层默认颜色,32位,低24位有效,RGB888格式
//返回值:无
void LTDC_Layer_Parameter_Config(uint8_t layerx,uint32_t bufaddr,uint8_t pixformat,uint8_t alpha,uint8_t alpha0,uint8_t bfac1,uint8_t bfac2,uint32_t bkcolor)
{
	LTDC_LayerCfgTypeDef pLayerCfg;
	
	pLayerCfg.WindowX0=0;                       //窗口起始X坐标
	pLayerCfg.WindowY0=0;                       //窗口起始Y坐标
	pLayerCfg.WindowX1=lcdltdc.pwidth;          //窗口终止X坐标
	pLayerCfg.WindowY1=lcdltdc.pheight;         //窗口终止Y坐标
	pLayerCfg.PixelFormat=pixformat;		    //像素格式
	pLayerCfg.Alpha=alpha;				        //Alpha值设置，0~255,255为完全不透明
	pLayerCfg.Alpha0=alpha0;			        //默认Alpha值
	pLayerCfg.BlendingFactor1=(u32)bfac1<<8;    //设置层混合系数
	pLayerCfg.BlendingFactor2=(u32)bfac2<<8;	//设置层混合系数
	pLayerCfg.FBStartAdress=bufaddr;	        //设置层颜色帧缓存起始地址
	pLayerCfg.ImageWidth=lcdltdc.pwidth;        //设置颜色帧缓冲区的宽度    
	pLayerCfg.ImageHeight=lcdltdc.pheight;      //设置颜色帧缓冲区的高度
	pLayerCfg.Backcolor.Red=(u8)(bkcolor&0X00FF0000)>>16;   //背景颜色红色部分
	pLayerCfg.Backcolor.Green=(u8)(bkcolor&0X0000FF00)>>8;  //背景颜色绿色部分
	pLayerCfg.Backcolor.Blue=(u8)bkcolor&0X000000FF;        //背景颜色蓝色部分
	HAL_LTDC_ConfigLayer(&LTDC_Handler,&pLayerCfg,layerx);   //设置所选中的层
}  


//LTDC填充矩形,DMA2D填充
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//注意:sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
//color:要填充的颜色
void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color)
{ 
	u32 psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	//坐标系转换
	if(lcdltdc.dir)	//横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//竖屏
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	} 
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	RCC->AHB1ENR|=1<<23;			//使能DM2D时钟
	DMA2D->CR=3<<16;				//寄存器到存储器模式
	DMA2D->OPFCCR=LCD_PIXEL_FORMAT_RGB565;	//设置颜色格式
	DMA2D->OOR=offline;				//设置行偏移 
	DMA2D->CR&=~(1<<0);				//先停止DMA2D
	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器
	DMA2D->OCOLR=color;				//设定输出颜色寄存器 
	DMA2D->CR|=1<<0;				//启动DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//超时退出
	}  
	DMA2D->IFCR|=1<<1;				//清除传输完成标志 	 	
}


void LTDC_Clear(uint32_t color)
{
	LTDC_Fill(0,0,lcdltdc.width-1,lcdltdc.height-1,color);
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LTDC_Display_Dir(u8 dir)
{
    lcdltdc.dir=dir; 	//显示方向
	if(dir==0)			//竖屏
	{
		lcdltdc.width=lcdltdc.pheight;
		lcdltdc.height=lcdltdc.pwidth;	
	}else if(dir==1)	//横屏
	{
		lcdltdc.width=lcdltdc.pwidth;
		lcdltdc.height=lcdltdc.pheight;
	}
}

//选择层
//layerx:层号;0,第一层;1,第二层;
void LTDC_Select_Layer(u8 layerx)
{
	lcdltdc.activelayer=layerx;
}

rt_err_t stm32_lcd_init(void)
{
	u16 lcdid=0;
	
    lcdltdc.pwidth=800;			    //面板宽度,单位:像素
    lcdltdc.pheight=480;		    //面板高度,单位:像素
    lcdltdc.hsw=1;				    //水平同步宽度
    lcdltdc.vsw=1;				    //垂直同步宽度
    lcdltdc.hbp=46;				    //水平后廊
    lcdltdc.vbp=23;				    //垂直后廊
    lcdltdc.hfp=210;			    //水平前廊
    lcdltdc.vfp=22;				    //垂直前廊
	//LTDC_Clk_Set(5,160,28);			//设置像素时钟 33M(如果开双显,需要降低DCLK到:18.75Mhz  pll3r=43,才会比较好)
	 

//	lcddev.width=lcdltdc.pwidth;
//	lcddev.height=lcdltdc.pheight;

    lcdltdc.pixsize=2;				//每个像素占2个字节
	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;
    
    //LTDC配置
    LTDC_Handler.Instance=LTDC;
    LTDC_Handler.Init.HSPolarity=LTDC_HSPOLARITY_AL;         //水平同步极性
    LTDC_Handler.Init.VSPolarity=LTDC_VSPOLARITY_AL;         //垂直同步极性
    LTDC_Handler.Init.DEPolarity=LTDC_DEPOLARITY_AL;         //数据使能极性
    LTDC_Handler.Init.PCPolarity=LTDC_PCPOLARITY_IPC;        //像素时钟极性
    LTDC_Handler.Init.HorizontalSync=lcdltdc.hsw-1;          //水平同步宽度
    LTDC_Handler.Init.VerticalSync=lcdltdc.vsw-1;            //垂直同步宽度
    LTDC_Handler.Init.AccumulatedHBP=lcdltdc.hsw+lcdltdc.hbp-1; //水平同步后沿宽度
    LTDC_Handler.Init.AccumulatedVBP=lcdltdc.vsw+lcdltdc.vbp-1; //垂直同步后沿高度
    LTDC_Handler.Init.AccumulatedActiveW=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth-1;//有效宽度
    LTDC_Handler.Init.AccumulatedActiveH=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight-1;//有效高度
    LTDC_Handler.Init.TotalWidth=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth+lcdltdc.hfp-1;   //总宽度
    LTDC_Handler.Init.TotalHeigh=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight+lcdltdc.vfp-1;  //总高度
    LTDC_Handler.Init.Backcolor.Red=0;           //屏幕背景层红色部分
    LTDC_Handler.Init.Backcolor.Green=0;         //屏幕背景层绿色部分
    LTDC_Handler.Init.Backcolor.Blue=0;          //屏幕背景色蓝色部分
    HAL_LTDC_Init(&LTDC_Handler);
 	
	//层配置
	LTDC_Layer_Parameter_Config(0,(u32)ltdc_framebuf[0],LCD_PIXEL_FORMAT_RGB565,255,0,6,7,0X000000);//层参数配置
	LTDC_Layer_Window_Config(0,0,0,lcdltdc.pwidth,lcdltdc.pheight);	//层窗口配置,以LCD面板坐标系为基准,不要随便修改!	
	 	
 	LTDC_Display_Dir(0);			//默认竖屏
	LTDC_Select_Layer(0); 			//选择第1层
    //LCD_LED(1);         		    //点亮背光
    LTDC_Clear(0xFFFFFFFF);			//清屏 
    LTDC_Fill(0, 0, 100, 200, 0x7FFF);    
}
#if defined(LCD_BACKLIGHT_USING_PWM)
void turn_on_lcd_backlight(void)
{
    struct rt_device_pwm *pwm_dev;

    /* turn on the LCD backlight */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(LCD_PWM_DEV_NAME);
    /* pwm frequency:100K = 10000ns */
    rt_pwm_set(pwm_dev, LCD_PWM_DEV_CHANNEL, 10000, 10000);
    rt_pwm_enable(pwm_dev, LCD_PWM_DEV_CHANNEL);
}
#elif defined(LCD_BACKLIGHT_USING_GPIO)
void turn_on_lcd_backlight(void)
{
    rt_pin_mode(LCD_BL_GPIO_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_DISP_GPIO_NUM, PIN_MODE_OUTPUT);

    rt_pin_write(LCD_DISP_GPIO_NUM, PIN_HIGH);
    rt_pin_write(LCD_BL_GPIO_NUM, PIN_HIGH);
}
#else
void turn_on_lcd_backlight(void)
{
    
}
#endif

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops lcd_ops =
{
    drv_lcd_init,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    drv_lcd_control
};
#endif

//画点函数
//x,y:坐标
//color:颜色值
void LTDC_Draw_Point(uint16_t x,uint16_t y,uint32_t color)
{ 

    *(uint16_t*)((uint32_t)_lcd.front_buf[0]+2*(LCD_WIDTH*y+x))=color;

}

//读点函数
//返回值:颜色值
uint32_t LTDC_Read_Point(uint16_t x,uint16_t y)
{ 

    return *(uint16_t*)((uint32_t)_lcd.front_buf[0]+2*(LCD_WIDTH*(LCD_HEIGHT-x-1)+y)); 

}


int drv_lcd_hw_init(void)
{
    stm32_lcd_init();
    turn_on_lcd_backlight();
//   
//    rt_err_t result = RT_EOK;
//    struct rt_device *device = &_lcd.parent;
// 
//    
//    
//    /* memset _lcd to zero */
//    memset(&_lcd, 0x00, sizeof(_lcd));

//    /* init lcd_lock semaphore */
//    result = rt_sem_init(&_lcd.lcd_lock, "lcd_lock", 0, RT_IPC_FLAG_FIFO);
//    if (result != RT_EOK)
//    {
//        LOG_E("init semaphore failed!\n");
//        result = -RT_ENOMEM;
//        //goto __exit;
//    }

//    /* config LCD dev info */
//    _lcd.lcd_info.height = LCD_HEIGHT;
//    _lcd.lcd_info.width = LCD_WIDTH;
//    _lcd.lcd_info.bits_per_pixel = LCD_BITS_PER_PIXEL;
//    _lcd.lcd_info.pixel_format = LCD_PIXEL_FORMAT;

//    /* malloc memory for Triple Buffering */
//    //_lcd.lcd_info.framebuffer = ltdc_lcd_framebuf;//rt_malloc(LCD_BUF_SIZE);
//    //_lcd.back_buf = ltdc_lcd_framebuf1;//rt_malloc(LCD_BUF_SIZE);
//    //_lcd.front_buf = ltdc_lcd_framebuf;//rt_malloc(LCD_BUF_SIZE);
////    if (_lcd.lcd_info.framebuffer == RT_NULL || _lcd.back_buf == RT_NULL || _lcd.front_buf == RT_NULL)
////    {
////        LOG_E("init frame buffer failed!\n");
////        result = -RT_ENOMEM;
////        //goto __exit;
////    }

//    /* memset buff to 0xFF */
//    //memset(_lcd.lcd_info.framebuffer, 0xFF, LCD_BUF_SIZE);
//    //memset(_lcd.back_buf, 0xFF, LCD_BUF_SIZE);
//    //memset(_lcd.front_buf, 0xAA, LCD_BUF_SIZE);

//    device->type    = RT_Device_Class_Graphic;
//#ifdef RT_USING_DEVICE_OPS
//    device->ops     = &lcd_ops;
//#else
//    device->init    = drv_lcd_init;
//    device->control = NULL;
//#endif

//    /* register lcd device */
//    rt_device_register(device, "lcd", RT_DEVICE_FLAG_RDWR);

//    /* init stm32 LTDC */
//    if (stm32_lcd_init(&_lcd) != RT_EOK)
//    {
//        result = -RT_ERROR;
//        //goto __exit;
//    }
//    turn_on_lcd_backlight();
//    return result;
////    else
////    {
////        turn_on_lcd_backlight();
////    }
//    //LTDC_Fill(0, 0, 100, 200, 0xF81F);
//    
//__exit:
//    if (result != RT_EOK)
//    {
//        rt_sem_delete(&_lcd.lcd_lock);

//        if (_lcd.lcd_info.framebuffer)
//        {
//            rt_free(_lcd.lcd_info.framebuffer);
//        }

//        if (_lcd.back_buf)
//        {
//            rt_free(_lcd.back_buf);
//        }

//        if (_lcd.front_buf)
//        {
//            rt_free(_lcd.front_buf);
//        }
//    }
//    return result;
}
INIT_DEVICE_EXPORT(drv_lcd_hw_init);

static void setlcdColor(uint16_t color)
{
    uint16_t x, y;
    
    for(x= 0 ; x < 800; x++)
    {
        for(y = 0; y < 480; y++)
        {
            LTDC_Draw_Point(x, y, color);
        }
    }
}

#define DRV_DEBUG

#ifdef DRV_DEBUG
#ifdef FINSH_USING_MSH
int lcd_test()
{
    struct drv_lcd_device *lcd;
    lcd = (struct drv_lcd_device *)rt_device_find("lcd");
    
    int count = 0;
    int rgbData = 0;
    static uint8_t rData = 255;
    static uint8_t gData = 0;
    static uint8_t bData = 0;
    
    
//    LTDC_LAYER(&LtdcHandle, 0)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
//    LTDC_LAYER(&LtdcHandle, 0)->CFBAR = (uint32_t)(_lcd.front_buf);
//    rt_sem_take(&_lcd.lcd_lock, RT_TICK_PER_SECOND / 20);
//    HAL_LTDC_Relaod(&LtdcHandle, LTDC_SRCR_VBR);

    //while (1)
    {
        rgbData = 0;
        
        
        
//        setlcdColor(0x001F);
//        rt_thread_mdelay(1000);
//        setlcdColor(0X07FF);
//        rt_thread_mdelay(1000);
//        setlcdColor(0x07E0);
//        rt_thread_mdelay(1000);
//        setlcdColor(0xF800);
//        rt_thread_mdelay(1000);
//            memset(_lcd.front_buf, 0xAA, LCD_BUF_SIZE / 2);
//            rt_thread_mdelay(1000);
//        memset(_lcd.front_buf, 0xAA, LCD_BUF_SIZE / 3);
//            rt_thread_mdelay(1000);
//        memset(_lcd.front_buf, 0xAA, LCD_BUF_SIZE / 2);
//            rt_thread_mdelay(1000);
//        memset(_lcd.front_buf, 0xAA, LCD_BUF_SIZE / 5);
//            rt_thread_mdelay(1000);
//        memset(_lcd.front_buf, 0xAA, LCD_BUF_SIZE/ 8);
//            rt_thread_mdelay(1000);
//        memset(_lcd.front_buf, 0xAA, LCD_BUF_SIZE / 10);
//            rt_thread_mdelay(1000);
        
//        LTDC_Fill(0, 0, 500, 400, 0xF800);
//        rt_thread_mdelay(1000);
//        LTDC_Fill(0, 0, 500, 400, 0x001F);
//        rt_thread_mdelay(1000);
//        LTDC_Fill(0, 0, 500, 400, 0XFFE0);
//        rt_thread_mdelay(1000);
//        LTDC_Fill(0, 0, 500, 400, 0X07FF);
//        rt_thread_mdelay(1000);
//        LTDC_Fill(0, 0, 500, 400, 0xFFE0);
//        rt_thread_mdelay(1000);
        /* red */
//        for (int i = 0; i < LCD_BUF_SIZE / 3; i += 3)
//        {
//            lcd->front_buf[3 * i] = rData;
//            lcd->front_buf[3 * i + 1] = gData;
//            lcd->front_buf[3 * i + 2] = bData;
//            
//            
//            
//            if(rgbData == 255)
//            {
//                rgbData = 0;
//            }
//        }
        //lcd->parent.control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
//        rt_thread_mdelay(1000);
//        /* green */
//        for (int i = 0; i < LCD_BUF_SIZE / 3; i += 3)
//        {
//            lcd->lcd_info.framebuffer[3 * i] = 0;
//            lcd->lcd_info.framebuffer[3 * i + 1] = 0;
//            lcd->lcd_info.framebuffer[3 * i + 2] = 0xFF;
//            
//            if(rgbData == 255)
//            {
//                rgbData = 0;
//            }
//        }
//        lcd->parent.control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
//        rt_thread_mdelay(1000);
//        /* blue */
//        for (int i = 0; i < LCD_BUF_SIZE / 3; i += 3)
//        {
//            lcd->lcd_info.framebuffer[3 * i] = 0;
//            lcd->lcd_info.framebuffer[3 * i + 1] = 0xFF;
//            lcd->lcd_info.framebuffer[3 * i + 2] = 0;
//            
//            if(rgbData == 255)
//            {
//                rgbData = 0;
//            }
//        }
//        lcd->parent.control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
//        rt_thread_mdelay(1000);
        count ++;
        
//        if(count == 5)
//        {
//            break;
//        }
//        rt_thread_mdelay(1000);
    }
}
MSH_CMD_EXPORT(lcd_test, lcd_test);
#endif /* FINSH_USING_MSH */
#endif /* DRV_DEBUG */
#endif /* BSP_USING_LCD */
