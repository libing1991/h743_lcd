#include "cpu.h"
#include "logo.h"
#include "mcu_gd450.h"

#define LCD_LAYER_BACKGROUND         0x0000
#define LCD_LAYER_FOREGROUND         0x0001

#define LCD_LINEDIR_HORIZONTAL       0x0000
#define LCD_LINEDIR_VERTICAL         0x0001

#define LCD_FRAME_BUFFER             ((uint32_t)0xC0000000)
#define BUFFER_OFFSET                ((uint32_t)(LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT * 4))

#define LCD_PIXEL_WIDTH              ((uint16_t)480)
#define LCD_PIXEL_HEIGHT             ((uint16_t)272)

static uint16_t current_textcolor = 0x0000;
static uint16_t current_backcolor = 0xFFFF;
static uint32_t current_framebuffer = LCD_FRAME_BUFFER;
static uint32_t current_layer = LCD_LAYER_BACKGROUND;


/* SDRAM Defininations */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0003)

#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)

#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)

#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_TIMEOUT                            ((uint32_t)0x0000FFFF)


static void SDRAM_PortInit(void)
{
	/* enable EXMC clock*/
    rcu_periph_clock_enable(RCU_EXMC);
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
	rcu_periph_clock_enable(RCU_GPIOH);
	
	//PC
	setPortPinDir(2, 0, 0, 4); 
	setPortPinDir(2, 2, 0, 4); 
    setPortPinDir(2, 3, 0, 4); 
	setPortPinMode(2, 0, GPIO_AF_12);	//nWE
	setPortPinMode(2, 2, GPIO_AF_12);	//LnSDCS0
	setPortPinMode(2, 3, GPIO_AF_12);	//LnSDCKE0
	
	//PD
	setPortPinDir(3, 0, 0, 4); 
	setPortPinDir(3, 1, 0, 4);
	setPortPinDir(3, 8, 0, 4); 
	setPortPinDir(3, 9, 0, 4); 
    setPortPinDir(3, 10, 0, 4); 
	setPortPinDir(3, 14, 0, 4); 
	setPortPinDir(3, 15, 0, 4);
	setPortPinMode(3, 0, GPIO_AF_12);	//DATA2
	setPortPinMode(3, 1, GPIO_AF_12);	//DATA3
	setPortPinMode(3, 8, GPIO_AF_12);	//DATA13
	setPortPinMode(3, 9, GPIO_AF_12);	//DATA14
	setPortPinMode(3, 10, GPIO_AF_12);	//DATA15
	setPortPinMode(3, 14, GPIO_AF_12);	//DATA0
	setPortPinMode(3, 15, GPIO_AF_12);	//DATA1
	
	//PE
	setPortPinDir(4, 0, 0, 4); 
	setPortPinDir(4, 1, 0, 4); 
    setPortPinMode(4, 0, GPIO_AF_12);	//LDQM0
	setPortPinMode(4, 1, GPIO_AF_12);	//LDQM1
	
	setPortPinDir(4, 7, 0, 4); 
	setPortPinDir(4, 8, 0, 4); 
	setPortPinDir(4, 9, 0, 4); 
	setPortPinDir(4, 10, 0, 4); 
	setPortPinDir(4, 11, 0, 4); 
	setPortPinDir(4, 12, 0, 4); 
	setPortPinDir(4, 13, 0, 4); 
	setPortPinDir(4, 14, 0, 4); 
	setPortPinDir(4, 15, 0, 4); 
	setPortPinMode(4, 7, GPIO_AF_12);	//DATA4
	setPortPinMode(4, 8, GPIO_AF_12);	//DATA5
	setPortPinMode(4, 9, GPIO_AF_12);	//DATA6
	setPortPinMode(4, 10, GPIO_AF_12);	//DATA7
	setPortPinMode(4, 11, GPIO_AF_12);	//DATA8
	setPortPinMode(4, 12, GPIO_AF_12);	//DATA9
	setPortPinMode(4, 13, GPIO_AF_12);	//DATA10
	setPortPinMode(4, 14, GPIO_AF_12);	//DATA11
	setPortPinMode(4, 15, GPIO_AF_12);	//DATA12

    //PF
	setPortPinDir(5, 11, 0, 4);
	setPortPinMode(5, 11, GPIO_AF_12);	//LnSDRAS
	
	setPortPinDir(5, 0, 0, 4); 
	setPortPinDir(5, 1, 0, 4); 
	setPortPinDir(5, 2, 0, 4); 
	setPortPinDir(5, 3, 0, 4); 
	setPortPinDir(5, 4, 0, 4); 
	setPortPinDir(5, 5, 0, 4); 
	setPortPinDir(5, 12, 0, 4); 
	setPortPinDir(5, 13, 0, 4); 
	setPortPinDir(5, 14, 0, 4); 
    setPortPinDir(5, 15, 0, 4); 
	setPortPinMode(5, 0, GPIO_AF_12);	//ADDER0
	setPortPinMode(5, 1, GPIO_AF_12);	//ADDER1
	setPortPinMode(5, 2, GPIO_AF_12);	//ADDER2
	setPortPinMode(5, 3, GPIO_AF_12);	//ADDER3
	setPortPinMode(5, 4, GPIO_AF_12);	//ADDER4
	setPortPinMode(5, 5, GPIO_AF_12);	//ADDER5
	setPortPinMode(5, 12, GPIO_AF_12);	//ADDER6
	setPortPinMode(5, 13, GPIO_AF_12);	//ADDER7
	setPortPinMode(5, 14, GPIO_AF_12);	//ADDER8
	setPortPinMode(5, 15, GPIO_AF_12);	//ADDER9  
	
	//PG
	setPortPinDir(6, 0, 0, 4); 
	setPortPinDir(6, 1, 0, 4); 	
	setPortPinDir(6, 2, 0, 4); 
	setPortPinDir(6, 4, 0, 4); 
	setPortPinDir(6, 5, 0, 4);
	setPortPinDir(6, 8, 0, 4); 
	setPortPinDir(6, 15, 0, 4);
	
	setPortPinMode(6, 0, GPIO_AF_12);	//ADDER10
	setPortPinMode(6, 1, GPIO_AF_12);	//ADDER11
	setPortPinMode(6, 2, GPIO_AF_12);	//ADDER12
	setPortPinMode(6, 4, GPIO_AF_12);	//ADDER14 SDR_BA0
	setPortPinMode(6, 5, GPIO_AF_12);	//ADDER15 SDR_BA1
	setPortPinMode(6, 8, GPIO_AF_12);	//LSDCLK0
	setPortPinMode(6, 15, GPIO_AF_12);	//LnSDCAS 
}

/**
    @brief      sdram 初始化
    @param[in]  none
    @param[out] none
    @retval     none
*/
void SDRAMInit(void)
{
	exmc_sdram_parameter_struct        sdram_init_struct;
    exmc_sdram_timing_parameter_struct  sdram_timing_init_struct;
    exmc_sdram_command_parameter_struct     sdram_command_init_struct;
    uint32_t command_content = 0;
    uint32_t timeout = SDRAM_TIMEOUT;
    

	
	SDRAM_PortInit();	//端口时钟初始化
	
    /* EXMC SDRAM device initialization sequence --------------------------------*/
    /* Step 1 : configure SDRAM timing registers --------------------------------*/
    /* LMRD: 2 clock cycles -*/
    sdram_timing_init_struct.load_mode_register_delay = 2;
    /* XSRD: min = 67ns -*/
    sdram_timing_init_struct.exit_selfrefresh_delay = 8; 
    /* RASD: min=42ns , max=120k (ns) -*/
    sdram_timing_init_struct.row_address_select_delay = 6;
    /* ARFD: min=60ns */
    sdram_timing_init_struct.auto_refresh_delay = 6;
    /* WRD:  min=1 Clock cycles +6ns -*/
    sdram_timing_init_struct.write_recovery_delay = 2;
    /* RPD:  min=18ns -*/
    sdram_timing_init_struct.row_precharge_delay = 2;
    /* RCD:  min=18ns -*/
    sdram_timing_init_struct.row_to_column_delay = 2;

    /* step 2 : configure SDRAM control registers ---------------------------------*/
    sdram_init_struct.sdram_device = EXMC_SDRAM_DEVICE0;
    sdram_init_struct.column_address_width = EXMC_SDRAM_COW_ADDRESS_9;
    sdram_init_struct.row_address_width = EXMC_SDRAM_ROW_ADDRESS_13;
    sdram_init_struct.data_width = EXMC_SDRAM_DATABUS_WIDTH_16B;
    sdram_init_struct.internal_bank_number = EXMC_SDRAM_4_INTER_BANK;
    sdram_init_struct.cas_latency = EXMC_CAS_LATENCY_3_SDCLK; 
    sdram_init_struct.write_protection = DISABLE;
    sdram_init_struct.sdclock_config = EXMC_SDCLK_PERIODS_2_HCLK;  
    sdram_init_struct.brust_read_switch = ENABLE;
    sdram_init_struct.pipeline_read_delay = EXMC_PIPELINE_DELAY_1_HCLK;
    sdram_init_struct.timing  = &sdram_timing_init_struct;
    /* EXMC SDRAM bank initialization */
    exmc_sdram_init(&sdram_init_struct);

    /* step 3 : configure CKE high command---------------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_CLOCK_ENABLE;
    sdram_command_init_struct.bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */ 
    while((exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

	/* step 4 : insert 10ms delay----------------------------------------------*/
	timeout = SDRAM_TIMEOUT;
	
	while(timeout > 0)
	{
		timeout--;
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
	}

    /* step 5 : configure precharge all command----------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_PRECHARGE_ALL;
    sdram_command_init_struct.bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 6 : configure Auto-Refresh command-----------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_AUTO_REFRESH;
    sdram_command_init_struct.bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_8_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */ 
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 7 : configure load mode register command-----------------------------*/
    /* program mode register */
    command_content = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1        |
                                SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                                SDRAM_MODEREG_CAS_LATENCY_3           |
                                SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                                SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    sdram_command_init_struct.command = EXMC_SDRAM_LOAD_MODE_REGISTER;
    sdram_command_init_struct.bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = command_content;

    /* wait until the SDRAM controller is ready */ 
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 8 : set the auto-refresh rate counter--------------------------------*/
    /* 64ms, 4096-cycle refresh, 64ms/4096=7.81us */
    /* SDCLK_Freq = SYS_Freq/2 */
    /* (7.81 us * SDCLK_Freq) - 20 */
    exmc_sdram_refresh_count_set(700);

    /* wait until the SDRAM controller is ready */ 
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
}

void initLcdPort(void)
{
    
	/* configure HSYNC(PC6), VSYNC(PA4), PCLK(PG7), DE(PF10) */
    /* configure LCD_R7(PG6), LCD_R6(PA8), LCD_R5(PA12), LCD_R4(PA11), LCD_R3(PB0), 
                 LCD_G7(PD3), LCD_G6(PC7), LCD_G5(PB11), LCD_G4(PB10), LCD_G3(PG10), LCD_G2(PA6),
                 LCD_B7(PB9), LCD_B6(PB8), LCD_B5(PA3), LCD_B4(PG12), LCD_B3(PG11) */
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_3);  
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_4);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_6);  
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_12); 
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_11);  
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_8); 
    gpio_af_set(GPIOB,GPIO_AF_9,GPIO_PIN_0);  
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_10);     
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_11);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_8);     
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_9);
    gpio_af_set(GPIOC,GPIO_AF_14,GPIO_PIN_6);     
    gpio_af_set(GPIOC,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOD,GPIO_AF_14,GPIO_PIN_3);     
    gpio_af_set(GPIOF,GPIO_AF_14,GPIO_PIN_10);     
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_6);     
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOG,GPIO_AF_9,GPIO_PIN_10);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_11);     
    gpio_af_set(GPIOG,GPIO_AF_9,GPIO_PIN_12);    

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_6
    |GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4|GPIO_PIN_3
    |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
    |GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
    |GPIO_PIN_11);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_6|GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11
    |GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11
    |GPIO_PIN_12);
}

static void initPort(void)
{
    setPortPinDir(3 , 7 , 0 , 0);
    
    //LCD
    initLcdPort();
    
    //BL_CL
    setPortPinDir(3 , 13 , 0 , 0);
    
}

void setLcdBlState(BOOLEAN state)
{
    setPortPinState(3, 13, state);
}
void setPortPinState(INT8U enPort , INT8U number , BOOLEAN state)
{
    INT32U port;
    INT32U pin;
    
    port = GPIOA + (0x00000400 * enPort);
    pin = 0x01 << number;
    
    if(state)
    {
        GPIO_BOP(port) = (uint32_t)pin;
    }
    else
    {
        GPIO_BC(port) = (uint32_t)pin;
    }
}
void WriteLedPortData(INT8U Buffer[])//向灯的引脚写值 
{
	setPortPinState(3, 7, (Buffer[0] & 0x01) != 0);
}

static void initClock(void)
{
    rcu_periph_clock_enable(RCU_SYSCFG);	
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);

	/* enable the periphral clock for LCD*/
	rcu_periph_clock_enable(RCU_TLI);
	
	//图像处理加速器
	rcu_periph_clock_enable(RCU_IPA);
	
	/* enable can clock */
    rcu_periph_clock_enable(RCU_CAN0);
	rcu_periph_clock_enable(RCU_CAN1);
    rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_USART1);
	rcu_periph_clock_enable(RCU_USART2);
	rcu_periph_clock_enable(RCU_UART3);
	rcu_periph_clock_enable(RCU_UART6);
	
	rcu_periph_clock_enable(RCU_TIMER0);
	rcu_periph_clock_enable(RCU_TIMER2);
	rcu_periph_clock_enable(RCU_TIMER7);
	rcu_periph_clock_enable(RCU_TIMER1);
	rcu_periph_clock_enable(RCU_TIMER11);
	
	rcu_periph_clock_enable(RCU_DMA0);
	rcu_periph_clock_enable(RCU_DMA1);
	rcu_periph_clock_enable(RCU_SPI3);
    
    
}


/*!
    \brief      set the transparency of LCD
    \param[in]  trans: transparency of LCD, from 0 to 255
    \param[out] none
    \retval     none
*/
void lcd_transparency_set(uint8_t trans)
{
    if (LCD_LAYER_BACKGROUND == current_layer){
        TLI_LxSA(LAYER0) &= ~(TLI_LxSA_SA);
        TLI_LxSA(LAYER0) = trans;
    }else{
        TLI_LxSA(LAYER1) &= ~(TLI_LxSA_SA);
        TLI_LxSA(LAYER1) = trans;
    }
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/**
    \brief      configure the packeted pixel format
    \param[in]  pixel_format: pixel format
      \arg        LAYER_PPF_ARGB8888
      \arg        LAYER_PPF_RGB888
      \arg        LAYER_PPF_RGB565
      \arg        LAYER_PPF_ARGB1555
      \arg        LAYER_PPF_ARGB4444
      \arg        LAYER_PPF_L8
      \arg        LAYER_PPF_AL44
      \arg        LAYER_PPF_AL88
    \param[out] none
    \retval     none
*/
void lcd_pixel_format_config(uint32_t pixel_format)
{
    if(LCD_LAYER_BACKGROUND == current_layer){
        TLI_LxPPF(LAYER0) &= ~(TLI_LxPPF_PPF);
        TLI_LxPPF(LAYER0) = pixel_format;
    }else{
        TLI_LxPPF(LAYER1) &= ~(TLI_LxPPF_PPF);
        TLI_LxPPF(LAYER1) = pixel_format;
    }
}

/*!
    \brief      configure the frame buffer base address
    \param[in]  address: frame buffer base address
    \param[out] none
    \retval     none
*/
void lcd_address_config(uint32_t address)
{
    if (LCD_LAYER_BACKGROUND == current_layer){
        TLI_LxFBADDR(LAYER0) &= ~(TLI_LxFBADDR_FBADD);
        TLI_LxFBADDR(LAYER0) = address;
    }else{
        TLI_LxFBADDR(LAYER1) &= ~(TLI_LxFBADDR_FBADD);
        TLI_LxFBADDR(LAYER1) = address;
    }

    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*!
    \brief      clear the LCD with specified color
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    for (index = 0x00; index < BUFFER_OFFSET; index++){
        *(__IO uint16_t*)(current_framebuffer + (2*index)) = color;
    }
}
/**
    \brief      initialize the LCD GPIO and TLI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_init(void)
{
    tli_parameter_struct  tli_init_struct;
	tli_layer_parameter_struct  tli_layer_init_struct;

    /* configure the PLLSAI clock to generate lcd clock */
    if(ERROR == rcu_pllsai_config(256, 2, 3))
	{
        while(1);
    }
    rcu_tli_clock_div_config(RCU_PLLSAIR_DIV8);
    rcu_osci_on(RCU_PLLSAI_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK))
	{
        while(1);
    }

//	SDRAMInit();
	
    /* TLI initialization */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;

    /* LCD display timing configuration */
	tli_init_struct.synpsz_hpsz = 43 - 1;
	tli_init_struct.synpsz_vpsz = 12 - 1;
	tli_init_struct.backpsz_hbpsz = 43 + 2 - 1;
	tli_init_struct.backpsz_vbpsz = 12 + 2 - 1;
	tli_init_struct.activesz_hasz = 43 + 2 + 480 - 1;
	tli_init_struct.activesz_vasz = 12 + 2 + 272 - 1;
	tli_init_struct.totalsz_htsz = 43 + 2 + 480 + 2 - 1;
	tli_init_struct.totalsz_vtsz = 12 + 2 + 272 + 2 - 1;
	
    /* LCD background color configure*/
    tli_init_struct.backcolor_red     = 0xFF;
    tli_init_struct.backcolor_green   = 0xFF;
    tli_init_struct.backcolor_blue    = 0xFF;
    tli_init(&tli_init_struct);
	
	/* TLI layer0 configuration */
	tli_layer_init_struct.layer_window_leftpos = 43 + 2;
	tli_layer_init_struct.layer_window_rightpos = (480 + 43 + 2 - 1); 
	tli_layer_init_struct.layer_window_toppos = 12;						//10 + 2
	tli_layer_init_struct.layer_window_bottompos = (272 + 12 + 2 - 1);	
	tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
	tli_layer_init_struct.layer_sa = 0xFF;
	tli_layer_init_struct.layer_default_blue = 0xFF;        
	tli_layer_init_struct.layer_default_green = 0xFF;       
	tli_layer_init_struct.layer_default_red = 0xFF;
	tli_layer_init_struct.layer_default_alpha = 0X0;
	tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
	tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
	//tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)&gImage_Image_RGB565;
	tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)LCD_FRAME_BUFFER;
	tli_layer_init_struct.layer_frame_line_length = ((480 * 2) + 3);
	tli_layer_init_struct.layer_frame_buf_stride_offset = (480 * 2);
	tli_layer_init_struct.layer_frame_total_line_number = 272; 
	tli_layer_init(LAYER0, &tli_layer_init_struct);
	
	 /* enable layer0 and layer1 */
	tli_layer_enable(LAYER0);
	tli_reload_config(TLI_REQUEST_RELOAD_EN);
    //lcd_font_set(&LCD_DEFAULT_FONT);
    tli_dither_config(TLI_DITHER_DISABLE);
	tli_enable();
	
	current_framebuffer = LCD_FRAME_BUFFER;
	current_layer = LCD_LAYER_BACKGROUND;
}


/**
    @brief      向SDRAM内写一组字符
    @param[in]  pbuffer: 要装填数组的指针
	@param[in]  writeaddr: 选择SDRAM的内部地址
	@param[in]  value: value to fill on the buffer
    @param[out] none
    @retval     none
*/
void sdram_writebuffer_8(uint8_t* pbuffer, uint32_t writeaddr, uint32_t numbytetowrite)
{
    uint8_t data ;
    uint8_t *data1 ;
    
    data1 = &data;
    
    for(; numbytetowrite != 0; numbytetowrite--) 
	{
        *(uint8_t *) (((uint32_t)0xC0000000) + writeaddr) = *pbuffer++;  
        writeaddr += 1;
        
        *data1 = *(uint8_t*) (((uint32_t)0xC0000000) + 0x0);
        if(data != 0x55)
        {
            __nop();
        }
    }
}

void initcpu(void)
{
    initClock();
    initPort();
    setLcdBlState(1);
    lcd_init();
    sdram_writebuffer_8((uint8_t*)gImage_Image_RGB565, 0x00000, 0x3FC00);
}

void LCD_Color_Fill(INT16U sx, INT16U sy, INT16U ex, INT16U ey, INT16U *color)
{
    uint32_t time_out;
    uint16_t offline;
    offline = 480 - (ex - sx + 1);

    IPA_CTL = 0x0;
    IPA_FMADDR = (uint32_t)color;
    IPA_DMADDR = (uint32_t)(uint32_t)LCD_FRAME_BUFFER + 2*(480*sy+sx);
    IPA_FLOFF = 0;
    IPA_DLOFF = offline;
    IPA_FPCTL = FOREGROUND_PPF_RGB565;
    IPA_IMS = (uint32_t)((ex - sx + 1) << 16) | (uint16_t)(ey - sy + 1);

    IPA_CTL |= IPA_CTL_TEN;

    while (IPA_CTL & IPA_CTL_TEN)
    {
        if(time_out++ >= 0XFFFFFFFF) break;
    }
}