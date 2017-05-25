/* Part of the code in this file has been copied and modified from ARM copyrighted code and STMicroelectronics 
		copyrighted code example snippets for MCU configuration purposes only. Following is the copyright information. 
		The copyrighted code is unrelated to the AES implementation */

/* Copyright (c) 2013 - 2016 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


/*
Copyright ? 2015 STMicroelectronics International N.V.. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted, provided that the following conditions are met:

1.	Redistribution of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
2.	Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
3.	Neither the name of STMicroelectronics nor the names of other contributors to 
this software may be used to endorse or promote products derived from this software 
without specific written permission.
4.	This software, including modifications and/or derivative works of this software, 
must execute solely and exclusively on microcontroller or microprocessor devices 
manufactured by or for STMicroelectronics.
5.	Redistribution and use of this software other than as permitted under this 
license is void and will automatically terminate your rights under this license. 

THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS, 
IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY 
INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. 
IN NO EVENT SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


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
				
    int mystdout_init (void);
    void SystemClock_Config(void);
    void ConfigureGPIO(void);
    uint8_t stdin_getchar(void);
		int stdout_putchar (int ch);
		void pulse (void);
		void ConfigureRNG(void);
		uint8_t getrn(void);
		void rdser( uint8_t * buffer );
		void delayCycles(uint32_t cycles);
    
    __INLINE void SystemClock_Config(void)
    {
        RCC->APB1ENR |= (RCC_APB1ENR_PWREN); 
        PWR->CR = (PWR->CR & ~(PWR_CR_VOS)) | PWR_CR_VOS_0; 
        
        //Set MSI to 1.048MHz
        RCC->ICSCR = (RCC->ICSCR & ~(RCC_ICSCR_MSIRANGE)) | RCC_ICSCR_MSIRANGE_4 ;  
        RCC->CR |= RCC_CR_MSION ; 

        while ((RCC->CR & RCC_CR_MSIRDY ) != RCC_CR_MSIRDY )  
        {
            __NOP();
        }
        
				RCC->CFGR |= RCC_CFGR_SW_MSI ; 
        
				while ((RCC->CFGR & RCC_CFGR_SWS_MSI)  != 0) 
        {
            __NOP();
        }
				
        //Enable HSE
        RCC->CR |=RCC_CR_HSEON;
				//Configure PLL to generate a 48MHz signal to clock RNG
        RCC->CFGR |= RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL24 | RCC_CFGR_PLLDIV2;   
        //Enable PLL
        RCC->CR |= RCC_CR_PLLON;



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
    
