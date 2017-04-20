#ifndef PARAMETERS
#define PARAMETERS

    #include "stm32l053xx.h"
    
    #define M 8

    /************************* Parameter selection ********************************/
    //Select (n,d) scheme, options: (3,1), (4,1), (5,1), (6,1), (6,2):
    //other schemes would work but need shares and inverse Vandermonde matrix.
    #define N 3
    #define D 1

    //Select the speed:
    //#define FAST 1    // Uses LUTs for field multiplication and squaring
    #define SLOW 1    // Uses code only, no LUTs
    //default           // Uses a trade-off between code and LUTs

    //Permute the functions of the shares:
    //works only for (3,1),(4,1),(5,1),(6,1),(6,2)
    //#define REORDER 1
    /******************************************************************************/

    #define EPS 0 //N-(2*D)-1
    #define gfadd(a,b) ((a)^(b))
    
    /* Time-out values */
    #define MSI_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
    #define PLL_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
    #define CLOCKSWITCH_TIMEOUT_VALUE  ((uint32_t)5000) /* 5 s    */

    /* Delay values */
    #define LONG_DELAY 500
    #define SHORT_DELAY 100

    /* Error codes */
    #define ERROR_MSI_TIMEOUT 1
    #define ERROR_PLL_TIMEOUT 2
    #define ERROR_CLKSWITCH_TIMEOUT 3

    #define RTE_Compiler_IO_STDOUT_User 1
    /* Define  Baudrate setting (BRR) for USART */
    #define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
    #define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
    #define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
    #define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

    /* Define  USART */
    #define USARTx  USART2

    #define BUF_SIZE 15  //31 // for AES-128 use 32    


    #ifdef SLOW
        //nothing
    #elif FAST 
        //extern static const uint8_t lLUT[16][256] ;
        //extern static const uint8_t hLUT[16][256] ;    
        extern uint8_t sqrLUT[256] ;
    #else   
        extern uint8_t secondOp[256] ;
        extern uint8_t sqrLUT[256] ;
    #endif

    
    //typedef unsigned char uint8_t ;
    extern volatile uint32_t msTicks; 
    extern __IO uint32_t Tick;
    extern volatile uint16_t error ;      
    extern uint8_t E1[N], E2[N] ; 
    extern uint8_t InvVand[N][N] ;
    extern uint8_t alpha[N] ;
    
#endif
