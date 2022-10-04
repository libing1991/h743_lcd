
#ifndef  __INCLUDES_INC
#define  __INCLUDES_INC


#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned char  BOOLEAN;                                         /*  Boolean 布尔变量            */
typedef unsigned char  INT8U;                                           /*  Unsigned  8 bit quantity    */                       
typedef signed   char  INT8S;                                           /*  Signed    8 bit quantity    */                         
typedef unsigned short INT16U;                                          /*  Unsigned 16 bit quantity    */
typedef signed   short INT16S;                                          /*  Signed   16 bit quantity    */
typedef unsigned int   INT32U;                                          /*  Unsigned 32 bit quantity    */
typedef signed   int   INT32S;                                          /*  Signed   32 bit quantity    */
typedef float          FP32;                                            /*  Single precision floating 
                                                                            point                       */
typedef double         FP64;                                            /*  Double precision floating 
                                                                            point                       */

typedef unsigned int   OS_STK;                                          /*  Each stack entry is 32-bit 
                                                                            wide  堆栈是32位宽度        */   
typedef unsigned int   OS_CPU_SR;                                       /*  Define size of CPU status 
                                                                            register (PSR = 32 bits)    */

/*********************************************************************************************************
  Date types(Compiler specific)  数据类型（和编译器相关）                
*********************************************************************************************************/
typedef unsigned char   uint8;                                          /* Unsigned  8 bit quantity     */
typedef signed   char   int8;                                           /* Signed    8 bit quantity     */
typedef unsigned short  uint16;                                         /* Unsigned  16 bit quantity    */
typedef signed   short  int16;                                          /* Signed    16 bit quantity    */
typedef unsigned int    uint32;                                         /* Unsigned  32 bit quantity    */
typedef signed   int    int32;                                          /* Signed    32 bit quantity    */
typedef float           fp32;                                           /* Single    precision          */
                                                                        /* floating  point              */
typedef double          fp64;                                           /* Double    precision          */                                                                        /* floating  point              */

#include "stdio.h"
#include "string.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL  0
#endif



#ifdef __cplusplus
    }
#endif

typedef enum
{
	HBUS = 1,
	TBUS = 3,
	XBUS = 4,
}BUSPROTOCAL;


	typedef	union
	{
		INT32U DataL;
		INT8U Data[4];
	}INT32U_UNION;


	
typedef struct
{
	INT16U groupCount;
	INT16U partIndex;
	INT32U firstGroup;
}GROUPINFO_STRU;
	
#include "config.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "xdatastru.h"
#include "xdatatype.h"
#include "xsystem.h"

typedef BOOLEAN (*rxCallBack)(INT8U srcAddress, INT8U *rxBuffer , INT16U length, void *user);

extern INT16U GetHostType(void);
extern BOOLEAN GetGasName(INT16U Gas ,char str[]);
extern BOOLEAN GetGasName(INT16U Gas ,char str[]);
extern void delay_ms(INT16U ms);

extern void GetPartSencondDescribe(BOOLEAN OutPart , void *Node ,char *Describe);

#endif







