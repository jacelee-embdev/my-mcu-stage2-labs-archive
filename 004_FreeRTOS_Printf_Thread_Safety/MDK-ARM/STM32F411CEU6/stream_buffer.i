#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"

























 

 
#line 1 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"
 
 





 









     
#line 27 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"
     











#line 46 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"





 

     

     
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       __int64 int64_t;

     
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;

     

     
     
typedef   signed          char int_least8_t;
typedef   signed short     int int_least16_t;
typedef   signed           int int_least32_t;
typedef   signed       __int64 int_least64_t;

     
typedef unsigned          char uint_least8_t;
typedef unsigned short     int uint_least16_t;
typedef unsigned           int uint_least32_t;
typedef unsigned       __int64 uint_least64_t;

     

     
typedef   signed           int int_fast8_t;
typedef   signed           int int_fast16_t;
typedef   signed           int int_fast32_t;
typedef   signed       __int64 int_fast64_t;

     
typedef unsigned           int uint_fast8_t;
typedef unsigned           int uint_fast16_t;
typedef unsigned           int uint_fast32_t;
typedef unsigned       __int64 uint_fast64_t;

     




typedef   signed           int intptr_t;
typedef unsigned           int uintptr_t;


     
typedef   signed     long long intmax_t;
typedef unsigned     long long uintmax_t;




     

     





     





     





     

     





     





     





     

     





     





     





     

     






     






     






     

     


     


     


     

     
#line 216 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"

     



     






     
    
 



#line 241 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"

     







     










     











#line 305 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"






 
#line 30 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
#line 1 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
 
 
 
 




 








 












#line 38 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"


  



    typedef unsigned int size_t;    
#line 54 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"




extern __declspec(__nothrow) void *memcpy(void * __restrict  ,
                    const void * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) void *memmove(void *  ,
                    const void *  , size_t  ) __attribute__((__nonnull__(1,2)));
   







 
extern __declspec(__nothrow) char *strcpy(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) char *strncpy(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   





 

extern __declspec(__nothrow) char *strcat(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) char *strncat(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 






 

extern __declspec(__nothrow) int memcmp(const void *  , const void *  , size_t  ) __attribute__((__nonnull__(1,2)));
   





 
extern __declspec(__nothrow) int strcmp(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) int strncmp(const char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int strcasecmp(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   





 
extern __declspec(__nothrow) int strncasecmp(const char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int strcoll(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   







 

extern __declspec(__nothrow) size_t strxfrm(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(2)));
   













 


#line 193 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) void *memchr(const void *  , int  , size_t  ) __attribute__((__nonnull__(1)));

   





 

#line 209 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strchr(const char *  , int  ) __attribute__((__nonnull__(1)));

   




 

extern __declspec(__nothrow) size_t strcspn(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   




 

#line 232 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strpbrk(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));

   




 

#line 247 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strrchr(const char *  , int  ) __attribute__((__nonnull__(1)));

   





 

extern __declspec(__nothrow) size_t strspn(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   



 

#line 270 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strstr(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));

   





 

extern __declspec(__nothrow) char *strtok(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(2)));
extern __declspec(__nothrow) char *_strtok_r(char *  , const char *  , char **  ) __attribute__((__nonnull__(2,3)));

extern __declspec(__nothrow) char *strtok_r(char *  , const char *  , char **  ) __attribute__((__nonnull__(2,3)));

   

































 

extern __declspec(__nothrow) void *memset(void *  , int  , size_t  ) __attribute__((__nonnull__(1)));
   



 
extern __declspec(__nothrow) char *strerror(int  );
   





 
extern __declspec(__nothrow) size_t strlen(const char *  ) __attribute__((__nonnull__(1)));
   



 

extern __declspec(__nothrow) size_t strlcpy(char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   
















 

extern __declspec(__nothrow) size_t strlcat(char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






















 

extern __declspec(__nothrow) void _membitcpybl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpybb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpyhl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpyhb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpywl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpywb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovebl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovebb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovehl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovehb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovewl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovewb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
    














































 







#line 502 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"



 

#line 31 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"



 


 
#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"

























 






 
#line 1 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"
 






 

 
 
 





 





#line 34 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"




  typedef signed int ptrdiff_t;



  



    typedef unsigned int size_t;    
#line 57 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"



   



      typedef unsigned short wchar_t;  
#line 82 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"



    




   




  typedef long double max_align_t;









#line 114 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"



 

#line 35 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"













 
#line 50 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"





 
#line 1 "../Core/Inc/FreeRTOSConfig.h"
 


























 
 














 

 
 
 

 
#line 53 "../Core/Inc/FreeRTOSConfig.h"
  extern uint32_t SystemCoreClock;








#line 80 "../Core/Inc/FreeRTOSConfig.h"
 

 

 

 



 





 
#line 103 "../Core/Inc/FreeRTOSConfig.h"


 
#line 119 "../Core/Inc/FreeRTOSConfig.h"




 


 
#line 133 "../Core/Inc/FreeRTOSConfig.h"


 





 



 


 



 
 

 


 



 



 
 
 

#line 57 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"

 
#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/projdefs.h"

























 







 
typedef void (*TaskFunction_t)( void * );



 












 




 











 
#line 110 "../Middlewares/Third_Party/FreeRTOS/Source/include/projdefs.h"


 



 








#line 60 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"

 
#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/portable.h"

























 



 













 
#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/deprecated_definitions.h"

























 












 











































































































































































#line 218 "../Middlewares/Third_Party/FreeRTOS/Source/include/deprecated_definitions.h"

#line 226 "../Middlewares/Third_Party/FreeRTOS/Source/include/deprecated_definitions.h"







#line 240 "../Middlewares/Third_Party/FreeRTOS/Source/include/deprecated_definitions.h"








































#line 46 "../Middlewares/Third_Party/FreeRTOS/Source/include/portable.h"




 
#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/portmacro.h"

























 

















 

 
#line 54 "../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/portmacro.h"

typedef uint32_t StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;





	typedef uint32_t TickType_t;


	
 


 

 




 


 

 
#line 93 "../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/portmacro.h"
 





 

 
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#line 111 "../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/portmacro.h"

 

 

	extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );


 

 




#line 142 "../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/portmacro.h"
 



 


 


	void vPortValidateInterruptPriority( void );



 








 

static __forceinline void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
	__asm
	{
		
 
		msr basepri, ulBASEPRI
	}
}
 

static __forceinline void vPortRaiseBASEPRI( void )
{
uint32_t ulNewBASEPRI = ( 5 << (8 - 4) );

	__asm
	{
		
 
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
}
 

static __forceinline void vPortClearBASEPRIFromISR( void )
{
	__asm
	{
		

 
		msr basepri, #0
	}
}
 

static __forceinline uint32_t ulPortRaiseBASEPRI( void )
{
uint32_t ulReturn, ulNewBASEPRI = ( 5 << (8 - 4) );

	__asm
	{
		
 
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}

	return ulReturn;
}
 

static __forceinline BaseType_t xPortIsInsideInterrupt( void )
{
uint32_t ulCurrentInterrupt;
BaseType_t xReturn;

	 
	__asm
	{
		mrs ulCurrentInterrupt, ipsr
	}

	if( ulCurrentInterrupt == 0 )
	{
		xReturn = ( ( BaseType_t ) 0 );
	}
	else
	{
		xReturn = ( ( BaseType_t ) 1 );
	}

	return xReturn;
}








#line 53 "../Middlewares/Third_Party/FreeRTOS/Source/include/portable.h"














































#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/mpu_wrappers.h"

























 





 
#line 179 "../Middlewares/Third_Party/FreeRTOS/Source/include/mpu_wrappers.h"











#line 100 "../Middlewares/Third_Party/FreeRTOS/Source/include/portable.h"






 
#line 117 "../Middlewares/Third_Party/FreeRTOS/Source/include/portable.h"
		StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters ) ;




 
typedef struct HeapRegion
{
	uint8_t *pucStartAddress;
	size_t xSizeInBytes;
} HeapRegion_t;

 
typedef struct xHeapStats
{
	size_t xAvailableHeapSpaceInBytes;		 
	size_t xSizeOfLargestFreeBlockInBytes; 	 
	size_t xSizeOfSmallestFreeBlockInBytes;  
	size_t xNumberOfFreeBlocks;				 
	size_t xMinimumEverFreeBytesRemaining;	 
	size_t xNumberOfSuccessfulAllocations;	 
	size_t xNumberOfSuccessfulFrees;		 
} HeapStats_t;











 
void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions ) ;




 
void vPortGetHeapStats( HeapStats_t *pxHeapStats );



 
void *pvPortMalloc( size_t xSize ) ;
void vPortFree( void *pv ) ;
void vPortInitialiseBlocks( void ) ;
size_t xPortGetFreeHeapSize( void ) ;
size_t xPortGetMinimumEverFreeHeapSize( void ) ;




 
BaseType_t xPortStartScheduler( void ) ;





 
void vPortEndScheduler( void ) ;







 











#line 63 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"

 




 







 































































































































































#line 243 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"





 
#line 255 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"









 


















































 

	
 




	
 




	
 




	
 




	 




	 




	
 




	



 




	


 




	


 




	


 




	


 















 





















































































































































































































































































































#line 732 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"




























































































































	
 



#line 867 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"

 


















#line 896 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"
	
 







 





	








 




	
 




	
 



#line 957 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"

	
 





	
 










 





 





 





 





 













































 












 
struct xSTATIC_LIST_ITEM
{



	TickType_t xDummy2;
	void *pvDummy3[ 4 ];



};
typedef struct xSTATIC_LIST_ITEM StaticListItem_t;

 
struct xSTATIC_MINI_LIST_ITEM
{



	TickType_t xDummy2;
	void *pvDummy3[ 2 ];
};
typedef struct xSTATIC_MINI_LIST_ITEM StaticMiniListItem_t;

 
typedef struct xSTATIC_LIST
{



	UBaseType_t uxDummy2;
	void *pvDummy3;
	StaticMiniListItem_t xDummy4;



} StaticList_t;













 
typedef struct xSTATIC_TCB
{
	void				*pxDummy1;



	StaticListItem_t	xDummy3[ 2 ];
	UBaseType_t			uxDummy5;
	void				*pxDummy6;
	uint8_t				ucDummy7[ ( 16 ) ];
#line 1129 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"
		UBaseType_t		uxDummy10[ 2 ];


		UBaseType_t		uxDummy12[ 2 ];
#line 1147 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"
		uint32_t 		ulDummy18;
		uint8_t 		ucDummy19;


		uint8_t			uxDummy20;


#line 1160 "../Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h"
} StaticTask_t;














 
typedef struct xSTATIC_QUEUE
{
	void *pvDummy1[ 3 ];

	union
	{
		void *pvDummy2;
		UBaseType_t uxDummy2;
	} u;

	StaticList_t xDummy3[ 2 ];
	UBaseType_t uxDummy4[ 3 ];
	uint8_t ucDummy5[ 2 ];


		uint8_t ucDummy6;







		UBaseType_t uxDummy8;
		uint8_t ucDummy9;


} StaticQueue_t;
typedef StaticQueue_t StaticSemaphore_t;














 
typedef struct xSTATIC_EVENT_GROUP
{
	TickType_t xDummy1;
	StaticList_t xDummy2;


		UBaseType_t uxDummy3;



			uint8_t ucDummy4;


} StaticEventGroup_t;














 
typedef struct xSTATIC_TIMER
{
	void				*pvDummy1;
	StaticListItem_t	xDummy2;
	TickType_t			xDummy3;
	void 				*pvDummy5;
	TaskFunction_t		pvDummy6;

		UBaseType_t		uxDummy7;

	uint8_t 			ucDummy8;

} StaticTimer_t;














 
typedef struct xSTATIC_STREAM_BUFFER
{
	size_t uxDummy1[ 4 ];
	void * pvDummy2[ 3 ];
	uint8_t ucDummy3;

		UBaseType_t uxDummy4;

} StaticStreamBuffer_t;

 
typedef StaticStreamBuffer_t StaticMessageBuffer_t;







#line 39 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/task.h"

























 









#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/list.h"

























 



























 



































 












 

	 
#line 134 "../Middlewares/Third_Party/FreeRTOS/Source/include/list.h"




 
struct xLIST;
struct xLIST_ITEM
{
				 
	 TickType_t xItemValue;			 
	struct xLIST_ITEM *  pxNext;		 
	struct xLIST_ITEM *  pxPrevious;	 
	void * pvOwner;										 
	struct xLIST *  pvContainer;		 
				 
};
typedef struct xLIST_ITEM ListItem_t;					 

struct xMINI_LIST_ITEM
{
				 
	 TickType_t xItemValue;
	struct xLIST_ITEM *  pxNext;
	struct xLIST_ITEM *  pxPrevious;
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;



 
typedef struct xLIST
{
					 
	volatile UBaseType_t uxNumberOfItems;
	ListItem_t *  pxIndex;			 
	MiniListItem_t xListEnd;							 
					 
} List_t;







 








 








 









 








 







 







 







 








 




 





















 
#line 289 "../Middlewares/Third_Party/FreeRTOS/Source/include/list.h"

















 










 







 






 











 
void vListInitialise( List_t * const pxList ) ;









 
void vListInitialiseItem( ListItem_t * const pxItem ) ;











 
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem ) ;



















 
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem ) ;













 
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove ) ;







#line 37 "../Middlewares/Third_Party/FreeRTOS/Source/include/task.h"







 







 















 
struct tskTaskControlBlock;  
typedef struct tskTaskControlBlock* TaskHandle_t;




 
typedef BaseType_t (*TaskHookFunction_t)( void * );

 
typedef enum
{
	eRunning = 0,	 
	eReady,			 
	eBlocked,		 
	eSuspended,		 
	eDeleted,		 
	eInvalid		 
} eTaskState;

 
typedef enum
{
	eNoAction = 0,				 
	eSetBits,					 
	eIncrement,					 
	eSetValueWithOverwrite,		 
	eSetValueWithoutOverwrite	 
} eNotifyAction;



 
typedef struct xTIME_OUT
{
	BaseType_t xOverflowCount;
	TickType_t xTimeOnEntering;
} TimeOut_t;



 
typedef struct xMEMORY_REGION
{
	void *pvBaseAddress;
	uint32_t ulLengthInBytes;
	uint32_t ulParameters;
} MemoryRegion_t;



 
typedef struct xTASK_PARAMETERS
{
	TaskFunction_t pvTaskCode;
	const char * const pcName;	 
	uint16_t usStackDepth;
	void *pvParameters;
	UBaseType_t uxPriority;
	StackType_t *puxStackBuffer;
	MemoryRegion_t xRegions[ 1 ];



} TaskParameters_t;


 
typedef struct xTASK_STATUS
{
	TaskHandle_t xHandle;			 
	const char *pcTaskName;			   
	UBaseType_t xTaskNumber;		 
	eTaskState eCurrentState;		 
	UBaseType_t uxCurrentPriority;	 
	UBaseType_t uxBasePriority;		 
	uint32_t ulRunTimeCounter;		 
	StackType_t *pxStackBase;		 
	uint16_t usStackHighWaterMark;	 
} TaskStatus_t;

 
typedef enum
{
	eAbortSleep = 0,		 
	eStandardSleep,			 
	eNoTasksWaitingTimeout	 
} eSleepModeStatus;





 









 













 














 









 









 




 







 





























































































 

	BaseType_t xTaskCreate(	TaskFunction_t pxTaskCode,
							const char * const pcName,	 
							const uint16_t usStackDepth,
							void * const pvParameters,
							UBaseType_t uxPriority,
							TaskHandle_t * const pxCreatedTask ) ;












































































































 

	TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,
									const char * const pcName,  
									const uint32_t ulStackDepth,
									void * const pvParameters,
									UBaseType_t uxPriority,
									StackType_t * const puxStackBuffer,
									StaticTask_t * const pxTaskBuffer ) ;









































































 























































































 

















































 
void vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const pxRegions ) ;







































 
void vTaskDelete( TaskHandle_t xTaskToDelete ) ;



 














































 
void vTaskDelay( const TickType_t xTicksToDelay ) ;

























































 
void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement ) ;




























 
BaseType_t xTaskAbortDelay( TaskHandle_t xTask ) ;













































 
UBaseType_t uxTaskPriorityGet( const TaskHandle_t xTask ) ;






 
UBaseType_t uxTaskPriorityGetFromISR( const TaskHandle_t xTask ) ;
















 
eTaskState eTaskGetState( TaskHandle_t xTask ) ;






















































 
void vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState ) ;








































 
void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority ) ;

















































 
void vTaskSuspend( TaskHandle_t xTaskToSuspend ) ;















































 
void vTaskResume( TaskHandle_t xTaskToResume ) ;



























 
BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume ) ;



 



























 
void vTaskStartScheduler( void ) ;






















































 
void vTaskEndScheduler( void ) ;

















































 
void vTaskSuspendAll( void ) ;




















































 
BaseType_t xTaskResumeAll( void ) ;



 









 
TickType_t xTaskGetTickCount( void ) ;














 
TickType_t xTaskGetTickCountFromISR( void ) ;












 
UBaseType_t uxTaskGetNumberOfTasks( void ) ;











 
char *pcTaskGetName( TaskHandle_t xTaskToQuery ) ;  














 
TaskHandle_t xTaskGetHandle( const char *pcNameToQuery ) ;  

























 
UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask ) ;

























 
uint16_t uxTaskGetStackHighWaterMark2( TaskHandle_t xTask ) ;






 
#line 1509 "../Middlewares/Third_Party/FreeRTOS/Source/include/task.h"

#line 1521 "../Middlewares/Third_Party/FreeRTOS/Source/include/task.h"











 
BaseType_t xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter ) ;







 
TaskHandle_t xTaskGetIdleTaskHandle( void ) ;

































































































 
UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t * const pulTotalRunTime ) ;













































 
void vTaskList( char * pcWriteBuffer ) ;  




















































 
void vTaskGetRunTimeStats( char *pcWriteBuffer ) ;  




























 
uint32_t ulTaskGetIdleRunTimeCounter( void ) ;















































































 
BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue ) ;

























































































 
BaseType_t xTaskGenericNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue, BaseType_t *pxHigherPriorityTaskWoken ) ;











































































 
BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait ) ;












































 






















































 
void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken ) ;



































































 
uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ) ;














 
BaseType_t xTaskNotifyStateClear( TaskHandle_t xTask );
















 
uint32_t ulTaskNotifyValueClear( TaskHandle_t xTask, uint32_t ulBitsToClear ) ;












 
void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut ) ;

















































































 
BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait ) ;



 















 
BaseType_t xTaskIncrementTick( void ) ;































 
void vTaskPlaceOnEventList( List_t * const pxEventList, const TickType_t xTicksToWait ) ;
void vTaskPlaceOnUnorderedEventList( List_t * pxEventList, const TickType_t xItemValue, const TickType_t xTicksToWait ) ;











 
void vTaskPlaceOnEventListRestricted( List_t * const pxEventList, TickType_t xTicksToWait, const BaseType_t xWaitIndefinitely ) ;
























 
BaseType_t xTaskRemoveFromEventList( const List_t * const pxEventList ) ;
void vTaskRemoveFromUnorderedEventList( ListItem_t * pxEventListItem, const TickType_t xItemValue ) ;








 
 void vTaskSwitchContext( void ) ;




 
TickType_t uxTaskResetEventItemValue( void ) ;



 
TaskHandle_t xTaskGetCurrentTaskHandle( void ) ;




 
void vTaskMissedYield( void ) ;




 
BaseType_t xTaskGetSchedulerState( void ) ;




 
BaseType_t xTaskPriorityInherit( TaskHandle_t const pxMutexHolder ) ;




 
BaseType_t xTaskPriorityDisinherit( TaskHandle_t const pxMutexHolder ) ;








 
void vTaskPriorityDisinheritAfterTimeout( TaskHandle_t const pxMutexHolder, UBaseType_t uxHighestPriorityWaitingTask ) ;



 
UBaseType_t uxTaskGetTaskNumber( TaskHandle_t xTask ) ;




 
void vTaskSetTaskNumber( TaskHandle_t xTask, const UBaseType_t uxHandle ) ;








 
void vTaskStepTick( const TickType_t xTicksToJump ) ;











 
BaseType_t xTaskCatchUpTicks( TickType_t xTicksToCatchUp ) ;














 
eSleepModeStatus eTaskConfirmSleepModeStatus( void ) ;




 
TaskHandle_t pvTaskIncrementMutexHeldCount( void ) ;




 
void vTaskInternalSetTimeOutState( TimeOut_t * const pxTimeOut ) ;









#line 40 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
#line 1 "../Middlewares/Third_Party/FreeRTOS/Source/include/stream_buffer.h"

























 






















 

















 
struct StreamBufferDef_t;
typedef struct StreamBufferDef_t * StreamBufferHandle_t;


































































 
















































































 





























































































 
size_t xStreamBufferSend( StreamBufferHandle_t xStreamBuffer,
						  const void *pvTxData,
						  size_t xDataLengthBytes,
						  TickType_t xTicksToWait ) ;
































































































 
size_t xStreamBufferSendFromISR( StreamBufferHandle_t xStreamBuffer,
								 const void *pvTxData,
								 size_t xDataLengthBytes,
								 BaseType_t * const pxHigherPriorityTaskWoken ) ;




















































































 
size_t xStreamBufferReceive( StreamBufferHandle_t xStreamBuffer,
							 void *pvRxData,
							 size_t xBufferLengthBytes,
							 TickType_t xTicksToWait ) ;

















































































 
size_t xStreamBufferReceiveFromISR( StreamBufferHandle_t xStreamBuffer,
									void *pvRxData,
									size_t xBufferLengthBytes,
									BaseType_t * const pxHigherPriorityTaskWoken ) ;




















 
void vStreamBufferDelete( StreamBufferHandle_t xStreamBuffer ) ;


















 
BaseType_t xStreamBufferIsFull( StreamBufferHandle_t xStreamBuffer ) ;


















 
BaseType_t xStreamBufferIsEmpty( StreamBufferHandle_t xStreamBuffer ) ;





















 
BaseType_t xStreamBufferReset( StreamBufferHandle_t xStreamBuffer ) ;



















 
size_t xStreamBufferSpacesAvailable( StreamBufferHandle_t xStreamBuffer ) ;



















 
size_t xStreamBufferBytesAvailable( StreamBufferHandle_t xStreamBuffer ) ;



































 
BaseType_t xStreamBufferSetTriggerLevel( StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel ) ;





































 
BaseType_t xStreamBufferSendCompletedFromISR( StreamBufferHandle_t xStreamBuffer, BaseType_t *pxHigherPriorityTaskWoken ) ;






































 
BaseType_t xStreamBufferReceiveCompletedFromISR( StreamBufferHandle_t xStreamBuffer, BaseType_t *pxHigherPriorityTaskWoken ) ;

 
StreamBufferHandle_t xStreamBufferGenericCreate( size_t xBufferSizeBytes,
												 size_t xTriggerLevelBytes,
												 BaseType_t xIsMessageBuffer ) ;

StreamBufferHandle_t xStreamBufferGenericCreateStatic( size_t xBufferSizeBytes,
													   size_t xTriggerLevelBytes,
													   BaseType_t xIsMessageBuffer,
													   uint8_t * const pucStreamBufferStorageArea,
													   StaticStreamBuffer_t * const pxStaticStreamBuffer ) ;

size_t xStreamBufferNextMessageLengthBytes( StreamBufferHandle_t xStreamBuffer ) ;


	void vStreamBufferSetStreamBufferNumber( StreamBufferHandle_t xStreamBuffer, UBaseType_t uxStreamBufferNumber ) ;
	UBaseType_t uxStreamBufferGetStreamBufferNumber( StreamBufferHandle_t xStreamBuffer ) ;
	uint8_t ucStreamBufferGetStreamBufferType( StreamBufferHandle_t xStreamBuffer ) ;






#line 41 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"








 




 
 
#line 70 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"

#line 91 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"



 
#line 109 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"

#line 129 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
 

 


 



 

 
typedef struct StreamBufferDef_t  
{
	volatile size_t xTail;				 
	volatile size_t xHead;				 
	size_t xLength;						 
	size_t xTriggerLevelBytes;			 
	volatile TaskHandle_t xTaskWaitingToReceive;  
	volatile TaskHandle_t xTaskWaitingToSend;	 
	uint8_t *pucBuffer;					 
	uint8_t ucFlags;


		UBaseType_t uxStreamBufferNumber;		 

} StreamBuffer_t;



 
static size_t prvBytesInBuffer( const StreamBuffer_t * const pxStreamBuffer ) ;






 
static size_t prvWriteBytesToBuffer( StreamBuffer_t * const pxStreamBuffer, const uint8_t *pucData, size_t xCount ) ;







 
static size_t prvReadMessageFromBuffer( StreamBuffer_t *pxStreamBuffer,
										void *pvRxData,
										size_t xBufferLengthBytes,
										size_t xBytesAvailable,
										size_t xBytesToStoreMessageLength ) ;







 
static size_t prvWriteMessageToBuffer(  StreamBuffer_t * const pxStreamBuffer,
										const void * pvTxData,
										size_t xDataLengthBytes,
										size_t xSpace,
										size_t xRequiredSpace ) ;




 
static size_t prvReadBytesFromBuffer( StreamBuffer_t *pxStreamBuffer,
									  uint8_t *pucData,
									  size_t xMaxCount,
									  size_t xBytesAvailable ) ;




 
static void prvInitialiseNewStreamBuffer( StreamBuffer_t * const pxStreamBuffer,
										  uint8_t * const pucBuffer,
										  size_t xBufferSizeBytes,
										  size_t xTriggerLevelBytes,
										  uint8_t ucFlags ) ;

 



	StreamBufferHandle_t xStreamBufferGenericCreate( size_t xBufferSizeBytes, size_t xTriggerLevelBytes, BaseType_t xIsMessageBuffer )
	{
	uint8_t *pucAllocatedMemory;
	uint8_t ucFlags;

		


 
		if( xIsMessageBuffer == ( ( BaseType_t ) 1 ) )
		{
			 
			ucFlags = ( ( uint8_t ) 1 );
			if ((xBufferSizeBytes > ( sizeof( size_t ) )) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		}
		else
		{
			 
			ucFlags = 0;
			if ((xBufferSizeBytes > 0) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		}
		if ((xTriggerLevelBytes <= xBufferSizeBytes) == 0) {vPortRaiseBASEPRI(); for( ;; );};

		
 
		if( xTriggerLevelBytes == ( size_t ) 0 )
		{
			xTriggerLevelBytes = ( size_t ) 1;
		}

		






 
		xBufferSizeBytes++;
		pucAllocatedMemory = ( uint8_t * ) pvPortMalloc( xBufferSizeBytes + sizeof( StreamBuffer_t ) );  

		if( pucAllocatedMemory != 0 )
		{
			prvInitialiseNewStreamBuffer( ( StreamBuffer_t * ) pucAllocatedMemory,      
										   pucAllocatedMemory + sizeof( StreamBuffer_t ),     
										   xBufferSizeBytes,
										   xTriggerLevelBytes,
										   ucFlags );

			;
		}
		else
		{
			;
		}

		return ( StreamBufferHandle_t ) pucAllocatedMemory;  
	}


 



	StreamBufferHandle_t xStreamBufferGenericCreateStatic( size_t xBufferSizeBytes,
														   size_t xTriggerLevelBytes,
														   BaseType_t xIsMessageBuffer,
														   uint8_t * const pucStreamBufferStorageArea,
														   StaticStreamBuffer_t * const pxStaticStreamBuffer )
	{
	StreamBuffer_t * const pxStreamBuffer = ( StreamBuffer_t * ) pxStaticStreamBuffer;  
	StreamBufferHandle_t xReturn;
	uint8_t ucFlags;

		if ((pucStreamBufferStorageArea) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		if ((pxStaticStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		if ((xTriggerLevelBytes <= xBufferSizeBytes) == 0) {vPortRaiseBASEPRI(); for( ;; );};

		
 
		if( xTriggerLevelBytes == ( size_t ) 0 )
		{
			xTriggerLevelBytes = ( size_t ) 1;
		}

		if( xIsMessageBuffer != ( ( BaseType_t ) 0 ) )
		{
			 
			ucFlags = ( ( uint8_t ) 1 ) | ( ( uint8_t ) 2 );
		}
		else
		{
			 
			ucFlags = ( ( uint8_t ) 2 );
		}

		


 
		if ((xBufferSizeBytes > ( sizeof( size_t ) )) == 0) {vPortRaiseBASEPRI(); for( ;; );};


		{
			

 
			volatile size_t xSize = sizeof( StaticStreamBuffer_t );
			if ((xSize == sizeof( StreamBuffer_t )) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		}  


		if( ( pucStreamBufferStorageArea != 0 ) && ( pxStaticStreamBuffer != 0 ) )
		{
			prvInitialiseNewStreamBuffer( pxStreamBuffer,
										  pucStreamBufferStorageArea,
										  xBufferSizeBytes,
										  xTriggerLevelBytes,
										  ucFlags );

			
 
			pxStreamBuffer->ucFlags |= ( ( uint8_t ) 2 );

			;

			xReturn = ( StreamBufferHandle_t ) pxStaticStreamBuffer;  
		}
		else
		{
			xReturn = 0;
			;
		}

		return xReturn;
	}


 

void vStreamBufferDelete( StreamBufferHandle_t xStreamBuffer )
{
StreamBuffer_t * pxStreamBuffer = xStreamBuffer;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	;

	if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 2 ) ) == ( uint8_t ) ( ( BaseType_t ) 0 ) )
	{

		{
			
 
			vPortFree( ( void * ) pxStreamBuffer );  
		}
#line 382 "../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
	}
	else
	{
		
 
		( void ) memset( pxStreamBuffer, 0x00, sizeof( StreamBuffer_t ) );
	}
}
 

BaseType_t xStreamBufferReset( StreamBufferHandle_t xStreamBuffer )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
BaseType_t xReturn = ( ( ( BaseType_t ) 0 ) );


	UBaseType_t uxStreamBufferNumber;


	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};


	{
		
 
		uxStreamBufferNumber = pxStreamBuffer->uxStreamBufferNumber;
	}


	 
	vPortEnterCritical();
	{
		if( pxStreamBuffer->xTaskWaitingToReceive == 0 )
		{
			if( pxStreamBuffer->xTaskWaitingToSend == 0 )
			{
				prvInitialiseNewStreamBuffer( pxStreamBuffer,
											  pxStreamBuffer->pucBuffer,
											  pxStreamBuffer->xLength,
											  pxStreamBuffer->xTriggerLevelBytes,
											  pxStreamBuffer->ucFlags );
				xReturn = ( ( ( BaseType_t ) 1 ) );


				{
					pxStreamBuffer->uxStreamBufferNumber = uxStreamBufferNumber;
				}


				;
			}
		}
	}
	vPortExitCritical();

	return xReturn;
}
 

BaseType_t xStreamBufferSetTriggerLevel( StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
BaseType_t xReturn;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	 
	if( xTriggerLevel == ( size_t ) 0 )
	{
		xTriggerLevel = ( size_t ) 1;
	}

	
 
	if( xTriggerLevel <= pxStreamBuffer->xLength )
	{
		pxStreamBuffer->xTriggerLevelBytes = xTriggerLevel;
		xReturn = ( ( ( BaseType_t ) 1 ) );
	}
	else
	{
		xReturn = ( ( BaseType_t ) 0 );
	}

	return xReturn;
}
 

size_t xStreamBufferSpacesAvailable( StreamBufferHandle_t xStreamBuffer )
{
const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
size_t xSpace;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	xSpace = pxStreamBuffer->xLength + pxStreamBuffer->xTail;
	xSpace -= pxStreamBuffer->xHead;
	xSpace -= ( size_t ) 1;

	if( xSpace >= pxStreamBuffer->xLength )
	{
		xSpace -= pxStreamBuffer->xLength;
	}
	else
	{
		;
	}

	return xSpace;
}
 

size_t xStreamBufferBytesAvailable( StreamBufferHandle_t xStreamBuffer )
{
const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
size_t xReturn;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	xReturn = prvBytesInBuffer( pxStreamBuffer );
	return xReturn;
}
 

size_t xStreamBufferSend( StreamBufferHandle_t xStreamBuffer,
						  const void *pvTxData,
						  size_t xDataLengthBytes,
						  TickType_t xTicksToWait )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
size_t xReturn, xSpace = 0;
size_t xRequiredSpace = xDataLengthBytes;
TimeOut_t xTimeOut;

	if ((pvTxData) == 0) {vPortRaiseBASEPRI(); for( ;; );};
	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	


 
	if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) ) != ( uint8_t ) 0 )
	{
		xRequiredSpace += ( sizeof( size_t ) );

		 
		if ((xRequiredSpace > xDataLengthBytes) == 0) {vPortRaiseBASEPRI(); for( ;; );};
	}
	else
	{
		;
	}

	if( xTicksToWait != ( TickType_t ) 0 )
	{
		vTaskSetTimeOutState( &xTimeOut );

		do
		{
			
 
			vPortEnterCritical();
			{
				xSpace = xStreamBufferSpacesAvailable( pxStreamBuffer );

				if( xSpace < xRequiredSpace )
				{
					 
					( void ) xTaskNotifyStateClear( 0 );

					 
					if ((pxStreamBuffer->xTaskWaitingToSend == 0) == 0) {vPortRaiseBASEPRI(); for( ;; );};
					pxStreamBuffer->xTaskWaitingToSend = xTaskGetCurrentTaskHandle();
				}
				else
				{
					vPortExitCritical();
					break;
				}
			}
			vPortExitCritical();

			;
			( void ) xTaskNotifyWait( ( uint32_t ) 0, ( uint32_t ) 0, 0, xTicksToWait );
			pxStreamBuffer->xTaskWaitingToSend = 0;

		} while( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == ( ( BaseType_t ) 0 ) );
	}
	else
	{
		;
	}

	if( xSpace == ( size_t ) 0 )
	{
		xSpace = xStreamBufferSpacesAvailable( pxStreamBuffer );
	}
	else
	{
		;
	}

	xReturn = prvWriteMessageToBuffer( pxStreamBuffer, pvTxData, xDataLengthBytes, xSpace, xRequiredSpace );

	if( xReturn > ( size_t ) 0 )
	{
		;

		 
		if( prvBytesInBuffer( pxStreamBuffer ) >= pxStreamBuffer->xTriggerLevelBytes )
		{
			vTaskSuspendAll(); { if( ( pxStreamBuffer )->xTaskWaitingToReceive != 0 ) { ( void ) xTaskGenericNotify( ( ( pxStreamBuffer )->xTaskWaitingToReceive ), ( ( uint32_t ) 0 ), ( eNoAction ), 0 ); ( pxStreamBuffer )->xTaskWaitingToReceive = 0; } } ( void ) xTaskResumeAll();;
		}
		else
		{
			;
		}
	}
	else
	{
		;
		;
	}

	return xReturn;
}
 

size_t xStreamBufferSendFromISR( StreamBufferHandle_t xStreamBuffer,
								 const void *pvTxData,
								 size_t xDataLengthBytes,
								 BaseType_t * const pxHigherPriorityTaskWoken )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
size_t xReturn, xSpace;
size_t xRequiredSpace = xDataLengthBytes;

	if ((pvTxData) == 0) {vPortRaiseBASEPRI(); for( ;; );};
	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	


 
	if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) ) != ( uint8_t ) 0 )
	{
		xRequiredSpace += ( sizeof( size_t ) );
	}
	else
	{
		;
	}

	xSpace = xStreamBufferSpacesAvailable( pxStreamBuffer );
	xReturn = prvWriteMessageToBuffer( pxStreamBuffer, pvTxData, xDataLengthBytes, xSpace, xRequiredSpace );

	if( xReturn > ( size_t ) 0 )
	{
		 
		if( prvBytesInBuffer( pxStreamBuffer ) >= pxStreamBuffer->xTriggerLevelBytes )
		{
			{ UBaseType_t uxSavedInterruptStatus; uxSavedInterruptStatus = ( UBaseType_t ) ulPortRaiseBASEPRI(); { if( ( pxStreamBuffer )->xTaskWaitingToReceive != 0 ) { ( void ) xTaskGenericNotifyFromISR( ( ( pxStreamBuffer )->xTaskWaitingToReceive ), ( ( uint32_t ) 0 ), ( eNoAction ), 0, ( pxHigherPriorityTaskWoken ) ); ( pxStreamBuffer )->xTaskWaitingToReceive = 0; } } vPortSetBASEPRI(uxSavedInterruptStatus); };
		}
		else
		{
			;
		}
	}
	else
	{
		;
	}

	;

	return xReturn;
}
 

static size_t prvWriteMessageToBuffer( StreamBuffer_t * const pxStreamBuffer,
									   const void * pvTxData,
									   size_t xDataLengthBytes,
									   size_t xSpace,
									   size_t xRequiredSpace )
{
	BaseType_t xShouldWrite;
	size_t xReturn;

	if( xSpace == ( size_t ) 0 )
	{
		
 
		xShouldWrite = ( ( BaseType_t ) 0 );
	}
	else if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) ) == ( uint8_t ) 0 )
	{
		

 
		xShouldWrite = ( ( BaseType_t ) 1 );
		xDataLengthBytes = ( ( ( xDataLengthBytes ) < ( xSpace ) ) ? ( xDataLengthBytes ) : ( xSpace ) );
	}
	else if( xSpace >= xRequiredSpace )
	{
		


 
		xShouldWrite = ( ( BaseType_t ) 1 );
		( void ) prvWriteBytesToBuffer( pxStreamBuffer, ( const uint8_t * ) &( xDataLengthBytes ), ( sizeof( size_t ) ) );
	}
	else
	{
		 
		xShouldWrite = ( ( BaseType_t ) 0 );
	}

	if( xShouldWrite != ( ( BaseType_t ) 0 ) )
	{
		 
		xReturn = prvWriteBytesToBuffer( pxStreamBuffer, ( const uint8_t * ) pvTxData, xDataLengthBytes );  
	}
	else
	{
		xReturn = 0;
	}

	return xReturn;
}
 

size_t xStreamBufferReceive( StreamBufferHandle_t xStreamBuffer,
							 void *pvRxData,
							 size_t xBufferLengthBytes,
							 TickType_t xTicksToWait )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
size_t xReceivedLength = 0, xBytesAvailable, xBytesToStoreMessageLength;

	if ((pvRxData) == 0) {vPortRaiseBASEPRI(); for( ;; );};
	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	



 
	if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) ) != ( uint8_t ) 0 )
	{
		xBytesToStoreMessageLength = ( sizeof( size_t ) );
	}
	else
	{
		xBytesToStoreMessageLength = 0;
	}

	if( xTicksToWait != ( TickType_t ) 0 )
	{
		
 
		vPortEnterCritical();
		{
			xBytesAvailable = prvBytesInBuffer( pxStreamBuffer );

			



 
			if( xBytesAvailable <= xBytesToStoreMessageLength )
			{
				 
				( void ) xTaskNotifyStateClear( 0 );

				 
				if ((pxStreamBuffer->xTaskWaitingToReceive == 0) == 0) {vPortRaiseBASEPRI(); for( ;; );};
				pxStreamBuffer->xTaskWaitingToReceive = xTaskGetCurrentTaskHandle();
			}
			else
			{
				;
			}
		}
		vPortExitCritical();

		if( xBytesAvailable <= xBytesToStoreMessageLength )
		{
			 
			;
			( void ) xTaskNotifyWait( ( uint32_t ) 0, ( uint32_t ) 0, 0, xTicksToWait );
			pxStreamBuffer->xTaskWaitingToReceive = 0;

			 
			xBytesAvailable = prvBytesInBuffer( pxStreamBuffer );
		}
		else
		{
			;
		}
	}
	else
	{
		xBytesAvailable = prvBytesInBuffer( pxStreamBuffer );
	}

	



 
	if( xBytesAvailable > xBytesToStoreMessageLength )
	{
		xReceivedLength = prvReadMessageFromBuffer( pxStreamBuffer, pvRxData, xBufferLengthBytes, xBytesAvailable, xBytesToStoreMessageLength );

		 
		if( xReceivedLength != ( size_t ) 0 )
		{
			;
			vTaskSuspendAll(); { if( ( pxStreamBuffer )->xTaskWaitingToSend != 0 ) { ( void ) xTaskGenericNotify( ( ( pxStreamBuffer )->xTaskWaitingToSend ), ( ( uint32_t ) 0 ), ( eNoAction ), 0 ); ( pxStreamBuffer )->xTaskWaitingToSend = 0; } } ( void ) xTaskResumeAll();;
		}
		else
		{
			;
		}
	}
	else
	{
		;
		;
	}

	return xReceivedLength;
}
 

size_t xStreamBufferNextMessageLengthBytes( StreamBufferHandle_t xStreamBuffer )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
size_t xReturn, xBytesAvailable, xOriginalTail;
size_t xTempReturn;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	 
	if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) ) != ( uint8_t ) 0 )
	{
		xBytesAvailable = prvBytesInBuffer( pxStreamBuffer );
		if( xBytesAvailable > ( sizeof( size_t ) ) )
		{
			




 
			xOriginalTail = pxStreamBuffer->xTail;
			( void ) prvReadBytesFromBuffer( pxStreamBuffer, ( uint8_t * ) &xTempReturn, ( sizeof( size_t ) ), xBytesAvailable );
			xReturn = ( size_t ) xTempReturn;
			pxStreamBuffer->xTail = xOriginalTail;
		}
		else
		{
			


 
			if ((xBytesAvailable == 0) == 0) {vPortRaiseBASEPRI(); for( ;; );};
			xReturn = 0;
		}
	}
	else
	{
		xReturn = 0;
	}

	return xReturn;
}
 

size_t xStreamBufferReceiveFromISR( StreamBufferHandle_t xStreamBuffer,
									void *pvRxData,
									size_t xBufferLengthBytes,
									BaseType_t * const pxHigherPriorityTaskWoken )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
size_t xReceivedLength = 0, xBytesAvailable, xBytesToStoreMessageLength;

	if ((pvRxData) == 0) {vPortRaiseBASEPRI(); for( ;; );};
	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	



 
	if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) ) != ( uint8_t ) 0 )
	{
		xBytesToStoreMessageLength = ( sizeof( size_t ) );
	}
	else
	{
		xBytesToStoreMessageLength = 0;
	}

	xBytesAvailable = prvBytesInBuffer( pxStreamBuffer );

	



 
	if( xBytesAvailable > xBytesToStoreMessageLength )
	{
		xReceivedLength = prvReadMessageFromBuffer( pxStreamBuffer, pvRxData, xBufferLengthBytes, xBytesAvailable, xBytesToStoreMessageLength );

		 
		if( xReceivedLength != ( size_t ) 0 )
		{
			{ UBaseType_t uxSavedInterruptStatus; uxSavedInterruptStatus = ( UBaseType_t ) ulPortRaiseBASEPRI(); { if( ( pxStreamBuffer )->xTaskWaitingToSend != 0 ) { ( void ) xTaskGenericNotifyFromISR( ( ( pxStreamBuffer )->xTaskWaitingToSend ), ( ( uint32_t ) 0 ), ( eNoAction ), 0, ( pxHigherPriorityTaskWoken ) ); ( pxStreamBuffer )->xTaskWaitingToSend = 0; } } vPortSetBASEPRI(uxSavedInterruptStatus); };
		}
		else
		{
			;
		}
	}
	else
	{
		;
	}

	;

	return xReceivedLength;
}
 

static size_t prvReadMessageFromBuffer( StreamBuffer_t *pxStreamBuffer,
										void *pvRxData,
										size_t xBufferLengthBytes,
										size_t xBytesAvailable,
										size_t xBytesToStoreMessageLength )
{
size_t xOriginalTail, xReceivedLength, xNextMessageLength;
size_t xTempNextMessageLength;

	if( xBytesToStoreMessageLength != ( size_t ) 0 )
	{
		


 
		xOriginalTail = pxStreamBuffer->xTail;
		( void ) prvReadBytesFromBuffer( pxStreamBuffer, ( uint8_t * ) &xTempNextMessageLength, xBytesToStoreMessageLength, xBytesAvailable );
		xNextMessageLength = ( size_t ) xTempNextMessageLength;

		
 
		xBytesAvailable -= xBytesToStoreMessageLength;

		
 
		if( xNextMessageLength > xBufferLengthBytes )
		{
			

 
			pxStreamBuffer->xTail = xOriginalTail;
			xNextMessageLength = 0;
		}
		else
		{
			;
		}
	}
	else
	{
		
 
		xNextMessageLength = xBufferLengthBytes;
	}

	 
	xReceivedLength = prvReadBytesFromBuffer( pxStreamBuffer, ( uint8_t * ) pvRxData, xNextMessageLength, xBytesAvailable );  

	return xReceivedLength;
}
 

BaseType_t xStreamBufferIsEmpty( StreamBufferHandle_t xStreamBuffer )
{
const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
BaseType_t xReturn;
size_t xTail;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	 
	xTail = pxStreamBuffer->xTail;
	if( pxStreamBuffer->xHead == xTail )
	{
		xReturn = ( ( BaseType_t ) 1 );
	}
	else
	{
		xReturn = ( ( BaseType_t ) 0 );
	}

	return xReturn;
}
 

BaseType_t xStreamBufferIsFull( StreamBufferHandle_t xStreamBuffer )
{
BaseType_t xReturn;
size_t xBytesToStoreMessageLength;
const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	


 
	if( ( pxStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) ) != ( uint8_t ) 0 )
	{
		xBytesToStoreMessageLength = ( sizeof( size_t ) );
	}
	else
	{
		xBytesToStoreMessageLength = 0;
	}

	 
	if( xStreamBufferSpacesAvailable( xStreamBuffer ) <= xBytesToStoreMessageLength )
	{
		xReturn = ( ( BaseType_t ) 1 );
	}
	else
	{
		xReturn = ( ( BaseType_t ) 0 );
	}

	return xReturn;
}
 

BaseType_t xStreamBufferSendCompletedFromISR( StreamBufferHandle_t xStreamBuffer, BaseType_t *pxHigherPriorityTaskWoken )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
BaseType_t xReturn;
UBaseType_t uxSavedInterruptStatus;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	uxSavedInterruptStatus = ( UBaseType_t ) ulPortRaiseBASEPRI();
	{
		if( ( pxStreamBuffer )->xTaskWaitingToReceive != 0 )
		{
			( void ) xTaskGenericNotifyFromISR( ( ( pxStreamBuffer )->xTaskWaitingToReceive ), ( ( uint32_t ) 0 ), ( eNoAction ), 0, ( pxHigherPriorityTaskWoken ) );



			( pxStreamBuffer )->xTaskWaitingToReceive = 0;
			xReturn = ( ( BaseType_t ) 1 );
		}
		else
		{
			xReturn = ( ( BaseType_t ) 0 );
		}
	}
	vPortSetBASEPRI(uxSavedInterruptStatus);

	return xReturn;
}
 

BaseType_t xStreamBufferReceiveCompletedFromISR( StreamBufferHandle_t xStreamBuffer, BaseType_t *pxHigherPriorityTaskWoken )
{
StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
BaseType_t xReturn;
UBaseType_t uxSavedInterruptStatus;

	if ((pxStreamBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	uxSavedInterruptStatus = ( UBaseType_t ) ulPortRaiseBASEPRI();
	{
		if( ( pxStreamBuffer )->xTaskWaitingToSend != 0 )
		{
			( void ) xTaskGenericNotifyFromISR( ( ( pxStreamBuffer )->xTaskWaitingToSend ), ( ( uint32_t ) 0 ), ( eNoAction ), 0, ( pxHigherPriorityTaskWoken ) );



			( pxStreamBuffer )->xTaskWaitingToSend = 0;
			xReturn = ( ( BaseType_t ) 1 );
		}
		else
		{
			xReturn = ( ( BaseType_t ) 0 );
		}
	}
	vPortSetBASEPRI(uxSavedInterruptStatus);

	return xReturn;
}
 

static size_t prvWriteBytesToBuffer( StreamBuffer_t * const pxStreamBuffer, const uint8_t *pucData, size_t xCount )
{
size_t xNextHead, xFirstLength;

	if ((xCount > ( size_t ) 0) == 0) {vPortRaiseBASEPRI(); for( ;; );};

	xNextHead = pxStreamBuffer->xHead;

	

 
	xFirstLength = ( ( ( pxStreamBuffer->xLength - xNextHead ) < ( xCount ) ) ? ( pxStreamBuffer->xLength - xNextHead ) : ( xCount ) );

	 
	if ((( xNextHead + xFirstLength ) <= pxStreamBuffer->xLength) == 0) {vPortRaiseBASEPRI(); for( ;; );};
	( void ) memcpy( ( void* ) ( &( pxStreamBuffer->pucBuffer[ xNextHead ] ) ), ( const void * ) pucData, xFirstLength );  

	
 
	if( xCount > xFirstLength )
	{
		 
		if ((( xCount - xFirstLength ) <= pxStreamBuffer->xLength) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		( void ) memcpy( ( void * ) pxStreamBuffer->pucBuffer, ( const void * ) &( pucData[ xFirstLength ] ), xCount - xFirstLength );  
	}
	else
	{
		;
	}

	xNextHead += xCount;
	if( xNextHead >= pxStreamBuffer->xLength )
	{
		xNextHead -= pxStreamBuffer->xLength;
	}
	else
	{
		;
	}

	pxStreamBuffer->xHead = xNextHead;

	return xCount;
}
 

static size_t prvReadBytesFromBuffer( StreamBuffer_t *pxStreamBuffer, uint8_t *pucData, size_t xMaxCount, size_t xBytesAvailable )
{
size_t xCount, xFirstLength, xNextTail;

	 
	xCount = ( ( ( xBytesAvailable ) < ( xMaxCount ) ) ? ( xBytesAvailable ) : ( xMaxCount ) );

	if( xCount > ( size_t ) 0 )
	{
		xNextTail = pxStreamBuffer->xTail;

		

 
		xFirstLength = ( ( ( pxStreamBuffer->xLength - xNextTail ) < ( xCount ) ) ? ( pxStreamBuffer->xLength - xNextTail ) : ( xCount ) );

		
 
		if ((xFirstLength <= xMaxCount) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		if ((( xNextTail + xFirstLength ) <= pxStreamBuffer->xLength) == 0) {vPortRaiseBASEPRI(); for( ;; );};
		( void ) memcpy( ( void * ) pucData, ( const void * ) &( pxStreamBuffer->pucBuffer[ xNextTail ] ), xFirstLength );  

		
 
		if( xCount > xFirstLength )
		{
			 
			if ((xCount <= xMaxCount) == 0) {vPortRaiseBASEPRI(); for( ;; );};
			( void ) memcpy( ( void * ) &( pucData[ xFirstLength ] ), ( void * ) ( pxStreamBuffer->pucBuffer ), xCount - xFirstLength );  
		}
		else
		{
			;
		}

		
 
		xNextTail += xCount;

		if( xNextTail >= pxStreamBuffer->xLength )
		{
			xNextTail -= pxStreamBuffer->xLength;
		}

		pxStreamBuffer->xTail = xNextTail;
	}
	else
	{
		;
	}

	return xCount;
}
 

static size_t prvBytesInBuffer( const StreamBuffer_t * const pxStreamBuffer )
{
 
size_t xCount;

	xCount = pxStreamBuffer->xLength + pxStreamBuffer->xHead;
	xCount -= pxStreamBuffer->xTail;
	if ( xCount >= pxStreamBuffer->xLength )
	{
		xCount -= pxStreamBuffer->xLength;
	}
	else
	{
		;
	}

	return xCount;
}
 

static void prvInitialiseNewStreamBuffer( StreamBuffer_t * const pxStreamBuffer,
										  uint8_t * const pucBuffer,
										  size_t xBufferSizeBytes,
										  size_t xTriggerLevelBytes,
										  uint8_t ucFlags )
{
	

 

	{
		

 
		const BaseType_t xWriteValue = 0x55;
		if ((memset( pucBuffer, ( int ) xWriteValue, xBufferSizeBytes ) == pucBuffer) == 0) {vPortRaiseBASEPRI(); for( ;; );};
	}  


	( void ) memset( ( void * ) pxStreamBuffer, 0x00, sizeof( StreamBuffer_t ) );  
	pxStreamBuffer->pucBuffer = pucBuffer;
	pxStreamBuffer->xLength = xBufferSizeBytes;
	pxStreamBuffer->xTriggerLevelBytes = xTriggerLevelBytes;
	pxStreamBuffer->ucFlags = ucFlags;
}



	UBaseType_t uxStreamBufferGetStreamBufferNumber( StreamBufferHandle_t xStreamBuffer )
	{
		return xStreamBuffer->uxStreamBufferNumber;
	}


 



	void vStreamBufferSetStreamBufferNumber( StreamBufferHandle_t xStreamBuffer, UBaseType_t uxStreamBufferNumber )
	{
		xStreamBuffer->uxStreamBufferNumber = uxStreamBufferNumber;
	}


 



	uint8_t ucStreamBufferGetStreamBufferType( StreamBufferHandle_t xStreamBuffer )
	{
		return ( xStreamBuffer->ucFlags & ( ( uint8_t ) 1 ) );
	}


 
