#line 1 "../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c"








































































 

 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"

















  

 







 
#line 1 "../Core/Inc/stm32f4xx_hal_conf.h"
 


















 
 

 







 
 

 


 


   
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 


 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 91 "../Core/Inc/stm32f4xx_hal_conf.h"

 




 












 






 







 












 





 

 


 
#line 156 "../Core/Inc/stm32f4xx_hal_conf.h"

#line 196 "../Core/Inc/stm32f4xx_hal_conf.h"

 



 
 

 

 

 
#line 215 "../Core/Inc/stm32f4xx_hal_conf.h"

 





 

 

 

 





 




#line 249 "../Core/Inc/stm32f4xx_hal_conf.h"





 





 




 



 


 

#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"















 

 







 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"

















 

 







 
#line 1 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"


























 



 



 










 



 






 
#line 94 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"



 
#line 106 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"



 
#line 118 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"



 



 

#line 1 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"






















 



 



 










 



 








 



 




 
typedef enum
{
   
  NonMaskableInt_IRQn         = -14,     
  MemoryManagement_IRQn       = -12,     
  BusFault_IRQn               = -11,     
  UsageFault_IRQn             = -10,     
  SVCall_IRQn                 = -5,      
  DebugMonitor_IRQn           = -4,      
  PendSV_IRQn                 = -2,      
  SysTick_IRQn                = -1,      
   
  WWDG_IRQn                   = 0,       
  PVD_IRQn                    = 1,       
  TAMP_STAMP_IRQn             = 2,       
  RTC_WKUP_IRQn               = 3,       
  FLASH_IRQn                  = 4,       
  RCC_IRQn                    = 5,       
  EXTI0_IRQn                  = 6,       
  EXTI1_IRQn                  = 7,       
  EXTI2_IRQn                  = 8,       
  EXTI3_IRQn                  = 9,       
  EXTI4_IRQn                  = 10,      
  DMA1_Stream0_IRQn           = 11,      
  DMA1_Stream1_IRQn           = 12,      
  DMA1_Stream2_IRQn           = 13,      
  DMA1_Stream3_IRQn           = 14,      
  DMA1_Stream4_IRQn           = 15,      
  DMA1_Stream5_IRQn           = 16,      
  DMA1_Stream6_IRQn           = 17,      
  ADC_IRQn                    = 18,      
  EXTI9_5_IRQn                = 23,      
  TIM1_BRK_TIM9_IRQn          = 24,      
  TIM1_UP_TIM10_IRQn          = 25,      
  TIM1_TRG_COM_TIM11_IRQn     = 26,      
  TIM1_CC_IRQn                = 27,      
  TIM2_IRQn                   = 28,      
  TIM3_IRQn                   = 29,      
  TIM4_IRQn                   = 30,      
  I2C1_EV_IRQn                = 31,      
  I2C1_ER_IRQn                = 32,      
  I2C2_EV_IRQn                = 33,      
  I2C2_ER_IRQn                = 34,      
  SPI1_IRQn                   = 35,      
  SPI2_IRQn                   = 36,      
  USART1_IRQn                 = 37,      
  USART2_IRQn                 = 38,      
  EXTI15_10_IRQn              = 40,      
  RTC_Alarm_IRQn              = 41,      
  OTG_FS_WKUP_IRQn            = 42,      
  DMA1_Stream7_IRQn           = 47,      
  SDIO_IRQn                   = 49,      
  TIM5_IRQn                   = 50,      
  SPI3_IRQn                   = 51,      
  DMA2_Stream0_IRQn           = 56,      
  DMA2_Stream1_IRQn           = 57,      
  DMA2_Stream2_IRQn           = 58,      
  DMA2_Stream3_IRQn           = 59,      
  DMA2_Stream4_IRQn           = 60,      
  OTG_FS_IRQn                 = 67,      
  DMA2_Stream5_IRQn           = 68,      
  DMA2_Stream6_IRQn           = 69,      
  DMA2_Stream7_IRQn           = 70,      
  USART6_IRQn                 = 71,      
  I2C3_EV_IRQn                = 72,      
  I2C3_ER_IRQn                = 73,      
  FPU_IRQn                    = 81,      
                                SPI4_IRQn                   = 84,      
                                                              SPI5_IRQn                   = 85       
} IRQn_Type;



 

#line 1 "../Drivers/CMSIS/Include/core_cm4.h"
 




 
















 










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






 
#line 35 "../Drivers/CMSIS/Include/core_cm4.h"

















 




 



 

#line 1 "../Drivers/CMSIS/Include/cmsis_version.h"
 




 
















 










 
#line 64 "../Drivers/CMSIS/Include/core_cm4.h"

 









 
#line 87 "../Drivers/CMSIS/Include/core_cm4.h"

#line 161 "../Drivers/CMSIS/Include/core_cm4.h"

#line 1 "../Drivers/CMSIS/Include/cmsis_compiler.h"
 




 
















 




#line 29 "../Drivers/CMSIS/Include/cmsis_compiler.h"



 
#line 1 "../Drivers/CMSIS/Include/cmsis_armcc.h"
 




 
















 









 













   
   
   

 




 
#line 111 "../Drivers/CMSIS/Include/cmsis_armcc.h"

 





















 



 




 






 







 






 








 






 






 








 








 

__attribute__((section(".rev16_text"))) static __inline __asm uint32_t __REV16(uint32_t value)
{
  rev16 r0, r0
  bx lr
}








 

__attribute__((section(".revsh_text"))) static __inline __asm int16_t __REVSH(int16_t value)
{
  revsh r0, r0
  bx lr
}









 









 








 
#line 277 "../Drivers/CMSIS/Include/cmsis_armcc.h"







 











 












 












 














 














 














 










 









 









 









 

__attribute__((section(".rrx_text"))) static __inline __asm uint32_t __RRX(uint32_t value)
{
  rrx r0, r0
  bx lr
}








 








 








 








 








 








 


#line 525 "../Drivers/CMSIS/Include/cmsis_armcc.h"

   


 



 





 
 






 
 





 
static __inline uint32_t __get_CONTROL(void)
{
  register uint32_t __regControl         __asm("control");
  return(__regControl);
}






 
static __inline void __set_CONTROL(uint32_t control)
{
  register uint32_t __regControl         __asm("control");
  __regControl = control;
  __isb(0xF);
}






 
static __inline uint32_t __get_IPSR(void)
{
  register uint32_t __regIPSR          __asm("ipsr");
  return(__regIPSR);
}






 
static __inline uint32_t __get_APSR(void)
{
  register uint32_t __regAPSR          __asm("apsr");
  return(__regAPSR);
}






 
static __inline uint32_t __get_xPSR(void)
{
  register uint32_t __regXPSR          __asm("xpsr");
  return(__regXPSR);
}






 
static __inline uint32_t __get_PSP(void)
{
  register uint32_t __regProcessStackPointer  __asm("psp");
  return(__regProcessStackPointer);
}






 
static __inline void __set_PSP(uint32_t topOfProcStack)
{
  register uint32_t __regProcessStackPointer  __asm("psp");
  __regProcessStackPointer = topOfProcStack;
}






 
static __inline uint32_t __get_MSP(void)
{
  register uint32_t __regMainStackPointer     __asm("msp");
  return(__regMainStackPointer);
}






 
static __inline void __set_MSP(uint32_t topOfMainStack)
{
  register uint32_t __regMainStackPointer     __asm("msp");
  __regMainStackPointer = topOfMainStack;
}






 
static __inline uint32_t __get_PRIMASK(void)
{
  register uint32_t __regPriMask         __asm("primask");
  return(__regPriMask);
}






 
static __inline void __set_PRIMASK(uint32_t priMask)
{
  register uint32_t __regPriMask         __asm("primask");
  __regPriMask = (priMask);
}









 







 







 
static __inline uint32_t  __get_BASEPRI(void)
{
  register uint32_t __regBasePri         __asm("basepri");
  return(__regBasePri);
}






 
static __inline void __set_BASEPRI(uint32_t basePri)
{
  register uint32_t __regBasePri         __asm("basepri");
  __regBasePri = (basePri & 0xFFU);
}







 
static __inline void __set_BASEPRI_MAX(uint32_t basePri)
{
  register uint32_t __regBasePriMax      __asm("basepri_max");
  __regBasePriMax = (basePri & 0xFFU);
}






 
static __inline uint32_t __get_FAULTMASK(void)
{
  register uint32_t __regFaultMask       __asm("faultmask");
  return(__regFaultMask);
}






 
static __inline void __set_FAULTMASK(uint32_t faultMask)
{
  register uint32_t __regFaultMask       __asm("faultmask");
  __regFaultMask = (faultMask & (uint32_t)1U);
}









 
static __inline uint32_t __get_FPSCR(void)
{


  register uint32_t __regfpscr         __asm("fpscr");
  return(__regfpscr);



}






 
static __inline void __set_FPSCR(uint32_t fpscr)
{


  register uint32_t __regfpscr         __asm("fpscr");
  __regfpscr = (fpscr);



}


 


 



 



#line 870 "../Drivers/CMSIS/Include/cmsis_armcc.h"















 


#line 35 "../Drivers/CMSIS/Include/cmsis_compiler.h"




 
#line 280 "../Drivers/CMSIS/Include/cmsis_compiler.h"




#line 163 "../Drivers/CMSIS/Include/core_cm4.h"

















 
#line 212 "../Drivers/CMSIS/Include/core_cm4.h"

 






 
#line 228 "../Drivers/CMSIS/Include/core_cm4.h"

 




 













 



 






 



 
typedef union
{
  struct
  {
    uint32_t _reserved0:16;               
    uint32_t GE:4;                        
    uint32_t _reserved1:7;                
    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} APSR_Type;

 





















 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:23;               
  } b;                                    
  uint32_t w;                             
} IPSR_Type;

 






 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:1;                
    uint32_t ICI_IT_1:6;                  
    uint32_t GE:4;                        
    uint32_t _reserved1:4;                
    uint32_t T:1;                         
    uint32_t ICI_IT_2:2;                  
    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} xPSR_Type;

 

































 
typedef union
{
  struct
  {
    uint32_t nPRIV:1;                     
    uint32_t SPSEL:1;                     
    uint32_t FPCA:1;                      
    uint32_t _reserved0:29;               
  } b;                                    
  uint32_t w;                             
} CONTROL_Type;

 









 







 



 
typedef struct
{
  volatile uint32_t ISER[8U];                
        uint32_t RESERVED0[24U];
  volatile uint32_t ICER[8U];                
        uint32_t RESERVED1[24U];
  volatile uint32_t ISPR[8U];                
        uint32_t RESERVED2[24U];
  volatile uint32_t ICPR[8U];                
        uint32_t RESERVED3[24U];
  volatile uint32_t IABR[8U];                
        uint32_t RESERVED4[56U];
  volatile uint8_t  IP[240U];                
        uint32_t RESERVED5[644U];
  volatile  uint32_t STIR;                    
}  NVIC_Type;

 



 







 



 
typedef struct
{
  volatile const  uint32_t CPUID;                   
  volatile uint32_t ICSR;                    
  volatile uint32_t VTOR;                    
  volatile uint32_t AIRCR;                   
  volatile uint32_t SCR;                     
  volatile uint32_t CCR;                     
  volatile uint8_t  SHP[12U];                
  volatile uint32_t SHCSR;                   
  volatile uint32_t CFSR;                    
  volatile uint32_t HFSR;                    
  volatile uint32_t DFSR;                    
  volatile uint32_t MMFAR;                   
  volatile uint32_t BFAR;                    
  volatile uint32_t AFSR;                    
  volatile const  uint32_t PFR[2U];                 
  volatile const  uint32_t DFR;                     
  volatile const  uint32_t ADR;                     
  volatile const  uint32_t MMFR[4U];                
  volatile const  uint32_t ISAR[5U];                
        uint32_t RESERVED0[5U];
  volatile uint32_t CPACR;                   
} SCB_Type;

 















 






























 



 





















 









 


















 










































 









 


















 





















 


















 









 















 







 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile const  uint32_t ICTR;                    
  volatile uint32_t ACTLR;                   
} SCnSCB_Type;

 



 















 







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t LOAD;                    
  volatile uint32_t VAL;                     
  volatile const  uint32_t CALIB;                   
} SysTick_Type;

 












 



 



 









 







 



 
typedef struct
{
  volatile  union
  {
    volatile  uint8_t    u8;                  
    volatile  uint16_t   u16;                 
    volatile  uint32_t   u32;                 
  }  PORT [32U];                          
        uint32_t RESERVED0[864U];
  volatile uint32_t TER;                     
        uint32_t RESERVED1[15U];
  volatile uint32_t TPR;                     
        uint32_t RESERVED2[15U];
  volatile uint32_t TCR;                     
        uint32_t RESERVED3[32U];
        uint32_t RESERVED4[43U];
  volatile  uint32_t LAR;                     
  volatile const  uint32_t LSR;                     
        uint32_t RESERVED5[6U];
  volatile const  uint32_t PID4;                    
  volatile const  uint32_t PID5;                    
  volatile const  uint32_t PID6;                    
  volatile const  uint32_t PID7;                    
  volatile const  uint32_t PID0;                    
  volatile const  uint32_t PID1;                    
  volatile const  uint32_t PID2;                    
  volatile const  uint32_t PID3;                    
  volatile const  uint32_t CID0;                    
  volatile const  uint32_t CID1;                    
  volatile const  uint32_t CID2;                    
  volatile const  uint32_t CID3;                    
} ITM_Type;

 



 



























 









   







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t CYCCNT;                  
  volatile uint32_t CPICNT;                  
  volatile uint32_t EXCCNT;                  
  volatile uint32_t SLEEPCNT;                
  volatile uint32_t LSUCNT;                  
  volatile uint32_t FOLDCNT;                 
  volatile const  uint32_t PCSR;                    
  volatile uint32_t COMP0;                   
  volatile uint32_t MASK0;                   
  volatile uint32_t FUNCTION0;               
        uint32_t RESERVED0[1U];
  volatile uint32_t COMP1;                   
  volatile uint32_t MASK1;                   
  volatile uint32_t FUNCTION1;               
        uint32_t RESERVED1[1U];
  volatile uint32_t COMP2;                   
  volatile uint32_t MASK2;                   
  volatile uint32_t FUNCTION2;               
        uint32_t RESERVED2[1U];
  volatile uint32_t COMP3;                   
  volatile uint32_t MASK3;                   
  volatile uint32_t FUNCTION3;               
} DWT_Type;

 






















































 



 



 



 



 



 



 



























   







 



 
typedef struct
{
  volatile const  uint32_t SSPSR;                   
  volatile uint32_t CSPSR;                   
        uint32_t RESERVED0[2U];
  volatile uint32_t ACPR;                    
        uint32_t RESERVED1[55U];
  volatile uint32_t SPPR;                    
        uint32_t RESERVED2[131U];
  volatile const  uint32_t FFSR;                    
  volatile uint32_t FFCR;                    
  volatile const  uint32_t FSCR;                    
        uint32_t RESERVED3[759U];
  volatile const  uint32_t TRIGGER;                 
  volatile const  uint32_t FIFO0;                   
  volatile const  uint32_t ITATBCTR2;               
        uint32_t RESERVED4[1U];
  volatile const  uint32_t ITATBCTR0;               
  volatile const  uint32_t FIFO1;                   
  volatile uint32_t ITCTRL;                  
        uint32_t RESERVED5[39U];
  volatile uint32_t CLAIMSET;                
  volatile uint32_t CLAIMCLR;                
        uint32_t RESERVED7[8U];
  volatile const  uint32_t DEVID;                   
  volatile const  uint32_t DEVTYPE;                 
} TPI_Type;

 



 



 












 






 



 





















 






 





















 






 



 


















 






   








 



 
typedef struct
{
  volatile const  uint32_t TYPE;                    
  volatile uint32_t CTRL;                    
  volatile uint32_t RNR;                     
  volatile uint32_t RBAR;                    
  volatile uint32_t RASR;                    
  volatile uint32_t RBAR_A1;                 
  volatile uint32_t RASR_A1;                 
  volatile uint32_t RBAR_A2;                 
  volatile uint32_t RASR_A2;                 
  volatile uint32_t RBAR_A3;                 
  volatile uint32_t RASR_A3;                 
} MPU_Type;



 









 









 



 









 






























 








 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile uint32_t FPCCR;                   
  volatile uint32_t FPCAR;                   
  volatile uint32_t FPDSCR;                  
  volatile const  uint32_t MVFR0;                   
  volatile const  uint32_t MVFR1;                   
  volatile const  uint32_t MVFR2;                   
} FPU_Type;

 



























 



 












 
























 












 




 







 



 
typedef struct
{
  volatile uint32_t DHCSR;                   
  volatile  uint32_t DCRSR;                   
  volatile uint32_t DCRDR;                   
  volatile uint32_t DEMCR;                   
} CoreDebug_Type;

 




































 






 







































 







 






 







 


 







 

 
#line 1558 "../Drivers/CMSIS/Include/core_cm4.h"

#line 1567 "../Drivers/CMSIS/Include/core_cm4.h"









 










 


 



 





 

#line 1621 "../Drivers/CMSIS/Include/core_cm4.h"

#line 1631 "../Drivers/CMSIS/Include/core_cm4.h"




 
#line 1642 "../Drivers/CMSIS/Include/core_cm4.h"










 
static __inline void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);              

  reg_value  =  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;                                                    
  reg_value &= ~((uint32_t)((0xFFFFUL << 16U) | (7UL << 8U)));  
  reg_value  =  (reg_value                                   |
                ((uint32_t)0x5FAUL << 16U) |
                (PriorityGroupTmp << 8U)  );               
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR =  reg_value;
}






 
static __inline uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) >> 8U));
}







 
static __inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    __memory_changed();
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __memory_changed();
  }
}









 
static __inline uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}







 
static __inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __dsb(0xF);
    __isb(0xF);
  }
}









 
static __inline uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}







 
static __inline void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}







 
static __inline void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}









 
static __inline uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}










 
static __inline void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]               = (uint8_t)((priority << (8U - 4U)) & (uint32_t)0xFFUL);
  }
  else
  {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - 4U)) & (uint32_t)0xFFUL);
  }
}










 
static __inline uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]               >> (8U - 4U)));
  }
  else
  {
    return(((uint32_t)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - 4U)));
  }
}












 
static __inline uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4U)) ? (uint32_t)(4U) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4U)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4U));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority     & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL)))
         );
}












 
static __inline void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4U)) ? (uint32_t)(4U) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4U)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4U));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority     = (Priority                   ) & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL);
}










 
static __inline void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  vectors[(int32_t)IRQn + 16] = vector;
   
}









 
static __inline uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  return vectors[(int32_t)IRQn + 16];
}





 
__declspec(noreturn) static __inline void __NVIC_SystemReset(void)
{
  __dsb(0xF);                                                          
 
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR  = (uint32_t)((0x5FAUL << 16U)    |
                           (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) |
                            (1UL << 2U)    );          
  __dsb(0xF);                                                           

  for(;;)                                                            
  {
    __nop();
  }
}

 


 



#line 1 "../Drivers/CMSIS/Include/mpu_armv7.h"





 
















 
 





 



#line 62 "../Drivers/CMSIS/Include/mpu_armv7.h"

#line 69 "../Drivers/CMSIS/Include/mpu_armv7.h"





 












   














 
#line 110 "../Drivers/CMSIS/Include/mpu_armv7.h"












                          









  










  












  




 




 




 




 





 
typedef struct {
  uint32_t RBAR; 
  uint32_t RASR; 
} ARM_MPU_Region_t;
    


 
static __inline void ARM_MPU_Enable(uint32_t MPU_Control)
{
  __dmb(0xF);
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = MPU_Control | (1UL );

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= (1UL << 16U);

  __dsb(0xF);
  __isb(0xF);
}


 
static __inline void ARM_MPU_Disable(void)
{
  __dmb(0xF);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1UL << 16U);

  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL  &= ~(1UL );
  __dsb(0xF);
  __isb(0xF);
}



 
static __inline void ARM_MPU_ClrRegion(uint32_t rnr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = 0U;
}




    
static __inline void ARM_MPU_SetRegion(uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}





    
static __inline void ARM_MPU_SetRegionEx(uint32_t rnr, uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}





 
static __inline void ARM_MPU_OrderedMemcpy(volatile uint32_t* dst, const uint32_t* __restrict src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i) 
  {
    dst[i] = src[i];
  }
}




 
static __inline void ARM_MPU_Load(ARM_MPU_Region_t const* table, uint32_t cnt) 
{
  const uint32_t rowWordSize = sizeof(ARM_MPU_Region_t)/4U;
  while (cnt > 4U) {
    ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), 4U*rowWordSize);
    table += 4U;
    cnt -= 4U;
  }
  ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), cnt*rowWordSize);
}

#line 1961 "../Drivers/CMSIS/Include/core_cm4.h"




 





 








 
static __inline uint32_t SCB_GetFPUType(void)
{
  uint32_t mvfr0;

  mvfr0 = ((FPU_Type *) ((0xE000E000UL) + 0x0F30UL) )->MVFR0;
  if      ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x020U)
  {
    return 1U;            
  }
  else
  {
    return 0U;            
  }
}


 



 





 













 
static __inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > (0xFFFFFFUL ))
  {
    return (1UL);                                                    
  }

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD  = (uint32_t)(ticks - 1UL);                          
  __NVIC_SetPriority (SysTick_IRQn, (1UL << 4U) - 1UL);  
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL   = 0UL;                                              
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL  = (1UL << 2U) |
                   (1UL << 1U)   |
                   (1UL );                          
  return (0UL);                                                      
}



 



 





 

extern volatile int32_t ITM_RxBuffer;                               










 
static __inline uint32_t ITM_SendChar (uint32_t ch)
{
  if (((((ITM_Type *) (0xE0000000UL) )->TCR & (1UL )) != 0UL) &&       
      ((((ITM_Type *) (0xE0000000UL) )->TER & 1UL               ) != 0UL)   )      
  {
    while (((ITM_Type *) (0xE0000000UL) )->PORT[0U].u32 == 0UL)
    {
      __nop();
    }
    ((ITM_Type *) (0xE0000000UL) )->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}







 
static __inline int32_t ITM_ReceiveChar (void)
{
  int32_t ch = -1;                            

  if (ITM_RxBuffer != ((int32_t)0x5AA55AA5U))
  {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ((int32_t)0x5AA55AA5U);        
  }

  return (ch);
}







 
static __inline int32_t ITM_CheckChar (void)
{

  if (ITM_RxBuffer == ((int32_t)0x5AA55AA5U))
  {
    return (0);                               
  }
  else
  {
    return (1);                               
  }
}

 










#line 141 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
#line 1 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h"
















 



 



 



 









 



 




 







 
extern uint32_t SystemCoreClock;           

extern const uint8_t  AHBPrescTable[16];     
extern const uint8_t  APBPrescTable[8];      



 



 



 



 



 



 

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);


 









 



 
#line 142 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
#line 143 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 



 

typedef struct
{
  volatile uint32_t SR;      
  volatile uint32_t CR1;     
  volatile uint32_t CR2;     
  volatile uint32_t SMPR1;   
  volatile uint32_t SMPR2;   
  volatile uint32_t JOFR1;   
  volatile uint32_t JOFR2;   
  volatile uint32_t JOFR3;   
  volatile uint32_t JOFR4;   
  volatile uint32_t HTR;     
  volatile uint32_t LTR;     
  volatile uint32_t SQR1;    
  volatile uint32_t SQR2;    
  volatile uint32_t SQR3;    
  volatile uint32_t JSQR;    
  volatile uint32_t JDR1;    
  volatile uint32_t JDR2;    
  volatile uint32_t JDR3;    
  volatile uint32_t JDR4;    
  volatile uint32_t DR;      
} ADC_TypeDef;

typedef struct
{
  volatile uint32_t CSR;     
  volatile uint32_t CCR;     
  volatile uint32_t CDR;    
 
} ADC_Common_TypeDef;



 

typedef struct
{
  volatile uint32_t DR;          
  volatile uint8_t  IDR;         
  uint8_t       RESERVED0;   
  uint16_t      RESERVED1;   
  volatile uint32_t CR;          
} CRC_TypeDef;



 

typedef struct
{
  volatile uint32_t IDCODE;   
  volatile uint32_t CR;       
  volatile uint32_t APB1FZ;   
  volatile uint32_t APB2FZ;   
} DBGMCU_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;      
  volatile uint32_t NDTR;    
  volatile uint32_t PAR;     
  volatile uint32_t M0AR;    
  volatile uint32_t M1AR;    
  volatile uint32_t FCR;     
} DMA_Stream_TypeDef;

typedef struct
{
  volatile uint32_t LISR;    
  volatile uint32_t HISR;    
  volatile uint32_t LIFCR;   
  volatile uint32_t HIFCR;   
} DMA_TypeDef;



 

typedef struct
{
  volatile uint32_t IMR;     
  volatile uint32_t EMR;     
  volatile uint32_t RTSR;    
  volatile uint32_t FTSR;    
  volatile uint32_t SWIER;   
  volatile uint32_t PR;      
} EXTI_TypeDef;



 

typedef struct
{
  volatile uint32_t ACR;       
  volatile uint32_t KEYR;      
  volatile uint32_t OPTKEYR;   
  volatile uint32_t SR;        
  volatile uint32_t CR;        
  volatile uint32_t OPTCR;     
  volatile uint32_t OPTCR1;    
} FLASH_TypeDef;



 

typedef struct
{
  volatile uint32_t MODER;     
  volatile uint32_t OTYPER;    
  volatile uint32_t OSPEEDR;   
  volatile uint32_t PUPDR;     
  volatile uint32_t IDR;       
  volatile uint32_t ODR;       
  volatile uint32_t BSRR;      
  volatile uint32_t LCKR;      
  volatile uint32_t AFR[2];    
} GPIO_TypeDef;



 

typedef struct
{
  volatile uint32_t MEMRMP;        
  volatile uint32_t PMC;           
  volatile uint32_t EXTICR[4];     
  uint32_t      RESERVED[2];   
  volatile uint32_t CMPCR;         
} SYSCFG_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;         
  volatile uint32_t CR2;         
  volatile uint32_t OAR1;        
  volatile uint32_t OAR2;        
  volatile uint32_t DR;          
  volatile uint32_t SR1;         
  volatile uint32_t SR2;         
  volatile uint32_t CCR;         
  volatile uint32_t TRISE;       
  volatile uint32_t FLTR;        
} I2C_TypeDef;



 

typedef struct
{
  volatile uint32_t KR;    
  volatile uint32_t PR;    
  volatile uint32_t RLR;   
  volatile uint32_t SR;    
} IWDG_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;    
  volatile uint32_t CSR;   
} PWR_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t PLLCFGR;        
  volatile uint32_t CFGR;           
  volatile uint32_t CIR;            
  volatile uint32_t AHB1RSTR;       
  volatile uint32_t AHB2RSTR;       
  volatile uint32_t AHB3RSTR;       
  uint32_t      RESERVED0;      
  volatile uint32_t APB1RSTR;       
  volatile uint32_t APB2RSTR;       
  uint32_t      RESERVED1[2];   
  volatile uint32_t AHB1ENR;        
  volatile uint32_t AHB2ENR;        
  volatile uint32_t AHB3ENR;        
  uint32_t      RESERVED2;      
  volatile uint32_t APB1ENR;        
  volatile uint32_t APB2ENR;        
  uint32_t      RESERVED3[2];   
  volatile uint32_t AHB1LPENR;      
  volatile uint32_t AHB2LPENR;      
  volatile uint32_t AHB3LPENR;      
  uint32_t      RESERVED4;      
  volatile uint32_t APB1LPENR;      
  volatile uint32_t APB2LPENR;      
  uint32_t      RESERVED5[2];   
  volatile uint32_t BDCR;           
  volatile uint32_t CSR;            
  uint32_t      RESERVED6[2];   
  volatile uint32_t SSCGR;          
  volatile uint32_t PLLI2SCFGR;     
  uint32_t      RESERVED7[1];   
  volatile uint32_t DCKCFGR;        
} RCC_TypeDef;



 

typedef struct
{
  volatile uint32_t TR;       
  volatile uint32_t DR;       
  volatile uint32_t CR;       
  volatile uint32_t ISR;      
  volatile uint32_t PRER;     
  volatile uint32_t WUTR;     
  volatile uint32_t CALIBR;   
  volatile uint32_t ALRMAR;   
  volatile uint32_t ALRMBR;   
  volatile uint32_t WPR;      
  volatile uint32_t SSR;      
  volatile uint32_t SHIFTR;   
  volatile uint32_t TSTR;     
  volatile uint32_t TSDR;     
  volatile uint32_t TSSSR;    
  volatile uint32_t CALR;     
  volatile uint32_t TAFCR;    
  volatile uint32_t ALRMASSR; 
  volatile uint32_t ALRMBSSR; 
  uint32_t RESERVED7;     
  volatile uint32_t BKP0R;    
  volatile uint32_t BKP1R;    
  volatile uint32_t BKP2R;    
  volatile uint32_t BKP3R;    
  volatile uint32_t BKP4R;    
  volatile uint32_t BKP5R;    
  volatile uint32_t BKP6R;    
  volatile uint32_t BKP7R;    
  volatile uint32_t BKP8R;    
  volatile uint32_t BKP9R;    
  volatile uint32_t BKP10R;   
  volatile uint32_t BKP11R;   
  volatile uint32_t BKP12R;   
  volatile uint32_t BKP13R;   
  volatile uint32_t BKP14R;   
  volatile uint32_t BKP15R;   
  volatile uint32_t BKP16R;   
  volatile uint32_t BKP17R;   
  volatile uint32_t BKP18R;   
  volatile uint32_t BKP19R;   
} RTC_TypeDef;



 

typedef struct
{
  volatile uint32_t POWER;                  
  volatile uint32_t CLKCR;                  
  volatile uint32_t ARG;                    
  volatile uint32_t CMD;                    
  volatile const uint32_t  RESPCMD;         
  volatile const uint32_t  RESP1;           
  volatile const uint32_t  RESP2;           
  volatile const uint32_t  RESP3;           
  volatile const uint32_t  RESP4;           
  volatile uint32_t DTIMER;                 
  volatile uint32_t DLEN;                   
  volatile uint32_t DCTRL;                  
  volatile const uint32_t  DCOUNT;          
  volatile const uint32_t  STA;             
  volatile uint32_t ICR;                    
  volatile uint32_t MASK;                   
  uint32_t      RESERVED0[2];           
  volatile const uint32_t  FIFOCNT;         
  uint32_t      RESERVED1[13];          
  volatile uint32_t FIFO;                   
} SDIO_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;         
  volatile uint32_t CR2;         
  volatile uint32_t SR;          
  volatile uint32_t DR;          
  volatile uint32_t CRCPR;       
  volatile uint32_t RXCRCR;      
  volatile uint32_t TXCRCR;      
  volatile uint32_t I2SCFGR;     
  volatile uint32_t I2SPR;       
} SPI_TypeDef;




 

typedef struct
{
  volatile uint32_t CR1;          
  volatile uint32_t CR2;          
  volatile uint32_t SMCR;         
  volatile uint32_t DIER;         
  volatile uint32_t SR;           
  volatile uint32_t EGR;          
  volatile uint32_t CCMR1;        
  volatile uint32_t CCMR2;        
  volatile uint32_t CCER;         
  volatile uint32_t CNT;          
  volatile uint32_t PSC;          
  volatile uint32_t ARR;          
  volatile uint32_t RCR;          
  volatile uint32_t CCR1;         
  volatile uint32_t CCR2;         
  volatile uint32_t CCR3;         
  volatile uint32_t CCR4;         
  volatile uint32_t BDTR;         
  volatile uint32_t DCR;          
  volatile uint32_t DMAR;         
  volatile uint32_t OR;           
} TIM_TypeDef;



 

typedef struct
{
  volatile uint32_t SR;          
  volatile uint32_t DR;          
  volatile uint32_t BRR;         
  volatile uint32_t CR1;         
  volatile uint32_t CR2;         
  volatile uint32_t CR3;         
  volatile uint32_t GTPR;        
} USART_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;    
  volatile uint32_t CFR;   
  volatile uint32_t SR;    
} WWDG_TypeDef;


 
typedef struct
{
  volatile uint32_t GOTGCTL;               
  volatile uint32_t GOTGINT;               
  volatile uint32_t GAHBCFG;               
  volatile uint32_t GUSBCFG;               
  volatile uint32_t GRSTCTL;               
  volatile uint32_t GINTSTS;               
  volatile uint32_t GINTMSK;               
  volatile uint32_t GRXSTSR;               
  volatile uint32_t GRXSTSP;               
  volatile uint32_t GRXFSIZ;               
  volatile uint32_t DIEPTXF0_HNPTXFSIZ;    
  volatile uint32_t HNPTXSTS;              
  uint32_t Reserved30[2];              
  volatile uint32_t GCCFG;                 
  volatile uint32_t CID;                   
  uint32_t  Reserved40[48];            
  volatile uint32_t HPTXFSIZ;              
  volatile uint32_t DIEPTXF[0x0F];         
} USB_OTG_GlobalTypeDef;



 
typedef struct
{
  volatile uint32_t DCFG;             
  volatile uint32_t DCTL;             
  volatile uint32_t DSTS;             
  uint32_t Reserved0C;            
  volatile uint32_t DIEPMSK;          
  volatile uint32_t DOEPMSK;          
  volatile uint32_t DAINT;            
  volatile uint32_t DAINTMSK;         
  uint32_t  Reserved20;           
  uint32_t Reserved9;             
  volatile uint32_t DVBUSDIS;         
  volatile uint32_t DVBUSPULSE;       
  volatile uint32_t DTHRCTL;          
  volatile uint32_t DIEPEMPMSK;       
  volatile uint32_t DEACHINT;         
  volatile uint32_t DEACHMSK;         
  uint32_t Reserved40;            
  volatile uint32_t DINEP1MSK;        
  uint32_t  Reserved44[15];       
  volatile uint32_t DOUTEP1MSK;       
} USB_OTG_DeviceTypeDef;



 
typedef struct
{
  volatile uint32_t DIEPCTL;            
  uint32_t Reserved04;              
  volatile uint32_t DIEPINT;            
  uint32_t Reserved0C;              
  volatile uint32_t DIEPTSIZ;           
  volatile uint32_t DIEPDMA;            
  volatile uint32_t DTXFSTS;            
  uint32_t Reserved18;              
} USB_OTG_INEndpointTypeDef;



 
typedef struct
{
  volatile uint32_t DOEPCTL;        
  uint32_t Reserved04;          
  volatile uint32_t DOEPINT;        
  uint32_t Reserved0C;          
  volatile uint32_t DOEPTSIZ;       
  volatile uint32_t DOEPDMA;        
  uint32_t Reserved18[2];       
} USB_OTG_OUTEndpointTypeDef;



 
typedef struct
{
  volatile uint32_t HCFG;              
  volatile uint32_t HFIR;              
  volatile uint32_t HFNUM;             
  uint32_t Reserved40C;            
  volatile uint32_t HPTXSTS;           
  volatile uint32_t HAINT;             
  volatile uint32_t HAINTMSK;          
} USB_OTG_HostTypeDef;



 
typedef struct
{
  volatile uint32_t HCCHAR;            
  volatile uint32_t HCSPLT;            
  volatile uint32_t HCINT;             
  volatile uint32_t HCINTMSK;          
  volatile uint32_t HCTSIZ;            
  volatile uint32_t HCDMA;             
  uint32_t Reserved[2];            
} USB_OTG_HostChannelTypeDef;



 



 
#line 645 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 



 





 
#line 673 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 





 
#line 691 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 720 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 

 


#line 739 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"






 



 
#line 771 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 811 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 



 



 



 



 

 
 
 

 
 
 
 
 

 
#line 861 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 915 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 965 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1021 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1083 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 




 




 




 




 
#line 1154 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1204 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1254 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1293 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 




 




 
#line 1321 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1341 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 


 
#line 1380 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1388 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 



 
 
 
 
 
 





 





 





 
 
 
 
 
 
#line 1494 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 1520 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1539 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1601 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1663 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1725 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1787 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 





 
 
 
 
 
 
#line 1879 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1907 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 1978 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2003 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2074 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2145 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2216 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2287 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
 
 
 
 
 
#line 2305 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


#line 2322 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2348 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2384 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2392 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 2433 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2450 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 2460 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
 
 
 
 
 
#line 2547 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2629 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2679 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2697 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2779 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2829 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2911 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 2961 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3011 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3029 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3079 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 3096 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3194 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3276 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 3328 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 3385 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3427 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3485 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3527 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 
 
 
 
 
 
#line 3577 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3588 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 3604 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 



#line 3639 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





 
#line 3651 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 3700 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3726 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3737 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 3750 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
 
 
 
 
 




 
#line 3768 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 3781 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
 
 
 
 
 
#line 3805 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 3812 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3848 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 


 
#line 3874 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 


 
 
 
 
 
 
#line 3890 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 3899 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 3911 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 3930 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 


#line 3941 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 3952 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 3965 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 3979 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 3987 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 
 










 










 
#line 4020 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 4030 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4038 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







 
#line 4052 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







 
#line 4068 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 










#line 4086 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 4093 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







 
#line 4119 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 4141 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 4160 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





 
#line 4193 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 



 


 
#line 4238 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4276 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 


 
#line 4308 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 


 






 
#line 4355 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4393 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4428 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 




 

 
#line 4474 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4512 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4526 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 4539 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4571 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 



 
#line 4588 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4599 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 4612 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 4619 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 






 
 
 
 
 
 
#line 4674 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4718 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4788 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 


 
#line 4841 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4849 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 4862 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 4932 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5002 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 
#line 5020 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5063 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5093 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 5121 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5169 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 


 
#line 5184 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5196 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 




 



 
 
 
 
 
 






 
#line 5326 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 5339 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 










#line 5377 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 




 




 




 




 




 




 
#line 5431 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 5439 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 5452 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 5531 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5572 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5646 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 
 
 
 
 


 
#line 5674 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 5681 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 5712 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5735 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5764 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 




 




 






























#line 5822 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5833 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
 
 
 
 
 





 




 
#line 5863 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 
#line 5872 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5882 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5892 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5902 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5916 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5926 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5936 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5946 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5956 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 5970 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5980 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 5990 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 6000 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 6010 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6024 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 6034 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 6044 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 6054 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"



 
#line 6064 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6072 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
 
 
 
 
 
#line 6094 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

















 
#line 6121 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6128 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6153 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6161 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6168 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





#line 6180 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 6193 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6240 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6278 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6304 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 






#line 6318 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6325 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"











#line 6342 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6349 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





 







#line 6369 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 6383 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 






#line 6397 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6404 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"











#line 6421 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6428 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





 







#line 6448 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 6462 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6509 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 




 




 




 




 




 




 
#line 6562 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 6587 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6597 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 6606 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 






#line 6629 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 
 
 
 
 
 
#line 6667 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 6680 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6727 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6750 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"











 
#line 6798 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6811 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





 
 
 
 
 
 
#line 6832 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 6840 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





 
#line 6856 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 6864 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"






 







 





 
 
 
 
 
 
#line 6896 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 6910 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







 
#line 6948 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 


 
#line 6964 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
 
 
 
 
 
#line 7001 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 

#line 7012 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 

#line 7023 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7034 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"





















 
#line 7065 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7085 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7099 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7121 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 7134 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




#line 7151 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7173 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 

#line 7237 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7254 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


#line 7270 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7296 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7312 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7324 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 7367 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 7446 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7526 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7534 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 7553 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7561 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 
#line 7585 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




#line 7607 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7618 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7626 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7642 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7658 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 7671 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7691 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7699 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 
#line 7733 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7762 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7771 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7779 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







 
#line 7820 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7828 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7842 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7851 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7877 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




#line 7896 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"













#line 7928 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 

#line 7941 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7952 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 7964 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 7999 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 8040 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 8075 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 

#line 8087 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 
#line 8102 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 




 




 
#line 8125 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 

#line 8166 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 8201 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 

#line 8209 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







 
#line 8226 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
 
#line 8239 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"







#line 8253 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 8261 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

#line 8269 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"


 



 



 

 



 



 
#line 8306 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 8314 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 




 


 







 


 



 



 








 
#line 8360 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 8370 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 8378 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 






 






 


 






 






 






 






 






 






 
#line 8445 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 



 






 




 
#line 8502 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 






 






 
#line 8526 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 



 






 
#line 8545 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 






 
#line 8560 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 
#line 8568 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

 


 
#line 8579 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"
 





 


 




 




 


 



 


 




 




 


 


 


 


 


 







 



#line 8653 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h"

















 



 



 





#line 155 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
#line 176 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"



 



 
typedef enum
{
  RESET = 0U,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0U,
  ENABLE = !DISABLE
} FunctionalState;


typedef enum
{
  SUCCESS = 0U,
  ERROR = !SUCCESS
} ErrorStatus;



 




 
















 
 
#line 236 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"

 
#line 245 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"

 
#line 254 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"

 
#line 263 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"

 
#line 272 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"

 
#line 281 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"



 

#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"

















  

 
#line 296 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"


#line 288 "../Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"









 



 
#line 30 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

















 

 







 
 
 



 
#line 46 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 
#line 93 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 101 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"
















 



 





 



 
#line 150 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 217 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"







 



 
#line 235 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 
#line 249 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 






 



 

#line 277 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"






#line 289 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"














 



 
#line 322 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"





#line 367 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 377 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 439 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"






 



 

#line 567 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 

#line 584 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 

#line 609 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 618 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 717 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"




 




 
#line 738 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 





 



 


















#line 789 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"





#line 801 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 808 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"









#line 824 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 839 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 
#line 867 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 
#line 883 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 892 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"





#line 907 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1019 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1035 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 
#line 1060 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 





 



 






 



 















 
 







 



 








 



 














 



 










 



 







































 



 


#line 1217 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"






 



 

 
#line 1239 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

 












 



 

































#line 1300 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"
















#line 1322 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 




 















 




 
#line 1363 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 









#line 1393 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 



#line 1431 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1441 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1460 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"










#line 1487 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"







 



 




 



 

























 




 








 



 




 



 
#line 1571 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 

#line 1588 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1600 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1641 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"





 



 











 

#line 1691 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1705 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 

 



 



 



 








 




 




 



 
#line 1756 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

 





































 



 
#line 1826 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 
#line 1841 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 









#line 1874 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1885 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 

#line 1915 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 1923 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"






#line 1939 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



#line 2028 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 
#line 2042 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 





 



 



 



 
#line 2081 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 



 



 






 




 



 

 



 





 



 
#line 2142 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"









 




 
#line 2170 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2191 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2202 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2211 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2224 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2233 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 







 



 
#line 2269 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2284 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


#line 2317 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 
#line 2485 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



#line 2495 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 

#line 2511 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 







 



 

#line 2534 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 

#line 2562 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 










 



 














 




 




 




 







 




 
#line 2640 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 




 
#line 2690 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2704 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 




 








#line 2984 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 2998 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3215 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3234 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3241 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3262 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3410 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

 



#line 3435 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3456 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3573 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3582 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3599 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3614 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"






#line 3643 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

















#line 3669 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"





#line 3696 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"





#line 3708 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3717 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3750 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3768 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"












#line 3786 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3809 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3849 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3935 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"



 



 




 



 
#line 3961 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3989 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 3996 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 4011 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"











 



 




#line 4039 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 4061 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"
 




#line 4091 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 4098 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 4110 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 

#line 4124 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"








 



 
#line 4145 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 







 



 













 




 









#line 4199 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 












#line 4225 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 4234 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"

#line 4243 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"




#line 4276 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 








#line 4303 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"







 



 

#line 4323 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"






 



 




 



 
#line 4357 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 







 



 
#line 4384 "../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"


 



 





 



 





 



 



 








#line 31 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"
#line 1 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"
 






 

 
 
 





 





#line 34 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"




  typedef signed int ptrdiff_t;



  



    typedef unsigned int size_t;    
#line 57 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"



   



      typedef unsigned short wchar_t;  
#line 82 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"



    




   




  typedef long double max_align_t;









#line 114 "D:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stddef.h"



 

#line 32 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"

 



   
typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;



 
typedef enum 
{
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED   = 0x01U  
} HAL_LockTypeDef;

 






























 


#line 104 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"







#line 126 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"


 
#line 155 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"




  









 


#line 188 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"



  



 


#line 205 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h"








#line 28 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

 
 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"















 

 







 
#line 28 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"



 



 

 


 



 
typedef struct
{
  uint32_t PLLState;   
 

  uint32_t PLLSource;  
 

  uint32_t PLLM;       
 

  uint32_t PLLN;       

 

  uint32_t PLLP;       
 

  uint32_t PLLQ;       
 
#line 73 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
} RCC_PLLInitTypeDef;

#line 174 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 200 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 291 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 376 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"





 
typedef struct
{

  uint32_t PLLI2SM;    
 


  uint32_t PLLI2SN;    


 

  uint32_t PLLI2SR;    

 

} RCC_PLLI2SInitTypeDef;



 
typedef struct
{
  uint32_t PeriphClockSelection; 
 

  RCC_PLLI2SInitTypeDef PLLI2S;  
 

  uint32_t RTCClockSelection;      
 

  uint8_t TIMPresSelection;        
 

} RCC_PeriphCLKInitTypeDef;



 

 


 



 
 
#line 452 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 462 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 479 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 493 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 505 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 517 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 


 






 




 





 
#line 546 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"


 



 
#line 560 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"


 



 
#line 573 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"


 

#line 598 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 627 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 720 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 775 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 854 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 888 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"








 




 










 




 










 






 




#line 952 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"



 

 


 
 
#line 2026 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 2907 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 3288 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 3571 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 







 
#line 3614 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"


 







 











 







 







 







 




 







 
#line 3714 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"


 







 













 




 
#line 3776 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"


 







 











 




 










 




 







 




 




 




 
#line 3849 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"








 




 












 








 













 








 




 




 













 




 











 

 

 
#line 4757 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 5795 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 5841 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"




























 
#line 5876 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 








 



#line 5959 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"














 

























 





#line 6027 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 6117 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 
#line 6136 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6155 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6167 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
 

 











 








 



#line 6226 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6391 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6440 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6674 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6729 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"


















 






 

#line 6780 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6793 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"



 

 


 



 
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);

uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk);





void HAL_RCCEx_SelectLSEMode(uint8_t Mode);


HAL_StatusTypeDef HAL_RCCEx_EnablePLLI2S(RCC_PLLI2SInitTypeDef  *PLLI2SInit);
HAL_StatusTypeDef HAL_RCCEx_DisablePLLI2S(void);







 



 
 
 
 


 




 
 
#line 6852 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"






 






 





 
#line 6878 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

 





 







#line 6904 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 6914 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"




 



 

 


 


 





































#line 6987 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"









#line 7015 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 7067 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 7088 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"

#line 7152 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"




















 



 



 



 






#line 32 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"



 



 

 


 



 
typedef struct
{
  uint32_t OscillatorType;       
 

  uint32_t HSEState;             
 

  uint32_t LSEState;             
 

  uint32_t HSIState;             
 

  uint32_t HSICalibrationValue;  
 

  uint32_t LSIState;             
 

  RCC_PLLInitTypeDef PLL;         
} RCC_OscInitTypeDef;



 
typedef struct
{
  uint32_t ClockType;             
 

  uint32_t SYSCLKSource;          
 

  uint32_t AHBCLKDivider;         
 

  uint32_t APB1CLKDivider;        
 

  uint32_t APB2CLKDivider;        
 

} RCC_ClkInitTypeDef;



 

 


 



 







 



 





 



 





 



 






 



 




 



 





 



 






 



 




 



 






 





 






 





 






 



 
#line 234 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 



 







 



 
#line 287 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 



 




 



 






 



 







 



 
#line 333 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 









 
 





 


 
#line 364 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 



 

 


 







 
#line 426 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 433 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 







 
#line 450 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 457 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 







 
#line 517 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 525 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 







 
#line 543 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 551 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 







 
#line 618 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 627 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 







 
#line 646 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 655 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 




 
#line 669 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 677 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 




 
#line 692 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 701 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 




 
#line 717 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 727 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 








 
#line 745 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 752 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 








 
#line 771 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 779 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 








 
#line 799 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"

#line 808 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 



 















 









 




 



 








 




 



 





















 
#line 907 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 



 


















 
#line 950 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"


 



 



 























 













 






 






 




 



 







 










 










 



 



 









 










 







 



 



 















 




















 




 




 











 












 













 













 




 



















 







 



 

 


 



 
 
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(const RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);


 



 
 
void     HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void     HAL_RCC_EnableCSS(void);
void     HAL_RCC_DisableCSS(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
void     HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void     HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);

 
void HAL_RCC_NMI_IRQHandler(void);

 
void HAL_RCC_CSSCallback(void);



 



 

 
 
 


 




 

 
 



 


 



 
 



 



 
 




 


 


 


 












 



 

 


 



 






















#line 1408 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"































 



 



 



 







#line 276 "../Core/Inc/stm32f4xx_hal_conf.h"


#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"
















  

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"



 



  

 


 



  
typedef struct
{
  uint32_t Pin;       
 

  uint32_t Mode;      
 

  uint32_t Pull;      
 

  uint32_t Speed;     
 

  uint32_t Alternate;  
 
}GPIO_InitTypeDef;



 
typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
}GPIO_PinState;


 

 



  



 
#line 102 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"




 









  







    



 






 




 






 

 


   





 
  


 

 


 






 







 







 







 







 



 

 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
















  

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"



 



  

 
 


 
  


 

 
#line 165 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 280 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 386 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 482 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"

 

 
#line 572 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 681 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"

 

 
#line 815 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"

 



  








  





  






  






  






  








 








  







  




  






  




  




  



 
#line 981 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"

 
#line 1101 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 1224 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 


  



 

 


 


 

 


 


 

 
 
 


 


 

 


 


 
#line 1276 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"

#line 1290 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"







#line 1304 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"

#line 1332 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"



 



   
 
#line 1366 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 1393 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 1417 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 1440 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"

 

 
#line 1460 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 
 




 
#line 1483 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"


 

 
#line 1517 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 
#line 1546 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
 

 



 

 



 



  



 

 


 



 



  



  
  






#line 214 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"

 


 



 
 
void  HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void  HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);


 



 
 
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);



  



  
 
 
 


 
#line 272 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"



 

 


 
#line 299 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"


 

 


 



 



  



 







#line 280 "../Core/Inc/stm32f4xx_hal_conf.h"


#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_exti.h"
















 

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_exti.h"



 




 

 



 
typedef enum
{
  HAL_EXTI_COMMON_CB_ID          = 0x00U
} EXTI_CallbackIDTypeDef;



 
typedef struct
{
  uint32_t Line;                     
  void (* PendingCallback)(void);    
} EXTI_HandleTypeDef;



 
typedef struct
{
  uint32_t Line;      
 
  uint32_t Mode;      
 
  uint32_t Trigger;   
 
  uint32_t GPIOSel;   

 
} EXTI_ConfigTypeDef;



 

 


 



 
#line 124 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_exti.h"



 



 





 



 







 




 
#line 182 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_exti.h"



 



 

 


 



 

 


 


 








 




 




 




 








 

 


 














#line 309 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_exti.h"




 

 



 




 
 
HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti);
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine);


 




 
 
void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti);
uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti);



 



 



 



 







#line 284 "../Core/Inc/stm32f4xx_hal_conf.h"


#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"
















  

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"



 



  

 




 
   


 
typedef struct
{
  uint32_t Channel;              
 

  uint32_t Direction;            

 

  uint32_t PeriphInc;            
 

  uint32_t MemInc;               
 

  uint32_t PeriphDataAlignment;  
 

  uint32_t MemDataAlignment;     
 

  uint32_t Mode;                 


 

  uint32_t Priority;             
 

  uint32_t FIFOMode;             


 

  uint32_t FIFOThreshold;        
 

  uint32_t MemBurst;             



 

  uint32_t PeriphBurst;          



 
}DMA_InitTypeDef;




 
typedef enum
{
  HAL_DMA_STATE_RESET             = 0x00U,   
  HAL_DMA_STATE_READY             = 0x01U,   
  HAL_DMA_STATE_BUSY              = 0x02U,   
  HAL_DMA_STATE_TIMEOUT           = 0x03U,   
  HAL_DMA_STATE_ERROR             = 0x04U,   
  HAL_DMA_STATE_ABORT             = 0x05U,   
}HAL_DMA_StateTypeDef;



 
typedef enum
{
  HAL_DMA_FULL_TRANSFER           = 0x00U,   
  HAL_DMA_HALF_TRANSFER           = 0x01U    
}HAL_DMA_LevelCompleteTypeDef;



 
typedef enum
{
  HAL_DMA_XFER_CPLT_CB_ID         = 0x00U,   
  HAL_DMA_XFER_HALFCPLT_CB_ID     = 0x01U,   
  HAL_DMA_XFER_M1CPLT_CB_ID       = 0x02U,   
  HAL_DMA_XFER_M1HALFCPLT_CB_ID   = 0x03U,   
  HAL_DMA_XFER_ERROR_CB_ID        = 0x04U,   
  HAL_DMA_XFER_ABORT_CB_ID        = 0x05U,   
  HAL_DMA_XFER_ALL_CB_ID          = 0x06U    
}HAL_DMA_CallbackIDTypeDef;



 
typedef struct __DMA_HandleTypeDef
{
  DMA_Stream_TypeDef         *Instance;                                                         

  DMA_InitTypeDef            Init;                                                               

  HAL_LockTypeDef            Lock;                                                                

  volatile HAL_DMA_StateTypeDef  State;                                                             

  void                       *Parent;                                                            

  void                       (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);          

  void                       (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);      

  void                       (* XferM1CpltCallback)( struct __DMA_HandleTypeDef * hdma);        
  
  void                       (* XferM1HalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);    
  
  void                       (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);         
  
  void                       (* XferAbortCallback)( struct __DMA_HandleTypeDef * hdma);           

  volatile uint32_t              ErrorCode;                                                         
  
  uint32_t                   StreamBaseAddress;                                                 

  uint32_t                   StreamIndex;                                                       
 
}DMA_HandleTypeDef;



 

 




 




  
#line 193 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"


 




  
#line 219 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"


 




  





 
        



  




  




  




 




  





  




 





 




  





 




 






  




 




  




 






  




  






  




  






 




 







 




  
#line 382 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"


 



 
 
 




 













 






 






 


 





 
#line 447 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"





       
#line 467 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"





 
#line 487 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"





 
#line 507 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"





 
#line 527 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"













 

















 
















 














 














 




















 







 



 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma_ex.h"
















 

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma_ex.h"



 



  

 



 
   


  
typedef enum
{
  MEMORY0      = 0x00U,     
  MEMORY1      = 0x01U      
}HAL_DMA_MemoryTypeDef;



 

 



 




 

 
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_ChangeMemory(DMA_HandleTypeDef *hdma, uint32_t Address, HAL_DMA_MemoryTypeDef memory);



 


 
         
 



 


 



 



 







#line 640 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"

 




 




 
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma); 
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma);


 




 
HAL_StatusTypeDef HAL_DMA_Start (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout);
void              HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_CleanCallbacks(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)(DMA_HandleTypeDef *_hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID);



  




 
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma);
uint32_t             HAL_DMA_GetError(DMA_HandleTypeDef *hdma);


  


  
 



 


  

 



 
#line 729 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h"

















































  

 



 


 



  



 







#line 288 "../Core/Inc/stm32f4xx_hal_conf.h"


#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"















  

 







 
#line 28 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"



 



  
 


 





 
typedef struct
{
  uint8_t                Enable;                
 
  uint8_t                Number;                
 
  uint32_t               BaseAddress;            
  uint8_t                Size;                  
 
  uint8_t                SubRegionDisable;      
          
  uint8_t                TypeExtField;          
                  
  uint8_t                AccessPermission;      
 
  uint8_t                DisableExec;           
 
  uint8_t                IsShareable;           
 
  uint8_t                IsCacheable;           
 
  uint8_t                IsBufferable;          
 
}MPU_Region_InitTypeDef;


 




 

 



 



 
#line 98 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"


 



 





 




 







 



 




 



 




 



 




 



 




 



 




 



 





 



 
#line 211 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"


 
   


 
#line 224 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"


 



 
#line 239 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"


 




 


 

 


 
  


 
 
void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup);
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
void HAL_NVIC_SystemReset(void);
uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb);


 



 
 
uint32_t HAL_NVIC_GetPriorityGrouping(void);
void HAL_NVIC_GetPriority(IRQn_Type IRQn, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority);
uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
uint32_t HAL_NVIC_GetActive(IRQn_Type IRQn);
void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource);
void HAL_SYSTICK_IRQHandler(void);
void HAL_SYSTICK_Callback(void);


void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_EnableRegion(uint32_t RegionNumber);
void HAL_MPU_DisableRegion(uint32_t RegionNumber);
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);

void HAL_CORTEX_ClearEvent(void);


 



 

 
 
 
 


 



































#line 348 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"

#line 357 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"

#line 386 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"






 

 



  



 
  





 

#line 292 "../Core/Inc/stm32f4xx_hal_conf.h"










































#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h"















 

 







 
#line 28 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h"



 



 

 


 



 
typedef enum
{
  FLASH_PROC_NONE = 0U,
  FLASH_PROC_SECTERASE,
  FLASH_PROC_MASSERASE,
  FLASH_PROC_PROGRAM
} FLASH_ProcedureTypeDef;



 
typedef struct
{
  volatile FLASH_ProcedureTypeDef ProcedureOnGoing;    

  volatile uint32_t               NbSectorsToErase;    

  volatile uint8_t                VoltageForErase;     

  volatile uint32_t               Sector;              

  volatile uint32_t               Bank;                

  volatile uint32_t               Address;             

  HAL_LockTypeDef             Lock;                

  volatile uint32_t               ErrorCode;           

} FLASH_ProcessTypeDef;



 

 


 



 
#line 95 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h"


 



 






 




 
#line 124 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h"


 




 




 



 







 



 







 



 

 


 





 






 





 





 





 





 





 





 






 








 










 









 
















 















 



 

 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"















 

 







 
#line 28 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"



 



 

 


 



 
typedef struct
{
  uint32_t TypeErase;   
 

  uint32_t Banks;       
 

  uint32_t Sector;      
 

  uint32_t NbSectors;   
 

  uint32_t VoltageRange;
 

} FLASH_EraseInitTypeDef;



 
typedef struct
{
  uint32_t OptionType;   
 

  uint32_t WRPState;     
 

  uint32_t WRPSector;         
 

  uint32_t Banks;        
 

  uint32_t RDPLevel;     
 

  uint32_t BORLevel;     
 

  uint8_t  USERConfig;    

} FLASH_OBProgramInitTypeDef;



 





typedef struct
{
  uint32_t OptionType;     
 

  uint32_t PCROPState;     
 



  uint16_t Sectors;        
 



#line 128 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
} FLASH_AdvOBProgramInitTypeDef;




 

 



 



 




 



 






 



 




 



 






 



 






 



 




 



 




 




 




 



 






 








 




 






 






#line 264 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"


 



 
 
#line 291 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 





#line 309 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 



 




 
#line 325 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"

#line 334 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"


 



 





#line 354 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"


 



 
 
#line 389 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 410 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 428 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 439 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 449 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 462 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 



 



 
 
#line 500 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 522 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 541 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 553 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 564 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 578 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 


 



 
 
#line 615 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 637 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 649 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 660 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 

 
#line 675 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"
 



 



 







 



 
#line 706 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"


 



 

 

 


 



 
 
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void              HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);






HAL_StatusTypeDef HAL_FLASHEx_AdvOBProgram(FLASH_AdvOBProgramInitTypeDef *pAdvOBInit);
void              HAL_FLASHEx_AdvOBGetConfig(FLASH_AdvOBProgramInitTypeDef *pAdvOBInit);
HAL_StatusTypeDef HAL_FLASHEx_OB_SelectPCROP(void);
HAL_StatusTypeDef HAL_FLASHEx_OB_DeSelectPCROP(void);










 



 
 
 
 


 
 




 




 





 




 




 






 






 

 


 



 



























#line 847 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"







#line 861 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"

#line 881 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"

#line 896 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"







#line 911 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"

#line 926 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"

#line 937 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"

#line 947 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"













#line 966 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"

























































#line 1032 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h"


 



 

 


 
void FLASH_Erase_Sector(uint32_t Sector, uint8_t VoltageRange);
void FLASH_FlushCaches(void);


 



 



 







#line 296 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h"
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ramfunc.h"















 

 









 
#line 30 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ramfunc.h"



 



 

 
 
 


 



 
 HAL_StatusTypeDef HAL_FLASHEx_StopFlashInterfaceClk(void);
 HAL_StatusTypeDef HAL_FLASHEx_StartFlashInterfaceClk(void);
 HAL_StatusTypeDef HAL_FLASHEx_EnableFlashSleepMode(void);
 HAL_StatusTypeDef HAL_FLASHEx_DisableFlashSleepMode(void);


 



 



 



 









#line 297 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h"

 


 


 
 
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
 
void HAL_FLASH_IRQHandler(void);
 
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);


 



 
 
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
 
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void);


 



 
 
uint32_t HAL_FLASH_GetError(void);
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);


 



 
 
 


 



 
 


 



 



 



 



 



 




 

 


 



 






 



 

 


 



 



 



 







#line 336 "../Core/Inc/stm32f4xx_hal_conf.h"














































#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h"















  

 







 
#line 28 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h"



 



  

 



 
   


 
typedef struct
{
  uint32_t PVDLevel;   
 

  uint32_t Mode;      
 
}PWR_PVDTypeDef;



 

 


 
  


 



 



  
#line 84 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h"


    
 


 
#line 98 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h"


 




 




 
    


 






 



 





 



 







 



  
  
 


 





















 







 





 





 





 





 





 





 





 






 






 








 







 





 





 




 

 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"















  

 







 
#line 28 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"



 



  

  
 


 
#line 64 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"



 
#line 78 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"


 
#line 97 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"



  
  
 


 

#line 125 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"









 
#line 143 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"

#line 191 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"


 

 


 
 


 
void HAL_PWREx_EnableFlashPowerDown(void);
void HAL_PWREx_DisableFlashPowerDown(void); 
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void); 
uint32_t HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);




void HAL_PWREx_EnableMainRegulatorLowVoltage(void);
void HAL_PWREx_DisableMainRegulatorLowVoltage(void);
void HAL_PWREx_EnableLowRegulatorLowVoltage(void);
void HAL_PWREx_DisableLowRegulatorLowVoltage(void);



#line 226 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"



 



 
 
 
 


 



 
 
 
 



 



 


    
 



 



 

 



   
 
 





 



 

 


 



 






#line 308 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"

#line 319 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h"


 



 



  



 
  





#line 276 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h"

 


 
  


 
 
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);


 



 
 
 
void HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);

 
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinx);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);

 
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

 
void HAL_PWR_PVD_IRQHandler(void);
void HAL_PWR_PVDCallback(void);

 
void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);


 



 

 
 
 


 



 



 



 
 







 



 
 
 



 



 




 



 
 
 




 



 
 


 



 
#line 407 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h"










 



 



  



 
  





#line 384 "../Core/Inc/stm32f4xx_hal_conf.h"






















#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"
















 

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"



 



 

 


 



 
typedef struct
{
  uint32_t Prescaler;         
 

  uint32_t CounterMode;       
 

  uint32_t Period;            

 

  uint32_t ClockDivision;     
 

  uint32_t RepetitionCounter;  








 

  uint32_t AutoReloadPreload;  
 
} TIM_Base_InitTypeDef;



 
typedef struct
{
  uint32_t OCMode;        
 

  uint32_t Pulse;         
 

  uint32_t OCPolarity;    
 

  uint32_t OCNPolarity;   

 

  uint32_t OCFastMode;    

 


  uint32_t OCIdleState;   

 

  uint32_t OCNIdleState;  

 
} TIM_OC_InitTypeDef;



 
typedef struct
{
  uint32_t OCMode;        
 

  uint32_t Pulse;         
 

  uint32_t OCPolarity;    
 

  uint32_t OCNPolarity;   

 

  uint32_t OCIdleState;   

 

  uint32_t OCNIdleState;  

 

  uint32_t ICPolarity;    
 

  uint32_t ICSelection;   
 

  uint32_t ICFilter;      
 
} TIM_OnePulse_InitTypeDef;



 
typedef struct
{
  uint32_t  ICPolarity;  
 

  uint32_t ICSelection;  
 

  uint32_t ICPrescaler;  
 

  uint32_t ICFilter;     
 
} TIM_IC_InitTypeDef;



 
typedef struct
{
  uint32_t EncoderMode;   
 

  uint32_t IC1Polarity;   
 

  uint32_t IC1Selection;  
 

  uint32_t IC1Prescaler;  
 

  uint32_t IC1Filter;     
 

  uint32_t IC2Polarity;   
 

  uint32_t IC2Selection;  
 

  uint32_t IC2Prescaler;  
 

  uint32_t IC2Filter;     
 
} TIM_Encoder_InitTypeDef;



 
typedef struct
{
  uint32_t ClockSource;     
 
  uint32_t ClockPolarity;   
 
  uint32_t ClockPrescaler;  
 
  uint32_t ClockFilter;     
 
} TIM_ClockConfigTypeDef;



 
typedef struct
{
  uint32_t ClearInputState;      
 
  uint32_t ClearInputSource;     
 
  uint32_t ClearInputPolarity;   
 
  uint32_t ClearInputPrescaler;  

 
  uint32_t ClearInputFilter;     
 
} TIM_ClearInputConfigTypeDef;



 
typedef struct
{
  uint32_t  MasterOutputTrigger;   
 
  uint32_t  MasterSlaveMode;       





 
} TIM_MasterConfigTypeDef;



 
typedef struct
{
  uint32_t  SlaveMode;         
 
  uint32_t  InputTrigger;      
 
  uint32_t  TriggerPolarity;   
 
  uint32_t  TriggerPrescaler;  
 
  uint32_t  TriggerFilter;     
 

} TIM_SlaveConfigTypeDef;





 
typedef struct
{
  uint32_t OffStateRunMode;       

  uint32_t OffStateIDLEMode;      

  uint32_t LockLevel;             

  uint32_t DeadTime;              

  uint32_t BreakState;            

  uint32_t BreakPolarity;         

  uint32_t BreakFilter;           

  uint32_t AutomaticOutput;       

} TIM_BreakDeadTimeConfigTypeDef;



 
typedef enum
{
  HAL_TIM_STATE_RESET             = 0x00U,     
  HAL_TIM_STATE_READY             = 0x01U,     
  HAL_TIM_STATE_BUSY              = 0x02U,     
  HAL_TIM_STATE_TIMEOUT           = 0x03U,     
  HAL_TIM_STATE_ERROR             = 0x04U      
} HAL_TIM_StateTypeDef;



 
typedef enum
{
  HAL_TIM_CHANNEL_STATE_RESET             = 0x00U,     
  HAL_TIM_CHANNEL_STATE_READY             = 0x01U,     
  HAL_TIM_CHANNEL_STATE_BUSY              = 0x02U,     
} HAL_TIM_ChannelStateTypeDef;



 
typedef enum
{
  HAL_DMA_BURST_STATE_RESET             = 0x00U,     
  HAL_DMA_BURST_STATE_READY             = 0x01U,     
  HAL_DMA_BURST_STATE_BUSY              = 0x02U,     
} HAL_TIM_DMABurstStateTypeDef;



 
typedef enum
{
  HAL_TIM_ACTIVE_CHANNEL_1        = 0x01U,     
  HAL_TIM_ACTIVE_CHANNEL_2        = 0x02U,     
  HAL_TIM_ACTIVE_CHANNEL_3        = 0x04U,     
  HAL_TIM_ACTIVE_CHANNEL_4        = 0x08U,     
  HAL_TIM_ACTIVE_CHANNEL_CLEARED  = 0x00U      
} HAL_TIM_ActiveChannel;



 



typedef struct

{
  TIM_TypeDef                        *Instance;          
  TIM_Base_InitTypeDef               Init;               
  HAL_TIM_ActiveChannel              Channel;            
  DMA_HandleTypeDef                  *hdma[7];          
 
  HAL_LockTypeDef                    Lock;               
  volatile HAL_TIM_StateTypeDef          State;              
  volatile HAL_TIM_ChannelStateTypeDef   ChannelState[4];    
  volatile HAL_TIM_ChannelStateTypeDef   ChannelNState[4];   
  volatile HAL_TIM_DMABurstStateTypeDef  DMABurstState;      

#line 380 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"
} TIM_HandleTypeDef;

#line 423 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"



 
 

 


 



 




 



 
#line 466 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 
#line 481 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 





 



 




 



 






 



 







 



 





 



 




 



 





 



 




 



 




 



 




 



 




 



 




 



 




 



 





 



 




 



 





 



 






 



 




 



 





 



 
#line 680 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 




 



 
#line 703 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 




 



 
#line 731 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 







 



 
#line 760 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 







 



 






 



 




 



 






 



 




 



 




 


 






 



 




 



 




 



 




 



 
#line 873 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 




 



 







 



 
#line 909 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 
#line 925 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 







 



 






 



 




 



 
#line 982 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 
#line 996 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"


 



 






 



 
 

 


 




 
#line 1066 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"





 






 






 
#line 1096 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"







 
#line 1114 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"






 















 















 














 














 



















 



















 
















 
















 








 







 







 






 







 










 











 
#line 1326 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"








 



















 




















 

















 
















 
















 
















 




















 




















 













 












 
















 













 





 
 

 


 

 




 
 

 


 



#line 1591 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"









































































#line 1674 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"














































#line 1728 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"













#line 1747 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"

#line 1756 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"























#line 1797 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"













































#line 1849 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"













#line 1873 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"



 
 

 
#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"
















 

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"



 



 

 


 



 

typedef struct
{
  uint32_t IC1Polarity;         
 

  uint32_t IC1Prescaler;        
 

  uint32_t IC1Filter;           
 

  uint32_t Commutation_Delay;   
 
} TIM_HallSensor_InitTypeDef;


 
 

 


 



 
#line 82 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"












#line 106 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"


 



 
 

 


 



 
 

 


 
#line 193 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h"



 
 

 


 




 
 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, const TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim);

 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim);


 




 
 
 
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

 
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

 
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                          uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
 
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

 
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                           uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
 
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

 
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);


 




 
 
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                              uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                                 uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                                  uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
                                                        const TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
                                                const TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap);


 




 
 
void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);


 




 
 
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIMEx_GetChannelNState(const TIM_HandleTypeDef *htim,  uint32_t ChannelN);


 



 
 

 


 
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma);


 
 



 



 






#line 1881 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"

 


 




 
 
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim, const uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim);


 




 
 
HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                       uint16_t Length);
HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                        uint16_t Length);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim, uint32_t OnePulseMode);
HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
 
HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);


 




 
 
HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef *htim, const TIM_Encoder_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData1,
                                            uint32_t *pData2, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);


 




 
 
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef *htim, const TIM_OC_InitTypeDef *sConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, const TIM_OC_InitTypeDef *sConfig,
                                            uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef *htim, const TIM_IC_InitTypeDef *sConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef *sConfig,
                                                 uint32_t OutputChannel,  uint32_t InputChannel);
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(TIM_HandleTypeDef *htim,
                                           const TIM_ClearInputConfigTypeDef *sClearInputConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, const TIM_ClockConfigTypeDef *sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim, uint32_t TI1_Selection);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro_IT(TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                              uint32_t BurstRequestSrc, const uint32_t  *BurstBuffer,
                                              uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiWriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                   uint32_t BurstRequestSrc, const uint32_t *BurstBuffer,
                                                   uint32_t BurstLength,  uint32_t DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                             uint32_t BurstRequestSrc, uint32_t  *BurstBuffer, uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                  uint32_t BurstRequestSrc, uint32_t  *BurstBuffer,
                                                  uint32_t  BurstLength, uint32_t  DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim, uint32_t EventSource);
uint32_t HAL_TIM_ReadCapturedValue(const TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);

 








 




 
 
HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(const TIM_HandleTypeDef *htim);

 
HAL_TIM_ActiveChannel HAL_TIM_GetActiveChannel(const TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIM_GetChannelState(const TIM_HandleTypeDef *htim,  uint32_t Channel);
HAL_TIM_DMABurstStateTypeDef HAL_TIM_DMABurstState(const TIM_HandleTypeDef *htim);


 



 
 

 


 
void TIM_Base_SetConfig(TIM_TypeDef *TIMx, const TIM_Base_InitTypeDef *Structure);
void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, const TIM_OC_InitTypeDef *OC_Config);
void TIM_ETR_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ExtTRGPrescaler,
                       uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);

void TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_DMAError(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureCplt(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_CCxChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelState);







 
 



 



 





#line 408 "../Core/Inc/stm32f4xx_hal_conf.h"


#line 1 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"
















 

 







 
#line 29 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"



 



 

 


 



 
typedef struct
{
  uint32_t BaudRate;                  



 

  uint32_t WordLength;                
 

  uint32_t StopBits;                  
 

  uint32_t Parity;                    




 

  uint32_t Mode;                      
 

  uint32_t HwFlowCtl;                 
 

  uint32_t OverSampling;              
 
} UART_InitTypeDef;







































 
typedef enum
{
  HAL_UART_STATE_RESET             = 0x00U,    
 
  HAL_UART_STATE_READY             = 0x20U,    
 
  HAL_UART_STATE_BUSY              = 0x24U,    
 
  HAL_UART_STATE_BUSY_TX           = 0x21U,    
 
  HAL_UART_STATE_BUSY_RX           = 0x22U,    
 
  HAL_UART_STATE_BUSY_TX_RX        = 0x23U,    

 
  HAL_UART_STATE_TIMEOUT           = 0xA0U,    
 
  HAL_UART_STATE_ERROR             = 0xE0U     
 
} HAL_UART_StateTypeDef;







 
typedef uint32_t HAL_UART_RxTypeTypeDef;









 
typedef uint32_t HAL_UART_RxEventTypeTypeDef;



 
typedef struct __UART_HandleTypeDef
{
  USART_TypeDef                 *Instance;         

  UART_InitTypeDef              Init;              

  const uint8_t                 *pTxBuffPtr;       

  uint16_t                      TxXferSize;        

  volatile uint16_t                 TxXferCount;       

  uint8_t                       *pRxBuffPtr;       

  uint16_t                      RxXferSize;        

  volatile uint16_t                 RxXferCount;       

  volatile HAL_UART_RxTypeTypeDef ReceptionType;       

  volatile HAL_UART_RxEventTypeTypeDef RxEventType;    

  DMA_HandleTypeDef             *hdmatx;           

  DMA_HandleTypeDef             *hdmarx;           

  HAL_LockTypeDef               Lock;              

  volatile HAL_UART_StateTypeDef    gState;           

 

  volatile HAL_UART_StateTypeDef    RxState;          
 

  volatile uint32_t                 ErrorCode;         

#line 212 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"

} UART_HandleTypeDef;

#line 243 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"



 

 


 



 
#line 265 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"


 



 




 



 




 



 





 



 






 



 





 



 




 



 




 



 




 



 




 





 
#line 369 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"


 









 













 



 




 



 





 



 

 


 






 
#line 444 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"





 



















 























 







 
#line 509 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"






 







 







 







 

















 



















 


















 
















 



















 



















 



















 









 





 






 





 



 

 


 



 

 
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart, uint32_t BreakDetectLength);
HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart, uint8_t Address, uint32_t WakeUpMethod);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

 
#line 735 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"



 



 

 
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint16_t *RxLen,
                                           uint32_t Timeout);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

HAL_UART_RxEventTypeTypeDef HAL_UARTEx_GetRxEventType(UART_HandleTypeDef *huart);

 
HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart);

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);



 



 
 
HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart);


 



 
 
HAL_UART_StateTypeDef HAL_UART_GetState(const UART_HandleTypeDef *huart);
uint32_t              HAL_UART_GetError(const UART_HandleTypeDef *huart);


 



 
 
 
 


 


 







 

 


 
#line 859 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h"






 









 






 

 


 

HAL_StatusTypeDef UART_Start_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef UART_Start_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);



 



 



 







#line 412 "../Core/Inc/stm32f4xx_hal_conf.h"






























































 
#line 490 "../Core/Inc/stm32f4xx_hal_conf.h"





#line 30 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"



 



  

 
 



 



 
typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;


 



 
   
 


 


 
#line 93 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"

#line 116 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"


 



 





 




#line 140 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"

#line 155 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"

#line 185 "../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"


 



 





 

 



 
extern volatile uint32_t uwTick;
extern uint32_t uwTickPrio;
extern HAL_TickFreqTypeDef uwTickFreq;


 

 


 


 
 
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority);


 



 
 
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickPrio(void);
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef HAL_GetTickFreq(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
void HAL_DBGMCU_EnableDBGSleepMode(void);
void HAL_DBGMCU_DisableDBGSleepMode(void);
void HAL_DBGMCU_EnableDBGStopMode(void);
void HAL_DBGMCU_DisableDBGStopMode(void);
void HAL_DBGMCU_EnableDBGStandbyMode(void);
void HAL_DBGMCU_DisableDBGStandbyMode(void);
void HAL_EnableCompensationCell(void);
void HAL_DisableCompensationCell(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);







 



 
 
 


 


 
 


 


 
 
 


 



  
  







#line 77 "../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c"



 




 



 
 


 



 
 
 


 
 
FLASH_ProcessTypeDef pFlash  = {.ProcedureOnGoing = FLASH_PROC_NONE,
                                .NbSectorsToErase = 0U,
                                .VoltageForErase= 0x00000000U,
                                .Sector = 0U,
                                .Bank = 1U,
                                .Address = 0U,
                                .Lock = HAL_UNLOCKED,
                                .ErrorCode = 0x00000000U};


 

 


 
 
static void   FLASH_Program_DoubleWord(uint32_t Address, uint64_t Data);
static void   FLASH_Program_Word(uint32_t Address, uint32_t Data);
static void   FLASH_Program_HalfWord(uint32_t Address, uint16_t Data);
static void   FLASH_Program_Byte(uint32_t Address, uint8_t Data);
static void   FLASH_SetErrorCode(void);

HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);


 

 


 














 









 
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
  HAL_StatusTypeDef status;

   
  do{ if((&pFlash)->Lock == HAL_LOCKED) { return HAL_BUSY; } else { (&pFlash)->Lock = HAL_LOCKED; } }while (0U);

   
  ((void)0U);

   
  status = FLASH_WaitForLastOperation((uint32_t)50000U);

  if (status == HAL_OK)
  {
    if (TypeProgram == 0x00000000U)
    {
       
      FLASH_Program_Byte(Address, (uint8_t) Data);
    }
    else if (TypeProgram == 0x00000001U)
    {
       
      FLASH_Program_HalfWord(Address, (uint16_t) Data);
    }
    else if (TypeProgram == 0x00000002U)
    {
       
      FLASH_Program_Word(Address, (uint32_t) Data);
    }
    else
    {
       
      FLASH_Program_DoubleWord(Address, Data);
    }

     
    status = FLASH_WaitForLastOperation((uint32_t)50000U);

     
    ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR &= (~(0x1UL << (0U)));
  }

   
  do{ (&pFlash)->Lock = HAL_UNLOCKED; }while (0U);

  return status;
}









 
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
  HAL_StatusTypeDef status = HAL_OK;

   
  ((void)0U);

   
  (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= ((0x1UL << (24U))));

   
  (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= (0x02000000U));

  pFlash.ProcedureOnGoing = FLASH_PROC_PROGRAM;
  pFlash.Address = Address;

  if (TypeProgram == 0x00000000U)
  {
     
    FLASH_Program_Byte(Address, (uint8_t) Data);
  }
  else if (TypeProgram == 0x00000001U)
  {
     
    FLASH_Program_HalfWord(Address, (uint16_t) Data);
  }
  else if (TypeProgram == 0x00000002U)
  {
     
    FLASH_Program_Word(Address, (uint32_t) Data);
  }
  else
  {
     
    FLASH_Program_DoubleWord(Address, Data);
  }

  return status;
}




 
void HAL_FLASH_IRQHandler(void)
{
  uint32_t addresstmp = 0U;

   

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & (((0x1UL << (1U)) | (0x1UL << (4U)) | (0x1UL << (5U)) | (0x1UL << (6U)) | (0x1UL << (7U)) | (0x1UL << (8U)))))) != RESET)





  {
    if (pFlash.ProcedureOnGoing == FLASH_PROC_SECTERASE)
    {
       
      addresstmp = pFlash.Sector;
      pFlash.Sector = 0xFFFFFFFFU;
    }
    else if (pFlash.ProcedureOnGoing == FLASH_PROC_MASSERASE)
    {
       
      addresstmp = pFlash.Bank;
    }
    else
    {
       
      addresstmp = pFlash.Address;
    }

     
    FLASH_SetErrorCode();

     
    HAL_FLASH_OperationErrorCallback(addresstmp);

     
    pFlash.ProcedureOnGoing = FLASH_PROC_NONE;
  }

   
  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (0U))))) != RESET)
  {
     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (0U))));

    if (pFlash.ProcedureOnGoing == FLASH_PROC_SECTERASE)
    {
       
      pFlash.NbSectorsToErase--;

       
      if (pFlash.NbSectorsToErase != 0U)
      {
        addresstmp = pFlash.Sector;
         
        HAL_FLASH_EndOfOperationCallback(addresstmp);

         
        pFlash.Sector++;
        addresstmp = pFlash.Sector;
        FLASH_Erase_Sector(addresstmp, pFlash.VoltageForErase);
      }
      else
      {
         
         
        pFlash.Sector = addresstmp = 0xFFFFFFFFU;
        pFlash.ProcedureOnGoing = FLASH_PROC_NONE;

         
        FLASH_FlushCaches();

         
        HAL_FLASH_EndOfOperationCallback(addresstmp);
      }
    }
    else
    {
      if (pFlash.ProcedureOnGoing == FLASH_PROC_MASSERASE)
      {
         
         
        FLASH_FlushCaches();

         
        HAL_FLASH_EndOfOperationCallback(pFlash.Bank);
      }
      else
      {
         
         
        HAL_FLASH_EndOfOperationCallback(pFlash.Address);
      }
      pFlash.ProcedureOnGoing = FLASH_PROC_NONE;
    }
  }

  if (pFlash.ProcedureOnGoing == FLASH_PROC_NONE)
  {
     
    ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR) &= ~(((0x1UL << (0U)) | (0x1UL << (1U)) | (0x1FUL << (3U)) | ((0x1UL << (2U))))));

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR &= ~(uint32_t)((0x1UL << (24U))));

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR &= ~(uint32_t)(0x02000000U));
  }
}









 
__weak void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
   
  (void)ReturnValue;
  

 
}








 
__weak void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
   
  (void)ReturnValue;
  

 
}



 














 




 
HAL_StatusTypeDef HAL_FLASH_Unlock(void)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR) & ((0x1UL << (31U)))) != RESET)
  {
     
    ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->KEYR) = (0x45670123U));
    ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->KEYR) = (0xCDEF89ABU));

     
    if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR) & ((0x1UL << (31U)))) != RESET)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}




 
HAL_StatusTypeDef HAL_FLASH_Lock(void)
{
   
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= (0x1UL << (31U));

  return HAL_OK;
}




 
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void)
{
  if ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->OPTCR & (0x1UL << (0U))) != RESET)
  {
     
    ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->OPTKEYR = 0x08192A3BU;
    ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->OPTKEYR = 0x4C5D6E7FU;
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}




 
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void)
{
   
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->OPTCR |= (0x1UL << (0U));

  return HAL_OK;
}




 
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void)
{
   
  *(volatile uint8_t *)0x40023C14U |= (0x1UL << (1U));

   
  return (FLASH_WaitForLastOperation((uint32_t)50000U));
}



 













 










 
uint32_t HAL_FLASH_GetError(void)
{
  return pFlash.ErrorCode;
}



 





 
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout)
{
  uint32_t tickstart = 0U;

   
  pFlash.ErrorCode = 0x00000000U;

  

 
   
  tickstart = HAL_GetTick();

  while (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (16U))))) != RESET)
  {
    if (Timeout != 0xFFFFFFFFU)
    {
      if ((Timeout == 0U) || ((HAL_GetTick() - tickstart) > Timeout))
      {
        return HAL_TIMEOUT;
      }
    }
  }

   
  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (0U))))) != RESET)
  {
     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (0U))));
  }

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & (((0x1UL << (1U)) | (0x1UL << (4U)) | (0x1UL << (5U)) | (0x1UL << (6U)) | (0x1UL << (7U)) | (0x1UL << (8U)))))) != RESET)





  {
     
    FLASH_SetErrorCode();
    return HAL_ERROR;
  }

   
  return HAL_OK;

}












 
static void FLASH_Program_DoubleWord(uint32_t Address, uint64_t Data)
{
   
  ((void)0U);

   
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR) &= ~((0x3UL << (8U))));
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= 0x00000300U;
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= (0x1UL << (0U));

   
  *(volatile uint32_t *)Address = (uint32_t)Data;

  
 
  __isb(0xF);

   
  *(volatile uint32_t *)(Address + 4) = (uint32_t)(Data >> 32);
}













 
static void FLASH_Program_Word(uint32_t Address, uint32_t Data)
{
   
  ((void)0U);

   
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR) &= ~((0x3UL << (8U))));
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= 0x00000200U;
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= (0x1UL << (0U));

  *(volatile uint32_t *)Address = Data;
}












 
static void FLASH_Program_HalfWord(uint32_t Address, uint16_t Data)
{
   
  ((void)0U);

   
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR) &= ~((0x3UL << (8U))));
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= 0x00000100U;
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= (0x1UL << (0U));

  *(volatile uint16_t *)Address = Data;
}












 
static void FLASH_Program_Byte(uint32_t Address, uint8_t Data)
{
   
  ((void)0U);

   
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR) &= ~((0x3UL << (8U))));
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= 0x00000000U;
  ((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->CR |= (0x1UL << (0U));

  *(volatile uint8_t *)Address = Data;
}




 
static void FLASH_SetErrorCode(void)
{
  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (4U))))) != RESET)
  {
    pFlash.ErrorCode |= 0x00000010U;

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (4U))));
  }

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (5U))))) != RESET)
  {
    pFlash.ErrorCode |= 0x00000008U;

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (5U))));
  }

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (6U))))) != RESET)
  {
    pFlash.ErrorCode |= 0x00000004U;

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (6U))));
  }

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (7U))))) != RESET)
  {
    pFlash.ErrorCode |= 0x00000002U;

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (7U))));
  }

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (8U))))) != RESET)
  {
    pFlash.ErrorCode |= 0x00000001U;

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (8U))));
  }

  if (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR & ((0x1UL << (1U))))) != RESET)
  {
    pFlash.ErrorCode |= 0x00000020U;

     
    (((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->SR = ((0x1UL << (1U))));
  }
}



 





 



 

