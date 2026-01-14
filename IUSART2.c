#include "stm32f4xx.h"
#include "IUSART2.h"

void IUSART2_Init(void)
{
	RCC->AHB1ENR |= ( 1 << 0 );
	RCC->APB1ENR |= ( 1 << 17 ); // USART2
	//PA2 as Tx
	GPIOA->MODER &= ~(3 << ( 2 * 2));
	GPIOA->MODER |= ( 2 << ( 2 * 2));

	GPIOA->AFR[0] &= ~(15 <<( 4* 2));
	GPIOA->AFR[0] |= ( 7 <<(4 *2));
	//PA3 as Rx
	GPIOA->MODER &= ~(3 <<(2 *3));
	GPIOA->MODER |= (2 << (2 * 3));

	GPIOA->AFR[0] &= ~(15 <<( 4* 3));
	GPIOA->AFR[0] |= ( 7 <<(4 *3));

	USART2->CR1 |= USART_CR1_UE;
	USART2->CR1 |= USART_CR1_TE;
	USART1->BRR = 0x8B;
	USART2->CR1 |= USART_CR1_RE;
	USART2->CR1 |= USART_CR1_RXNEIE;
	USART2->CR1 &= ~USART_CR1_OVER8;// 16 over sampling
	USART2->CR1 &= ~USART_CR1_M; // 8N1 data frame
	USART2->CR1 &= ~USART_CR1_PCE; //parity disable

	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn,2);

	USART2->BRR = 0x8B;
}
