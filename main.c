/*************************************************************
	Open SSL AES-128 Encryption
	
	Vernam Lab - WPI - May 2017
	Authors: Abraham Fernandez-Rubio and Kewal Vora
	
	Description:
	
	
**************************************************************/

#include "stm32l0xx.h"
#include "utils.h"
#include "AES.h"
#include "string.h"

void delayCycles(uint32_t cycles);

int main(void)
{ 
	uint8_t i;

	uint8_t randstr[16] ;
	uint8_t empty[16] =  "NULLNULLNULLNUL\0";


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
	delayCycles(20);
	
	while(1){ 
		
		memcpy(randstr,empty,16); //init
		rdser(randstr); //get new string
		
		if(memcmp(randstr,empty,16)!=0){
			
			//AES-128 set encryption key
			private_AES_set_encrypt_key(key, 128, &expanded) ;
			
			//Generate random plaintext using built in hardware RNG
			for (i=0; i<16; i++) {*(plaintext+i) = getrn();} //generating a random plaintext
			
			//Send the plaintext over serial interface
			Display(plaintext);
			
			//Trigger GPIO pin PB8
			GPIOB->BSRR = (1<<8);	
			delayCycles(10);
			GPIOB->BRR = (1<<8);
			
			//AES encryption
			AES_encrypt(plaintext,ciphertext,&expanded);
		}
		
	}

}


