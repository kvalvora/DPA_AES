#include "stm32l0xx.h"
#include "utils.h"
#include "AES.h"

void delayCycles(uint32_t cycles);

int main(void)
{ 
    uint8_t i;
	uint32_t j = 0;
	unsigned char key[16] = {0x2b,0x28,0xab,0x09,0x7e,0xae,0xf7,0xcf,0x15,0xd2,0x15,0x4f,0x16,0xa6,0x88,0x3c}; 
    AES_KEY expanded;
    unsigned char ciphertext[16];
    unsigned char plaintext[16] = {0x32,0x88,0x31,0xe0,0x43,0x5a,0x31,0x37,0xf6,0x30,0x98,0x07,0xa8,0x8d,0xa2,0x34};
		
	SystemClock_Config();
	ConfigureRNG();
	ConfigureGPIO();
	mystdout_init();   
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK; /* Select system clock to be output on the MCO without prescaler */
	
	GPIOB->BRR = (1<<8);

	while(j<10){ 
		//AES-128 software implementation
        private_AES_set_encrypt_key(key, 128, &expanded) ;
		
		for (i=0; i<16; i++) {*(plaintext+i) = getrn();} //generating a random plaintext
		//printf("%d:",j++);
		Display(plaintext);
		GPIOB->BSRR = (1<<8);	//trigger set
		delayCycles(10);
		GPIOB->BRR = (1<<8);
		AES_encrypt(plaintext,ciphertext,&expanded);
        //GPIOB->BRR = (1<<8);	//trigger reset
        //Display(ciphertext) ;       
        //printf("Key:");
		//Display(key);
		j++;
		
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

/******************************************************************************/
/*            Cortex-M0 Plus Processor Exceptions Handlers                         */
/******************************************************************************/
/**
  * Brief   This function handles NMI exception.
  * Param   None
  * Retval  None
  */
void NMI_Handler(void)
{
}

/**
  * Brief   This function handles Hard Fault exception.
  * Param   None
  * Retval  None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
      
  }
}

/**
  * Brief   This function handles SVCall exception.
  * Param   None
  * Retval  None
  */
void SVC_Handler(void)
{
}

/**
  * Brief   This function handles PendSVC exception.
  * Param   None
  * Retval  None
  */
void PendSV_Handler(void)
{
}
