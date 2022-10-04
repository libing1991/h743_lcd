#ifndef OSCFG_INC
#define OSCFG_INC

#include "includes.h"

	void initOscfg(void);
	void setSpiMutex(BOOLEAN mutex);
	void SetHashReadMutex(BOOLEAN Mutex);
	
	
	void setMsgMutex(BOOLEAN Mutex);
	
	void setRs485Mutex(BOOLEAN Mutex);
	
	BOOLEAN PendXCan2Event(INT16U Delay);
    
    void clearLoopNewEvent(void);
    void postLoopNewEvent(void);
    BOOLEAN pendLoopNewEvent(INT16U ms);
    
    void setLoopMutex(BOOLEAN mutex);
    void setXccSyncMutex(BOOLEAN mutex);
    
    void postXccLinkNewRespond(void);
    BOOLEAN pendXccLinkNewRespond(INT16U second);
	void setRtcDriverMutex(BOOLEAN mutex);
	void setPartMutex(BOOLEAN Mutex);
    
    void clearXLinkNewRespond(void);
    BOOLEAN pendXLinkNewRespond(INT16U second);
    void postXLinkNewRespond(void);
    
    void postXgSemRespond(void);
    BOOLEAN pendXgSemRespond(INT16U ms);
    void clearXgSemRespond(void);
	

	
	void Post232aAckSem(void);
	void Clear232aAckSem(void);
	BOOLEAN Pend232aAckSem(INT16U ms);
	
	void PostUartNewEvent(INT8U Index);
	BOOLEAN PendUartNewEvent(INT8U Index , INT16U Delay);
	
	void setSoundMutex(BOOLEAN Mutex);
	
	void setRelayStartListMutex(BOOLEAN Mutex);
	
	BOOLEAN PendXgMasterOrder(INT16U ms);
	
	void PostXCan2NewEvent(void);
	BOOLEAN pendModuleAppSem(INT16U mill);
	void PostRelationEvent(void);
	BOOLEAN PendXgMasterOrder(INT16U ms);
	void SetKeyWriteMutex(BOOLEAN Mute);
	void SetXgMemMutex(BOOLEAN Mutex);
	//操作系统消息
	void PostXgMasterOrder(void);
	BOOLEAN PendXgMasterOrder(INT16U ms);
	void PostMasterXgRespond(INT8U SemIndex);
	void ClearMasterXgRespond(INT8U SemIndex);
	BOOLEAN PendMasterXgRespond(INT8U SemIndex , INT16U WaitTimer);
	
	void setBusLockMutex(BOOLEAN Mutex);
	
	void postRs232aSendSem(void);
	BOOLEAN pendRs232SendSem(void);
	
	void setRs232Mutex(BOOLEAN mutex);
	
	void postUartTTLRespond(void);
	BOOLEAN pendUartTTLRespond(INT16U second);
	void clearUartTTLRespond(void);
	
	void postTTLXgv2Sem(void);
	BOOLEAN pendTTLXgv2Sem(INT16U ms);
	void clearTTLXgv2Sem(void);
	
	void postXgv2AckSem(void);
	BOOLEAN pendXgv2AckSem(INT16U ms);
	void clearXgv2AckSem(void);

	void postRs485AckSem(void);
	BOOLEAN pendRs485AckSem(INT16U ms);
	void clearRs485AckSem(void);
	
#endif
