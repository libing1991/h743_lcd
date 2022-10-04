#include "cpu.h"
//#include "gd32f4xx_rcu.h"
//#include "gd32f4xx_tli.h"
//#include "gd32f4xx_ipa.h"
#include "mcu_gd450.h"


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

static void initPort(void)
{
    setPortPinDir(3 , 7 , 0 , 0);
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


void initcpu(void)
{
    initClock();
    initPort();
}
