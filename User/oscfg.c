#include "includes.h"
#include "rtl.h"
#include "oscfg.h"

#define OS_ERR_NONE          1
#define OS_ERR_TIMEOUT       0

static OS_MUT EventMsg;		//����MSG
static OS_MUT LoopMutex;		//����MSG
static OS_MUT XgComApiMutex;		//����MSG
static OS_MUT p_PartManageMutex;
static OS_MUT p_MsgManageMutex;
static OS_MUT p_RelayStartMutex;
static OS_MUT p_XccSyncMutex;
static OS_MUT p_XcclinkSem;

static OS_MUT p_AtSendMutex;

static OS_MUT p_atRespNoticeSem;	//AT ָ��Ӧ�������ź���
static OS_MUT p_moduleAppSem;	//ģ���Ӧ�ó���֮�佻���ź���
static OS_MUT p_xLinkResponSem;

static OS_MUT p_MboxNorBuf;		//���⣺NORBuffer
static OS_MUT p_SoundStateMutexp;
static OS_MUT p_xtimerMutex;
static OS_MUT busLockMutex;
static OS_MUT p_rs232Mutex;

static OS_MUT p_rs485Mutex;		//485���ݷ��ͻ�����

static OS_SEM Uart1RxReady;		//��Ϣ������2����׼����
static OS_SEM Uart2RxReady;		//��Ϣ������2����׼����
static OS_SEM Uart3RxReady;		//��Ϣ������3����׼����
static OS_SEM HashMutex;
static OS_SEM p_fileDownLoadSem;
static OS_SEM FecBusRxRespond;		//��Ϣ��FEC��������׼����
static OS_SEM KeyUartRxReady;		//
static OS_SEM p_232aAckSem;
static OS_SEM p_485AckSem;
static OS_SEM PowerRespond;		//
static OS_SEM  p_XgNewOrder;
static OS_SEM p_XgClientRespond;		//Can���´�������
		//Can���´�������
static OS_SEM RelationRequest;		//��Ϣ:����������
static OS_SEM LoopRxReady;
static OS_SEM MultiKeyRxReady;
static OS_SEM MultiDriverRxReady;
static OS_SEM ExKeyBoardRxReady;
static OS_SEM KeyBoardRxReady;
static OS_SEM BusRxReady;
static OS_SEM p_Can1Mutex;
static OS_SEM p_Can2Mutex;
static OS_SEM Can1Ready;		//Can���´�������
static OS_SEM Can2Ready;		//Can���´�������
static OS_SEM p_XgMemMutex;    	 //XCAN�ڴ滥����
static OS_SEM p_XgMainMemMutex;    //�ڲ��ڴ��������
static OS_SEM SemXgRespond;		//Can���´�������
static OS_SEM MboxUart1;		//���⣺����1����
static OS_SEM p_KeyWriteMutex;    	 //XCAN�ڴ滥����
static OS_SEM p_XgMasterOrder;

static OS_SEM p_232aSendSem;
static OS_SEM Rs232SendData;		//����һ���ź��������ڷ��ʹ�����Դ
static OS_SEM p_SoundStateMutex;    //�ڲ��ڴ��������

static OS_SEM p_uartTTLSem;
static OS_SEM p_uartTTLXgv2Sem;

void initOscfg(void)
{
    INT16U ii;

    os_sem_init(Rs232SendData, 0);
    os_sem_init(Uart1RxReady, 0);
	os_sem_init(Uart2RxReady, 0);
	os_sem_init(Uart3RxReady, 0);
    os_sem_init(p_XgNewOrder, 0);
	
    os_sem_init(KeyUartRxReady, 0);
    os_sem_init(FecBusRxRespond, 0);
 
    os_sem_init(PowerRespond, 0);
	
	os_sem_init(p_232aAckSem, 0);
	os_sem_init(p_485AckSem, 0);
    
    os_sem_init(RelationRequest, 0);
	
	os_sem_init(Can1Ready, 0);
    os_sem_init(Can2Ready, 0);
    os_sem_init(p_XgMasterOrder, 0);
    
    os_sem_init(SemXgRespond, 0);
    

	
    os_sem_init(p_XgClientRespond, 0);
    
    os_sem_init(LoopRxReady, 0);
    os_sem_init(MultiKeyRxReady, 0);
    os_sem_init(p_fileDownLoadSem, 0);
    os_sem_init(MultiDriverRxReady, 0);
    os_sem_init(ExKeyBoardRxReady, 0);
	os_sem_init(KeyBoardRxReady, 0);
    os_sem_init(BusRxReady, 0);

	os_sem_init(p_uartTTLSem, 0);
	os_sem_init(p_uartTTLXgv2Sem, 0);
	
	os_mut_init(HashMutex);
    os_mut_init(EventMsg);
	//����
	os_mut_init(p_MboxNorBuf);
    os_mut_init(p_XgMemMutex);
    os_mut_init(MboxUart1);
    
    os_mut_init(p_Can1Mutex);
    os_mut_init(p_Can2Mutex);
    os_mut_init(LoopMutex);
	os_mut_init(XgComApiMutex);
	os_mut_init(p_XgMainMemMutex);
	
	os_mut_init(p_PartManageMutex);
    os_mut_init(p_MsgManageMutex);
	os_mut_init(p_RelayStartMutex);
    os_mut_init(p_KeyWriteMutex);
    os_mut_init(p_SoundStateMutex);

    os_mut_init(p_AtSendMutex);
    os_mut_init(p_XccSyncMutex);
	
    os_mut_init(p_atRespNoticeSem);
	os_mut_init(p_moduleAppSem);
    os_mut_init(p_xLinkResponSem);
    os_mut_init(p_XcclinkSem);
	
	os_mut_init(p_rs485Mutex);
}

void delay_ms(INT16U ms)
{
	INT32U ticks;
		
	ticks = ms / 5;
	ticks = (ticks == 0) ? 1 : ticks;	
	OSTimeDly(ticks);
}

void clearSemaphore(OS_ID sem)
{
    while(os_sem_wait(sem , 0) == OS_R_OK)
	{
		;
	}
}

void setSoundMutex(BOOLEAN Mutex)
{
	INT8U err;
    
    if(Mutex == 0)
    {
       os_mut_release(p_SoundStateMutex);
    }
    else
    {
       os_mut_wait(p_SoundStateMutex, 0xFFFF);		
    } 	
}

void setXTimerMutex(BOOLEAN Mutex)
{
	INT8U err;
    
    if(Mutex == 0)
    {
       os_mut_release(p_xtimerMutex);
    }
    else
    {
       os_mut_wait(p_xtimerMutex, 0xFFFF);		
    } 	
}

void postRs232aSendSem(void)
{
	os_sem_send(p_232aSendSem);	
}

BOOLEAN pendRs232SendSem(void)
{
	INT8U err;
	
	os_sem_wait(p_232aSendSem, 0xFFFF);
	
	return err == OS_ERR_NONE;	
}

void postXgv2AckSem(void)
{
	os_sem_send(p_232aAckSem);	
}

void clearXgv2AckSem(void)
{
	clearSemaphore(p_232aAckSem);
}

BOOLEAN pendXgv2AckSem(INT16U ms)
{
	if(os_sem_wait(p_232aAckSem , ms) == OS_R_SEM)
	{
		return 1;
	}
	
	return 0;
}

//���Ƿ���,0��ռ��
void setRs232Mutex(BOOLEAN mutex)
{
    INT8U err;
	
    if(mutex != 0)
    {
		os_mut_release(p_rs232Mutex);
    }
    else
    {
		os_mut_wait(p_rs232Mutex, 0xFFFF);		
    } 	
}

void setRs485Mutex(BOOLEAN Mutex)
{
	INT8U err;
    
    if(Mutex == 0)
    {
       os_mut_release(p_rs485Mutex);
    }
    else
    {
       os_mut_wait(p_rs485Mutex, 0xFFFF);		
    } 	
}

void PostRelationEvent(void)
{
	os_sem_send(RelationRequest);	
}

BOOLEAN PendRelationEvent(void)
{
	INT8U err;
	
	os_sem_wait(RelationRequest , 1000);
	
	return err == OS_ERR_NONE;	
}


BOOLEAN PendXgMasterOrder(INT16U ms)
{
    INT8U err;
	
	os_sem_wait(p_XgMasterOrder , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}

void PostXgMasterOrder(void)
{
    os_sem_send(p_XgMasterOrder);
}

BOOLEAN PendXCan2Event(INT16U ms)
{
    INT8U err;
	
	os_sem_wait(Can2Ready , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}

void PostXCan2NewEvent(void)
{
    os_sem_send(Can2Ready);
}



//�ӻ��µ�Ӧ��
void postXgSemRespond(void)
{	
	os_sem_send(SemXgRespond);	
}

BOOLEAN pendXgSemRespond(INT16U ms)
{
	INT8U err;
	
	os_sem_wait(SemXgRespond , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}
void clearXgSemRespond(void)
{
	clearSemaphore(SemXgRespond);	
}

//�ӻ��µ�Ӧ��
void PostXgNewOrder(void)
{	
	os_sem_send(p_XgNewOrder);	
}

BOOLEAN PendXgNewOrder(INT16U ms)
{
	INT8U err;    
	
	os_sem_wait(p_XgNewOrder , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}





void PostUartNewEvent(INT8U Index)
{
    if(Index == 1)      //RS485
	{
		os_sem_send(Uart1RxReady);
	}
	else if(Index == 2)     //RS232A
	{
		os_sem_send(Uart2RxReady);
	}
	else if(Index == 3)     //RS232B
	{
		os_sem_send(Uart3RxReady);
	}
}
BOOLEAN PendUartNewEvent(INT8U Index , INT16U ms)
{
    INT8U err;
	
	if(Index == 1)
	{
		os_sem_wait(Uart1RxReady , ms);
	}
	else if(Index == 2)
	{
		os_sem_wait(Uart2RxReady , ms);
	}
	else if(Index == 3)
	{
		os_sem_wait(Uart3RxReady , ms);
	}
    
    if(err == OS_ERR_NONE)
	{
		return 1;
	}
    return 0;
}

void setRelayStartListMutex(BOOLEAN Mutex)
{
	INT8U err;
    
    if(Mutex == 0)
    {
		os_mut_release(p_RelayStartMutex);
    }
    else
    {
		os_mut_wait(p_RelayStartMutex, 0xFFFF);		
    } 	
}


void setXccSyncMutex(BOOLEAN mutex)
{
	INT8U err;
    
    if(mutex)
    {
		os_mut_wait(p_XccSyncMutex, 0xFFFF);
    }
    else
    {
 		os_mut_release(p_XccSyncMutex);
    }
}


void setLoopMutex(BOOLEAN mutex)
{
	INT8U err;
    
    if(mutex)
    {
 		os_mut_wait(LoopMutex, 0xFFFF);
    }
    else
    {
		os_mut_release(LoopMutex);
    }
}

void ClearKeyBoardNewEvent(void)
{	
	clearSemaphore(KeyBoardRxReady);	
}
void PostKeyBoardNewEvent(void)
{
    os_sem_send(KeyBoardRxReady);	
}

BOOLEAN PendKeyBoardNewEvent(INT16U ms)
{
    INT8U err;
	
	os_sem_wait(KeyBoardRxReady , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}

void ClearExKeyBoardNewEvent(void)
{	
	clearSemaphore(ExKeyBoardRxReady);	
}
void PostExKeyBoardNewEvent(void)
{
    os_sem_send(ExKeyBoardRxReady);	
}
BOOLEAN PendExKeyBoardNewEvent(INT16U ms)
{
    INT8U err;
	
	os_sem_wait(ExKeyBoardRxReady , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}

void clearLoopNewEvent(void)
{	
	clearSemaphore(LoopRxReady);	
}

void postLoopNewEvent(void)
{
    os_sem_send(LoopRxReady);	
}
BOOLEAN pendLoopNewEvent(INT16U ms)
{
    INT8U err;
	
	os_sem_wait(LoopRxReady , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}


//�ȴ�XCAN������
void setXCan1Mutex(BOOLEAN Mutx)
{
    INT8U err;
    
    if(Mutx == 0)
    {
		os_mut_release(p_Can1Mutex);
	}
	else
	{
		os_mut_wait(p_Can1Mutex, 0xFFFF);
    }
}

void setBusLockMutex(BOOLEAN Mutex)
{
    INT8U err;
    
	if(Mutex == 0)
	{
		os_mut_release(busLockMutex);
	}
	else
	{
		os_mut_wait(busLockMutex, 0xFFFF);
	}
}

void SetHashReadMutex(BOOLEAN Mutex)
{
    INT8U err;
    

    {
        if(Mutex == 0)
        {
 //           os_mut_release(HashMutex);
        }
        else
        {
 //           os_mut_wait(HashMutex, 0xFFFF);
        }
    }	
}

//�ȴ�XCAN������
void setXCan2Mutex(BOOLEAN Mutx)
{
    INT8U err;
    

    {
        if(Mutx == 0)
        {
 //           os_mut_release(p_Can2Mutex);
        }
        else
        {
 //           os_mut_wait(p_Can2Mutex, 0xFFFF);
        }
    }
}



void setMsgMutex(BOOLEAN Mutex)
{
	INT8U err;

	if(Mutex != 0)
	{
		os_mut_wait(p_MsgManageMutex, 0xFFFF);
	}
	else
	{
		os_mut_release(p_MsgManageMutex);
	}
}

//����������
void setPartMutex(BOOLEAN Mutex)
{
	INT8U err;
	
	if(Mutex != 0)
	{
		os_mut_wait(p_PartManageMutex, 0xFFFF);
	}
	else
	{
		os_mut_release(p_PartManageMutex);
	}
}

void SetKeyWriteMutex(BOOLEAN Mute)
{
	INT8U err;
	
	if(Mute != 0)
	{
		os_mut_wait(p_KeyWriteMutex, 0xFFFF);
	}
	else
	{
		os_mut_release(p_KeyWriteMutex);
	}
}

//�ӻ��µ�Ӧ��
void PostXgRespond(void)
{	
	os_sem_send(p_XgClientRespond);	
}

void ClearXgRespond(void)
{	
	clearSemaphore(p_XgClientRespond);	
}

BOOLEAN PendXgRespond(INT16U ms)
{
	INT8U err;
	
	os_sem_wait(p_XgClientRespond , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}


void SetXgMemMutex(BOOLEAN Mutex)
{
	INT8U err;
	
	if(Mutex == 0)
	{
	   os_mut_release(p_XgMemMutex);
	}
	else
	{
	   os_mut_wait(p_XgMemMutex, 0xFFFF);		
	}
}

void setSpiMutex(BOOLEAN mutex)
{
    INT8U err;
    
    if(mutex)
    {
		os_mut_wait(p_MboxNorBuf, 0xFFFF);
    }
    else
    {
		os_mut_release(p_MboxNorBuf);	
    }
}

/**
 * �ͷ�ģ�� ATָ��ͻ�����
 *
 * @param none
 * @return none
 */
void SetAtSendCmdMutex(BOOLEAN Mutex)
{
    INT8U err;
	
	if(Mutex == 0)
	{
	   os_mut_release(p_AtSendMutex);
	}
	else
	{
	   os_mut_wait(p_AtSendMutex, 0xFFFF);		
	}  
}

//����Ӧ��
void postAtRespSemRespond(void)
{
    os_sem_send(p_atRespNoticeSem);
}

void clearAtRespSemRespond(void)
{
    clearSemaphore(p_atRespNoticeSem);	
}

//����ֵ0��ʾ��ʱ��1��ʾ��Ӧ��
BOOLEAN pendAtRespSemRespond(INT16U ms)
{
    INT8U err;
	
	os_sem_wait(p_atRespNoticeSem , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;
}

void postModuleAppSem(void)
{
    os_sem_send(p_moduleAppSem);
}

BOOLEAN pendModuleAppSem(INT16U ms)
{
    INT8U err;
	
	clearSemaphore(p_moduleAppSem);	
	os_sem_wait(p_moduleAppSem , ms);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;	
}


void postXccLinkNewRespond(void)
{
    os_sem_send(p_XcclinkSem);
}

BOOLEAN pendXccLinkNewRespond(INT16U second)
{
	INT8U err;
    INT32U WaitDelay;
    
    WaitDelay = second * 1000;    
	
	os_sem_wait(p_XcclinkSem , WaitDelay);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;	
}

//Xlink���� ���շ������ظ�
void postXLinkNewRespond(void)
{
    os_sem_send(p_xLinkResponSem);
}

BOOLEAN pendXLinkNewRespond(INT16U second)
{
	INT8U err;
    INT32U WaitDelay;
    
    WaitDelay = second * 1000;    
	
	os_sem_wait(p_xLinkResponSem , WaitDelay);	
	
	if(err == OS_ERR_TIMEOUT)
	{
		return 0;
	}
	return 1;	
}

void clearXLinkNewRespond(void)
{	
    clearSemaphore(p_xLinkResponSem);	
}

void setRtcDriverMutex(BOOLEAN mutex)
{
	
}

void postUartTTLRespond(void)
{
    os_sem_send(p_uartTTLSem);
}

BOOLEAN pendUartTTLRespond(INT16U ms)
{
	if(os_sem_wait(p_uartTTLSem , ms) == OS_R_SEM)
	{
		return 1;
	}
	
	return 0 ;	
}

void clearUartTTLRespond(void)
{	
    clearSemaphore(p_uartTTLSem);	
}


void postTTLXgv2Sem(void)
{
    os_sem_send(p_uartTTLXgv2Sem);
}

BOOLEAN pendTTLXgv2Sem(INT16U ms)
{
	if(os_sem_wait(p_uartTTLXgv2Sem , ms) == OS_R_SEM)
	{
		return 1;
	}
	
	return 0 ;	
}

void clearTTLXgv2Sem(void)
{	
    clearSemaphore(p_uartTTLXgv2Sem);	
}

void postRs485AckSem(void)
{
	isr_sem_send(p_485AckSem);
}

BOOLEAN pendRs485AckSem(INT16U ms)
{
	if(os_sem_wait(p_485AckSem , ms) == OS_R_SEM)
	{
		return 1;
	}
	
	return 0 ;	
}

void clearRs485AckSem(void)
{
    clearSemaphore(p_485AckSem);	
}


