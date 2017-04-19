#ifndef AES_UTILITIES
#define AES_UTILITIES
		
	#include "stm32l053xx.h"
    #include <stdio.h> 

    /* Define  Baudrate setting (BRR) for USART */
    #define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
    #define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
    #define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
    #define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

    /* Define  USART */
    #define USARTx  USART2
		
	#define RTE_Compiler_IO_STDOUT_User 1
    //#define BUF_SIZE 31  //31 // for AES-128 use 32  		
		
    int mystdout_init (void);
    void SystemClock_Config(void);
    void ConfigureGPIO(void);
    uint8_t stdin_getchar(void);
	void pulse (void);
	void ConfigureRNG(void);
	uint8_t getrn(void);
    
    __INLINE void SystemClock_Config(void)
    {
        RCC->APB1ENR |= (RCC_APB1ENR_PWREN); /* (1) */
        PWR->CR = (PWR->CR & ~(PWR_CR_VOS)) | PWR_CR_VOS_0; /* (2) */
        
        //Set MSI to 1.048MHz
        RCC->ICSCR = (RCC->ICSCR & ~(RCC_ICSCR_MSIRANGE)) | RCC_ICSCR_MSIRANGE_4 ;  
        RCC->CR |= RCC_CR_MSION ; /* (3) */

        while ((RCC->CR & RCC_CR_MSIRDY ) != RCC_CR_MSIRDY ) /* (4) */  
        {
            __NOP();
        }

        //Enable HSE
        //RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP; 
        RCC->CR |=RCC_CR_HSEON;
		//Configure PLL to generate a 48MHz signal to clock RNG
        RCC->CFGR |= RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL6 | RCC_CFGR_PLLDIV2;   
        //Enable PLL
        RCC->CR |= RCC_CR_PLLON;

        RCC->CFGR |= RCC_CFGR_SW_MSI ; /* (8) */
        while ((RCC->CFGR & RCC_CFGR_SWS_MSI)  != 0) /* (9) */
        {
            __NOP();
        }

        //Switch system clock to HSE
        RCC->CFGR = ((RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_HSE);

        while ((RCC->CFGR & RCC_CFGR_SWS_HSE) != 0)
        {
            __NOP();
        }
        //Turn off the MSI
        RCC->CR &= (~RCC_CR_MSION) ;
    };    
    
    __INLINE void ConfigureGPIO(void)
    {
        
        RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;

        GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE9)) | (GPIO_MODER_MODE9_0);
        GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE8)) | (GPIO_MODER_MODE8_0);  
        
        GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE8)) | (GPIO_MODER_MODE8_1);
        GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEED8;
    };    

    
#endif
    
