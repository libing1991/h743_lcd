/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-04     zylx         first version
 */

#include <board.h>

#ifdef BSP_USING_SDRAM
#include <sdram_port.h>
#define DRV_DEBUG
#define LOG_TAG             "drv.sdram"
#include <drv_log.h>

static SDRAM_HandleTypeDef hsdram1;
static FMC_SDRAM_CommandTypeDef command;
#ifdef RT_USING_MEMHEAP_AS_HEAP
static struct rt_memheap system_heap;
#endif

static SDRAM_HandleTypeDef SDRAM_Handler;   //SDRAM���

/**
  * @brief  Perform the SDRAM exernal memory inialization sequence
  * @param  hsdram: SDRAM handle
  * @param  Command: Pointer to SDRAM command structure
  * @retval None
  */
static void SDRAM_Initialization_Sequence1(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
    __IO uint32_t tmpmrd = 0;
    uint32_t target_bank = 0;

#if SDRAM_TARGET_BANK == 1
    target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
#else
    target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
#endif

    /* Configure a clock configuration enable command */
    Command->CommandMode           = FMC_SDRAM_CMD_CLK_ENABLE;
    Command->CommandTarget         = target_bank;
    Command->AutoRefreshNumber     = 1;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

    /* Insert 100 ms delay */
    /* interrupt is not enable, just to delay some time. */
    for (tmpmrd = 0; tmpmrd < 0xffffff; tmpmrd ++)
        ;

    /* Configure a PALL (precharge all) command */
    Command->CommandMode            = FMC_SDRAM_CMD_PALL;
    Command->CommandTarget          = target_bank;
    Command->AutoRefreshNumber      = 1;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

    /* Configure a Auto-Refresh command */
    Command->CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command->CommandTarget          = target_bank;
    Command->AutoRefreshNumber      = 8;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

    /* Program the external memory mode register */
#if SDRAM_DATA_WIDTH == 8
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1     |
#elif SDRAM_DATA_WIDTH == 16
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2     |
#else
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4     |
#endif
             SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL        |
#if SDRAM_CAS_LATENCY == 3
             SDRAM_MODEREG_CAS_LATENCY_3                |
#else
             SDRAM_MODEREG_CAS_LATENCY_2                |
#endif
             SDRAM_MODEREG_OPERATING_MODE_STANDARD      |
             SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    Command->CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
    Command->CommandTarget          = target_bank;
    Command->AutoRefreshNumber      = 1;
    Command->ModeRegisterDefinition = tmpmrd;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

    /* Set the device refresh counter */
    HAL_SDRAM_ProgramRefreshRate(hsdram, SDRAM_REFRESH_COUNT);
}


//��SDRAM��������
//bankx:0,��BANK5�����SDRAM����ָ��
//      1,��BANK6�����SDRAM����ָ��
//cmd:ָ��(0,����ģʽ/1,ʱ������ʹ��/2,Ԥ������д洢��/3,�Զ�ˢ��/4,����ģʽ�Ĵ���/5,��ˢ��/6,����)
//refresh:��ˢ�´���
//regval:ģʽ�Ĵ����Ķ���
//����ֵ:0,����;1,ʧ��.
uint8_t SDRAM_Send_Cmd(uint8_t bankx,uint8_t cmd,uint8_t refresh,uint16_t regval)
{
    uint32_t target_bank=0;
    FMC_SDRAM_CommandTypeDef Command;
    
    if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;       
    else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;   
    Command.CommandMode=cmd;                //����
    Command.CommandTarget=target_bank;      //Ŀ��SDRAM�洢����
    Command.AutoRefreshNumber=refresh;      //��ˢ�´���
    Command.ModeRegisterDefinition=regval;  //Ҫд��ģʽ�Ĵ�����ֵ
    if(HAL_SDRAM_SendCommand(&SDRAM_Handler,&Command,0XFFFF)==HAL_OK) //��SDRAM��������
    {
        return 0;  
    }
    else return 1;    
}

//����SDRAM��ʼ������
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
	uint32_t temp=0;
    __IO uint32_t tmpmrd = 0;

    //SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0); //ʱ������ʹ��
    /* Insert 100 ms delay */
    /* interrupt is not enable, just to delay some time. */
    for (tmpmrd = 0; tmpmrd < 0xffffff; tmpmrd ++)
        ;                                 //������ʱ200us
	SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       //�����д洢��Ԥ���
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);//������ˢ�´��� 
    //����ģʽ�Ĵ���,SDRAM��bit0~bit2Ϊָ��ͻ�����ʵĳ��ȣ�
	//bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7��bit8Ϊ����ģʽ
	//bit9Ϊָ����дͻ��ģʽ��bit10��bit11λ����λ
	temp=(uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |	//����ͻ������:1(������1/2/4/8)
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	//����ͻ������:����(����������/����)
              SDRAM_MODEREG_CAS_LATENCY_2           |	//����CASֵ:3(������2/3)
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //���ò���ģʽ:0,��׼ģʽ
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //����ͻ��дģʽ:1,�������
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);   //����SDRAM��ģʽ�Ĵ���
    
    //ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���),���㷽��:
	//COUNT=SDRAMˢ������/����-20=SDRAMˢ������(us)*SDCLKƵ��(Mhz)/����
    //����ʹ�õ�SDRAMˢ������Ϊ64ms,SDCLK=200/2=100Mhz,����Ϊ8192(2^13).
	//����,COUNT=64*1000*100/8192-20=677
	HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler,677);	
}

static int SDRAM_Init(void)
{
    int result = RT_EOK;
    
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;
                                                     
    SDRAM_Handler.Instance=FMC_SDRAM_DEVICE;                          	 //SDRAM��BANK5,6  
    SDRAM_Handler.Init.SDBank=FMC_SDRAM_BANK1;                           //SDRAM����BANK5��
    SDRAM_Handler.Init.ColumnBitsNumber=FMC_SDRAM_COLUMN_BITS_NUM_9;     //������
    SDRAM_Handler.Init.RowBitsNumber=FMC_SDRAM_ROW_BITS_NUM_13;          //������
    SDRAM_Handler.Init.MemoryDataWidth=FMC_SDRAM_MEM_BUS_WIDTH_16;       //���ݿ��Ϊ16λ
    SDRAM_Handler.Init.InternalBankNumber=FMC_SDRAM_INTERN_BANKS_NUM_4;  //һ��4��BANK
    SDRAM_Handler.Init.CASLatency=FMC_SDRAM_CAS_LATENCY_2;               //CASΪ2
    SDRAM_Handler.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;//ʧ��д����
    SDRAM_Handler.Init.SDClockPeriod=FMC_SDRAM_CLOCK_PERIOD_2;           //SDRAMʱ��ΪHCLK/2=200M/2=100M=10ns
    SDRAM_Handler.Init.ReadBurst=FMC_SDRAM_RBURST_ENABLE;                //ʹ��ͻ��
    SDRAM_Handler.Init.ReadPipeDelay=FMC_SDRAM_RPIPE_DELAY_0;            //��ͨ����ʱ
    
    SDRAM_Timing.LoadToActiveDelay=2;                                   //����ģʽ�Ĵ���������ʱ����ӳ�Ϊ2��ʱ������
    SDRAM_Timing.ExitSelfRefreshDelay=8;                                //�˳���ˢ���ӳ�Ϊ8��ʱ������
    SDRAM_Timing.SelfRefreshTime=6;                                     //��ˢ��ʱ��Ϊ6��ʱ������                                 
    SDRAM_Timing.RowCycleDelay=6;                                       //��ѭ���ӳ�Ϊ6��ʱ������
    SDRAM_Timing.WriteRecoveryTime=2;                                   //�ָ��ӳ�Ϊ2��ʱ������
    SDRAM_Timing.RPDelay=2;                                             //��Ԥ����ӳ�Ϊ2��ʱ������
    SDRAM_Timing.RCDDelay=2;                                            //�е����ӳ�Ϊ2��ʱ������ 
	
	

    /* Initialize the SDRAM controller */
    if (HAL_SDRAM_Init(&SDRAM_Handler,&SDRAM_Timing) != HAL_OK)
    {
        LOG_E("SDRAM init failed!");
        result = -RT_ERROR;
    }
    else
    {
        /* Program the SDRAM external device */
        SDRAM_Initialization_Sequence(&SDRAM_Handler);//����SDRAM��ʼ������
        LOG_D("sdram init success, mapped at 0x%X, size is %d bytes, data width is %d", SDRAM_BANK_ADDR, SDRAM_SIZE, SDRAM_DATA_WIDTH);
#ifdef RT_USING_MEMHEAP_AS_HEAP
        /* If RT_USING_MEMHEAP_AS_HEAP is enabled, SDRAM is initialized to the heap */
        rt_memheap_init(&system_heap, "sdram", (void *)SDRAM_BANK_ADDR, SDRAM_SIZE);
#endif
    }

    return result;
}
INIT_BOARD_EXPORT(SDRAM_Init);

#ifdef DRV_DEBUG
#ifdef FINSH_USING_MSH
int sdram_test(void)
{
    int i = 0;
    uint32_t start_time = 0, time_cast = 0;
#if SDRAM_DATA_WIDTH == 8
    char data_width = 1;
    uint8_t data = 0;
#elif SDRAM_DATA_WIDTH == 16
    char data_width = 2;
    uint16_t data = 0;
#else
    char data_width = 4;
    uint32_t data = 0;
#endif

    /* write data */
    LOG_D("Writing the %ld bytes data, waiting....", SDRAM_SIZE);
    start_time = rt_tick_get();
    for (i = 0; i < SDRAM_SIZE / data_width; i++)
    {
#if SDRAM_DATA_WIDTH == 8
        *(__IO uint8_t *)(SDRAM_BANK_ADDR + i * data_width) = (uint8_t)0x55;
#elif SDRAM_DATA_WIDTH == 16
        *(__IO uint16_t *)(SDRAM_BANK_ADDR + i * data_width) = (uint16_t)0x5555;
#else
        *(__IO uint32_t *)(SDRAM_BANK_ADDR + i * data_width) = (uint32_t)0x55555555;
#endif
    }
    time_cast = rt_tick_get() - start_time;
    LOG_D("Write data success, total time: %d.%03dS.", time_cast / RT_TICK_PER_SECOND,
          time_cast % RT_TICK_PER_SECOND / ((RT_TICK_PER_SECOND * 1 + 999) / 1000));

    /* read data */
    LOG_D("start Reading and verifying data, waiting....");
    for (i = 0; i < SDRAM_SIZE / data_width; i++)
    {
#if SDRAM_DATA_WIDTH == 8
        data = *(__IO uint8_t *)(SDRAM_BANK_ADDR + i * data_width);
        if (data != 0x55)
        {
            LOG_E("SDRAM test failed!");
            break;
        }
#elif SDRAM_DATA_WIDTH == 16
        data = *(__IO uint16_t *)(SDRAM_BANK_ADDR + i * data_width);
        if (data != 0x5555)
        {
            LOG_E("SDRAM test failed!");
            break;
        }
#else
        data = *(__IO uint32_t *)(SDRAM_BANK_ADDR + i * data_width);
        if (data != 0x55555555)
        {
            LOG_E("SDRAM test failed!");
            break;
        }
#endif
    }

    if (i >= SDRAM_SIZE / data_width)
    {
        LOG_D("SDRAM test success!");
    }

    return RT_EOK;
}
MSH_CMD_EXPORT(sdram_test, sdram test)
#endif /* FINSH_USING_MSH */
#endif /* DRV_DEBUG */
#endif /* BSP_USING_SDRAM */
