#include "spi.h"

void spi_init(){
    //1. Write proper GPIO registers: Configure GPIO for MOSI, MISO and SCK pins.

   //MOSI configuration
   gpio_config_otype(D2, PUSH_PULL);
   gpio_config_alternate_function(D2, 5); //SPI AF = 6

   //SCK configuration
   gpio_config_otype(A1, PUSH_PULL);
   gpio_config_alternate_function(A1, 5); //SPI AF = 6

   RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //enable interrupt clock

// 2. Write to the SPI_CR1 register:
    // a) Configure the serial clock baud rate using the BR[2:0] bits (Note: 4).
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= (0b000 << SPI_CR1_BR);

    // b) Configure the CPOL and CPHA bits combination to define one of the four
    //     relationships between the data transfer and the serial clock (CPHA must be
    //     cleared in NSSP mode). (Note: 2 - except the case when CRC is enabled at TI
    //     mode).
    SPI1->CR1 &= ~SPI_CR1_CPOL;
    SPI1->CR1 &= ~SPI_CR1_CPHA;

    // c) Select simplex or half-duplex mode by configuring RXONLY or BIDIMODE and
    //     BIDIOE (RXONLY and BIDIMODE cannot be set at the same time).
    SPI1->CR1 &= ~SPI_CR1_RXONLY;
    SPI1->CR1 &= ~SPI_CR1_BIDIMODE; //set to full-duplex, what example code uses
      
    // d) Configure the LSBFIRST bit to define the frame format (Note: 2).
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

    // e) Configure the CRCL and CRCEN bits if CRC is needed (while SCK clock signal is
    //     at idle state).
    SPI1->CR1 &= ~SPI_CR1_CRCEN; 

    // f) Configure SSM and SSI (Notes: 2 & 3).
    SPI1->CR1 |= SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SSI;
    
    // g) Configure the MSTR bit (in multimaster NSS configuration, avoid conflict state on
    //     NSS if master is configured to prevent MODF error).
    SPI1->CR1 |= SPI_CR1_MSTR;

// 3. Write to SPI_CR2 register:
    // a) Configure the DS[3:0] bits to select the data length for the transfer.
    SPI1->CR2 &= ~SPI_CR2_DS; 
    SPI1->CR2 |= (0b0111 << SPI_CR2_DS_Pos); //8 bit transfer

    // b) Configure SSOE (Notes: 1 & 2 & 3).
    SPI1->CR2 |= SPI_CR2_SSOE;
    
    // c) Set the FRF bit if the TI protocol is required (keep NSSP bit cleared in TI mode).
    SPI1->CR2 &= ~SPI_CR2_FRF;

    // d) Set the NSSP bit if the NSS pulse mode between two data units is required (keep
    //     CHPA and TI bits cleared in NSSP mode).
    SPI1->CR2 &= ~SPI_CR2_NSSP;

    // e) Configure the FRXTH bit. The RXFIFO threshold must be aligned to the read
    //     access size for the SPIx_DR register.
    SPI1->CR2 &= ~SPI_CR2_FRXTH;

    // f) Initialize LDMA_TX and LDMA_RX bits if DMA is used in packed mode.
    SPI1->CR2 &= ~SPI_CR2_LDMATX;
    SPI1->CR2 &= ~SPI_CR2_LDMARX; //NOT USING DMA


//4. Write to SPI_CRCPR register: Configure the CRC polynomial if needed.
    SPI1->CRCPR = 7; 

    SPI1->CR1 |= SPI_CR1_SPE; //Enable the SPI interrupt

    //clock starts when TXFIFO isn't empty

}


void spi_disable() {
    while (SPI1->SR & SPI_SR_BSY) {
        // Wait until SPI is no longer busy
    }

    SPI1->CR1 &= ~SPI_CR1_SPE; //clear peripheral

    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN; //disable peripheral clock
}


void spi_write(uint8_t data) {
    //check if transmit buffer is ready
    while(!(SPI1->SR & SPI_SR_TXE)){
        //wait for transmit to be ready, aka SPI isn't busy
    }
    *((__IO uint8_t *)&SPI1->DR) = data; //put our data inside of the data register
    while(SPI1->SR & SPI_SR_BSY);
}












