#include"stm32f4xx.h"
#include "I2C1.h"

void I2C1_Init(void)
{
	    RCC->AHB1ENR |= ( 1 << 1 );
	    RCC->APB1ENR |= ( 1 << 21);
	    //PB6 as SCL
     	GPIOB->MODER &= ~(3 << ( 2 * 6));
		GPIOB->MODER |= ( 2 << ( 2 * 6));
		GPIOB->OTYPER &= ~( 3 << 6);
		GPIOB->OTYPER |= ( 1 << 6);
		GPIOB->OSPEEDR &= ~( 3 << ( 2* 6));
		GPIOB->OSPEEDR |= ( 2 << ( 2 * 6));
		GPIOB->PUPDR &= ~(3 << (2 * 6));
		GPIOB->PUPDR |=  (1 << (2 * 6));

		GPIOB->AFR[0] &= ~(15 <<( 4* 6));
		GPIOB->AFR[0] |= ( 4 <<(4 *6));
		//PB7 as SDA
		GPIOB->MODER &= ~(3 <<(2 *7));
		GPIOB->MODER |= (2 << (2 * 7));
		GPIOB->OTYPER &= ~( 3 << 7);
		GPIOB->OTYPER |= ( 1 << 7);
		GPIOB->OSPEEDR &= ~( 3 << ( 2* 7));
	    GPIOB->OSPEEDR |= ( 2 << ( 2 * 7));
	    GPIOB->PUPDR &= ~(3 << (2 * 7));
	    GPIOB->PUPDR |=  (1 << (2 * 7));

		GPIOB->AFR[0] &= ~(15 <<( 4* 7));
		GPIOB->AFR[0] |= ( 4 <<(4 *7));

		I2C1->CR1 &= ~I2C_CR1_PE;
		I2C1->CR1 |= I2C_CR1_NOSTRETCH;
		I2C1->CR1 |= I2C_CR1_ACK;
		I2C1->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ERREN | I2C_CR2_ITBUFEN;
		I2C1->CR2 |= (16 << 0);
		I2C1->CCR = 0x50; //  100kHz speed
		I2C1->TRISE = 0x11; // 16(PCLK) + 1
		I2C1->CR1 |= I2C_CR1_PE;

		NVIC_EnableIRQ(I2C1_EV_IRQn);
		NVIC_SetPriority(I2C1_EV_IRQn,3);
}
