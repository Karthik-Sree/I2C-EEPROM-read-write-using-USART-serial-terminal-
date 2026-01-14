
#include "stm32f4xx.h"
#include "I2C1.h"
#include "IUSART2.h"
volatile char read_buf[16];
volatile uint8_t data_size;

volatile uint8_t write_data = 'K';
volatile uint16_t write_addr = 0x1234;

volatile uint8_t stage = 0;
volatile uint8_t i2c_done_flag = 0;

volatile uint8_t read_data = 0;
volatile char *tptr;
volatile uint8_t busy_flag = 0;
volatile uint8_t read_stage = 0;
void I2C_WriteByte(void)
{
    stage = 0;
    i2c_done_flag = 0;

    //busy flag
    while (I2C1->SR2 & I2C_SR2_BUSY);

    // Start generated
    I2C1->CR1 |= I2C_CR1_START;
}

void I2C_ReadByte(uint16_t address)
{
    read_stage = 0;
    i2c_done_flag = 0;

    while (I2C1->SR2 & I2C_SR2_BUSY);

    I2C1->CR1 |= I2C_CR1_START;
}
void I2C1_EV_IRQHandler(void)
{
    uint32_t sr1 = I2C1->SR1;
   // I2C write sequence
    if(stage <= 4)
    {
        switch(stage)
        {
            case 0: // START
                if(sr1 & I2C_SR1_SB)
                {
                    I2C1->DR = 0xA0; // Slave addr + W
                    stage = 1;
                }
                break;
            case 1:
                if(sr1 & I2C_SR1_ADDR)
                {
                    (void)I2C1->SR2;
                    I2C1->DR = (write_addr >> 8) & 255;
                    stage = 2;
                }
                break;
            case 2: // MSB of target addr
                if(sr1 & I2C_SR1_TXE)
                {
                    I2C1->DR = write_addr & 255;
                    stage = 3;
                }
                break;
            case 3: // LSB of target addr
                if(sr1 & I2C_SR1_TXE)
                {
                    I2C1->DR = write_data;
                    stage = 4;
                }
                break;
            case 4: // Byte transfer finished
                if(sr1 & I2C_SR1_BTF)
                {
                    I2C1->CR1 |= I2C_CR1_STOP;
                    stage = 5;
                    i2c_done_flag = 1;
                }
                break;
        }
        return;
    }

// Read sequence
    if(read_stage <= 3)
    {
        switch(read_stage)
        {
            case 0: // START generated for read
                if(sr1 & I2C_SR1_SB)
                {
                    I2C1->DR = 0xA0; // Slave addr + write
                    read_stage = 1;
                }
                break;
            case 1: //MSB of target addr
                if(sr1 & I2C_SR1_ADDR)
                {
                    (void)I2C1->SR2;
                    I2C1->DR = (write_address >> 8) & 255;
                    read_stage = 2;
                }
                break;
            case 2: // LSB of target addr
                if(sr1 & I2C_SR1_TXE)
                {
                    I2C1->DR = write_address & 255;
                    read_stage = 3;
                }
                break;
            case 3: //  repeated START for read
                if(sr1 & I2C_SR1_TXE)
                {
                    I2C1->CR1 |= I2C_CR1_START;
                    read_stage = 4;
                }
                break;
            case 4: // Send Slave addr +read
                if(sr1 & I2C_SR1_SB)
                {
                    I2C1->DR = 0xA1; // SLA+R
                    read_stage = 5;
                }
                break;
            case 5:
                if(sr1 & I2C_SR1_ADDR)
                {
                    (void)I2C1->SR2;
                    I2C1->CR1 &= ~I2C_CR1_ACK;
                    I2C1->CR1 |= I2C_CR1_STOP;
                    read_stage = 6;
                }
                break;
            case 6: //  data received
                if(sr1 & I2C_SR1_RXNE)
                {
                    read_data = I2C1->DR;
                    i2c_done_flag = 2;
                    read_stage = 7;
                }
                break;
        }
    }
}

void IUSART2_send_string(char *str)
{
    if(str == 0) return;
    while(busy_flag);
    tptr = str;
    busy_flag = 1;
    USART2->CR1 |= USART_CR1_TXEIE;
}

void USART2_IRQHandler(void)
{
    if(USART2->SR & USART_SR_TXE)
    {
        if(*tptr)
            USART2->DR = *tptr++;
        else
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;
            busy_flag = 0;
        }
    }
}
void IUSART2_print_read(char *string, uint8_t lenth)
{
    for(uint8_t x = 0; x < lenth; x++)
    {
        while(!(USART2->SR & USART_SR_TXE)); // wait until TX ready
        USART2->DR = str[x];
    }

    // send newline
    while(!(USART2->SR & USART_SR_TXE));
    USART2->DR = '\r';
    while(!(USART2->SR & USART_SR_TXE));
    USART2->DR = '\n';
}
int main(void)
{
    IUSART2_Init();
    I2C1_Init();


    I2C_WriteByte();

    while(1)
    {

        if(i2c_done_flag == 1)
        {
            i2c_done_flag = 0;
            IUSART2_send_string("Byte Sent successfully\r\n");         
            I2C_ReadByte(write_addr);
        }
        if(i2c_done_flag == 2)
        {
            i2c_done_flag = 0;

            IUSART2_print_read(read_buf, data_size);
        }
    }
}
