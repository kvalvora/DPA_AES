/* Part of the code in this file has been copied and modified from ARM copyrighted code,
		following is the copyright information. 
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


#include "utils.h"
#define BUF_SIZE 32

//----------------------------------------------------------------------------
// Enable serial communication at 57600 bauds
//----------------------------------------------------------------------------
int mystdout_init (void) {
  

  RCC->IOPENR   |=   ( 1ul <<  0);                   /* Enable GPIOA clock */
  RCC->APB1ENR  |=   ( 1ul << 17);                   /* Enable USART#2 clock */

  /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
  GPIOA->AFR[0] &= ~((15ul << 4* 3) | (15ul << 4* 2) );
  GPIOA->AFR[0] |=  (( 4ul << 4* 3) | ( 4ul << 4* 2) );
  GPIOA->MODER  &= ~(( 3ul << 2* 3) | ( 3ul << 2* 2) );
  GPIOA->MODER  |=  (( 2ul << 2* 3) | ( 2ul << 2* 2) );

  USARTx->BRR  = __USART_BRR(4000000ul, 9600ul);  /* 57600 baud @ 4MHz */
  USARTx->CR3    = 0x0000;                           /* no flow control */
  USARTx->CR2    = 0x0000;                           /* 1 stop bit */
  USARTx->CR1    = ((   1ul <<  2) |                 /* enable RX */
                    (   1ul <<  3) |                 /* enable TX */
                    (   0ul << 12) |                 /* 8 data bits */
                    (   0ul << 28) |                 /* 8 data bits */
                    (   1ul <<  0) );                /* enable USART */
   
 return (0); 

}



// Get a char from the serial interface
uint8_t stdin_getchar(){
    while( (USARTx->ISR & USART_ISR_RXNE) == 0);
    return USARTx->RDR ;
}

int stdout_putchar (int ch) {

  while (!(USARTx->ISR & 0x0080));
  USARTx->TDR = (ch & 0xFF);
  return (ch);
}

// Generate a pulse
void pulse (void){
	
	GPIOB->BSRR = (1 << 8); //Set pin PA8
	GPIOB->BSRR = (1 << 24); //Reset pin PA8
	
}

void ConfigureRNG(void){

    RCC->AHBENR |= RCC_AHBENR_RNGEN ;           /*!< RNG clock enable */
    RNG->CR |= RNG_CR_RNGEN  ;  
}

uint8_t getrn(){
    while( (RNG->SR & RNG_SR_DRDY) == 0);
    return ((uint8_t)(RNG->DR)) ;
}

void rdser( uint8_t * buffer ){
    uint8_t i = 0 , c = 0 ;
    
    //Clear the buffer
    for( i=0 ; i<BUF_SIZE ; i++)
        *(buffer+i) = 0 ;
    
    i = 0 ;
    c = stdin_getchar() ; 
    //Get data from the serial port until it is a CR
    while ( c != 0x0d ){
        buffer[i] = c ;
        c = stdin_getchar() ;                                
        i++ ;
        //Prevent buffer overflow
        if( i == BUF_SIZE )
            i = 0 ;
    }    
}

void delayCycles(uint32_t cycles)
{
	uint32_t i;
	for (i=0; i<cycles; i++)
	{
		__NOP();
	}
}
