#include "utils.h"


//----------------------------------------------------------------------------
// Enable serial communication at 57600 bauds
//----------------------------------------------------------------------------
int mystdout_init (void) {
  
#ifdef RTE_Compiler_IO_STDOUT_User
  //uint8_t idx ;  

  RCC->IOPENR   |=   ( 1ul <<  0);                   /* Enable GPIOA clock */
  RCC->APB1ENR  |=   ( 1ul << 17);                   /* Enable USART#2 clock */

  /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
  GPIOA->AFR[0] &= ~((15ul << 4* 3) | (15ul << 4* 2) );
  GPIOA->AFR[0] |=  (( 4ul << 4* 3) | ( 4ul << 4* 2) );
  GPIOA->MODER  &= ~(( 3ul << 2* 3) | ( 3ul << 2* 2) );
  GPIOA->MODER  |=  (( 2ul << 2* 3) | ( 2ul << 2* 2) );

  USARTx->BRR  = __USART_BRR(4000000ul, 57600ul);  /* 57600 baud @ 4MHz */
  USARTx->CR3    = 0x0000;                           /* no flow control */
  USARTx->CR2    = 0x0000;                           /* 1 stop bit */
  USARTx->CR1    = ((   1ul <<  2) |                 /* enable RX */
                    (   1ul <<  3) |                 /* enable TX */
                    (   0ul << 12) |                 /* 8 data bits */
                    (   0ul << 28) |                 /* 8 data bits */
                    (   1ul <<  0) );                /* enable USART */
   
    
#endif

 return (0); 

}



// Get a char from the serial interface
uint8_t stdin_getchar(){
    while( (USARTx->ISR & USART_ISR_RXNE) == 0);
    return USARTx->RDR ;
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


